#include <iostream>
#include <algorithm>

#include "library.h"

using namespace std;

// Constructor
Library::Library() {}

// Add book to library
void Library::addBook(const Book& book) {
    books.push_back(make_unique<Book>(book));
}

// Remove book from library
bool Library::removeBook(const string& isbn) {
    auto it = find_if(books.begin(), books.end(),
        [&isbn](const unique_ptr<Book>& book) {
            return book->getISBN() == isbn;
        });
    
    if (it != books.end()) {
        books.erase(it);
        return true;
    }
    return false;
}

// Find book by ISBN
Book* Library::findBookByISBN(const string& isbn) {
    auto it = find_if(books.begin(), books.end(),
        [&isbn](const unique_ptr<Book>& book) {
            return book->getISBN() == isbn;
        });
    
    return (it != books.end()) ? it->get() : nullptr;
}

// Search books by title (case-insensitive partial match)
vector<Book*> Library::searchBooksByTitle(const string& title) {
    vector<Book*> results;
    string lowerTitle = title;
    transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
    
    for (auto& book : books) {
        string bookTitle = book->getTitle();
        transform(bookTitle.begin(), bookTitle.end(), bookTitle.begin(), ::tolower);
        
        if (bookTitle.find(lowerTitle) != string::npos) {
            results.push_back(book.get());
        }
    }
    return results;
}

// Search books by author (case-insensitive partial match)
vector<Book*> Library::searchBooksByAuthor(const string& author) {
    vector<Book*> results;
    string lowerAuthor = author;
    transform(lowerAuthor.begin(), lowerAuthor.end(), lowerAuthor.begin(), ::tolower);
    
    for (auto& book : books) {
        string bookAuthor = book->getAuthor();
        transform(bookAuthor.begin(), bookAuthor.end(), bookAuthor.begin(), ::tolower);
        
        if (bookAuthor.find(lowerAuthor) != string::npos) {
            results.push_back(book.get());
        }
    }
    return results;
}

// Get all available books
vector<Book*> Library::getAvailableBooks() {
    vector<Book*> available;
    for (auto& book : books) {
        if (book->getAvailability()) {
            available.push_back(book.get());
        }
    }
    return available;
}

// Get all books
vector<Book*> Library::getAllBooks() {
    vector<Book*> allBooks;
    for (auto& book : books) {
        allBooks.push_back(book.get());
    }
    return allBooks;
}

// Add user to library
void Library::addUser(const User& user) {
    users.push_back(make_unique<User>(user));
}

// Find user by ID
User* Library::findUserById(const string& userId) {
    auto it = find_if(users.begin(), users.end(),
        [&userId](const unique_ptr<User>& user) {
            return user->getUserId() == userId;
        });
    
    return (it != users.end()) ? it->get() : nullptr;
}

// Get all users
vector<User*> Library::getAllUsers() {
    vector<User*> allUsers;
    for (auto& user : users) {
        allUsers.push_back(user.get());
    }
    return allUsers;
}

// Check out book
bool Library::checkOutBook(const string& isbn, const string& userId) {
    Book* book = findBookByISBN(isbn);
    User* user = findUserById(userId);
    
    if (book && user && book->getAvailability()) {
        book->checkOut(user->getUserId());
        user->borrowBook(isbn);
        return true;
    }
    return false;
}

// Return book
bool Library::returnBook(const string& isbn) {
    Book* book = findBookByISBN(isbn);
    
    if (book && !book->getAvailability()) {
        // Find the user who borrowed this book
        for (auto& user : users) {
            if (user->hasBorrowedBook(isbn)) {
                user->returnBook(isbn);
                break;
            }
        }
        book->returnBook();
        return true;
    }
    return false;
}

// Display all books
void Library::displayAllBooks(int triOption) {
    vector<Book*> livres = getAllBooks(); // copie de la liste des livres

    if (livres.empty()) {
        cout << "Aucun livre dans la bibliothèque.\n";
        return;
    }

    trierLivres(livres, triOption); // trier selon l'option choisie
    
    cout << "\n=== TOUS LES LIVRES ===\n";
    for (size_t i = 0; i < livres.size(); ++i) {
        cout << "\nLivre " << (i + 1) << " :\n";
        cout << livres[i]->toString() << "\n";
        cout << "-------------------------\n";
    }
}

// Display available books
void Library::displayAvailableBooks(int triOption) {
    auto available = getAvailableBooks();
    
    if (available.empty()) {
        cout << "Aucun livre disponible pour emprunt.\n";
        return;
    }

    trierLivres(available, triOption); // trier selon l'option choisie
    
    cout << "\n=== LIVRES DISPONIBLES ===\n";
    for (size_t i = 0; i < available.size(); ++i) {
        cout << "\nLivre " << (i + 1) << " :\n";
        cout << available[i]->toString() << "\n";
        cout << "---------------------------\n";
    }
}

void Library::trierLivres(vector<Book*>& livres, int triOption) {
    switch (triOption) {
        case 1: // Tri par titre
            sort(livres.begin(), livres.end(),
                [](Book* a, Book* b) {
                    return a->getTitle() < b->getTitle();
                });
            break;
        case 2: // Tri par auteur
            sort(livres.begin(), livres.end(),
                [](Book* a, Book* b) {
                    return a->getAuthor() < b->getAuthor();
                });
            break;
        default:
            break; // pas de tri par defaut
    }
}

// Display all users
void Library::displayAllUsers() {
    if (users.empty()) {
        cout << "Aucun utilisateur enregistré.\n";
        return;
    }
    
    cout << "\n=== TOUS LES UTILISATEURS ===\n";
    for (size_t i = 0; i < users.size(); ++i) {
        cout << "\nUtilisateur " << (i + 1) << " :\n";
        cout << users[i]->toString() << "\n";
        cout << "------------------------------\n";
    }
}

// Statistics
int Library::getTotalBooks() const { return books.size(); }
int Library::getAvailableBookCount() const {
    return count_if(books.begin(), books.end(),
        [](const unique_ptr<Book>& book) {
            return book->getAvailability();
        });
}
int Library::getCheckedOutBookCount() const { return getTotalBooks() - getAvailableBookCount(); }