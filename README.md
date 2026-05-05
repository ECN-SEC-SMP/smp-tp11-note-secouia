# Les Aventuriers du Rail

Implémentation en C++ du jeu de société *Les Aventuriers du Rail*, réalisée dans le cadre du TP noté de Systèmes Embarqués Communicants - Spécification et modélisation de programme à l'École Centrale de Nantes.

## Documentation

La documentation Doxygen du projet est disponible en ligne :
[https://ecn-sec-smp.github.io/smp-tp11-note-secouia/](https://ecn-sec-smp.github.io/smp-tp11-note-secouia/)

Elle est générée et déployée automatiquement sur GitHub Pages à chaque push sur `main`.

## Auteurs

- Thomas **METAIS**
- Guillaume **PIRAUBE**
- Jean-Marc **KERVIL**
- Aymeric **ZIMINSKI**

## Fonctionnalités

- **Mode console** : jeu interactif en ligne de commande (2 à 4 joueurs)
- **Interface graphique Qt** : rendu visuel de la carte des États-Unis avec interactions souris/clavier
- Chargement de la carte et des tickets depuis des fichiers CSV
- Pioche et gestion des cartes train (6 couleurs + locomotives joker)
- Prise de voies ferrées avec vérification des cartes et wagons disponibles
- Validation automatique des tickets par recherche de chemin (Dijkstra)
- Recyclage de la défausse lorsque la pioche est vide
- Détection de la **grande traversée** (côte ouest vers côte est)
- Conditions de fin de partie : 6 tickets complétés ou plus de wagons

## Prérequis

- **CMake** >= 3.19
- **Compilateur C++17** (g++, clang++, MSVC)
- **Google Test** (pour les tests unitaires)
- **Qt5 ou Qt6** (optionnel, pour l'interface graphique)
- **Doxygen + Graphviz** (optionnel, pour la documentation)

### Installation des dépendances

**Linux (Ubuntu/Debian) :**
```bash
sudo apt install cmake g++ libgtest-dev qt6-base-dev doxygen graphviz
```

**macOS :**
```bash
brew install cmake googletest qt doxygen graphviz
```

## Compilation

```bash
# Configurer et compiler
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

L'exécutable principal `aventuriers` est généré dans le dossier `build/`. Si Qt est détecté, la cible `aventuriers_gui` est également disponible.

## Tests

Le projet inclut une suite de tests unitaires Google Test couvrant les classes `Joueur`, `Ticket`, `Train`, `VoieFerree`, ainsi que la logique de jeu (Dijkstra, pioche, prise de voie).

```bash
ctest --test-dir build --output-on-failure
```

## Lancement

**Mode console :**
```bash
./build/aventuriers
```

**Interface graphique (si Qt est installé) :**
```bash
./build/aventuriers_gui
```

## Génération de la documentation

```bash
cmake --build build --target doc
```

La documentation HTML est générée dans `docs/html/`. Ouvrir `docs/html/index.html` pour la consulter localement.

## Structure du projet

```
.
├── include/            # En-têtes (.hpp)
│   ├── Joueur.hpp
│   ├── Partie.hpp
│   ├── Plateau.hpp
│   ├── Ticket.hpp
│   ├── Train.hpp
│   ├── Ville.hpp
│   ├── VoieFerree.hpp
│   └── couleurEnum.hpp
├── src/                # Sources (.cpp)
│   ├── main.cpp        # Point d'entrée console
│   ├── gui_main.cpp    # Point d'entrée Qt
│   ├── Joueur.cpp
│   ├── Partie.cpp
│   ├── Plateau.cpp
│   ├── Ticket.cpp
│   ├── Train.cpp
│   ├── Ville.cpp
│   └── VoieFerree.cpp
├── tests/              # Tests unitaires Google Test
├── files/              # Données de jeu (CSV)
│   ├── map.csv         # Carte : villes, voies, couleurs, longueurs
│   └── ticket.csv      # Tickets : ville départ / arrivée
├── CMakeLists.txt
├── Doxyfile
└── README.md
```

## CI/CD

Le projet utilise GitHub Actions pour :
- **Compilation et tests** automatiques à chaque push et pull request
- **Déploiement de la documentation** Doxygen sur GitHub Pages à chaque push sur `main`