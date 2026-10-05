#include "../src/LinkedList.h"
#include "../src/LinkedList_Pooled.h"
#include "../src/NodePool.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <string>

using Clock = std::chrono::high_resolution_clock;
using Micros = std::chrono::microseconds;

template <typename F>
long long timeOnce(F fn) {
    auto start = Clock::now();
    fn();
    auto end = Clock::now();
    return std::chrono::duration_cast<Micros>(end - start).count();
}

struct Stats {
    double mean;
    double median;
    long long min;
    long long max;
};

Stats summarize(const std::vector<long long>& samples) {
    std::vector<long long> sorted = samples;
    std::sort(sorted.begin(), sorted.end());

    double sum = std::accumulate(sorted.begin(), sorted.end(), 0LL);
    double mean = sum / sorted.size();
    double median = sorted[sorted.size() / 2];

    return { mean, median, sorted.front(), sorted.back() };
}

void printRow(const std::string& phase, const std::string& variant, const Stats& s) {
    std::cout << std::left  << std::setw(10) << phase
              << std::left  << std::setw(12) << variant
              << std::right << std::setw(14) << std::fixed << std::setprecision(2) << s.mean
              << std::right << std::setw(14) << s.median
              << std::right << std::setw(12) << s.min
              << std::right << std::setw(12) << s.max
              << "\n";
}

void Run() {
    constexpr int N = 100'000;
    constexpr int TRIALS = 10;

    std::vector<long long> npInsert, npDelete;
    std::vector<long long> ppInsert, ppDelete;

    // Warm-up run (results discarded)
    {
        LinkedList l;
        for (int i = 0; i < N; i++) l.insertFront(i);
        NodePool pool(N);
        LinkedList_Pooled lp(pool);
        for (int i = 0; i < N; i++) lp.insertFront(i);
    }

    for (int t = 0; t < TRIALS; t++) {
        // Non-pooled
        {
            LinkedList l;
            npInsert.push_back(timeOnce([&]{
                for (int i = 0; i < N; i++) l.insertFront(i);
            }));
            npDelete.push_back(timeOnce([&]{
                for (int i = 0; i < N; i++) l.deleteValue(i);
            }));
        }

        // Pooled — fresh pool per trial
        {
            NodePool pool(N);
            LinkedList_Pooled lp(pool);
            ppInsert.push_back(timeOnce([&]{
                for (int i = 0; i < N; i++) lp.insertFront(i);
            }));
            ppDelete.push_back(timeOnce([&]{
                for (int i = 0; i < N; i++) lp.deleteValue(i);
            }));
        }
    }

    auto npIns = summarize(npInsert);
    auto npDel = summarize(npDelete);
    auto ppIns = summarize(ppInsert);
    auto ppDel = summarize(ppDelete);

    std::cout << "=== Benchmark: " << N << " ops per phase, "
              << TRIALS << " trials ===\n\n";

    std::cout << std::left  << std::setw(10) << "Phase"
              << std::left  << std::setw(12) << "Variant"
              << std::right << std::setw(14) << "Mean (us)"
              << std::right << std::setw(14) << "Median (us)"
              << std::right << std::setw(12) << "Min (us)"
              << std::right << std::setw(12) << "Max (us)" << "\n";
    std::cout << std::string(74, '-') << "\n";

    printRow("Insert", "new/delete", npIns);
    printRow("",       "pooled",     ppIns);
    printRow("Delete", "new/delete", npDel);
    printRow("",       "pooled",     ppDel);

    std::cout << "\n" << std::fixed << std::setprecision(2);
    std::cout << "Insert speedup: " << (npIns.mean / ppIns.mean) << "x\n";
    std::cout << "Delete speedup: " << (npDel.mean / ppDel.mean) << "x\n";
    std::cout << "Overall (sum):  "
              << ((npIns.mean + npDel.mean) / (ppIns.mean + ppDel.mean)) << "x\n";
}

// int main() {
//     Run();
//     return 0;
// }

// to build:
// comment main.cpp's int main()
// uncomment this file's int main
// g++ -std=c++17 -O2 bench/benchmark.cpp src/LinkedList.cpp src/LinkedList_Pooled.cpp src/NodePool.cpp -I src -o benchmark.exe

// to run:
// .\benchmark.exe