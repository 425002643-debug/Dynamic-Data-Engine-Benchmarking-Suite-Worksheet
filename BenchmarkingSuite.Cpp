#include "BenchmarkingSuite.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>

using Clock = std::chrono::steady_clock;

BenchmarkingSuite::BenchmarkingSuite(unsigned int seed) : seed(seed) {}

std::vector<size_t> BenchmarkingSuite::defaultSizes() {
    return {1000, 5000, 10000, 25000, 50000};
}

std::string BenchmarkingSuite::patternName(DataPattern pattern) {
    switch (pattern) {
        case DataPattern::RANDOM:        return "Random";
        case DataPattern::SORTED:        return "Sorted";
        case DataPattern::REVERSED:      return "Reversed";
        case DataPattern::NEARLY_SORTED: return "Nearly Sorted";
    }
    return "Unknown";
}

std::vector<int> BenchmarkingSuite::generateData(std::size_t n, DataPattern pattern) const {
    std::vector<int> data(n);
    std::iota(data.begin(), data.end(), 1);          // 1..n (unique, like ISBN-style keys)

    // Same seed + size => identical data every run, so results are reproducible.
    std::mt19937 rng(seed + static_cast<unsigned int>(n));

    switch (pattern) {
        case DataPattern::RANDOM:
            std::shuffle(data.begin(), data.end(), rng);
            break;
        case DataPattern::SORTED:
            break;
        case DataPattern::REVERSED:
            std::reverse(data.begin(), data.end());
            break;
        case DataPattern::NEARLY_SORTED: {
            if (n < 2) break;
            std::uniform_int_distribution<std::size_t> pick(0, n - 1);
            std::size_t swaps = n / 20;               // disturb ~5% of the array
            for (std::size_t i = 0; i < swaps; ++i) {
                std::swap(data[pick(rng)], data[pick(rng)]);
            }
            break;
        }
    }
    return data;
}

bool BenchmarkingSuite::isSortedAscending(const std::vector<int>& arr) {
    for (std::size_t i = 1; i < arr.size(); ++i) {
        if (arr[i - 1] > arr[i]) return false;
    }
    return true;
}

// Sort benchmarks
void BenchmarkingSuite::benchmarkSort(const std::string& name,
                                      void (SearchSortEngine::*sortFn)(std::vector<int>&),
                                      const std::vector<int>& original,
                                      DataPattern pattern) {
    std::vector<int> working = original;

    engine.resetCounters();
    auto start = Clock::now();
    (engine.*sortFn)(working);
    auto stop = Clock::now();

    SortBenchmarkResult r;
    r.algorithm = name;
    r.pattern = patternName(pattern);
    r.size = original.size();
    r.comparisons = engine.getComparisonCount();
    r.swaps = engine.getSwapCount();
    r.timeMs = std::chrono::duration<double, std::milli>(stop - start).count();
    r.verified = isSortedAscending(working);
    sortResults.push_back(r);
}

void BenchmarkingSuite::runSortBenchmarks(const std::vector<std::size_t>& sizes) {
    const DataPattern patterns[] = {
        DataPattern::RANDOM, DataPattern::SORTED,
        DataPattern::REVERSED, DataPattern::NEARLY_SORTED
    };

    for (std::size_t n : sizes) {
        for (DataPattern p : patterns) {
            std::vector<int> data = generateData(n, p);
            std::cout << "  Sorting " << n << " items (" << patternName(p) << ")..." << std::endl;

            benchmarkSort("Bubble Sort",    &SearchSortEngine::bubbleSort,    data, p);
            benchmarkSort("Selection Sort", &SearchSortEngine::selectionSort, data, p);
            benchmarkSort("Insertion Sort", &SearchSortEngine::insertionSort, data, p);
        }
    }
}

