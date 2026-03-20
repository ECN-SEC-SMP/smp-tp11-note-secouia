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
    int nbJoueurs;
    do {
        cout << "Entrez le nombre de joueurs entre 2 et 4 : ";
        cin >> nbJoueurs;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            nbJoueurs = 0; // force la boucle à continuer
        }

        if (nbJoueurs < 2 || nbJoueurs > 4) {
            cout << "Valeur invalide. Merci d'entrer 2, 3 ou 4." << endl;
        }
    } while (nbJoueurs < 2 || nbJoueurs > 4);

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
    return nullptr; // Aucun vainqueur pour le moment
}

void partieFinie(Partie &partie, Joueur &vainqueur) {
    // Vérifier si la partie est finie
    cout << "La partie est finie !" << endl;
    cout << "Le vainqueur est : " << vainqueur.getNom() << " avec " << vainqueur.getTicketFini() << " tickets réussis." << endl;
}

void loop() {
    Partie partie;
    
    Joueur *vainqueur = ::vainqueur(partie);
    if (vainqueur != nullptr) {
        partieFinie(partie, *vainqueur);
        return;
    }
}

int main() {
    setup();
    while (1) {
        loop();
    }
    
    return 0;
}