#include "../headers/print.hpp"
#include <string_view>
#include <fstream>
#include <iostream>
using namespace std;

void print_file(string_view f)
{
    ifstream file(f.data());

    if (!file)
        cerr << "Unable to open the file.\n";

    if (file.is_open())
    {
        cout << file.rdbuf();
        cout << endl;
    }

    file.close();
}

void print_fill(pair<int, double> fill, int quantity, uint64_t startTime, uint64_t endTime)
{
    cout << "\033[33mFilled " << fill.first << "/" << quantity << " units @ $" << fill.second / fill.first << " average price. Time taken: " << (endTime - startTime) << " nano seconds\033[0m" << "\n";
}