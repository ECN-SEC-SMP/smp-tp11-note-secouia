#include "Ville.hpp"

Ville::Ville(string nom){
    this->nom= nom;
}

string Ville::getNom() const {
    return this->nom;
}