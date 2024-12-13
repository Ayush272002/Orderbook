#pragma once
#include <cstdint>
#include "enum.hpp"
#include "print.hpp"

struct Order
{
    int quantity;
    BookSide side;
    double price;
    uint64_t timeStamp;

    Order(int quantity, BookSide side, double price, uint64_t timeStamp = time())
        : quantity(quantity), side(side), price(price), timeStamp(timeStamp)
    {
    }
};
