@mainpage Les Aventuriers du Rail

@tableofcontents

Implémentation C++17 du jeu de société, réalisée pour le TP noté de SMP à
l'École Centrale de Nantes. Le projet propose un mode console et une
interface Qt optionnelle.

# Architecture

Les classes du domaine sont compilées dans une bibliothèque statique
`aventuriers_lib`, partagée par les deux exécutables (`main.cpp` pour la
console, `gui_main.cpp` pour Qt).

@dot
digraph architecture {
    rankdir=LR;
    node [shape=box, style="rounded,filled", fillcolor="#f5f5f5",
          fontname="Helvetica", fontsize=11];
    edge [fontname="Helvetica", fontsize=10];

    Partie     [URL="\ref Partie"];
    Plateau    [URL="\ref Plateau"];
    Joueur     [URL="\ref Joueur"];
    Ville      [URL="\ref Ville"];
    VoieFerree [URL="\ref VoieFerree"];
    Train      [URL="\ref Train"];
    Ticket     [URL="\ref Ticket"];

    Partie -> Plateau    [arrowhead=diamond, label=" 1"];
    Partie -> Joueur     [arrowhead=diamond, label=" 2..4"];
    Partie -> Ticket     [arrowhead=diamond, label=" pioche"];
    Partie -> Train      [arrowhead=diamond, label=" pioche"];

    Joueur -> Train      [arrowhead=diamond, label=" main"];
    Joueur -> Ticket     [arrowhead=diamond, label=" missions"];

    Plateau -> Ville      [arrowhead=diamond, label=" *"];
    Plateau -> VoieFerree [arrowhead=diamond, label=" *"];

    VoieFerree -> Ville  [label=" extrémités"];
    VoieFerree -> Joueur [style=dashed, label=" propriétaire ?"];

    Ticket -> Ville      [label=" départ / arrivée"];
}
@enddot

Losanges : composition. Flèche pointillée : association optionnelle (une
voie peut ne pas avoir de propriétaire). Les nœuds sont cliquables.

# Classes principales

| Classe          | Rôle |
|-----------------|------|
| @ref Partie     | Orchestre une session : pioches, tours, fin de partie. |
| @ref Plateau    | Charge la carte et les tickets depuis les CSV, valide les chemins par BFS. |
| @ref Joueur     | Wagons, main de cartes, missions. |
| @ref VoieFerree | Route entre deux villes : couleur, longueur, propriétaire. |
| @ref Ville      | Sommet du graphe. |
| @ref Train      | Carte wagon (couleur ou locomotive). |
| @ref Ticket     | Mission reliant deux villes. |

# Données

Tout est paramétré sans recompilation via les fichiers du dossier `files/` :

- `map.csv` : `ville_a, ville_b, couleur, longueur`
- `ticket.csv` : `id, ville_a, ville_b`

# Points d'entrée utiles

- @ref Plateau::existeChemin : validation d'un ticket par BFS.
- @ref Plateau::placeWagon : prise d'une voie ferrée.
- @ref Joueur::peutPrendreVoie : vérification des cartes avant achat.
- @ref Partie::piocherTrain : pioche d'une carte train.
