#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
using namespace std;

const string BOOKS_FILE = "books.txt";
const string MEMBERS_FILE = "members.txt";

// ===================== Structures =====================

struct Book {
    int bookId;
    string title;
    string author;
    bool available;   // true = on shelf, false = issued
    int issuedTo;      // memberId who currently holds it (0 if available)
};

struct Member {
    int memberId;
    string name;
};

// ===================== Conversion Helpers =====================

string bookToLine(const Book& b) {
    ostringstream oss;
    oss << b.bookId << "," << b.title << "," << b.author << ","
        << (b.available ? 1 : 0) << "," << b.issuedTo;
    return oss.str();
}

Book lineToBook(const string& line) {
    Book b;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); b.bookId = stoi(token);
    getline(ss, token, ','); b.title = token;
    getline(ss, token, ','); b.author = token;
    getline(ss, token, ','); b.available = (stoi(token) == 1);
    getline(ss, token, ','); b.issuedTo = stoi(token);

    return b;
}

string memberToLine(const Member& m) {
    ostringstream oss;
    oss << m.memberId << "," << m.name;
    return oss.str();
}

Member lineToMember(const string& line) {
    Member m;
    stringstream ss(line);
    string token;

    getline(ss, token, ','); m.memberId = stoi(token);
    getline(ss, token, ','); m.name = token;

    return m;
}

// ===================== Lookup Helpers =====================

bool bookIdExists(int bookId) {
    ifstream file(BOOKS_FILE);
    if (!file) return false;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (lineToBook(line).bookId == bookId) return true;
    }
    return false;
}

bool memberIdExists(int memberId) {
    ifstream file(MEMBERS_FILE);
    if (!file) return false;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (lineToMember(line).memberId == memberId) return true;
    }
    return false;
}

// Finds a book by id. Returns true and fills 'result' if found.
bool findBook(int bookId, Book& result) {
    ifstream file(BOOKS_FILE);
    if (!file) return false;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        Book b = lineToBook(line);
        if (b.bookId == bookId) {
            result = b;
            return true;
        }
    }
    return false;
}

// Rewrites books.txt, replacing the book matching updated.bookId
void saveBook(const Book& updated) {
    ifstream inFile(BOOKS_FILE);
    ofstream tempFile("temp_books.txt");

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        Book b = lineToBook(line);
        if (b.bookId == updated.bookId) {
            tempFile << bookToLine(updated) << "\n";
        } else {
            tempFile << bookToLine(b) << "\n";
        }
    }

    inFile.close();
    tempFile.close();

    remove(BOOKS_FILE.c_str());
    rename("temp_books.txt", BOOKS_FILE.c_str());
}

// Lowercase helper for case-insensitive search
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// ===================== Menu Operations =====================

void registerMember() {
    int memberId;
    string name;

    cout << "\nEnter new Member ID: ";
    cin >> memberId;

    if (memberIdExists(memberId)) {
        cout << "A member with this ID already exists!\n";
        return;
    }

    cout << "Enter Member Name: ";
    cin >> name;

    Member m{memberId, name};

    ofstream file(MEMBERS_FILE, ios::app);
    file << memberToLine(m) << "\n";
    file.close();

    cout << "Member registered successfully!\n";
}

void addBook() {
    int bookId;
    string title, author;

    cout << "\nEnter new Book ID: ";
    cin >> bookId;

    if (bookIdExists(bookId)) {
        cout << "A book with this ID already exists!\n";
        return;
    }

    cin.ignore();
    cout << "Enter Title: ";
    getline(cin, title);
    cout << "Enter Author: ";
    getline(cin, author);

    Book b{bookId, title, author, true, 0};

    ofstream file(BOOKS_FILE, ios::app);
    file << bookToLine(b) << "\n";
    file.close();

    cout << "Book added successfully!\n";
}

void issueBook() {
    int bookId, memberId;

    cout << "\nEnter Book ID to issue: ";
    cin >> bookId;

    Book b;
    if (!findBook(bookId, b)) {
        cout << "No book found with this ID.\n";
        return;
    }
    if (!b.available) {
        cout << "This book is already issued.\n";
        return;
    }

    cout << "Enter Member ID: ";
    cin >> memberId;

    if (!memberIdExists(memberId)) {
        cout << "No member found with this ID. Please register the member first.\n";
        return;
    }

    b.available = false;
    b.issuedTo = memberId;
    saveBook(b);

    cout << "Book issued successfully!\n";
}

void returnBook() {
    int bookId;
    cout << "\nEnter Book ID to return: ";
    cin >> bookId;

    Book b;
    if (!findBook(bookId, b)) {
        cout << "No book found with this ID.\n";
        return;
    }
    if (b.available) {
        cout << "This book was not issued.\n";
        return;
    }

    b.available = true;
    b.issuedTo = 0;
    saveBook(b);

    cout << "Book returned successfully!\n";
}

void searchBooks() {
    string keyword;
    cin.ignore();
    cout << "\nEnter title or author keyword to search: ";
    getline(cin, keyword);
    string lowerKeyword = toLower(keyword);

    ifstream file(BOOKS_FILE);
    if (!file) {
        cout << "No books found.\n";
        return;
    }

    string line;
    bool found = false;
    cout << "\n--- Search Results ---\n";
    while (getline(file, line)) {
        if (line.empty()) continue;
        Book b = lineToBook(line);
        if (toLower(b.title).find(lowerKeyword) != string::npos ||
            toLower(b.author).find(lowerKeyword) != string::npos) {
            cout << "ID: " << b.bookId << " | Title: " << b.title
                 << " | Author: " << b.author
                 << " | Status: " << (b.available ? "Available" : "Issued") << "\n";
            found = true;
        }
    }

    if (!found) cout << "No matching books found.\n";
}

void displayAllBooks() {
    ifstream file(BOOKS_FILE);
    if (!file) {
        cout << "\nNo books found.\n";
        return;
    }

    string line;
    bool any = false;
    cout << "\n--- All Books ---\n";
    while (getline(file, line)) {
        if (line.empty()) continue;
        Book b = lineToBook(line);
        cout << "ID: " << b.bookId << " | Title: " << b.title
             << " | Author: " << b.author
             << " | Status: " << (b.available ? "Available" : "Issued") << "\n";
        any = true;
    }
    if (!any) cout << "No books found.\n";
}

// ===================== Main =====================

int main() {
    int choice;

    do {
        cout << "\n===== Library Management System =====\n";
        cout << "1. Register Member\n";
        cout << "2. Add Book\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "5. Search Books (Title/Author)\n";
        cout << "6. Display All Books\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                registerMember();
                break;
            case 2:
                addBook();
                break;
            case 3:
                issueBook();
                break;
            case 4:
                returnBook();
                break;
            case 5:
                searchBooks();
                break;
            case 6:
                displayAllBooks();
                break;
            case 7:
                cout << "\nExiting Library Management System. Goodbye!\n";
                break;
            default:
                cout << "\nInvalid choice. Try again.\n";
        }
    } while (choice != 7);

    return 0;
}
