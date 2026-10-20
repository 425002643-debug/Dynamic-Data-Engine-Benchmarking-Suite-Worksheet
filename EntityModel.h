#ifndef ENTITY_MODEL_H
#define ENTITY_MODEL_H
#include <string>

struct Book {
std::string isbn;
std::string title;
std::string author;
std::string category;
bool isBorrowed = false;
std::string patronId;
std::string dueDate;
};
#endif
