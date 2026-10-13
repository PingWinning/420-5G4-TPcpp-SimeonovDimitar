#include <iostream>
#include <limits>
#include <string>
#include <filesystem>

#include "library.h"
#include "filemanager.h"

using namespace std;

void clearScreen() {
    system("cls || clear");
}

void pauseForInput() {
    cout << "\nAppuyez sur Entrée pour continuer...";
    cin.ignore();
    cin.get();
}

void displayMenu() {
    cout << "\n=== SYSTÈME DE GESTION DE BIBLIOTHÈQUE PERSONNELLE ===\n";
    cout << "1.  Ajouter un Livre\n";
    cout << "2.  Supprimer un Livre\n";
    cout << "3.  Rechercher des Livres par Titre\n";
    cout << "4.  Rechercher des Livres par Auteur\n";
    cout << "5.  Afficher Tous les Livres\n";
    cout << "6.  Afficher les Livres Disponibles\n";
    cout << "7.  Ajouter un Utilisateur\n";
    cout << "8.  Afficher Tous les Utilisateurs\n";
    cout << "9.  Emprunter un Livre\n";
    cout << "10. Retourner un Livre\n";
    cout << "11. Statistiques de la Bibliothèque\n";
    cout << "12. Sauvegarder les Données\n";
    cout << "13. Créer une Sauvegarde\n";
    cout << "0.  Quitter\n";
    cout << "======================================================\n";
    cout << "Entrez votre choix : ";
}

string getInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}

// Demande à l'utilisateur comment trier l'affichage des livres.
// Retourne 1 (par titre), 2 (par auteur) ou 0 (aucun tri).
int demanderTri() {
    cout << "\n--- OPTIONS DE TRI ---\n";
    cout << "1. Trier par titre (ordre alphabétique)\n";
    cout << "2. Trier par auteur (ordre alphabétique)\n";
    cout << "3. Aucun tri \n";
    string choix = getInput("Comment voulez-vous trier les livres ? ");

    int option = 0;   // par défaut : aucun tri

    if (choix == "1") {
        option = 1;
        cout << "Tri par titre appliqué.\n";
    } else if (choix == "2") {
        option = 2;
        cout << "Tri par auteur appliqué.\n";
    } else if (choix != "3") {
        cout << "Choix invalide : les livres seront affichés sans tri.\n";
    }

    return option;
}

void printUsage(const char* programName) {
    cerr << "Usage: " << programName << " [--data-dir|-d <répertoire>]\n";
}

