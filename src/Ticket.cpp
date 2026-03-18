#include <string>
#include "Ticket.hpp"

using namespace std;


Ticket::Ticket(Ville* villeDepart, Ville* villeArrivee) : villeDepart(villeDepart), villeArrivee(villeArrivee) {}


Ticket::~Ticket(){
    // Destructeur vide, pas de ressources à libérer
}

Ville* Ticket::getVilleDepart() const {
    return this->villeDepart;
}

Ville* Ticket::getVilleArrivee() const {
    return this->villeArrivee;
}
