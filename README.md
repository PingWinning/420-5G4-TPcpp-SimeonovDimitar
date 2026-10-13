# TP C++ - Gestion de bibliothèque

**Nom :** Simeonov, Dimitar
**Numéro de DA :** 2043761

## Fonctionnalités implémentées

### Code manquant : `book.cpp`
Réécriture complète de `book.cpp` à partir de `book.h` : constructeurs, getters/setters,
emprunt et retour (`checkOut`, `returnBook`), affichage (`toString`) et lecture/écriture
au format fichier (`toFileFormat`, `fromFileFormat`). Le programme lit correctement
le fichier d'exemple `data/books.txt`.

### Correction du bug de sauvegarde
Sans répertoire de données (`-d`), les chemins des fichiers sont vides.
`saveLibraryData` vérifie maintenant ce cas : au lieu d'essayer d'écrire dans `""` et d'échouer
silencieusement, elle affiche un message explicite et ne sauvegarde rien.
À la fermeture (option 0), le résultat de la sauvegarde est maintenant vérifié et affiché.

### Journal d'activités
- Fichier `journal.txt` créé dans le répertoire de données (`-d`).
- Méthode `FileManager::logOperation`, qui ouvre le fichier en mode ajout (`ios::app`) : le fichier est créé s'il n'existe pas, et les anciennes lignes sont conservées d'une session à l'autre.
- Format : `AAAA-MM-JJ HH:MM:SS - [ACTION] description`. Pour les livres, la description reprend le format de `books.txt` (`toFileFormat`).
- Actions enregistrées :

| Action du menu | Étiquette dans le journal |
|---|---|
| Ajouter / supprimer un livre | `[AJOUT LIVRE]`, `[SUPPRESSION LIVRE]` |
| Rechercher par titre / auteur | `[RECHERCHE LIVRE]` |
| Afficher les livres (avec le tri choisi) | `[AFFICHAGE LIVRE]` |
| Ajouter / afficher les utilisateurs | `[AJOUT UTILISATEUR]`, `[AFFICHAGE UTILISATEUR]` |
| Emprunter / retourner un livre | `[EMPRUNT LIVRE]`, `[RETOUR LIVRE]` |
| Statistiques | `[STATISTIQUES LIVRE]` |
| Sauvegarde / copie de sauvegarde | `[SAUVEGARDE LIVRE]`, `[BACKUP LIVRE]` |
| Actions refusées (doublon, livre introuvable, choix invalide…) | `[ERREUR]` |

Exemple :
```
2026-10-13 15:20:04 - [AJOUT LIVRE] Livre ajouté : Test|Moi|555|1|
2026-10-13 15:20:31 - [EMPRUNT LIVRE] Livre emprunté : Test|Moi|555|0|USR001
2026-10-13 15:21:02 - [ERREUR] Tentative d'ajout d'un livre avec un ISBN existant : 555
```

### Gestion des données : tri des résultats (STL)
- Lors de l'affichage de **tous les livres** (option 5) et des **livres disponibles** (option 6), l'utilisateur choisit : tri par titre, par auteur ou aucun tri.
- Le tri utilise `std::sort` (`<algorithm>`) avec une lambda comme règle de comparaison, dans la méthode `Library::trierLivres`.
- Seule une copie de la liste (des pointeurs) est triée : l'ordre réel de la bibliothèque et du fichier `books.txt` n'est pas modifié.