int main(int argc, char* argv[]) {
    string dataDir;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--data-dir" || arg == "-d") {
            if (i + 1 >= argc) {
                cerr << "Erreur : " << arg << " nécessite un répertoire.\n";
                printUsage(argv[0]);
                return 1;
            }
            dataDir = argv[++i];
        } else {
            cerr << "Erreur : argument inconnu '" << arg << "'.\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    string booksFile;
    string usersFile;
    string journalFile; 
    if (!dataDir.empty()) {
        if (!filesystem::is_directory(dataDir)) {
            cerr << "Erreur : le répertoire " << dataDir << " n'existe pas.\n";
            printUsage(argv[0]);
            return 1;
        }
        booksFile = (filesystem::path(dataDir) / "books.txt").string();
        usersFile = (filesystem::path(dataDir) / "users.txt").string();
        journalFile = (filesystem::path(dataDir) / "journal.txt").string(); 
    }

    Library library;
    FileManager fileManager(booksFile, usersFile, journalFile);
    
    // Load existing data
    cout << "Chargement des données de la bibliothèque...\n";
    fileManager.loadLibraryData(library);
    
    int choice;
    bool running = true;
    
    while (running) {
        displayMenu();
        
        if (!(cin >> choice)) {
            cout << "Saisie invalide. Veuillez entrer un nombre.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            pauseForInput();
            continue;
        }
        cin.ignore(); // Clear newline from buffer
        
        switch (choice) {
            case 1: { // Add Book
                string title = getInput("Entrez le titre du livre : ");
                string author = getInput("Entrez l'auteur du livre : ");
                string isbn = getInput("Entrez l'ISBN du livre : ");

                Book* doublon = library.findDuplicate(title, author, isbn);
                
                if (library.findBookByISBN(isbn)) {
                    cout << "Erreur : Un livre avec l'ISBN " << isbn << " existe déjà.\n";
                    fileManager.logOperation("ERREUR", "Tentative d'ajout d'un livre avec un ISBN existant : " + isbn);
                }else if (doublon != nullptr) {
                    cout << "Erreur : Ce livre existe déjà dans la bibliothèque :\n"
                        << "  \"" << doublon->getTitle() << "\" de " << doublon->getAuthor()
                        << " (ISBN " << doublon->getISBN() << ")\n";
                    fileManager.logOperation("ERREUR", "Tentative d'ajout d'un doublon : " + title + "|" + author + "|" + isbn);
                } else {
                    Book newBook(title, author, isbn);
                    library.addBook(newBook);
                    fileManager.logOperation("AJOUT LIVRE", "Livre ajouté : " + newBook.toFileFormat());
                    cout << "Livre ajouté avec succès !\n";
                }
                pauseForInput();
                break;
            }
            
            case 2: { // Remove Book
                string isbn = getInput("Entrez l'ISBN du livre à supprimer : ");

                // On copie les infos du livre AVANT de le supprimer
                string description = "";
                Book* livre = library.findBookByISBN(isbn);
                if (livre != nullptr) {
                    description = livre->toFileFormat();
                }

                if (library.removeBook(isbn)) {
                    cout << "Livre supprimé avec succès !\n";
                    fileManager.logOperation("SUPPRESSION LIVRE", description);   // texte copié : sans danger
                } else {
                    cout << "Livre non trouvé.\n";
                    fileManager.logOperation("ERREUR", "Tentative de suppression d'un livre non trouvé : " + isbn);
                }
                pauseForInput();
                break;
            }
            
            case 3: { // Search by Title
                string title = getInput("Entrez le titre à rechercher : ");
                auto results = library.searchBooksByTitle(title);
                
                if (results.empty()) {
                    cout << "Aucun livre trouvé avec ce titre.\n";
                    fileManager.logOperation("RECHERCHE LIVRE", "Aucun livre trouvé avec le titre : " + title);
                } else {
                    cout << "\n=== RÉSULTATS DE RECHERCHE ===\n";
                    for (size_t i = 0; i < results.size(); ++i) {
                        cout << "\nRésultat " << (i + 1) << " :\n";
                        cout << results[i]->toString() << "\n";
                        cout << "-----------------------------\n";
                    }
                    fileManager.logOperation("RECHERCHE LIVRE", "Livre(s) trouvé(s) avec le titre : " + title);
                }
                pauseForInput();
                break;
            }
            
            case 4: { // Search by Author
                string author = getInput("Entrez l'auteur à rechercher : ");
                auto results = library.searchBooksByAuthor(author);
                
                if (results.empty()) {
                    cout << "Aucun livre trouvé de cet auteur.\n";
                    fileManager.logOperation("RECHERCHE LIVRE", "Aucun livre trouvé de l'auteur : " + author);
                } else {
                    cout << "\n=== RÉSULTATS DE RECHERCHE ===\n";
                    for (size_t i = 0; i < results.size(); ++i) {
                        cout << "\nRésultat " << (i + 1) << " :\n";
                        cout << results[i]->toString() << "\n";
                        cout << "-----------------------------\n";
                    }
                    fileManager.logOperation("RECHERCHE LIVRE", "Livre(s) trouvé(s) de l'auteur : " + author);
                }
                pauseForInput();
                break;
            }
            
            case 5: { // Display All Books
                int triOption = demanderTri();
                library.displayAllBooks(triOption);

                string description = "Affichage de tous les livres ";
                if (triOption == 1) {
                    description += "triés par titre.";
                } else if (triOption == 2) {
                    description += "triés par auteur.";
                } else {
                    description += "sans tri.";
                }
                fileManager.logOperation("AFFICHAGE LIVRE", description);

                pauseForInput();
                break;
            }

            case 6: { // Display Available Books
                int triOption = demanderTri();
                library.displayAvailableBooks(triOption);

                string description = "Affichage des livres disponibles ";
                if (triOption == 1) {
                    description += "triés par titre.";
                } else if (triOption == 2) {
                    description += "triés par auteur.";
                } else {
                    description += "sans tri.";
                }
                fileManager.logOperation("AFFICHAGE LIVRE", description);

                pauseForInput();
                break;
            }
            
            case 7: { // Add User
                string name = getInput("Entrez le nom de l'utilisateur : ");
                string userId = getInput("Entrez l'ID de l'utilisateur : ");
                
                if (library.findUserById(userId)) {
                    cout << "Erreur : Un utilisateur avec l'ID " << userId << " existe déjà.\n";
                    fileManager.logOperation("ERREUR", "Tentative d'ajout d'un utilisateur existant : " + userId);
                } else {
                    User newUser(name, userId);
                    library.addUser(newUser);
                    cout << "Utilisateur ajouté avec succès !\n";
                    fileManager.logOperation("AJOUT UTILISATEUR", "Utilisateur ajouté : " + name);
                }
                pauseForInput();
                break;
            }
            
            case 8: // Display All Users
                library.displayAllUsers();
                fileManager.logOperation("AFFICHAGE UTILISATEUR", "Affichage de tous les utilisateurs.");
                pauseForInput();
                break;
            
            case 9: { // Check Out Book
                string isbn = getInput("Entrez l'ISBN du livre à emprunter : ");
                string userId = getInput("Entrez l'ID de l'utilisateur : ");
                
                if (library.checkOutBook(isbn, userId)) {
                    cout << "Livre emprunté avec succès !\n";
                    fileManager.logOperation("EMPRUNT LIVRE", "Livre emprunté : " + library.findBookByISBN(isbn)->toFileFormat());
                } else {
                    cout << "Erreur : Impossible d'emprunter le livre. Vérifiez l'ISBN, l'ID utilisateur et la disponibilité du livre.\n";
                    fileManager.logOperation("ERREUR", "Tentative d'emprunt d'un livre non disponible : " + isbn);
                }
                pauseForInput();
                break;
            }
            
            case 10: { // Return Book
                string isbn = getInput("Entrez l'ISBN du livre à retourner : ");
                
                if (library.returnBook(isbn)) {
                    cout << "Livre retourné avec succès !\n";
                    fileManager.logOperation("RETOUR LIVRE", "Livre retourné : " + library.findBookByISBN(isbn)->toFileFormat());
                } else {
                    cout << "Erreur : Impossible de retourner le livre. Vérifiez l'ISBN et que le livre est bien emprunté.\n";
                    fileManager.logOperation("ERREUR", "Tentative de retour d'un livre non emprunté : " + isbn);
                }
                pauseForInput();
                break;
            }
            
            case 11: { // Library Statistics
                cout << "\n=== STATISTIQUES DE LA BIBLIOTHÈQUE ===\n";
                cout << "Total des Livres : " << library.getTotalBooks() << "\n";
                cout << "Livres Disponibles : " << library.getAvailableBookCount() << "\n";
                cout << "Livres Empruntés : " << library.getCheckedOutBookCount() << "\n";
                cout << "Total des Utilisateurs : " << library.getAllUsers().size() << "\n";
                fileManager.logOperation("STATISTIQUES LIVRE", "Affichage des statistiques de la bibliothèque.");
                pauseForInput();
                break;
            }
            
            case 12: { // Save Data
                if (fileManager.saveLibraryData(library)) {
                    cout << "Données de la bibliothèque sauvegardées avec succès !\n";
                } else {
                    cout << "Erreur lors de la sauvegarde des données de la bibliothèque.\n";
                    fileManager.logOperation("ERREUR", "Échec de la sauvegarde des données de la bibliothèque.");
                }
                fileManager.logOperation("SAUVEGARDE LIVRE", "Données de la bibliothèque sauvegardées.");
                pauseForInput();
                break;
            }
            
            case 13: { // Create Backup
                fileManager.createBackup();
                fileManager.logOperation("BACKUP LIVRE", "Sauvegarde de la bibliothèque créée.");
                pauseForInput();
                break;
            }
            
            case 0: // Exit
                cout << "Sauvegarde des données avant la fermeture...\n";
                if (fileManager.saveLibraryData(library)) {
                    cout << "Données sauvegardées avec succès !\n";
                    fileManager.logOperation("SAUVEGARDE LIVRE", "Données de la bibliothèque sauvegardées avant fermeture.");
                } else {
                    cout << "Erreur lors de la sauvegarde des données.\n";
                    fileManager.logOperation("ERREUR", "Échec de la sauvegarde des données de la bibliothèque.");
                }
                cout << "Merci d'avoir utilisé le Système de Gestion de Bibliothèque Personnelle !\n";
                running = false;
                break;
            
            default:
                cout << "Choix invalide. Veuillez réessayer.\n";
                fileManager.logOperation("ERREUR", "Choix invalide dans le menu principal.");
                pauseForInput();
                break;
        }
    }
    
    return 0;
}