/**
 * @author Aymeric ZIMINSKI
 * @file Plateau.hpp
 * @brief Classe Plateau – Gère le plateau de jeu avec villes et voies ferrées
 */

#pragma once
#include "VoieFerree.hpp"
#include "Ville.hpp"
#include <limits>
#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Ticket.hpp"
#include "Train.hpp"

/**
 * @class Plateau
 * @brief Représente le plateau de jeu avec les villes et les voies ferrées
 * 
 * Gère l'ensemble des villes, des voies ferrées et les interactions
 * entre les joueurs et le plateau (placement de wagons, vérification de chemins).
 */
class Plateau {
    private:
        /** @brief Liste des voies ferrées du plateau */
        vector<VoieFerree*> listeVoieFerree; 
        
        /** @brief Liste des villes du plateau */
        vector<Ville*> listeVille;

        /**
         * @brief Vérifie si une ville (chaîne) existe dans un vecteur de Ville*
         * @param ville Nom de la ville à chercher
         * @param listeVille Vecteur des villes à explorer
         * @return true si la ville existe, false sinon
         */
        bool isVilleStringInVector(string ville, vector<Ville*> listeVille) const;
        
        /**
         * @brief Vérifie si un pointeur Ville* existe dans un vecteur
         * @param ville Pointeur sur la Ville à chercher
         * @param listeVille Vecteur des villes à explorer
         * @return true si la ville existe, false sinon
         */
        bool isVilleInVector(Ville* ville, vector<Ville*> listeVille) const;
        
        /**
         * @brief Récupère un pointeur Ville à partir de son nom
         * @param ville Nom de la ville recherchée
         * @param v Vecteur des villes
         * @return Pointeur sur la Ville trouvée, nullptr sinon
         */
        Ville* getVilleFromString(string ville, vector<Ville*> v) const;

    public:
        /**
         * @brief Constructeur du Plateau
         * 
         * Initialise le plateau en chargant les villes et voies ferrées
         * depuis le fichier de configuration.
         */
        Plateau();
        
        /**
         * @brief Destructeur du Plateau
         * 
         * Libère la mémoire allouée pour les villes et voies ferrées.
         */
        ~Plateau();
        
        /**
         * @brief Retourne la liste des voies ferrées du plateau
         * @return Vecteur des pointeurs sur VoieFerree
         */
        vector<VoieFerree*> getListeVoieFerree() const;
        
        /**
         * @brief Retourne la liste des villes du plateau
         * @return Vecteur des pointeurs sur Ville
         */
        vector<Ville*> getListeVille() const;
        
        /**
         * @brief Place un wagon sur une voie ferrée
         * @param villeA Nom de la première ville
         * @param villeB Nom de la deuxième ville
         * @param joueur Pointeur sur le joueur qui place le wagon
         * @param couleur Couleur du train à placer
         */
        void placeWagon(string villeA, string villeB, Joueur* joueur, couleurTrain couleur);
        
        /**
         * @brief Vérifie s'il existe un chemin entre deux villes pour un joueur
         * @param villeA Nom de la première ville
         * @param villeB Nom de la deuxième ville
         * @param joueur Pointeur sur le joueur
         * @return true si un chemin existe, false sinon
         */
        bool existeChemin(string villeA, string villeB, Joueur* joueur) const;
        
        /**
         * @brief Vérifie s'il existe une traversée disponible pour un joueur
         * @param joueur Pointeur sur le joueur
         * @return true si une traversée existe, false sinon
         * @todo Implémentation à compléter
         */
        bool existeTraversee(Joueur* joueur);
        
        /**
         * @brief Retourne la pioche de tickets
         * @return Vecteur des pointeurs sur Ticket disponibles
         */
        vector<Ticket*> getPiocheTickets();
        
        /**
         * @brief Retourne la pioche de trains
         * @return Vecteur des pointeurs sur Train disponibles
         */
        vector<Train*> getPiocheTrain();

};