// Search benchmarks
void BenchmarkingSuite::benchmarkSearch(const std::string& scenario,
                                        const std::vector<int>& sortedArr,
                                        int target, int repetitions) {
    struct Algo {
        const char* name;
        int (SearchSortEngine::*fn)(const std::vector<int>&, int) const;
    };
    const Algo algos[] = {
        {"Linear Search", &SearchSortEngine::linearSearch},
        {"Binary Search", &SearchSortEngine::binarySearch}
    };

    for (const Algo& a : algos) {
        engine.resetCounters();
        int idx = (engine.*(a.fn))(sortedArr, target);
        long long comparisons = engine.getComparisonCount();

        volatile int sink = 0;
        auto start = Clock::now();
        for (int i = 0; i < repetitions; ++i) {
            sink = (engine.*(a.fn))(sortedArr, target);
        }
        auto stop = Clock::now();
        (void)sink;

        SearchBenchmarkResult r;
        r.algorithm = a.name;
        r.scenario = scenario;
        r.size = sortedArr.size();
        r.comparisons = comparisons;
        r.avgTimeUs = std::chrono::duration<double, std::micro>(stop - start).count() / repetitions;
        r.found = (idx != -1);
        searchResults.push_back(r);
    }
}

void BenchmarkingSuite::runSearchBenchmarks(const std::vector<std::size_t>& sizes, int repetitions) {
    if (repetitions < 1) repetitions = 1;

    for (std::size_t n : sizes) {
        if (n == 0) continue;
        std::cout << "  Searching in " << n << " items..." << std::endl;

        std::vector<int> arr(n);
        for (std::size_t i = 0; i < n; ++i) arr[i] = static_cast<int>(2 * (i + 1));

        std::mt19937 rng(seed + static_cast<unsigned int>(n));
        std::uniform_int_distribution<std::size_t> pick(0, n - 1);

        benchmarkSearch("First element",  arr, arr.front(),        repetitions);
        benchmarkSearch("Middle element", arr, arr[n / 2],         repetitions);
        benchmarkSearch("Last element",   arr, arr.back(),         repetitions);
        benchmarkSearch("Random hit",     arr, arr[pick(rng)],     repetitions);
        benchmarkSearch("Not found",      arr, arr.back() + 1,     repetitions);
    }
}

void BenchmarkingSuite::runAll(const std::vector<std::size_t>& sizes) {
    std::cout << "\n--- Running sort benchmarks ---" << std::endl;
    runSortBenchmarks(sizes);
    std::cout << "\n--- Running search benchmarks ---" << std::endl;
    runSearchBenchmarks(sizes);
    std::cout << "\nBenchmarks complete." << std::endl;
}

static std::string withCommas(long long v) {
    std::string s = std::to_string(v);
    for (int i = static_cast<int>(s.size()) - 3; i > 0; i -= 3) s.insert(i, ",");
    return s;
}

static std::string fixedStr(double v, int prec) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(prec) << v;
    return os.str();
}

static std::string cell(const std::string& text, std::size_t width) {
    return text.size() >= width ? text : text + std::string(width - text.size(), ' ');
}

