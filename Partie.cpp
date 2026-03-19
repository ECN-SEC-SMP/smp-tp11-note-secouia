#include <algorithm>
#include <random>
#include <vector>

#include "Partie.hpp"
#include "Joueur.hpp"
#include "Ticket.hpp"
#include "Train.hpp"
#include "Plateau.hpp

Partie::Partie(bool grandeTraversee, vector<Train*> piocheTrain, vector<Joueur*> joueurs, Plateau* plateau) {
	this->plateau = new Plateau();

	// On mélange les cartes
    std::random_device rd;
    std::mt19937 g(rd());
    this->piocheTicket = std::shuffle(this->plateau.getPiocheTicket.begin(), this->plateau.getPiocheTicket.end(), g);
    this->grandeTraversee = grandeTraversee;
    this->piocheTrain = std::shuffle(piocheTrain.begin(), piocheTrain.end(), g);
    this->joueurs = joueurs;
	for (auto joueur : joueurs){

		// Distribution des 4 trains
		for (int j = 0; j < 4; j++){
			this.piocherTrain(joueur));
		}

		// Distribution des 2 tickets
		for (int j = 0; j < 2; j++){
			this.piocherTicket(joueur);
		}
	}
}

void Partie::piocherTicket(Joueur &joueur){
	Ticket* ticketPioche = this->piocheTicket.front();
	this->piocheTicket.erase(this->piocheTicket.begin());
	joueur.ajouterTicket(ticketPioche);
}

void Partie::piocherTrain(Joueur &joueur){
	Train* trainPioche = this->piocheTrain.front();
	this->piocheTrain.erase(this->piocheTrain.begin());
	joueur.ajouterCarte(trainPioche);
}

Plateau* Partie::getPlateau(){
	return this->plateau;
}