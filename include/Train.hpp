#pragma once 
#include <string>
#include "couleurEnum.hpp"

using namespace std;

/**
 * @brief Représente un train dans le jeu
 * 
 */
class Train {
    private:
        enum couleurTrain couleur ;
    public:
        /**
         * @brief Construct a new Train object
         * 
         * @param couleur 
         */
        Train(enum couleurTrain couleur);
        /**
         * @brief Destroy the Train object
         * 
         */
        ~Train();
        /**
         * @brief Get the Couleur Train object
         * 
         * @return enum couleurTrain 
         */
        enum couleurTrain getCouleurTrain()const;
};