void BenchmarkingSuite::printRequiredReport(std::size_t n) {
    if (n < 2) n = 10000;

    struct Row { std::string name, comps, swaps, time, bigO; };
    std::vector<Row> searchRows, sortRows;

    // ---- searches: sorted array, 1000 random targets that exist ----
    std::vector<int> sortedArr = generateData(n, DataPattern::SORTED);
    std::mt19937 rng(seed + static_cast<unsigned int>(n));
    std::uniform_int_distribution<std::size_t> pick(0, n - 1);
    std::vector<int> targets;
    for (int i = 0; i < 1000; ++i) targets.push_back(sortedArr[pick(rng)]);
    const int passes = 20;

    struct SAlgo { const char* name; int (SearchSortEngine::*fn)(const std::vector<int>&, int) const; const char* bigO; };
    const SAlgo sAlgos[] = {
        {"Linear Search", &SearchSortEngine::linearSearch, "O(n)"},
        {"Binary Search", &SearchSortEngine::binarySearch, "O(log n)"}
    };
    for (const SAlgo& a : sAlgos) {
        engine.resetCounters();
        for (int t : targets) (engine.*(a.fn))(sortedArr, t);
        long long avgComps = (engine.getComparisonCount() + static_cast<long long>(targets.size()) / 2)
                             / static_cast<long long>(targets.size());

        volatile int sink = 0;
        auto start = Clock::now();
        for (int p = 0; p < passes; ++p)
            for (int t : targets) sink = (engine.*(a.fn))(sortedArr, t);
        auto stop = Clock::now();
        (void)sink;
        double ms = std::chrono::duration<double, std::milli>(stop - start).count()
                    / (static_cast<double>(passes) * targets.size());

        searchRows.push_back({a.name, withCommas(avgComps), "N/A", fixedStr(ms, 4) + " ms", a.bigO});
    }

    // ---- sorts: same random data for each ----
    std::vector<int> randomData = generateData(n, DataPattern::RANDOM);
    struct TAlgo { const char* name; void (SearchSortEngine::*fn)(std::vector<int>&); };
    const TAlgo tAlgos[] = {
        {"Bubble Sort",    &SearchSortEngine::bubbleSort},
        {"Selection Sort", &SearchSortEngine::selectionSort},
        {"Insertion Sort", &SearchSortEngine::insertionSort}
    };
    for (const TAlgo& a : tAlgos) {
        std::vector<int> copy = randomData;
        engine.resetCounters();
        auto start = Clock::now();
        (engine.*(a.fn))(copy);
        auto stop = Clock::now();
        double ms = std::chrono::duration<double, std::milli>(stop - start).count();
        sortRows.push_back({a.name, withCommas(engine.getComparisonCount()),
                            withCommas(engine.getSwapCount()), fixedStr(ms, 3) + " ms", "O(n^2)"});
    }

    // ---- print ----
    const std::size_t wA = 15, wN = 16, wC = 12, wS = 12, wT = 17, wB = 8;
    const std::string eq(95, '=');
    const std::string sep = std::string(wA + 1, '-') + "+" + std::string(wN + 2, '-') + "+" +
                            std::string(wC + 2, '-') + "+" + std::string(wS + 2, '-') + "+" +
                            std::string(wT + 2, '-') + "+" + std::string(wB + 2, '-');

    std::cout << "\n" << eq << "\n"
              << std::string(32, ' ') << "BENCHMARK SUITE PERFORMANCE METRICS REPORT\n"
              << eq << "\n"
              << cell("Algorithm", wA) << " | " << cell("Dataset Size (N)", wN) << " | "
              << cell("Comparisons", wC) << " | " << cell("Swaps", wS) << " | "
              << cell("Time Elapsed (ms)", wT) << " | " << "Big-O" << "\n"
              << sep << "\n";

    auto printRows = [&](const std::vector<Row>& rows) {
        for (const Row& r : rows) {
            std::cout << cell(r.name, wA) << " | " << cell(withCommas(static_cast<long long>(n)), wN) << " | "
                      << cell(r.comps, wC) << " | " << cell(r.swaps, wS) << " | "
                      << cell(r.time, wT) << " | " << r.bigO << "\n";
        }
    };
    printRows(searchRows);
    std::cout << sep << "\n";
    printRows(sortRows);
    std::cout << eq << "\n";
}

