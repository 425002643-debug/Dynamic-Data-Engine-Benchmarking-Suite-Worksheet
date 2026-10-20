#include "LinkedListEngine.h"
#include <iostream>
#include <iomanip>

LinkedListEngine::LinkedListEngine() : head(nullptr), tail(nullptr), count(0) {}
LinkedListEngine::~LinkedListEngine() { clear(); }

bool LinkedListEngine::insertHead(const Book& book) {
    if (book.isbn.empty() || findByIsbn(book.isbn)) return false;
    Node* node = new Node(book);
    node ->next = head;
        if (head != nullptr) {
            head->prev = node;
        }else{
            tail = node;
        }
        head = node;
        ++count;
    return true;
}

bool LinkedListEngine::insertTail(const Book& book) {
    if (book.isbn.empty() || findByIsbn(book.isbn)) return false;
        Node* node = new Node(book);
        node->prev = tail;
            if (tail != nullptr) {
                tail->next = node;
            }else{
                head = node;
            }
            tail = node;
            ++count;
    return true;
}

Book* LinkedListEngine::findByIsbn(const std::string& isbn) {
    for (Node* current = head; current != nullptr; current = current->next)
        if (current->data.isbn == isbn) return &current->data;
        return nullptr;
}

const Book* LinkedListEngine::findByIsbn(const std::string& isbn) const {
    for (const Node* current = head; current != nullptr; current = current->next)
        if (current->data.isbn == isbn) return &current->data;
    return nullptr;
}

bool LinkedListEngine::deleteByIsbn(const std::string& isbn) {
    Node* current = head;
        while (current != nullptr && current->data.isbn != isbn) {
                current = current->next;
}
    if (current == nullptr) {
        return false;
    }
    if (current->prev != nullptr) {
        current->prev->next = current->next;
    } else {
        head = current->next;
    }
    if (current->next != nullptr) {
        current->next->prev = current->prev;
    } else {
        tail = current->prev;
    }
    delete current;
        current = nullptr;
        --count;
    return true;
}

bool LinkedListEngine::updateBook(const std::string& isbn, const Book& replacement) {
    Book* existing = findByIsbn(isbn);
        if (!existing) return false;
            if (replacement.isbn != isbn) return false;
                *existing = replacement; // Changes record fields, not the Node's links.
    return true;
}

static void printBook(const Book& b) {
    std::cout << std::left << std::setw(17) << b.isbn
    << std::setw(26) << b.title.substr(0, 25)
    << std::setw(22) << b.author.substr(0, 21)
    << std::setw(18) << b.category.substr(0, 17)

    << std::setw(12) << (b.isBorrowed ? "Borrowed" : "Available")
    << std::setw(17) << b.patronId << b.dueDate << '\n';
}

void LinkedListEngine::displayForward() const {
    for (const Node* p = head; p; p = p->next) printBook(p->data);
}

void LinkedListEngine::displayBackward() const {
    for (const Node* p = tail; p; p = p->prev) printBook(p->data);
}

std::size_t LinkedListEngine::size() const { return count; }
    bool LinkedListEngine::empty() const { return head == nullptr; }

void LinkedListEngine::clear() {
    Node* p = head;
        while (p) {
            Node* next = p->next;
            delete p;
            p = next;
        }
    head = tail = nullptr;
    count = 0;
}
