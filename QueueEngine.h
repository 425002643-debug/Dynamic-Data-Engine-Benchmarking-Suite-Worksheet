#pragma once
#include <string>

struct HoldRecord {
    int reservationId;
    std::string isbn;
    std::string title;
    std::string patronId;
    int priorityTier; // Higher number = higher urgency/tier
};

class QueueEngine {
private:
    template <typename T>
    struct QueueNode {
        T data;
        QueueNode* next;
        QueueNode(const T& val) : data(val), next(nullptr) {}
    };

    // Standard FIFO Queue Pointers (Head & Tail for O(1) operations)
    QueueNode<HoldRecord>* fifoHead;
    QueueNode<HoldRecord>* fifoTail;
    size_t fifoSize;

    // Priority Queue Pointer (Sorted linked list by priorityTier descending)
    QueueNode<HoldRecord>* priorityHead;
    size_t prioritySize;

    void clearFIFO();
    void clearPriority();

public:
    QueueEngine();
    ~QueueEngine();

    // --- FIFO Hold Queue Operations ---
    void enqueueFIFO(int resId, const std::string& isbn, const std::string& title, const std::string& patronId, int priority = 0);
    bool dequeueFIFO(HoldRecord& outRecord);
    bool peekFIFO(HoldRecord& outRecord) const;
    size_t getFIFOCount() const;
    bool isFIFOEmpty() const;
    void displayFIFOState() const;

    // --- Priority Hold Queue Operations ---
    void enqueuePriority(int resId, const std::string& isbn, const std::string& title, const std::string& patronId, int priorityTier);
    bool dequeuePriority(HoldRecord& outRecord);
    bool peekPriority(HoldRecord& outRecord) const;
    size_t getPriorityCount() const;
    bool isPriorityEmpty() const;
    void displayPriorityState() const;

    void clearAll();
};
