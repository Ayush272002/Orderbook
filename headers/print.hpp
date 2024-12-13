#pragma once
#include <chrono>
#include <cstdint>
#include <string_view>
using namespace std;

inline uint64_t time()
{
    return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

void print_file(string_view path);

void print_fill(pair<int, double> fill, int quantity, uint64_t startTime, uint64_t endTime);
