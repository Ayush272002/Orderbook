#include <iostream>
#include <cassert>
#include "../headers/order.hpp"
#include "../headers/orderbook.hpp"

void testAddOrder()
{
    Orderbook orderbook(false);

    orderbook.addOrder(100, 100.50, BookSide::bid);
    orderbook.addOrder(200, 101.00, BookSide::ask);

    const auto &bids = orderbook.getBids();
    const auto &asks = orderbook.getAsks();

    assert(bids.size() == 1);
    assert(bids.at(100.50).size() == 1);
    assert(bids.at(100.50)[0]->quantity == 100);
    assert(bids.at(100.50)[0]->price == 100.50);

    assert(asks.size() == 1);
    assert(asks.at(101.00).size() == 1);
    assert(asks.at(101.00)[0]->quantity == 200);
    assert(asks.at(101.00)[0]->price == 101.00);

    std::cout << "testAddOrder passed!" << std::endl;
}

void testMarketOrderExecution()
{
    Orderbook orderbook(false);

    orderbook.addOrder(100, 100.50, BookSide::bid);
    orderbook.addOrder(150, 100.50, BookSide::bid);
    orderbook.addOrder(200, 101.00, BookSide::ask);
    orderbook.addOrder(250, 101.00, BookSide::ask);

    auto [unitsTransacted, totalValue] = orderbook.handleOrder(OrderType::market, 200, Side::sell, 0);

    const auto &bids = orderbook.getBids();

    assert(unitsTransacted == 200);
    assert(totalValue == 100.50 * 200);

    assert(bids.at(100.50)[0]->quantity == 50);

    std::cout << "testMarketOrderExecution passed!" << std::endl;
}

void testLimitOrderExecution()
{
    Orderbook orderbook(false);

    orderbook.addOrder(100, 100.50, BookSide::bid);
    orderbook.addOrder(200, 101.00, BookSide::ask);

    auto [unitsTransacted, totalValue] = orderbook.handleOrder(OrderType::limit, 150, Side::buy, 101.00);

    const auto &asks = orderbook.getAsks();

    assert(unitsTransacted == 150);
    assert(totalValue == 101.00 * 150);

    assert(asks.at(101.00)[0]->quantity == 50);

    std::cout << "testLimitOrderExecution passed!" << std::endl;
}

void testBestQuote()
{
    Orderbook orderbook(false);

    orderbook.addOrder(100, 100.50, BookSide::bid);
    orderbook.addOrder(200, 101.00, BookSide::ask);

    double bestBid = orderbook.bestQuote(BookSide::bid);
    double bestAsk = orderbook.bestQuote(BookSide::ask);

    assert(bestBid == 100.50);
    assert(bestAsk == 101.00);

    std::cout << "testBestQuote passed!" << std::endl;
}

int main()
{
    testAddOrder();
    testMarketOrderExecution();
    testLimitOrderExecution();
    testBestQuote();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
