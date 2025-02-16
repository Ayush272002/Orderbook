#include <gtest/gtest.h>
#include "../headers/order.hpp"
#include "../headers/orderbook.hpp"

class OrderbookTest : public ::testing::Test
{
protected:
    Orderbook orderbook = Orderbook(false);
};

TEST_F(OrderbookTest, AddOrder)
{
    orderbook.addOrder(100, 100.50, BookSide::bid);
    orderbook.addOrder(200, 101.00, BookSide::ask);

    const auto &bids = orderbook.getBids();
    const auto &asks = orderbook.getAsks();

    ASSERT_EQ(bids.size(), 1);
    ASSERT_EQ(bids.at(100.50).size(), 1);
    ASSERT_EQ(bids.at(100.50)[0]->quantity, 100);
    ASSERT_EQ(bids.at(100.50)[0]->price, 100.50);

    ASSERT_EQ(asks.size(), 1);
    ASSERT_EQ(asks.at(101.00).size(), 1);
    ASSERT_EQ(asks.at(101.00)[0]->quantity, 200);
    ASSERT_EQ(asks.at(101.00)[0]->price, 101.00);
}

TEST_F(OrderbookTest, MarketOrderExecution)
{
    orderbook.addOrder(100, 100.50, BookSide::bid);
    orderbook.addOrder(150, 100.50, BookSide::bid);
    orderbook.addOrder(200, 101.00, BookSide::ask);
    orderbook.addOrder(250, 101.00, BookSide::ask);

    auto [unitsTransacted, totalValue] = orderbook.handleOrder(OrderType::market, 200, Side::sell, 0);

    const auto &bids = orderbook.getBids();

    ASSERT_EQ(unitsTransacted, 200);
    ASSERT_DOUBLE_EQ(totalValue, 100.50 * 200);
    ASSERT_EQ(bids.at(100.50)[0]->quantity, 50);
}

TEST_F(OrderbookTest, LimitOrderExecution)
{
    orderbook.addOrder(100, 100.50, BookSide::bid);
    orderbook.addOrder(200, 101.00, BookSide::ask);

    auto [unitsTransacted, totalValue] = orderbook.handleOrder(OrderType::limit, 150, Side::buy, 101.00);

    const auto &asks = orderbook.getAsks();

    ASSERT_EQ(unitsTransacted, 150);
    ASSERT_DOUBLE_EQ(totalValue, 101.00 * 150);
    ASSERT_EQ(asks.at(101.00)[0]->quantity, 50);
}

TEST_F(OrderbookTest, BestQuote)
{
    orderbook.addOrder(100, 100.50, BookSide::bid);
    orderbook.addOrder(200, 101.00, BookSide::ask);

    double bestBid = orderbook.bestQuote(BookSide::bid);
    double bestAsk = orderbook.bestQuote(BookSide::ask);

    ASSERT_DOUBLE_EQ(bestBid, 100.50);
    ASSERT_DOUBLE_EQ(bestAsk, 101.00);
}

TEST_F(OrderbookTest, EmptyOrderbookBestQuote)
{
    ASSERT_DOUBLE_EQ(orderbook.bestQuote(BookSide::bid), 0.0);
    ASSERT_DOUBLE_EQ(orderbook.bestQuote(BookSide::ask), 0.0);
}

TEST_F(OrderbookTest, MarketOrderWithoutLiquidity)
{
    auto [unitsTransacted, totalValue] = orderbook.handleOrder(OrderType::market, 100, Side::buy, 0);
    ASSERT_EQ(unitsTransacted, 0);
    ASSERT_DOUBLE_EQ(totalValue, 0.0);
}

TEST_F(OrderbookTest, LargeMarketOrder)
{
    orderbook.addOrder(50, 100.50, BookSide::bid);
    orderbook.addOrder(50, 100.50, BookSide::bid);
    orderbook.addOrder(100, 101.00, BookSide::ask);

    auto [unitsTransacted, totalValue] = orderbook.handleOrder(OrderType::market, 200, Side::sell, 0);

    ASSERT_EQ(unitsTransacted, 100);
    ASSERT_DOUBLE_EQ(totalValue, 100 * 100.50);
}

TEST_F(OrderbookTest, LimitOrderWithNoImmediateMatch)
{
    orderbook.addOrder(100, 99.50, BookSide::bid);
    orderbook.addOrder(100, 101.50, BookSide::ask);

    auto [unitsTransacted, totalValue] = orderbook.handleOrder(OrderType::limit, 50, Side::buy, 100.00);
    ASSERT_EQ(unitsTransacted, 0);
    ASSERT_DOUBLE_EQ(totalValue, 0.0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
