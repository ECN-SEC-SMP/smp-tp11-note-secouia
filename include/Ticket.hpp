#pragma once 
#include <string>
#include "Ville.hpp"

using namespace std;

/**
 * @brief Représente un ticket dans le jeu
 * 
 */
class Ticket {
    private:
        Ville* villeDepart;
        Ville* villeArrivee;
    public:
        /**
         * @brief Construct a new Ticket object
         * 
         * @param villeDepart 
         * @param villeArrivee 
         */
        Ticket(Ville* villeDepart, Ville* villeArrivee);
        /**
         * @brief Destroy the Ticket object
         * 
         */
        ~Ticket();
        /**
         * @brief Get the Ville Depart object
         * 
         * @return Ville* 
         */
        Ville* getVilleDepart() const;
        /**
         * @brief Get the Ville Arrivee object
         * 
         * @return Ville* 
         */
        Ville* getVilleArrivee() const;
};


