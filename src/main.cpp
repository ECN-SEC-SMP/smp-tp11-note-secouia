// CORRECTION : suppression de "#pragma Once" (majuscule = pragma inconnu,
// et de toute façon #pragma once n'a aucun sens dans un .cpp)

#include <iostream>
#include <fstream>
#include "Partie.hpp"
#include "couleurEnum.hpp"
#include "Joueur.hpp"

using namespace std;

void setup(vector<Joueur *> &joueurs)
{
    cout << "Bienvenue dans le jeu de Secouia !" << endl;

    ifstream fichier("ascii-art.txt");
    if (!fichier.is_open())
    {
        cerr << "Erreur : impossible d'ouvrir le fichier." << endl;
    }
    else
    {
        string ligne;
        while (getline(fichier, ligne))
            cout << ligne << endl;
        fichier.close();
    }

    cout << "Initialisation de la partie..." << endl;
    cout << "Le but du jeu est d'être le premier à réussir 6 tickets." << endl;

    int nbJoueurs = 0;
    do
    {
        cout << "Entrez le nombre de joueurs entre 2 et 4 : ";
        cin >> nbJoueurs;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            nbJoueurs = 0;
        }

        if (nbJoueurs < 2 || nbJoueurs > 4)
            cout << "Valeur invalide. Merci d'entrer 2, 3 ou 4." << endl;

    } while (nbJoueurs < 2 || nbJoueurs > 4);

    // CORRECTION : on construit les Joueur* et on les stocke dans le vecteur
    // passé en paramètre, pour pouvoir ensuite initialiser Partie correctement.
    for (int i = 0; i < nbJoueurs; i++)
    {
        cout << "Entrez le nom du joueur " << i + 1 << " : ";
        string nom;
        cin >> nom;
        joueurs.push_back(new Joueur(nom, couleurJoueur(i)));
    }
}

// CORRECTION : getJoueur() n'existe pas dans Partie.
// On utilise getJoueurs() (vecteur) et getJoueurs()[i] pour accéder à un élément.
Joueur *vainqueur(Partie &partie)
{
    for (size_t i = 0; i < partie.getJoueurs().size(); i++)
    {
        if (partie.getJoueurs()[i]->aGagne())
        {
            cout << "Le vainqueur est : "
                 << partie.getJoueurs()[i]->getNom()
                 << " avec "
                 << partie.getJoueurs()[i]->getTicketFini()
                 << " tickets réussis." << endl;
            return partie.getJoueurs()[i];
        }
    }
    return nullptr;
}

// CORRECTION : suppression du paramètre inutilisé 'partie' pour éviter
// -Werror=unused-parameter
void partieFinie(Joueur &vainqueur)
{
    cout << "La partie est finie !" << endl;
    cout << "Le vainqueur est : "
         << vainqueur.getNom()
         << " avec "
         << vainqueur.getTicketFini()
         << " tickets réussis." << endl;
}

void loop()
{
    // CORRECTION : Partie n'a pas de constructeur par défaut.
    // On construit d'abord les joueurs dans setup(), puis on passe tout
    // à Partie(piocheTicket, grandeTraversee, piocheTrain, joueurs).
    vector<Joueur *> joueurs;
    setup(joueurs);

    vector<Ticket *> piocheTicket; // à remplir selon la logique du jeu
    vector<Train *> piocheTrain;   // à remplir selon la logique du jeu
    bool grandeTraversee = false;

    Partie partie(piocheTicket, grandeTraversee, piocheTrain, joueurs);

    Joueur *gagnant = ::vainqueur(partie);
    if (gagnant != nullptr)
    {
        partieFinie(*gagnant);
    }
}

int main()
{
    loop();
    return 0;
}