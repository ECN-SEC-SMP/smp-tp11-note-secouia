#include "Joueur.hpp"
#include <algorithm>
#include <ostream>

Joueur::Joueur(const std::string &nom, couleurJoueur couleur)
    : nom(nom), couleur(couleur), ticketFini(0), nbWagons(45) {}

const std::string &Joueur::getNom() const { return nom; }
couleurJoueur Joueur::getCouleur() const { return couleur; }
int Joueur::getNbWagons() const { return nbWagons; }
int Joueur::getTicketFini() const { return ticketFini; }
const std::vector<Ticket *> &Joueur::getMissions() const { return missions; }
const std::vector<Train *> &Joueur::getMainCartes() const { return mainCartes; }

int Joueur::getNbCartes(couleurTrain couleur) const {
    int count = 0;
    for (Train *t : mainCartes) {
        if (t->getCouleurTrain() == couleur) count++;
    }
    return count;
}

int Joueur::getNbCartesTotales() const {
    return (int)mainCartes.size();
}

void Joueur::ajouterCarte(Train *carte) {
    mainCartes.push_back(carte);
}

bool Joueur::retirerCartes(couleurTrain couleur, int quantite) {
    if (getNbCartes(couleur) < quantite) return false;
    int removed = 0;
    for (auto it = mainCartes.begin(); it != mainCartes.end() && removed < quantite;) {
        if ((*it)->getCouleurTrain() == couleur) {
            it = mainCartes.erase(it);
            removed++;
        } else {
            ++it;
        }
    }
    return true;
}

bool Joueur::peutPrendreVoie(couleurTrain couleurVoie, int longueur) const {
    if (nbWagons < longueur) return false;
    int colored = getNbCartes(couleurVoie);
    int loco = getNbCartes(couleurTrain::MULTI);
    return colored + loco >= longueur;
}

bool Joueur::defausserCartesVoie(couleurTrain couleurVoie, int longueur) {
    if (!peutPrendreVoie(couleurVoie, longueur)) return false;
    int colored = getNbCartes(couleurVoie);
    int toRemoveColored = std::min(colored, longueur);
    retirerCartes(couleurVoie, toRemoveColored);
    int remaining = longueur - toRemoveColored;
    if (remaining > 0) retirerCartes(couleurTrain::MULTI, remaining);
    return true;
}

bool Joueur::utiliserWagons(int nb) {
    if (nbWagons < nb) return false;
    nbWagons -= nb;
    return true;
}

void Joueur::ajouterTicket(Ticket *ticket) {
    missions.push_back(ticket);
}

std::vector<Ticket *> Joueur::defausserTickets() {
    std::vector<Ticket *> defausse = missions;
    missions.clear();
    return defausse;
}

bool Joueur::validerTicket(Ticket *ticket, const Plateau &plateau) const {
    return plateau.existeChemin(
        ticket->getVilleDepart()->getNom(),
        ticket->getVilleArrivee()->getNom(),
        const_cast<Joueur *>(this)
    );
}

bool Joueur::aGagne() const {
    return ticketFini >= 6;
}

bool Joueur::naPlusDeWagons() const {
    return nbWagons <= 2;
}

std::ostream &operator<<(std::ostream &os, const Joueur &joueur) {
    os << joueur.getNom()
       << " (" << joueur.getNbWagons() << " wagons, "
       << joueur.getTicketFini() << " tickets finis)";
    return os;
}
