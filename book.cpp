#include "book.h"
#include <sstream>
#include <iostream>

using namespace std;

Book:: Book() {
    title = "";
    author = "";
    isbn = "";
    isAvailable = true;
    borrowerId = "";
}

Book:: Book(const string& title, const string& author, const string& isbn) {
    this->title = title;
    this->author = author;
    this->isbn = isbn;
    isAvailable = true;
    borrowerId = "";
}

string Book:: getTitle() const {
    return title;
}

string Book:: getAuthor() const {
    return author;
}

string Book:: getISBN() const {
    return isbn;
}

bool Book:: getAvailability() const {
    return isAvailable;
}

string Book:: getBorrowerId() const {
    return borrowerId;
}


