#ifndef SEARCH_SORT_ENGINE_H
#define SEARCH_SORT_ENGINE_H
#include <vector>

class SearchSortEngine {
    private:
    mutable long long comparisonCount;
    mutable long long swapCount;

    public:
    SearchSortEngine();

    void resetCounters();
        long long getComparisonCount() const;
        long long getSwapCount() const;

    int linearSearch(const std::vector<int>& arr, int target) const;
    int binarySearch(const std::vector<int>& arr, int target) const;

    void bubbleSort(std::vector<int>& arr);
    void selectionSort(std::vector<int>& arr);
    void insertionSort(std::vector<int>& arr);
};
#endif
