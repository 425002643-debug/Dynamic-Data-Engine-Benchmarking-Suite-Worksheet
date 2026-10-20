#include "EntityModel.h"
#include "LinkedListEngine.h"
#include "AuditStackModule.h"
#include "QueueEngine.h"
#include "SearchSortEngine.h"
#include "BenchmarkingSuite.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

static int readInt(const std::string& prompt, int fallback = -1) {
    std::string s = readLine(prompt);
    try {
        return std::stoi(s);
    } catch (...) {
        return fallback;
    }
}

static std::string currentTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&now));
    return buf;
}

static void printCatalogHeader() {
    std::cout << std::left << std::setw(17) << "ISBN"
              << std::setw(26) << "Title"
              << std::setw(22) << "Author"
              << std::setw(18) << "Category"
              << std::setw(12) << "Status"
              << std::setw(17) << "Patron"
              << "Due Date\n"
              << std::string(125, '-') << "\n";
}

static void printCatalog(const LinkedListEngine& catalog, bool forward) {
    std::cout << "\nCatalog (" << catalog.size() << " books) - "
              << (forward ? "forward" : "backward") << "\n";
    printCatalogHeader();
    if (catalog.empty()) {
        std::cout << "(empty)\n";
        return;
    }
    if (forward) catalog.displayForward();
    else         catalog.displayBackward();
}

static void printBookRow(const Book& b) {
    printCatalogHeader();
    std::cout << std::left << std::setw(17) << b.isbn
              << std::setw(26) << b.title.substr(0, 25)
              << std::setw(22) << b.author.substr(0, 21)
              << std::setw(18) << b.category.substr(0, 17)
              << std::setw(12) << (b.isBorrowed ? "Borrowed" : "Available")
              << std::setw(17) << b.patronId << b.dueDate << "\n";
}

static Book inputNewBook() {
    Book b;
    b.isbn     = readLine("ISBN: ");
    b.title    = readLine("Title: ");
    b.author   = readLine("Author: ");
    b.category = readLine("Category: ");
    b.isBorrowed = false;
    return b;
}

static void seedCatalog(LinkedListEngine& catalog) {
    Book seed[] = {
        {"978-0131103627", "The C Programming Language", "Kernighan & Ritchie", "CS",          false, "", ""},
        {"978-0262033848", "Introduction to Algorithms",  "Cormen et al.",       "CS",          false, "", ""},
        {"978-0134685991", "Effective Modern C++",        "Scott Meyers",        "Programming", false, "", ""},
        {"978-0321573513", "Algorithms",                  "Sedgewick & Wayne",   "CS",          false, "", ""}
    };
    for (const Book& b : seed) catalog.insertTail(b);
}

