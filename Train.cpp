#pragma once
#include <string>
#include "Train.hpp"

using namespace std;


Train::Train(enum couleurTrain couleur) {
    this->couleur = couleur;
}

enum couleurTrain Train::getCouleurTrain()const {
    return this->couleur;
}

Train::~Train() {
    // Destructeur vide, pas de ressources à libérer
}