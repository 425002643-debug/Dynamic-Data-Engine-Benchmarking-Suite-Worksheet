#include "SearchSortEngine.h"

SearchSortEngine::SearchSortEngine() : comparisonCount(0), swapCount(0) {}

void SearchSortEngine::resetCounters() {
    comparisonCount = 0;
    swapCount = 0;
}

long long SearchSortEngine::getComparisonCount() const {
    return comparisonCount;
}

long long SearchSortEngine::getSwapCount() const {
    return swapCount;
}

// SEARCH ALGORITHMS
int SearchSortEngine::linearSearch(const std::vector<int>& arr, int target) const {
    for (size_t i = 0; i < arr.size(); ++i) {
        comparisonCount++;
        if (arr[i] == target) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int SearchSortEngine::binarySearch(const std::vector<int>& arr, int target) const {
    int left = 0;
    int right = static_cast<int>(arr.size()) - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        comparisonCount++;
        if (arr[mid] == target) {
            return mid;
        }
        comparisonCount++;
        if (arr[mid] < target) {
            left = mid + 1;

        } else {
            right = mid - 1;
        }
    }
    return -1;
}

// SORT ALGORITHMS
void SearchSortEngine::bubbleSort(std::vector<int>& arr) {
    int n = static_cast<int>(arr.size());
    bool swapped;
    for (int i = 0; i < n - 1; ++i) {
        swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            comparisonCount++;
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapCount++;
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void SearchSortEngine::selectionSort(std::vector<int>& arr) {
    int n = static_cast<int>(arr.size());
    for (int i = 0; i < n - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < n; ++j) {
            comparisonCount++;
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            int temp = arr[i];
            arr[i] = arr[minIndex];
            arr[minIndex] = temp;
            swapCount++;
        }
    }
}

void SearchSortEngine::insertionSort(std::vector<int>& arr) {
    int n = static_cast<int>(arr.size());
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0) {
            comparisonCount++;
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                swapCount++; // Counting shift as movement/swap operation
                j--;
            } else {
                break;
            }
        }
        arr[j + 1] = key;
    }
}
