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
            it = map.erase(it);
        else
            ++it;
    }
}

template <typename T>
pair<int, double> Orderbook::fillOrder(map<double, vector<unique_ptr<Order>>, T> &offers, const OrderType type, const Side side, int &qty, const double price, int &units, double &totalValue)
{
    for (auto it = offers.begin(); it != offers.end() && qty > 0;)
    {
        auto &price_level = it->first;
        auto &quotes = it->second;

        bool canTransact = true;
        if (type == OrderType::limit)
        {
            if (side == Side::buy && price_level > price)
                canTransact = false;
            else if (side == Side::sell && price_level < price)
                canTransact = false;
        }

        if (canTransact)
        {
            for (auto qit = quotes.begin(); qit != quotes.end() && qty > 0;)
            {
                int &curr_quote_qty = (*qit)->quantity;
                const double curr_quote_price = (*qit)->price;

                if (curr_quote_qty > qty)
                {
                    units += qty;
                    totalValue += qty * curr_quote_price;
                    curr_quote_qty -= qty;
                    qty = 0;
                    break;
                }
                else
                {
                    units += curr_quote_qty;
                    totalValue += curr_quote_qty * curr_quote_price;
                    qty -= curr_quote_qty;
                    qit = quotes.erase(qit);
                }
            }

            if (quotes.empty())
                it = offers.erase(it);
            else
                ++it;
        }
        else
        {
            ++it;
        }
    }

    return make_pair(units, totalValue);
}

pair<int, double> Orderbook::handleOrder(OrderType type, int qty, Side side, double price)
{
    int units_transacted = 0;
    double total_value = 0;

    if (type == OrderType::market)
    {
        if (side == Side::sell)
        {
            return fillOrder(bids, type, side, qty, price, units_transacted, total_value);
        }
        else if (side == Side::buy)
        {
            return fillOrder(asks, type, side, qty, price, units_transacted, total_value);
        }
    }
    else if (type == OrderType::limit)
    {
        if (side == Side::buy)
        {
            if (!asks.empty() && bestQuote(BookSide::ask) <= price)
            {
                auto fill = fillOrder(asks, type, side, qty, price, units_transacted, total_value);
                if (qty > 0)
                    addOrder(qty, price, BookSide::bid);
                return fill;
            }
            else
            {
                addOrder(qty, price, BookSide::bid);
                return {0, 0};
            }
        }
        else if (side == Side::sell)
        {
            if (!bids.empty() && bestQuote(BookSide::bid) >= price)
            {
                auto fill = fillOrder(bids, type, side, qty, price, units_transacted, total_value);
                if (qty > 0)
                    addOrder(qty, price, BookSide::ask);
                return fill;
            }
            else
            {
                addOrder(qty, price, BookSide::ask);
                return {0, 0};
            }
        }
    }

    throw runtime_error("Invalid order type encountered");
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
        return asks.begin()->first;
    }

    return 0.0;
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

            string color = "31";
            cout << "\t\033[1;" << color << "m" << "$" << setw(6) << fixed << setprecision(2) << pair.first << setw(5) << size_sum << "\033[0m ";

            for (int i = 0; i < size_sum / 10; i++)
                cout << "█";
            cout << "\n";
        }
    }
    else if (side == BookSide::bid)
    {
        for (auto pair = hashmap.rbegin(); pair != hashmap.rend(); ++pair)
        {
            int size_sum = 0;
            for (const auto &order : pair->second)
                size_sum += order->quantity;

            string color = "32";
            cout << "\t\033[1;" << color << "m" << "$" << setw(6) << fixed << setprecision(2) << pair->first << setw(5) << size_sum << "\033[0m ";

            for (int i = 0; i < size_sum / 10; i++)
                cout << "█";
            cout << "\n";
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