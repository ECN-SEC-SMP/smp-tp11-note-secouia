#include <string>
#include <vector>
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
        VoieFerree(vector<Ville*> listeVille, string couleur, int poids);
        vector<Ville*> getListeVille() const;

};
