#include "QueueEngine.h"
#include <iostream>

QueueEngine::QueueEngine()
    : fifoHead(nullptr), fifoTail(nullptr), fifoSize(0),
      priorityHead(nullptr), prioritySize(0) {}

QueueEngine::~QueueEngine() {
    clearAll();
}

void QueueEngine::clearFIFO() {
    while (fifoHead != nullptr) {
        QueueNode<HoldRecord>* temp = fifoHead;
        fifoHead = fifoHead->next;
        delete temp;
    }
    fifoTail = nullptr;
    fifoSize = 0;
}

void QueueEngine::clearPriority() {
    while (priorityHead != nullptr) {
        QueueNode<HoldRecord>* temp = priorityHead;
        priorityHead = priorityHead->next;
        delete temp;
    }
    prioritySize = 0;
}

void QueueEngine::clearAll() {
    clearFIFO();
    clearPriority();
}

// Standard FIFO Hold Queue Implementation
void QueueEngine::enqueueFIFO(int resId, const std::string& isbn, const std::string& title, const std::string& patronId, int priority) {
    HoldRecord record{resId, isbn, title, patronId, priority};
    QueueNode<HoldRecord>* newNode = new QueueNode<HoldRecord>(record);

    if (fifoTail == nullptr) {
        fifoHead = fifoTail = newNode;
    } else {
        fifoTail->next = newNode;
        fifoTail = newNode;
    }
    fifoSize++;
}

bool QueueEngine::dequeueFIFO(HoldRecord& outRecord) {
    if (fifoHead == nullptr) {
        return false;
    }
    QueueNode<HoldRecord>* temp = fifoHead;
    outRecord = temp->data;
    fifoHead = fifoHead->next;

    if (fifoHead == nullptr) {
        fifoTail = nullptr; // Queue is now empty
    }

    delete temp;
    fifoSize--;
    return true;
}

bool QueueEngine::peekFIFO(HoldRecord& outRecord) const {
    if (fifoHead == nullptr) {
        return false;
    }
    outRecord = fifoHead->data;
    return true;
}

size_t QueueEngine::getFIFOCount() const {
    return fifoSize;
}

bool QueueEngine::isFIFOEmpty() const {
    return fifoHead == nullptr;
}

void QueueEngine::displayFIFOState() const {
    std::cout << "========== Standard FIFO Hold Queue (" << fifoSize << " waiting) ==========" << std::endl;
    QueueNode<HoldRecord>* current = fifoHead;
    int position = 1;
    while (current != nullptr) {
        std::cout << "  [" << position++ << "] Res ID: " << current->data.reservationId
                  << " | Book: " << current->data.title
                  << " (ISBN: " << current->data.isbn << ")"
                  << " | Patron: " << current->data.patronId << std::endl;
        current = current->next;
    }
    std::cout << "==========================================================" << std::endl;
}

// Priority Hold Queue Implementation
void QueueEngine::enqueuePriority(int resId, const std::string& isbn, const std::string& title, const std::string& patronId, int priorityTier) {
    HoldRecord record{resId, isbn, title, patronId, priorityTier};
    QueueNode<HoldRecord>* newNode = new QueueNode<HoldRecord>(record);

    if (priorityHead == nullptr || priorityHead->data.priorityTier < priorityTier) {
        newNode->next = priorityHead;
        priorityHead = newNode;
    } else {
        QueueNode<HoldRecord>* current = priorityHead;
        while (current->next != nullptr && current->next->data.priorityTier >= priorityTier) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    prioritySize++;
}

bool QueueEngine::dequeuePriority(HoldRecord& outRecord) {
    if (priorityHead == nullptr) {
        return false;
    }
    QueueNode<HoldRecord>* temp = priorityHead;
    outRecord = temp->data;
    priorityHead = priorityHead->next;
    delete temp;
    prioritySize--;
    return true;
}

bool QueueEngine::peekPriority(HoldRecord& outRecord) const {
    if (priorityHead == nullptr) {
        return false;
    }
    outRecord = priorityHead->data;
    return true;
}

size_t QueueEngine::getPriorityCount() const {
    return prioritySize;
}

bool QueueEngine::isPriorityEmpty() const {
    return priorityHead == nullptr;
}

void QueueEngine::displayPriorityState() const {
    std::cout << "========== Priority Hold Queue (" << prioritySize << " waiting) ==========" << std::endl;
    QueueNode<HoldRecord>* current = priorityHead;
    int position = 1;
    while (current != nullptr) {
        std::cout << "  [" << position++ << "] Tier " << current->data.priorityTier
                  << " | Res ID: " << current->data.reservationId
                  << " | Book: " << current->data.title
                  << " | Patron: " << current->data.patronId << std::endl;
        current = current->next;
    }
    std::cout << "==========================================================" << std::endl;
}
