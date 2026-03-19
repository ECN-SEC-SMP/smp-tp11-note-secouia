#pragma once 
#include <string>
#include "couleurEnum.hpp"

using namespace std;


class Train {
    private:
        enum couleurTrain couleur ;
    public:
        Train(enum couleurTrain couleur);
        ~Train();
        enum couleurTrain getCouleurTrain()const;
};


