#pragma once 
#include <string>
#include "Ville.hpp"

using namespace std;


class Ticket {
    private:
        Ville* villeDepart;
        Ville* villeArrivee;
    public:
        Ticket(Ville* villeDepart, Ville* villeArrivee);
        ~Ticket();
        Ville* getVilleDepart() const;
        Ville* getVilleArrivee() const;
};


