#include "Plateau.hpp"

bool Plateau::isVilleStringInVector(string ville, vector<Ville*> listeVille){
    for(int i = 0; i < listeVille.size(); i++){
        if(listeVille[i]->getNom() == ville){
            return true;
        }
    }
    return false;
}

bool Plateau::isVilleInVector(Ville* ville, vector<Ville*> listeVille){
    for(int i = 0; i < listeVille.size(); i++){
        if(listeVille[i] == ville){
            return true;
        }
    }
    return false;
}


Plateau::Plateau(){
    vector<VoieFerree*> v;
    this->listeVoieFerree = v;
}

Plateau::~Plateau() {
    for(int i = 0; i < this->listeVoieFerree.size() ; i++){
        delete this->listeVoieFerree[i];
    }
}

void Plateau::placeWagon(string villeA, string villeB, Joueur* joueur, couleurTrain couleur){
    bool isAnEdit = false;
    for(int i = 0; i < this->getListeVoieFerree().size(); i++){
        VoieFerree* current = this->getListeVoieFerree()[i];
        if(isVilleStringInVector(villeA, current->getListeVille())
            && isVilleStringInVector(villeB, current->getListeVille())
            && couleur == current->getCouleur()){

            if(current->getJoueur() != nullptr){
                // TODO : RAISE AN ERROR
            }

            current->setJoueur(joueur);
            isAnEdit = true;
        }
    }

    if(!isAnEdit){
        //TODO : RAISE AN ERROR
    }
}

Ville* Plateau::getVilleFromString(string ville, vector<Ville*> v){
    for(int i = 0; i < v.size(); i ++){
        if(v[i]->getNom() == ville){
            return v[i];
        }
    }

    //TODO : RAISE AN ERROR
}

bool verifDiskstra(vector<Ville*> listeVille, map<Ville*, int> distance, vector<Ville*> sommets_visites){
    vector<Ville*> v; 
    
    for(int i = 0; i < listeVille.size(); i++){
        auto it = find(sommets_visites.begin(), sommets_visites.end(), listeVille[i]);
        if(distance[listeVille[i]] != numeric_limits<int>::max()
            && it == sommets_visites.end()){
                v.push_back(listeVille[i]);
            }
    }
}

bool Plateau::existeChemin(string villeA, string villeB, Joueur* joueur){
    /* Initialisation du tableau distances */

    map<Ville*, int> distance;

    for(int i = 0; i < this->getListeVille().size() ; i++){
        distance[this->getListeVille()[i]] = numeric_limits<int>::max();
    }

    distance[getVilleFromString(villeA, this->getListeVille())] = 0; 

    vector<Ville*> sommets_visites; 
    while(verifDiskstra(this->getListeVille(), distance, sommets_visites)){
        int distance_minimale = numeric_limits<int>::max();
        Ville* t = nullptr;


        map<Ville*, int>::iterator it;
        for(it = distance.begin(); it != distance.end() ; it++){
            auto recherche = find(sommets_visites.begin(), sommets_visites.end(), it->first);

            if(recherche != sommets_visites.end() && distance[it->first] < distance_minimale){
                distance_minimale = distance[it->first];
                t = it->first;
            }
        }

        vector<Ville*> voisins; 
        map<Ville*, int> distanceDuVoisin; 


        // Recherche des voisins
        for(int i = 0 ; i < this->getListeVille().size(); i++){
            for(int j = 0 ; j < this->getListeVoieFerree().size(); j++){
                if(isVilleInVector(t, this->getListeVoieFerree()[j]->getListeVille()) 
                    && isVilleInVector(this->getListeVille()[i], this->getListeVoieFerree()[j]->getListeVille())
                    && this->getListeVoieFerree()[j]->getJoueur() == joueur){
                    
                    //La ville est dans les villes d'adjances
                    voisins.push_back(this->getListeVille()[i]);
                    distanceDuVoisin[this->getListeVille()[i]] = this->getListeVoieFerree()[j]->getPoids();
                }
            }
        }


        for(int i = 0 ; i < voisins.size(); i++){
            distance[voisins[i]] = min(distance[voisins[i]], distance[t] + distanceDuVoisin[voisins[i]]);
        }
        sommets_visites.push_back(t);

    }

    return distance[getVilleFromString(villeB, this->getListeVille())];
}

vector<VoieFerree*> Plateau::getListeVoieFerree() const{
    return this->listeVoieFerree;
}

vector<Ville*> Plateau::getListeVille() const {
    return this->listeVille;
}