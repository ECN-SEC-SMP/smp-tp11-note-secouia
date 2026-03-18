#include <string>
#include <vector>
#include "Ville.hpp"
#include "Joueur.hpp"

using namespace std;


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
