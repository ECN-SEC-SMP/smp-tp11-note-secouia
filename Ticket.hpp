#pragma once 
#include <string>
#include "Ville.hpp"

using namespace std;


class Train {
    private:
        Ville* villeDepart;
        Ville* villeArrivee;
    public:
        Train(Ville* villeDepart, Ville* villeArrivee);
        ~Train();
};


