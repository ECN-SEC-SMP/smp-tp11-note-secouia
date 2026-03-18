#include "VoieFerree.hpp"

VoieFerree::VoieFerree(vector<Ville*> listeVille, couleurTrain couleur, int poids){
    this->listeVille = listeVille;

    assert(couleur != couleurTrain::MULTI);
    this->couleur = couleur;
    this->poids = poids;
    this->joueur = nullptr;
}


vector<Ville*> VoieFerree::getListeVille() const {
    return this->listeVille;
}

Joueur* VoieFerree::getJoueur() const {
    return this->joueur;
}

int VoieFerree::getPoids() const {
    return this->poids;
}

couleurTrain VoieFerree::getCouleur() const {
    return this->couleur;
}

void VoieFerree::setJoueur(Joueur* joueur) {
    this->joueur = joueur;
}
