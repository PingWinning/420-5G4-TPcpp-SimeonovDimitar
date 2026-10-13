#include <fstream>
#include <iostream>
#include <filesystem>

#include <ctime>

#include "filemanager.h"

using namespace std;

// Constructor
FileManager::FileManager(const string& booksFile, const string& usersFile, const string& journalFile)
    : booksFileName(booksFile), usersFileName(usersFile), journalFileName(journalFile) {}

// Save all library data
bool FileManager::saveLibraryData(Library& library) {
    if(booksFileName.empty() || usersFileName.empty()) {
        cout << "Aucun répertoire de données spécifié (option --data-dir / -d) : "  
        << "Les données ne seront pas sauvegardées.\n";
        return false;
    }
    return saveBooksToFile(library) && saveUsersToFile(library);
}

// Load all library data
bool FileManager::loadLibraryData(Library& library) {
    bool booksLoaded = loadBooksFromFile(library);
    bool usersLoaded = loadUsersFromFile(library);
    return booksLoaded || usersLoaded; // Return true if at least one file was loaded
}

// Save books to file
bool FileManager::saveBooksToFile(Library& library) {
    ofstream file(booksFileName);
    if (!file.is_open()) {
        cout << "Erreur : Impossible d'ouvrir " << booksFileName << " en écriture.\n";
        return false;
    }
    
    auto books = library.getAllBooks();
    for (Book* book : books) {
        file << book->toFileFormat() << "\n";
    }
    
    file.close();
    return true;
}

// Save users to file
bool FileManager::saveUsersToFile(Library& library) {
    ofstream file(usersFileName);
    if (!file.is_open()) {
        cout << "Erreur : Impossible d'ouvrir " << usersFileName << " en écriture.\n";
        return false;
    }
    
    auto users = library.getAllUsers();
    for (User* user : users) {
        file << user->toFileFormat() << "\n";
    }
    
    file.close();
    return true;
}

// Load books from file
bool FileManager::loadBooksFromFile(Library& library) {
    ifstream file(booksFileName);
    if (!file.is_open()) {
        cout << "Aucun fichier de livres existant trouvé. Démarrage avec une bibliothèque vide.\n";
        return false;
    }
    
    string line;
    int count = 0;
    while (getline(file, line)) {
        if (!line.empty()) {
            Book book;
            book.fromFileFormat(line);
            library.addBook(book);
            count++;
        }
    }
    
    file.close();
    cout << "Chargé " << count << " livre(s) depuis le fichier.\n";
    return true;
}

// Load users from file
bool FileManager::loadUsersFromFile(Library& library) {
    ifstream file(usersFileName);
    if (!file.is_open()) {
        cout << "Aucun fichier d'utilisateurs existant trouvé. Démarrage sans utilisateurs enregistrés.\n";
        return false;
    }
    
    string line;
    int count = 0;
    while (getline(file, line)) {
        if (!line.empty()) {
            User user;
            user.fromFileFormat(line);
            library.addUser(user);
            count++;
        }
    }
    
    file.close();
    cout << "Chargé " << count << " utilisateur(s) depuis le fichier.\n";
    return true;
}

string FileManager::deuxChiffres(int number) {
    if (number < 10) {
        return "0" + to_string(number);
    }
    return to_string(number);
}

// Log operation to journal
void FileManager::logOperation(const string& action, const string& operationDescription) {
    // 1. Vérifie si le fichier journal est spécifié
    if(journalFileName.empty()) {
        /*cout << "Aucun fichier journal spécifié (option --data-dir / -d) : "
             << "Les opérations ne seront pas enregistrées.\n";*/
        return;
    }

    // 2. Ouvre le fichier en mode ajout (ios::app) :
    //    - écrit à la fin sans effacer les anciennes lignes
    //    - crée le fichier s'il n'existe pas
    ofstream journalFile(journalFileName, ios::app);
    if (!journalFile.is_open()) {
        cout << "Erreur : Impossible d'ouvrir " << journalFileName << " en écriture.\n";
        return;
    }

    // 3. Lit l'heure actuelle
    time_t maintenant = time(nullptr); // Obtient l'heure actuelle
    tm* dateHeure = localtime(&maintenant);

    // 4. Construit la date "AAAA-MM-JJ HH:MM:SS"
    string date = to_string(1900 + dateHeure->tm_year) + "-" +
                  deuxChiffres(dateHeure->tm_mon + 1) + "-" +
                  deuxChiffres(dateHeure->tm_mday) + " " +
                  deuxChiffres(dateHeure->tm_hour) + ":" +
                  deuxChiffres(dateHeure->tm_min) + ":" +
                  deuxChiffres(dateHeure->tm_sec);

    // 5. Écrit l'opération dans le fichier journal
    journalFile << date << " - [" << action << "] " << operationDescription << "\n";
    journalFile.close();
}

// Check if file exists
bool FileManager::fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

// Create backup
void FileManager::createBackup() {
    if (fileExists(booksFileName)) {
        filesystem::copy_file(booksFileName, booksFileName + ".backup");
    }
    
    if (fileExists(usersFileName)) {
        filesystem::copy_file(usersFileName, usersFileName + ".backup");
    }
    
    cout << "Fichiers de sauvegarde créés.\n";
}