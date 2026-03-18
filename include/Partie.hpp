#pragma once

#include <vector>
#include "Ticket.hpp"
#include "Joueur.hpp"
#include "Train.hpp"
#include "Plateau.hpp"

class Partie
{
private:
    vector<Ticket *> piocheTicket;
    bool grandeTraversee;
    vector<Train *> piocheTrain;
    vector<Joueur *> joueurs;
    Plateau *plateau;

public:
    Partie(vector<Ticket *> piocheTicket, bool grandeTraversee, vector<Train *> piocheTrain, vector<Joueur *> joueurs);
    vector<Ticket *> melangerTicket(vector<Ticket *> paquetTicket);
    void piocherTicket(Joueur &joueur);
    void piocherTrain(Joueur &joueur);
    Plateau *getPlateau();

    // CORRECTION : getter manquant pour accéder aux joueurs depuis l'extérieur
    const vector<Joueur *> &getJoueurs() const;

    ~Partie();
};