#pragma Once
#include <iostream>
#include <fstream>
#include "Partie.hpp"
#include "couleurEnum.hpp"
#include "Joueur.hpp"


using namespace std;

//creer logo secouia en ascii
//appeler l'init de la partie
//fct vainqueur 
//fct parfie finie
//int//while

void setup() {

    cout << "Bienvenue dans le jeu de Secouia !" << endl;
    //afficher logo secouia en acsii-art.txt

    ifstream fichier("ascii-art.txt");

    if (!fichier.is_open()) {
        cerr << "Erreur : impossible d'ouvrir le fichier." << endl;
        return;
    }

    string ligne;
    while (getline(fichier, ligne)) {
        cout << ligne << endl;
    }

    fichier.close();

    cout << "Initialisation de la partie..." << endl;
    cout << "Le but du jeu est d’être le premier à réussir 6 tickets." << endl;
    cout << "Entrez le nombre de joueurs entre 2 et 4 : ";
    int nbJoueurs;
    cin >> nbJoueurs;
    // Appeler l'init de la partie
    for (int i =0; i < nbJoueurs; i++) {
        cout << "Entrez le nom du joueur " << i+1 << " : ";
        string nom;
        cin >> nom;
        Joueur joueur(nom, couleurJoueur(i));
    }
}

Joueur* vainqueur(Partie &partie) {
    // Vérifier les conditions de victoire
    for (int i =0; i<partie.getJoueur().size(); i++) {
        if (partie.getJoueur(i)->aGagne()) {
            cout << "Le vainqueur est : " << partie.getJoueur(i)->getNom() << " avec " << partie.getJoueur(i)->getTicketFini() << " tickets réussis." << endl;
            return partie.getJoueur(i);
        }
    }
}

void partieFinie(Partie &partie, Joueur &vainqueur) {
    // Vérifier si la partie est finie
    cout << "La partie est finie !" << endl;
    cout << "Le vainqueur est : " << vainqueur.getNom() << " avec " << vainqueur.getTicketFini() << " tickets réussis." << endl;
}

void loop() {
    setup();
    Partie partie;
    Joueur *vainqueur = ::vainqueur(partie);
    partieFinie(partie, *vainqueur);
}