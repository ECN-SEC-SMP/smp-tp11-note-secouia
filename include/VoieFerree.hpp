#pragma once
#include <string>
#include <vector>
#include <assert.h>
#include "Ville.hpp"
#include "Joueur.hpp"
#include "couleurEnum.hpp"

using namespace std;


class VoieFerree {
    private: 
        couleurTrain couleur;
        int poids; 
        vector<Ville*> listeVille;
        Joueur* joueur;
    public:
        VoieFerree(vector<Ville*> listeVille, couleurTrain couleur, int poids);
        vector<Ville*> getListeVille() const;
        Joueur* getJoueur() const;
        int getPoids() const;
        couleurTrain getCouleur() const;
        void setJoueur(Joueur* joueur);

};