// Menu: Catalog
static void catalogMenu(LinkedListEngine& catalog, AuditStackModule& audit) {
    while (true) {
        std::cout << "\n--- Master Catalog ---\n"
                  << "1. Add book at head\n"
                  << "2. Add book at tail\n"
                  << "3. Delete book by ISBN\n"
                  << "4. Borrow book\n"
                  << "5. Return book\n"
                  << "6. Find book by ISBN\n"
                  << "7. Display forward\n"
                  << "8. Display backward\n"
                  << "0. Back\n";
        int choice = readInt("Choice: ");
        if (!std::cin) return;

        if (choice == 0) return;

        if (choice == 1 || choice == 2) {
            Book b = inputNewBook();
            bool ok = (choice == 1) ? catalog.insertHead(b) : catalog.insertTail(b);
            if (ok) {
                Book marker;
                marker.isbn = b.isbn;
                audit.push(ActionType::BOOK_INSERTION, b.isbn, marker, b, currentTimestamp());
                std::cout << "Book added.\n";
            } else {
                std::cout << "Insert failed (empty/duplicate ISBN, or insert not implemented yet).\n";
            }
        } else if (choice == 3) {
            std::string isbn = readLine("ISBN to delete: ");
            Book* found = catalog.findByIsbn(isbn);
            if (!found) {
                std::cout << "No book with that ISBN.\n";
                continue;
            }
            Book snapshot = *found;
            if (catalog.deleteByIsbn(isbn)) {
                audit.push(ActionType::RECORD_DELETION, isbn, snapshot, Book{}, currentTimestamp());
                std::cout << "Book deleted.\n";
            } else {
                std::cout << "Delete failed (not implemented yet?).\n";
            }
        } else if (choice == 4 || choice == 5) {
            std::string isbn = readLine("ISBN: ");
            Book* found = catalog.findByIsbn(isbn);
            if (!found) {
                std::cout << "No book with that ISBN.\n";
                continue;
            }
            Book prev = *found;
            Book next = prev;
            if (choice == 4) {
                if (prev.isBorrowed) { std::cout << "Already borrowed.\n"; continue; }
                next.isBorrowed = true;
                next.patronId = readLine("Patron ID: ");
                next.dueDate  = readLine("Due date (YYYY-MM-DD): ");
            } else {
                if (!prev.isBorrowed) { std::cout << "Book is not borrowed.\n"; continue; }
                next.isBorrowed = false;
                next.patronId.clear();
                next.dueDate.clear();
            }
            if (catalog.updateBook(isbn, next)) {
                audit.push(ActionType::STATUS_UPDATE, isbn, prev, next, currentTimestamp());
                std::cout << (choice == 4 ? "Book borrowed.\n" : "Book returned.\n");
            }
        } else if (choice == 6) {
            std::string isbn = readLine("ISBN: ");
            const Book* found = catalog.findByIsbn(isbn);
            if (found) printBookRow(*found);
            else       std::cout << "Not found.\n";
        } else if (choice == 7) {
            printCatalog(catalog, true);
        } else if (choice == 8) {
            printCatalog(catalog, false);
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}

// Menu: Hold queues
static void holdMenu(QueueEngine& queues, int& nextReservationId) {
    while (true) {
        std::cout << "\n--- Hold / Reservation Queues ---\n"
                  << "1. Enqueue hold (FIFO)\n"
                  << "2. Enqueue hold (Priority)\n"
                  << "3. Dequeue FIFO\n"
                  << "4. Dequeue Priority\n"
                  << "5. Peek FIFO front\n"
                  << "6. Peek Priority front\n"
                  << "7. Display both queues\n"
                  << "0. Back\n";
        int choice = readInt("Choice: ");
        if (!std::cin) return;
        if (choice == 0) return;

        if (choice == 1 || choice == 2) {
            std::string isbn   = readLine("ISBN: ");
            std::string title  = readLine("Title: ");
            std::string patron = readLine("Patron ID: ");
            int resId = nextReservationId++;
            if (choice == 1) {
                queues.enqueueFIFO(resId, isbn, title, patron);
            } else {
                int tier = readInt("Priority tier (higher = more urgent): ", 0);
                queues.enqueuePriority(resId, isbn, title, patron, tier);
            }
            std::cout << "Hold queued with reservation ID " << resId << ".\n";
        } else if (choice == 3 || choice == 4) {
            HoldRecord r;
            bool ok = (choice == 3) ? queues.dequeueFIFO(r) : queues.dequeuePriority(r);
            if (ok) {
                std::cout << "Processed hold #" << r.reservationId << " - " << r.title
                          << " for patron " << r.patronId << "\n";
            } else {
                std::cout << "Queue is empty.\n";
            }
        } else if (choice == 5 || choice == 6) {
            HoldRecord r;
            bool ok = (choice == 5) ? queues.peekFIFO(r) : queues.peekPriority(r);
            if (ok) {
                std::cout << "Next: hold #" << r.reservationId << " - " << r.title
                          << " for patron " << r.patronId
                          << " (tier " << r.priorityTier << ")\n";
            } else {
                std::cout << "Queue is empty.\n";
            }
        } else if (choice == 7) {
            queues.displayFIFOState();
            queues.displayPriorityState();
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}

// Menu: Audit trail / undo
static void auditMenu(LinkedListEngine& catalog, AuditStackModule& audit) {
    while (true) {
        std::cout << "\n--- Audit Trail / Undo ---\n"
                  << "1. Peek most recent entry\n"
                  << "2. Display full trail (newest first)\n"
                  << "3. Undo last operation\n"
                  << "0. Back\n";
        int choice = readInt("Choice: ");
        if (!std::cin) return;
        if (choice == 0) return;

        if (choice == 1) {
            if (audit.isEmpty()) std::cout << "Audit trail is empty.\n";
            else audit.peek();
        } else if (choice == 2) {
            if (audit.isEmpty()) std::cout << "Audit trail is empty.\n";
            else {
                std::cout << "Timestamp | Action | ISBN\n";
                audit.display();
            }
        } else if (choice == 3) {
            Book restored;
            ActionType action;
            if (!audit.pop(restored, action)) {
                std::cout << "Nothing to undo.\n";
                continue;
            }
            switch (action) {
                case ActionType::STATUS_UPDATE:
                    if (catalog.updateBook(restored.isbn, restored))
                        std::cout << "Undo: restored status of " << restored.isbn << ".\n";
                    else
                        std::cout << "Undo failed: book no longer in catalog.\n";
                    break;
                case ActionType::RECORD_DELETION:
                    if (catalog.insertTail(restored))
                        std::cout << "Undo: re-added " << restored.isbn << " to the catalog.\n";
                    else
                        std::cout << "Undo failed: could not re-insert " << restored.isbn << ".\n";
                    break;
                case ActionType::BOOK_INSERTION:
                    if (catalog.deleteByIsbn(restored.isbn))
                        std::cout << "Undo: removed " << restored.isbn << " from the catalog.\n";
                    else
                        std::cout << "Undo failed: could not remove " << restored.isbn << ".\n";
                    break;
            }
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}

static void searchMenu() {
    SearchSortEngine engine;
    int n = readInt("Array size (default 100): ", 100);
    if (n < 1) n = 100;

    std::vector<int> arr(n);
    for (int i = 0; i < n; ++i) arr[i] = 2 * (i + 1);

    std::cout << "Array holds even numbers 2.." << arr.back() << ".\n";
    int target = readInt("Target value: ", -1);

    engine.resetCounters();
    int lin = engine.linearSearch(arr, target);
    long long linCmp = engine.getComparisonCount();

    engine.resetCounters();
    int bin = engine.binarySearch(arr, target);
    long long binCmp = engine.getComparisonCount();

    std::cout << std::left
              << std::setw(16) << "Algorithm" << std::setw(12) << "Index" << "Comparisons\n"
              << std::setw(16) << "Linear"    << std::setw(12) << lin << linCmp << "\n"
              << std::setw(16) << "Binary"    << std::setw(12) << bin << binCmp << "\n"
              << "(index -1 means not found)\n";
}

// Menu: Benchmarks
static void benchmarkMenu(BenchmarkingSuite& suite) {
    while (true) {
        std::cout << "\n--- Benchmarking Suite ---\n"
                  << "1. Quick run   (1,000 / 5,000 / 10,000 items)\n"
                  << "2. Full run    (1,000 up to 50,000 items - Bubble Sort may take a while)\n"
                  << "3. Custom size (single size)\n"
                  << "4. Show sort report\n"
                  << "5. Show search report\n"
                  << "6. Export results to CSV\n"
                  << "7. Clear results\n"
                  << "8. Show REQUIRED report format (single table, Big-O)\n"
                  << "0. Back\n";
        int choice = readInt("Choice: ");
        if (!std::cin) return;
        if (choice == 0) return;

        if (choice == 1) {
            suite.runAll({1000, 5000, 10000});
            suite.printSortResults();
            suite.printSearchResults();
        } else if (choice == 2) {
            suite.runAll(BenchmarkingSuite::defaultSizes());
            suite.printSortResults();
            suite.printSearchResults();
        } else if (choice == 3) {
            int n = readInt("Size (1 - 50000): ", -1);
            if (n < 1 || n > 50000) {
                std::cout << "Size must be between 1 and 50,000.\n";
                continue;
            }
            suite.runAll({static_cast<std::size_t>(n)});
            suite.printSortResults();
            suite.printSearchResults();
        } else if (choice == 4) {
            suite.printSortResults();
        } else if (choice == 5) {
            suite.printSearchResults();
        } else if (choice == 6) {
            if (!suite.hasResults()) {
                std::cout << "Nothing to export yet.\n";
            } else if (suite.exportCSV("benchmark")) {
                std::cout << "Wrote benchmark_sort.csv and benchmark_search.csv\n";
            } else {
                std::cout << "Could not write CSV files.\n";
            }
        } else if (choice == 8) {
            int n = readInt("Dataset size N (default 10000, max 50000): ", 10000);
            if (n < 2 || n > 50000) n = 10000;
            suite.printRequiredReport(static_cast<std::size_t>(n));
        } else if (choice == 7) {
            suite.clearResults();
            std::cout << "Results cleared.\n";
        } else {
            std::cout << "Invalid choice.\n";
        }
    }
}

// Main
int main() {
    LinkedListEngine catalog;
    AuditStackModule audit;
    QueueEngine queues;
    BenchmarkingSuite suite;
    int nextReservationId = 1001;

    seedCatalog(catalog);

    while (true) {
        std::cout << "\n=========================================\n"
                  << " Library Catalog, Reservation & Return\n"
                  << " Management Engine\n"
                  << "=========================================\n"
                  << "1. Master Catalog\n"
                  << "2. Hold / Reservation Queues\n"
                  << "3. Audit Trail / Undo\n"
                  << "4. Search Demo (Linear vs Binary)\n"
                  << "5. Benchmarking Suite\n"
                  << "0. Exit\n";
        int choice = readInt("Choice: ");
        if (!std::cin) break;

        switch (choice) {
            case 1: catalogMenu(catalog, audit);        break;
            case 2: holdMenu(queues, nextReservationId); break;
            case 3: auditMenu(catalog, audit);          break;
            case 4: searchMenu();                       break;
            case 5: benchmarkMenu(suite);               break;
            case 0: std::cout << "Goodbye.\n";          return 0;
            default: std::cout << "Invalid choice.\n";
        }
    }
    return 0;
}
