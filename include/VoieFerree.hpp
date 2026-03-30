/**
 * @file VoieFerree.hpp
 * @brief Classe VoieFerree – Représente une voie ferrée du plateau
 */

#pragma once
#include <string>
#include <vector>
#include <assert.h>
#include "Ville.hpp"
#include "couleurEnum.hpp"

using namespace std;

class Joueur;

/**
 * @class VoieFerree
 * @brief Représente une voie ferrée reliant deux villes
 * 
 * Une voie ferrée relie une ou plusieurs villes, possède une couleur,
 * un poids (nombre de wagons), et peut être contrôlée par un joueur.
 */
class VoieFerree {
    private: 
        /** @brief Couleur de la voie ferrée */
        couleurTrain couleur;
        
        /** @brief Nombre de wagons nécessaires pour contrôler la voie */
        int poids;
        
        /** @brief Vecteur des villes reliées par cette voie */
        vector<Ville*> listeVille;
        
        /** @brief Pointeur sur le joueur qui contrôle cette voie (nullptr si non contrôlée) */
        Joueur* joueur;
        
    public:
        /**
         * @brief Constructeur de VoieFerree
         * @param listeVille Vecteur des villes reliées par cette voie
         * @param couleur Couleur de la voie ferrée
         * @param poids Nombre de wagons nécessaires
         */
        VoieFerree(vector<Ville*> listeVille, couleurTrain couleur, int poids);
        
        /**
         * @brief Retourne la liste des villes reliées par cette voie
         * @return Vecteur des pointeurs sur Ville
         */
        vector<Ville*> getListeVille() const;
        
        /**
         * @brief Retourne le joueur qui contrôle cette voie
         * @return Pointeur sur le Joueur, nullptr si non contrôlée
         */
        Joueur* getJoueur() const;
        
        /**
         * @brief Retourne le poids de la voie (nombre de wagons)
         * @return Nombre de wagons nécessaires
         */
        int getPoids() const;
        
        /**
         * @brief Retourne la couleur de la voie ferrée
         * @return Couleur de la voie
         */
        couleurTrain getCouleur() const;
        
        /**
         * @brief Définit le joueur qui contrôle cette voie
         * @param joueur Pointeur sur le Joueur
         */
        void setJoueur(Joueur* joueur);

};
