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

        bool isVilleStringInVector(string ville, vector<Ville*> listeVille); //OK
        bool isVilleInVector(Ville* ville, vector<Ville*> listeVille);//OK
        Ville* getVilleFromString(string ville, vector<Ville*> v);//OK

    public:
        Plateau();//OK
        ~Plateau();//OK
        vector<VoieFerree*> getListeVoieFerree() const;//OK 
        vector<Ville*> getListeVille() const;//OK
        void placeWagon(string villeA, string villeB, Joueur* joueur, couleurTrain couleur); //OK
        bool existeChemin(string villeA, string villeB, Joueur* joueur); //OK
        bool existeTraversee(Joueur* joueur);  // TODO 

};