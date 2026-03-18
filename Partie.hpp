#pragma once

#include <vector>
#include "Ticket.hpp"
#include "Joueur.hpp"
#include "Train.hpp"

class Partie {
    private:
        vector<Ticket*> pioche;
        bool grandeTraversee;
        vector<Train*> piocheTrain;
        vector<Joueur*> joueurs;
    public:

};