### Interface et expérience utilisateur : détection des doublons plus intelligente
À l'ajout d'un livre, deux vérifications sont faites :
1. **ISBN identique** (vérification d'origine).
2. **Doublon** avec `Library::findDuplicate` : un livre est refusé s'il existe déjà un livre avec
   - le **même ISBN** en ignorant les tirets et les espaces (`978-2-07-038001-5` = `9782070380015`), **ou**
   - le **même titre et le même auteur**, sans tenir compte des majuscules (`candide` / `VOLTAIRE` = `Candide` / `Voltaire`).

Le message d'erreur affiche le livre déjà existant (titre, auteur, ISBN).

## Veille technologique

### Question 1 : C++
Journal d'activités, chaque ligne doit commencer par la date et l'heure au format `AAAA-MM-JJ HH:MM:SS`. Nous n'avons pas vu en cours comment obtenir l'heure de l'ordinateur en C++ : il faut utiliser la bibliothèque standard `<ctime>`. La fonction `time(nullptr)` retourne l'heure actuelle sous forme d'un nombre de type `time_t`, qui représente le nombre de secondes écoulées depuis le 1er janvier 1970. Ce nombre n'est pas lisible tel quel, alors la fonction `localtime` le convertit en une structure `tm` qui contient des champs séparés (`tm_year`, `tm_mon`, `tm_mday`, `tm_hour`, `tm_min`, `tm_sec`) selon le fuseau horaire de l'ordinateur. Comme `localtime` retourne un pointeur, on accède aux champs avec la flèche `->`. Il y a deux pièges : `tm_year` compte les années depuis 1900 (2026 donne 126), et `tm_mon` va de 0 à 11 (octobre donne 9). Il faut donc ajouter 1900 à l'année et 1 au mois, sinon la date serait `126-09-13` au lieu de `2026-10-13`. J'ai ensuite construit la date avec `to_string` et une méthode `deuxChiffres` qui ajoute un zéro devant les nombres plus petits que 10, pour obtenir `09:05` au lieu de `9:5`.

Extrait de `FileManager::logOperation` (`filemanager.cpp`) :

```cpp
time_t maintenant = time(nullptr);          // secondes depuis le 1er janvier 1970
tm* dateHeure = localtime(&maintenant);     // conversion en date et heure locales

string date = to_string(1900 + dateHeure->tm_year) + "-" +   // année depuis 1900
              deuxChiffres(dateHeure->tm_mon + 1) + "-" +    // mois de 0 à 11
              deuxChiffres(dateHeure->tm_mday) + " " +
              deuxChiffres(dateHeure->tm_hour) + ":" +
              deuxChiffres(dateHeure->tm_min) + ":" +
              deuxChiffres(dateHeure->tm_sec);
```

Résultat dans `journal.txt` :
```
2026-10-13 15:20:04 - [AJOUT LIVRE] Livre ajouté : Test|Moi|555|1|
```

### Question 2 : Options de développement possible
Avec des millions de livres, le programme actuel deviendrait trop lourd très rapidement. Tous les livres sont chargés en mémoire au démarrage, les recherches et les tris parcourent la liste au complet, ce qui prendrait beaucoup trop de temps vu l'énormité des données, et chaque sauvegarde réécrit tout le fichier `books.txt`. De plus, un seul programme peut utiliser les fichiers à la fois. J'utiliserais plutôt une base de données relationnelle comme **MySQL**. Les livres, les utilisateurs et les emprunts seraient stockés dans des tables, et on ajouterait des index sur l'ISBN, le titre et l'auteur pour que les recherches restent rapides, même avec des millions de livres. Le programme ne chargerait plus tout en mémoire : il demanderait seulement les livres dont il a besoin, avec des requêtes SQL. Les tris se feraient directement dans la base (`ORDER BY`), et plusieurs bibliothécaires pourraient travailler en même temps. Pour relier MySQL au C++, j'utiliserais la bibliothèque officielle **MySQL Connector/C++**, que l'on ajoute au projet dans le `CMakeLists.txt`. La classe `FileManager`, qui lit et écrit les fichiers texte, serait remplacée par une classe qui se connecte à la base et envoie des requêtes SQL. 

# Construire le projet
Vous pouvez utiliser un dev container de base C++ de VScode.
Le projet utilise cmake, pensez à l'inclure dans votre dev container.

Voici les lignes de commandes pour compiler le projet:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

# Répertoire data

Il contient 2 fichiers `books.txt`et `users.txt` que vous pouvez utilisez pour tester votre code.
Pour ca il suffit de donner chemin vers le repertoire data avec l'application `bibliotheque -d <chemin verrs data>`