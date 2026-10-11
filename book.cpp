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

// Getters

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

// Setters

void Book:: setTitle(const string& title) {
    this->title = title;
}

void Book:: setAuthor(const string& author) {
    this->author = author;
}

void Book:: setISBN(const string& isbn) {
    this->isbn = isbn;
}

void Book:: setAvailability(bool available) {
    isAvailable = available;
}

void Book:: setBorrowerId(const string& id) {
    borrowerId = id;
}

// Methods

void Book:: checkOut(const string& borrowerId) {
    setAvailability(false);
    setBorrowerId(borrowerId);
}

void Book:: returnBook() {
    setAvailability(true);
    setBorrowerId("");
}

string Book::toString() const {
    // ostringstream fonctionne comme cout, mais écrit dans une string
    ostringstream oss;
    oss << "Titre: " << title << "\n"
        << "Auteur: " << author << "\n"
        << "ISBN: " << isbn << "\n"
        << "Statu: ";

    if (isAvailable) {
        oss << "Disponible";
    } else {
        oss << "Emprunté par : " << borrowerId;
    }

    // Récupère le texte construit
    return oss.str();
}

string Book::toFileFormat() const {
    ostringstream oss;
    oss << title << "|" << author << "|" << isbn << "|"; 
    
    if (isAvailable) {
        oss << "1";
    } else {
        oss << "0";
    }
    oss << "|" << borrowerId;
    return oss.str();
}
