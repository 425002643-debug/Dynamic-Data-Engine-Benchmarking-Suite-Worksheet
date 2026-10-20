#ifndef AUDIT_STACK_MODULE_H
#define AUDIT_STACK_MODULE_H

#include <iostream>
#include <string>
#include <stack>
#include "EntityModel.h"

enum class ActionType {
    STATUS_UPDATE,
    RECORD_DELETION,
    BOOK_INSERTION
};

struct AuditLog {
    ActionType action;
    std::string isbn;
    Book previousState;
    Book newState;
    std::string timestamp;
};

class AuditStackModule {
private:
    std::stack<AuditLog> auditStack;

public:
    void push(ActionType action, const std::string& isbn, const Book& prevBook, const Book& newBook, const std::string& timestamp);
    bool pop(Book& bookToRestore, ActionType& revertedAction);
    void peek() const;
    void display() const;
    bool isEmpty() const;
};

#endif // AUDIT_STACK_MODULE_H
