#include "Partie.hpp"

Partie::Partie() {}

Partie::~Partie() {
    for (Joueur *j : joueurs) delete j;
    for (Ticket *t : pioche) delete t;
    for (Train *t : piocheTrain) delete t;
}

vector<Joueur *> Partie::getJoueur() const {
    return joueurs;
}

Joueur *Partie::getJoueur(int index) const {
    return joueurs[index];
}
