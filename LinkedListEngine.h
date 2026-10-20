#ifndef LINKED_LIST_ENGINE_H
#define LINKED_LIST_ENGINE_H
#include "EntityModel.h"
#include <cstddef>
class LinkedListEngine {
private:
struct Node {
Book data;
Node* prev;
Node* next;
explicit Node(const Book& book) : data(book), prev(nullptr), next(nullptr) {}
};
Node* head;
Node* tail;
std::size_t count;
public:
LinkedListEngine();
~LinkedListEngine();
LinkedListEngine(const LinkedListEngine&) = delete;
LinkedListEngine& operator=(const LinkedListEngine&) = delete;
bool insertHead(const Book& book);
bool insertTail(const Book& book);
bool deleteByIsbn(const std::string& isbn);
Book* findByIsbn(const std::string& isbn);
const Book* findByIsbn(const std::string& isbn) const;
bool updateBook(const std::string& isbn, const Book& replacement);
void displayForward() const;
void displayBackward() const;
std::size_t size() const;
bool empty() const;
void clear();
};
#endif
