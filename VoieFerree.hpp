#pragma once
#include <string>
#include <vector>
#include "Ville.hpp"

using namespace std;

class Joueur;


class VoieFerree {
    private: 
        string couleur; 
        int poids; 
        vector<Ville*> listeVille;
        Joueur* joueur;
    public:
        VoieFerree(vector<Ville*> listeVille, string couleur, int poids);
        vector<Ville*> getListeVille() const;
        
};
