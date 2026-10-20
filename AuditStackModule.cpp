#include "AuditStackModule.h"

void AuditStackModule::push(ActionType action, const std::string& isbn, const Book& prevBook, const Book& newBook, const std::string& timestamp) {
    AuditLog log;
    log.action = action;
    log.isbn = isbn;
    log.previousState = prevBook;
    log.newState = newBook;
    log.timestamp = timestamp;
    auditStack.push(log);
}

bool AuditStackModule::pop(Book& bookToRestore, ActionType& revertedAction) {
    if (auditStack.empty()) {
        return false;
    }
    AuditLog topLog = auditStack.top();
    auditStack.pop();
    bookToRestore = topLog.previousState;
    revertedAction = topLog.action;
    return true;
}

void AuditStackModule::peek() const {
    if (auditStack.empty()) {
        return;
    }
    const AuditLog& topLog = auditStack.top();
    std::cout << topLog.timestamp << " | " << topLog.isbn << " | ";
    switch (topLog.action) {
        case ActionType::STATUS_UPDATE:   std::cout << "STATUS_UPDATE\n"; break;
        case ActionType::RECORD_DELETION: std::cout << "RECORD_DELETION\n"; break;
        case ActionType::BOOK_INSERTION:  std::cout << "BOOK_INSERTION\n"; break;
    }
}

void AuditStackModule::display() const {
    if (auditStack.empty()) {
        return;
    }
    std::stack<AuditLog> tempStack = auditStack;
    while (!tempStack.empty()) {
        AuditLog log = tempStack.top();
        tempStack.pop();
        std::string actionStr;
        switch (log.action) {
            case ActionType::STATUS_UPDATE:   actionStr = "STATUS_UPDATE"; break;
            case ActionType::RECORD_DELETION: actionStr = "RECORD_DELETION"; break;
            case ActionType::BOOK_INSERTION:  actionStr = "BOOK_INSERTION"; break;
        }
        std::cout << log.timestamp << " | " << actionStr << " | " << log.isbn << "\n";
    }
}

bool AuditStackModule::isEmpty() const {
    return auditStack.empty();
}
