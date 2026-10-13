#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>

#include "library.h"

using namespace std;

class FileManager {
private:
    string booksFileName;
    string usersFileName;
    string journalFileName; // Ajouté : chemin du fichier journal pour les opérations de la bibliothèque
    string deuxChiffres(int number); // Ajouté : méthode pour formater les nombres à deux chiffres

public:
    // Constructor
    FileManager(const string& booksFile, const string& usersFile, const string& journalFile);
    
    // File operations
    bool saveLibraryData(Library& library);
    bool loadLibraryData(Library& library);
    
    // Individual file operations
    bool saveBooksToFile(Library& library);
    bool saveUsersToFile(Library& library);
    bool loadBooksFromFile(Library& library);
    bool loadUsersFromFile(Library& library);

    // Journal d'activité
    void logOperation(const string& action,const string& operationDescription);
    
    // Utility methods
    bool fileExists(const string& filename);
    void createBackup();
};

#endif
