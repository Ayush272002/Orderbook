#pragma once
#include <vector>
#include <map>
#include <memory>
#include "order.hpp"
using namespace std;

class Orderbook
{
private:
    map<double, vector<unique_ptr<Order>>, less<double>> bids;
    map<double, vector<unique_ptr<Order>>, greater<double>> asks;

public:
    Orderbook(bool gen);

    void addOrder(int q, double price, BookSide side);

    template <typename T>
    void cleanLeg(map<double, vector<unique_ptr<Order>>, T> &map);

    void removeEmptyKeys();

    pair<int, double> handleOrder(OrderType type, int qty, Side side, double price = 0);

    template <typename T>
    pair<int, double> fillOrder(map<double, vector<unique_ptr<Order>>, T> &offers, const OrderType type, const Side side, int &qty, const double price, int &units, double &totalValue);

    double bestQuote(BookSide side);

    const map<double, vector<unique_ptr<Order>>, less<double>> &getBids() { return bids; }
    const map<double, vector<unique_ptr<Order>>, greater<double>> &getAsks() { return asks; }

    template <typename T>
    void printLeg(map<double, vector<unique_ptr<Order>>, T> &hashmap, BookSide side);

    void print();
};