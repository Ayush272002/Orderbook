#include <iostream>
#include "../headers/print.hpp"
#include "../headers/orderbook.hpp"
using namespace std;

int main()
{
    print_file("./font/orderbook.txt");

    Orderbook ob(true);

    while (true)
    {
        int action;
        cout << "Options\n————————————————————\n|1. Print Orderbook|\n|2. Submit order   |\n ————————————————————\nChoice: ";
        cin >> action;
        cout << "\n";

        if (action == 1)
        {
            ob.print();
        }
        else if (action == 2)
        {
            int order_type_input;
            int quantity;
            int side_input;
            double price;

            cout << "Enter order type:\n0. Market order\n1. Limit order\nSelection: ";
            cin >> order_type_input;
            OrderType order_type = static_cast<OrderType>(order_type_input);

            cout << "\nEnter side:\n0. Buy\n1. Sell\nSelection: ";
            cin >> side_input;
            Side side = static_cast<Side>(side_input);

            cout << "\nEnter order quantity: ";
            cin >> quantity;

            if (order_type == OrderType::market)
            {
                cout << "\nSubmitting market " << ((side == Side::buy) ? "buy" : "sell")
                     << " order for " << quantity << " units.." << "\n";

                uint64_t start_time = time();
                std::pair<int, double> fill = ob.handleOrder(order_type, quantity, side);
                uint64_t end_time = time();

                print_fill(fill, quantity, start_time, end_time);
            }
            else if (order_type == OrderType::limit)
            {
                cout << "\nEnter limit price: ";
                cin >> price;

                cout << "\nSubmitting limit " << ((side == Side::buy) ? "buy" : "sell")
                     << " order for " << quantity << " units @ $" << price << ".." << "\n";

                uint64_t start_time = time();
                std::pair<int, double> fill = ob.handleOrder(order_type, quantity, side, price);
                uint64_t end_time = time();

                print_fill(fill, quantity, start_time, end_time);
            }
            cout << "\n";
        }
    }

    return 0;
}