void BenchmarkingSuite::printSortResults() const {
    if (sortResults.empty()) {
        std::cout << "No sort results yet. Run the benchmarks first.\n";
        return;
    }

    const std::string line(96, '=');
    std::cout << "\n" << line << "\n";
    std::cout << "SORTING BENCHMARK REPORT\n";
    std::cout << line << "\n";
    std::cout << std::left
              << std::setw(16) << "Algorithm"
              << std::setw(15) << "Pattern"
              << std::right
              << std::setw(9)  << "Size"
              << std::setw(16) << "Comparisons"
              << std::setw(16) << "Swaps/Shifts"
              << std::setw(14) << "Time (ms)"
              << std::setw(10) << "Sorted?"
              << "\n" << std::string(96, '-') << "\n";

    std::size_t lastSize = sortResults.front().size;
    for (const SortBenchmarkResult& r : sortResults) {
        if (r.size != lastSize) {
            std::cout << "\n";
            lastSize = r.size;
        }
        std::cout << std::left
                  << std::setw(16) << r.algorithm
                  << std::setw(15) << r.pattern
                  << std::right
                  << std::setw(9)  << r.size
                  << std::setw(16) << r.comparisons
                  << std::setw(16) << r.swaps
                  << std::setw(14) << std::fixed << std::setprecision(3) << r.timeMs
                  << std::setw(10) << (r.verified ? "yes" : "NO")
                  << "\n";
    }
    std::cout << line << "\n";
    std::cout << "Note: Insertion Sort 'swaps' counts element shifts, as implemented in SearchSortEngine.\n";
}

void BenchmarkingSuite::printSearchResults() const {
    if (searchResults.empty()) {
        std::cout << "No search results yet. Run the benchmarks first.\n";
        return;
    }

    const std::string line(82, '=');
    std::cout << "\n" << line << "\n";
    std::cout << "SEARCH BENCHMARK REPORT (sorted array)\n";
    std::cout << line << "\n";
    std::cout << std::left
              << std::setw(16) << "Algorithm"
              << std::setw(17) << "Scenario"
              << std::right
              << std::setw(9)  << "Size"
              << std::setw(14) << "Comparisons"
              << std::setw(16) << "Avg Time (us)"
              << std::setw(10) << "Found?"
              << "\n" << std::string(82, '-') << "\n";

    std::size_t lastSize = searchResults.front().size;
    for (const SearchBenchmarkResult& r : searchResults) {
        if (r.size != lastSize) {
            std::cout << "\n";
            lastSize = r.size;
        }
        std::cout << std::left
                  << std::setw(16) << r.algorithm
                  << std::setw(17) << r.scenario
                  << std::right
                  << std::setw(9)  << r.size
                  << std::setw(14) << r.comparisons
                  << std::setw(16) << std::fixed << std::setprecision(4) << r.avgTimeUs
                  << std::setw(10) << (r.found ? "yes" : "no")
                  << "\n";
    }
    std::cout << line << "\n";
    std::cout << "Note: Binary Search counts 2 comparisons per loop (equality check + less-than check).\n";
}

bool BenchmarkingSuite::exportCSV(const std::string& prefix) const {
    std::ofstream sortFile(prefix + "_sort.csv");
    std::ofstream searchFile(prefix + "_search.csv");
    if (!sortFile || !searchFile) return false;

    sortFile << "algorithm,pattern,size,comparisons,swaps,time_ms,verified\n";
    for (const SortBenchmarkResult& r : sortResults) {
        sortFile << r.algorithm << ',' << r.pattern << ',' << r.size << ','
                 << r.comparisons << ',' << r.swaps << ','
                 << std::fixed << std::setprecision(3) << r.timeMs << ','
                 << (r.verified ? 1 : 0) << '\n';
    }

    searchFile << "algorithm,scenario,size,comparisons,avg_time_us,found\n";
    for (const SearchBenchmarkResult& r : searchResults) {
        searchFile << r.algorithm << ',' << r.scenario << ',' << r.size << ','
                   << r.comparisons << ','
                   << std::fixed << std::setprecision(4) << r.avgTimeUs << ','
                   << (r.found ? 1 : 0) << '\n';
    }
    return true;
}

void BenchmarkingSuite::clearResults() {
    sortResults.clear();
    searchResults.clear();
}

bool BenchmarkingSuite::hasResults() const {
    return !sortResults.empty() || !searchResults.empty();
}
