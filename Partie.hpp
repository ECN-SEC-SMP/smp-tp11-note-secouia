#pragma once

#include <vector>
#include "Ticket.hpp"
#include "Joueur.hpp"
#include "Train.hpp"
#include "Plateau.hpp"

class Partie {
    private:
        vector<Ticket*> piocheTicket;
        bool grandeTraversee;
        vector<Train*> piocheTrain;
        vector<Joueur*> joueurs;
		Plateau* plateau;

    public:
        Partie();
        Partie(vector<Ticket*> piocheTicket, bool grandeTraversee, vector<Train*> piocheTrain, vector<Joueur*> joueurs);
        vector<Ticket*> melangerTicket(vector<Ticket*> paquetTicket);
		void piocherTicket(Joueur &joueur);
		void piocherTrain(Joueur &joueur);
		Plateau* getPlateau();
        vector<Joueur*> getJoueur() const;
        Joueur* getJoueur(int index) const;
        ~Partie();
};