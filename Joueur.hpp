/**
 * @file Joueur.hpp
 * @author Thomas Metais (metais.thomas92@gmail.com)
 * @brief Définition de la classe Joueur pour le jeu de société "Les Aventuriers du Rail"
 * @version 0.1
 * @date 2026-03-17
 */
#pragma once

#include "couleurEnum.hpp"
#include "Ticket.hpp"
#include "Train.hpp"
#include "Plateau.hpp"

#include <vector>
#include <string>
#include <ostream>

class Joueur
{
private:
    int ticketFini;
    int nbWagons;
    std::vector<Ticket *> missions;
    couleurJoueur couleur;
    std::string nom;
    std::vector<Train *> mainCartes;

public:
    Joueur(const std::string &nom, couleurJoueur couleur);
    ~Joueur() = default;

    // accesseurs
    const std::string &getNom() const;
    couleurJoueur getCouleur() const;
    int getNbWagons() const;
    int getTicketFini() const;
    const std::vector<Ticket *> &getMissions() const;
    int getNbCartes(couleurTrain couleur) const;
    int getNbCartesTotales() const;
    const std::vector<Train *> &getMainCartes() const;

    // gestion des cartes
    void ajouterCarte(Train *carte);
    bool retirerCartes(couleurTrain couleur, int quantite);
    bool peutPrendreVoie(couleurTrain couleurVoie, int longueur) const;
    bool defausserCartesVoie(couleurTrain couleurVoie, int longueur);

    // gestion des wagons
    bool utiliserWagons(int nb);

    // gestion des tickets
    void ajouterTicket(Ticket *ticket);
    std::vector<Ticket *> defausserTickets();
    bool validerTicket(Ticket *ticket, const Plateau &plateau) const;

    // fin de partie
    bool aGagne() const;
    bool naPlusDeWagons() const;
};

std::ostream &operator<<(std::ostream &os, const Joueur &joueur);