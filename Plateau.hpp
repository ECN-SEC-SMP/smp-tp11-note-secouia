#pragma once
#include "VoieFerree.hpp"
#include "Ville.hpp"
#include <limits>
#include <map>
#include <algorithm>



class Plateau {
    private:
        vector<VoieFerree*> listeVoieFerree; 
        vector<Ville*> listeVille;

        bool isVilleStringInVector(string ville, vector<Ville*> listeVille) const; //OK
        bool isVilleInVector(Ville* ville, vector<Ville*> listeVille) const;//OK
        Ville* getVilleFromString(string ville, vector<Ville*> v) const;//OK

    public:
        Plateau();//OK
        ~Plateau();//OK
        vector<VoieFerree*> getListeVoieFerree() const;//OK 
        vector<Ville*> getListeVille() const;//OK
        void placeWagon(string villeA, string villeB, Joueur* joueur, couleurTrain couleur); //OK
        bool existeChemin(string villeA, string villeB, Joueur* joueur) const; //OK
        bool existeTraversee(Joueur* joueur);  // TODO 

};