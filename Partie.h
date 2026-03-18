#pragma once

#include <vector>
#include "Ticket.h"
#include "Joueur.h"
#include "Train.h"

class Partie {
    private:
        vector<Ticket*> pioche;
        bool grandeTraversee;
        vector<Train*> piocheTrain;
        vector<Joueur*> joueurs;
    public:

};