#include <bits/stdc++.h>
#include "../headers/order.hpp"
#include "../headers/orderbook.hpp"
using namespace std;

// Generate dummy orders
Orderbook::Orderbook(bool gen)
{
    srand(12);

    if (gen)
    {
        for (int i = 0; i < 10; i++)
        {
            double random_price = 90.0 + (rand() % 1001) / 100.0;
            int random_qty = rand() % 100 + 1;
            int random_qty2 = rand() % 100 + 1;

            addOrder(random_qty, random_price, BookSide::bid);
            this_thread::sleep_for(chrono::milliseconds(1)); // Sleep for a millisecond so orders have different timestamps
            addOrder(random_qty2, random_price, BookSide::bid);
        }

        for (int i = 0; i < 10; i++)
        {
            double random_price = 100.0 + (rand() % 1001) / 100.0;
            int random_qty = rand() % 100 + 1;
            int random_qty2 = rand() % 100 + 1;

            addOrder(random_qty, random_price, BookSide::ask);
            this_thread::sleep_for(chrono::milliseconds(1)); // Sleep for a millisecond so orders have different timestamps
            addOrder(random_qty2, random_price, BookSide::ask);
        }
    }
}

void Orderbook::addOrder(int q, double price, BookSide side)
{
    unique_ptr<Order> order_ptr = make_unique<Order>(q, side, price);

    if (side == BookSide::bid)
        bids[price].push_back(move(order_ptr));
    else
        asks[price].push_back(move(order_ptr));
}

template <typename T>
void Orderbook::cleanLeg(map<double, vector<unique_ptr<Order>>, T> &map)
{
    for (auto it = map.begin(); it != map.end();)
    {
        if (it->second.empty())
        {
            it = map.erase(it); // Erase empty levels
        }
        else
        {
            int total_quantity = 0;
            for (const auto &order : it->second)
            {
                total_quantity += order->quantity;
            }
            if (total_quantity == 0) // Check for zero total quantity
            {
                it = map.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}

template <typename T>
pair<int, double> Orderbook::fillOrder(map<double, vector<unique_ptr<Order>>, T> &offers, const OrderType type, const Side side, int &order_quantity, const double price, int &units_transacted, double &total_value)
{
    for (auto rit = offers.rbegin(); rit != offers.rend();)
    {
        auto &pair = *rit;
        double price_level = pair.first;
        auto &quotes = pair.second;

        bool can_transact = true;
        if (type == OrderType::limit)
        {
            if (side == Side::buy && price_level > price)
            {
                can_transact = false;
            }
            else if (side == Side::sell && price_level < price)
            {
                can_transact = false;
            }
        }

        if (can_transact)
        {
            auto it = quotes.begin();
            while (it != quotes.end() && order_quantity > 0)
            {
                int &cur_quote_qty = (*it)->quantity;
                const double cur_quote_price = (*it)->price;

                if (cur_quote_qty > order_quantity)
                {
                    units_transacted += order_quantity;
                    total_value += order_quantity * cur_quote_price;
                    cur_quote_qty -= order_quantity;
                    order_quantity = 0;
                    break;
                }
                else
                {
                    units_transacted += cur_quote_qty;
                    total_value += cur_quote_qty * cur_quote_price;
                    order_quantity -= cur_quote_qty;
                    it = quotes.erase(it);
                }
            }
        }
        rit++;
    }

    cleanLeg(offers);

    return make_pair(units_transacted, total_value);
}

pair<int, double> Orderbook::handleOrder(OrderType type, int order_quantity, Side side, double price)
{
    int units_transacted = 0;
    double total_value = 0;

    if (type == OrderType::market)
    {
        if (side == Side::sell)
        {
            return fillOrder(bids, OrderType::market, Side::sell, order_quantity, price, units_transacted, total_value);
        }
        else if (side == Side::buy)
        {
            return fillOrder(asks, OrderType::market, Side::buy, order_quantity, price, units_transacted, total_value);
        }
    }
    else if (type == OrderType::limit)
    {
        if (side == Side::buy)
        {
            if (bestQuote(BookSide::ask) <= price)
            {
                pair<int, double> fill = fillOrder(asks, OrderType::limit, Side::buy, order_quantity, price, units_transacted, total_value);
                addOrder(order_quantity, price, BookSide::bid);
                return fill;
            }
            else
            {
                addOrder(order_quantity, price, BookSide::bid);
                return make_pair(units_transacted, total_value);
            }
        }
        else
        {
            if (bestQuote(BookSide::bid) >= price)
            {
                pair<int, double> fill = fillOrder(bids, OrderType::limit, Side::sell, order_quantity, price, units_transacted, total_value);
                addOrder(order_quantity, price, BookSide::ask);
                return fill;
            }
            else
            {
                addOrder(order_quantity, price, BookSide::ask);
                return make_pair(units_transacted, total_value);
            }
        }
    }
    else
    {
        throw runtime_error("Invalid order type encountered");
    }

    return make_pair(0, 0);
}

double Orderbook::bestQuote(BookSide side)
{
    if (side == BookSide::bid)
    {
        if (bids.empty())
            return 0.0;
        return prev(bids.end())->first;
    }
    else if (side == BookSide::ask)
    {
        if (asks.empty())
            return 0.0;
        return prev(asks.end())->first;
    }
    else
    {
        return 0.0;
    }
}

template <typename T>
void Orderbook::printLeg(map<double, vector<unique_ptr<Order>>, T> &hashmap, BookSide side)
{
    if (side == BookSide::ask)
    {
        for (const auto &pair : hashmap)
        {
            int size_sum = 0;
            for (const auto &order : pair.second)
                size_sum += order->quantity;

            if (size_sum > 0)
            {
                string color = "31";
                cout << "\t\033[1;" << color << "m" << "$" << setw(6) << fixed << setprecision(2) << pair.first << setw(5) << size_sum << "\033[0m ";

                for (int i = 0; i < size_sum / 10; i++)
                    cout << "█";
                cout << "\n";
            }
        }
    }
    else if (side == BookSide::bid)
    {
        for (auto pair = hashmap.rbegin(); pair != hashmap.rend(); ++pair)
        {
            int size_sum = 0;
            for (const auto &order : pair->second)
                size_sum += order->quantity;

            if (size_sum > 0)
            {
                string color = "32";
                cout << "\t\033[1;" << color << "m" << "$" << setw(6) << fixed << setprecision(2) << pair->first << setw(5) << size_sum << "\033[0m ";

                for (int i = 0; i < size_sum / 10; i++)
                    cout << "█";
                cout << "\n";
            }
        }
    }
}

void Orderbook::print()
{
    cout << "========== Orderbook =========" << "\n";
    printLeg(asks, BookSide::ask);

    double best_ask = bestQuote(BookSide::ask);
    double best_bid = bestQuote(BookSide::bid);

    cout << "\n\033[1;33m" << "======  " << 10000 * (best_ask - best_bid) / best_bid << "bps  ======\033[0m\n\n";

    printLeg(bids, BookSide::bid);
    cout << "==============================\n\n\n";
}
