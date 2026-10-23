** Library Catalog, Reservation & Return Management Engine **
--------------------------------------------------------------------
Scenario 3: Smart Library Transaction & Archival System

** Table of Contents **
- C++ Source Code
- System Technical Documentation
- Pointer Structural Diagram / Program Flowchart
- System Application Screenshots 
- Benchmark Results Log & Charts 
- Test-Case Results Log
- Bug & Debugging Log
- Completed Individual Contribution Matrix 
  
** Problem Statement **: Many School and community libraries still stack books, reserve, borrow, and return using manual logbooks or spreadsheets. As the collection and the number of borrowers grow, searching for books, checking their availability, handling reservation queues, and computing overdue returns become slow and error-prone. Records of past transactions pile up with no structured archive, so old data either clutters the active system or gets lost. A library needs a system that processes transactions quickly, keeps them in order, and moves old records to an archive without slowing daily operations. 

** Intended User **
- Librarians
- Students/Faculty/Borrowers
- Library Administrators

** General Objective **: To design and implement a C++ library transaction engine that uses appropriate data structures to manage catalog records, reservations, returns, and archival efficiently, and to evaluate its performance through benchmarking as data volume increases.

** Specific Objectives **
1. Catalog (linked list):  Add, delete, search, update, and display book records, with duplicate-ISBN checking and no memory leaks.
2. Stack: The returned-book drop stack and undo/audit trail.
3. Queues:  FIFO hold queue (plus the priority hold queue if you build it).
4. Search and Sort: Linear and binary search, plus bubble, selection and insertion sort, with comparison swap counters.
5. Benchmarking and CLI: The benchmarking suite with timing and report table, the menu-driven interface. 

** Features **
- Catalog Management: Add, update, delete, search, and display book records using a linked list, with duplicate-ISBN checking and no memory leaks.
- Return Handling: A stack manages returned books (LIFO) and supports undo and an audit trail of recent actions.
- Reservation Queue: A FIFO hold queue serves borrowers first-come, first-served, with an optional priority queue.
- Search & Sort: Linear and binary search, plus bubble, selection, and insertion sort, each tracking comparisons and swaps.
- Benchmarking Suite: Measures execution time and shows the results in a report table.
- Menu-Driven CLI: An easy-to-use command-line interface that ties every module together.

-------------------------------------------------------------------------------
** Team **
-------------------------------------------------------------------------------
| Member | Role |
|---|---|
| Serdenia, Sheayla Rihanna A. | Linked List & Catalog Developer |
| Cunanan, Jhoey | Stack & Undo/Audit Developer |
| Mier, Ronamae | Queue & Priority Queue Developer |
| De Pedro, Precious Mae M. | Search & Sort Developer |
| Quintela, Nicole M. | Benchmarking & CLI Developer |
