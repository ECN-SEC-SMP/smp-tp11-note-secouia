#pragma once
#include <string>

using namespace std;

class Ville {
    private:
        string nom;
    
    public:
        Ville(string nom);
        string getNom() const; 

};