# Orderbook Project

This project is a basic implementation of an order book system, simulating a financial order book used in markets (stocks, commodities, or cryptocurrency). It supports both **market** and **limit** orders, allowing you to view and manage the order book through a command-line interface.

## Summary of Time Complexities
- **Adding an order**: O(log N)
- **Filling an order (market or limit order)**: O(P * Q)
- **Best quote**: O(1)
- **Printing the orderbook**: O(P * Q)

Where:
- N is the number of unique price levels in the order book (bids or asks).
- P is the number of price levels.
- Q is the number of orders per price level. 

In practice, the time complexity can be highly dependent on the density of orders (Q) at each price level and the number of price levels (P) in the order book.

## Features

- **Market Orders**: Execute orders immediately at the best available price.
- **Limit Orders**: Place orders to buy or sell at a specified price.
- **Bid and Ask Sides**: Track both buy (bid) and sell (ask) orders.
- **Orderbook Visualization**: View the current state of the order book, showing best bid and ask prices.
- **Order Matching**: Market orders match with the best available prices, while limit orders remain in the order book until filled or canceled.

## Files and Structure

- **`main.cpp`**: The main program that handles user interaction, including submitting orders and displaying the order book.
- **`orderbook.cpp`**: Implements the core functionality of the order book, including order handling, matching, and printing.
- **`order.hpp`**: Defines the `Order` struct, which represents individual orders.
- **`orderbook.hpp`**: Defines the `Orderbook` class that manages the order book and order matching.
- **`print.hpp`**: Contains functions for printing the order book and transaction details (e.g., filled orders).
- **`test.cpp`**: Contains unit tests that verify the order book’s functionality.
- **`Makefile`**: A build automation file for compiling the project and running tests.

## How to Build

1. **Clone the repository**:

   ```bash
   git clone https://github.com/Ayush272002/Orderbook.git
   cd Orderbook
   ```

2. **Install dependencies (if needed)**:
   This project requires a C++17 compatible compiler (e.g., `g++`).

3. **Build the project**:
   To compile the main program and tests, simply run:

   ```bash
   make
   ```

4. **Run the main program**:
   After building, you can run the program using:

   ```bash
   make run
   ```

5. **Run the tests**:
   To run the unit tests and verify the order book functionality, use:
   ```bash
   make run_tests
   ```

## How to Use the Program

### Order Submission

Once the program is running, you will be presented with a menu to interact with the order book:

1. **Print Orderbook**: Displays the current state of the order book, including the best bid and ask prices, and their corresponding quantities.
2. **Submit Order**: Allows you to submit a new order (either market or limit), specifying quantity, price, and side (buy or sell).

#### Example Interaction:

```plaintext
Options
————————————————————
|1. Print Orderbook|
|2. Submit Order   |
————————————————————
Choice: 2

Enter order type:
0. Market order
1. Limit order
Selection: 1

Enter side:
0. Buy
1. Sell
Selection: 0

Enter order quantity: 100
Enter limit price: 101.50
Submitting limit buy order for 100 units @ $101.50..
```

### Order Matching

- **Market Orders**: The system will attempt to fill market orders immediately at the best available price from the order book.
- **Limit Orders**: If the order cannot be matched immediately, it will be placed in the order book and will remain there until a matching order is found or the order is canceled.

### Order Fill Example:

When a limit order is filled, the system will output the following:

```plaintext
Filled 100/100 units @ $101.50 average price. Time taken: 230 nano seconds
```

### Print Orderbook

To view the current order book, you can select the **Print Orderbook** option from the menu, which will display the bid (buy) and ask (sell) prices, as well as the quantities for each price level.

## Unit Tests

This project includes unit tests to verify key functionalities of the order book system. The tests are written in C++ using **Google Test (gtest)**.

To execute the tests:

```bash
make run_tests
```

The tests include:

- **`testAddOrder`**: Verifies that orders are correctly added to the order book.
- **`testMarketOrderExecution`**: Ensures that market orders are filled at the best available price.
- **`testLimitOrderExecution`**: Tests that limit orders are executed only when a matching order is available.
- **`testBestQuote`**: Ensures that the best bid and ask prices are correctly retrieved.
- **`testEmptyOrderbookBestQuote`**: Ensures that an empty order book returns zero as the best quote.
- **`testMarketOrderWithoutLiquidity`**: Tests that a market order without available liquidity does not execute.
- **`testLargeMarketOrder`**: Ensures that large market orders only fill up to the available liquidity.
- **`testLimitOrderWithNoImmediateMatch`**: Tests that limit orders are correctly placed without immediate execution if no match exists.

## Future Improvements

- **Order Cancellations**: Add functionality for canceling orders from the order book.
- **Advanced Order Types**: Implement additional order types (e.g., Stop-Loss, Take-Profit).
- **Performance Optimizations**: Optimize the order book for larger volumes of orders, improving speed and efficiency.
- **Web Interface**: Create a web-based interface to provide a more user-friendly way to interact with the system.

## Contributing

1. Fork this repository.
2. Create your feature branch (`git checkout -b feature-name`).
3. Commit your changes (`git commit -am 'Add new feature'`).
4. Push to the branch (`git push origin feature-name`).
5. Open a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
