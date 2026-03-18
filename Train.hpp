#pragma once 
#include <string>
#include "couleurEnum.hpp"

using namespace std;


class Train {
    private:
        enum couleurTrain couleur ;
    public:
        enum couleurTrain getCouleurTrain(string couleur);
        Train(enum couleurTrain couleur);
        ~Train();
};


