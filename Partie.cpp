#include <algorithm>
#include <random>
#include <vector>

#include "Partie.hpp"
#include "Joueur.hpp"
#include "Ticket.hpp"
#include "Train.hpp"
#include "Plateau.hpp"

Partie::Partie(vector<Ticket*> piocheTicket, bool grandeTraversee, vector<Train*> piocheTrain, vector<Joueur*> joueurs){
	// On mélange les cartes
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(piocheTicket.begin(), piocheTicket.end(), g);
	this->piocheTicket = piocheTicket;
	this->grandeTraversee = grandeTraversee;
	std::shuffle(piocheTrain.begin(), piocheTrain.end(), g);
	this->piocheTrain = piocheTrain;
    this->joueurs = joueurs;
	for (auto joueur : joueurs){

		// Distribution des 4 trains
		for (int j = 0; j < 4; j++){
			this->piocherTrain(*joueur);
		}

		// Distribution des 2 tickets
		for (int j = 0; j < 2; j++){
			this->piocherTicket(*joueur);
		}
	}
	this->plateau = new Plateau();
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