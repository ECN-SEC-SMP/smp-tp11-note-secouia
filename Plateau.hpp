#include "VoieFerree.hpp"

class Plateau {
    private:
        vector<VoieFerree*> listeVoieFerree; 
    
    public:
        void placeWagon(string villeA, string villeB, Joueur joueur);
        bool existeChemin(string villeA, string villeB);
        bool existeTraversee();
};