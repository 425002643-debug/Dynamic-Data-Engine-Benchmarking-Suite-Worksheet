#ifndef BENCHMARKING_SUITE_H
#define BENCHMARKING_SUITE_H

#include "SearchSortEngine.h"
#include <cstddef>
#include <string>
#include <vector>

enum class DataPattern {
    RANDOM,
    SORTED,
    REVERSED,
    NEARLY_SORTED
};

struct SortBenchmarkResult {
    std::string algorithm;
    std::string pattern;
    std::size_t size;
    long long comparisons;
    long long swaps;
    double timeMs;
    bool verified;
};

struct SearchBenchmarkResult {
    std::string algorithm;
    std::string scenario;
    std::size_t size;
    long long comparisons;
    double avgTimeUs;
    bool found;
};

class BenchmarkingSuite {
private:
    SearchSortEngine engine;
    unsigned int seed;
    std::vector<SortBenchmarkResult> sortResults;
    std::vector<SearchBenchmarkResult> searchResults;

    std::vector<int> generateData(std::size_t n, DataPattern pattern) const;
    static std::string patternName(DataPattern pattern);
    static bool isSortedAscending(const std::vector<int>& arr);

    void benchmarkSort(const std::string& name,
                       void (SearchSortEngine::*sortFn)(std::vector<int>&),
                       const std::vector<int>& original,
                       DataPattern pattern);

    void benchmarkSearch(const std::string& scenario,
                         const std::vector<int>& sortedArr,
                         int target, int repetitions);

public:
    explicit BenchmarkingSuite(unsigned int seed = 42);

    static std::vector<size_t> defaultSizes();

    void runSortBenchmarks(const std::vector<std::size_t>& sizes);
    void runSearchBenchmarks(const std::vector<std::size_t>& sizes,int repetitions = 1000);
    void runAll(const std::vector<std::size_t>& sizes);
    void printRequiredReport(std::size_t n = 10000);

    void printSortResults() const;
    void printSearchResults() const;

    bool exportCSV(const std::string& prefix) const;

    void clearResults();
    bool hasResults() const;
};
#endif
