# Library Management System

A console-based Library Management System built in C++ that manages books, members, and borrowing (issue/return) records, with search functionality by title or author.

## Features

- **Register Member** — add a new library member (Member ID, Name)
- **Add Book** — add a new book (Book ID, Title, Author)
- **Issue Book** — issue an available book to a registered member; blocks issuing an already-issued book or issuing to an unregistered member
- **Return Book** — return an issued book, making it available again
- **Search Books** — case-insensitive search by title or author keyword
- **Display All Books** — view all books with their current status (Available/Issued)
- **Persistent Storage** — books and members are saved to `books.txt` and `members.txt`

## Data Structures

Built using C++ `struct`:

```cpp
struct Book {
    int bookId;
    string title;
    string author;
    bool available;
    int issuedTo;   // member ID currently holding the book, 0 if available
};

struct Member {
    int memberId;
    string name;
};
```

Each book's issue status is tracked directly on the book record (`available` + `issuedTo`), which doubles as the borrowing record — avoiding a separate transaction log while still tracking who currently holds each book.

## How It Works

- `books.txt`: one line per book — `bookId,title,author,available,issuedTo`
- `members.txt`: one line per member — `memberId,name`

Issue/Return operations locate the book by ID, update its status, and rewrite the file (via a temp file).

## How to Compile and Run

```bash
g++ -o library main.cpp
./library
```

On Windows (with g++ installed):

```bash
g++ -o library.exe main.cpp
library.exe
```

## Menu Options

```
1. Register Member
2. Add Book
3. Issue Book
4. Return Book
5. Search Books (Title/Author)
6. Display All Books
7. Exit
```

## Files

- `main.cpp` — source code
- `books.txt`, `members.txt` — data files (auto-created at runtime; not included in the repo)

## Tech Used

- C++ (Structures)
- Standard Library: `<fstream>`, `<sstream>`, `<string>`, `<algorithm>`
- File handling for persistent storage
