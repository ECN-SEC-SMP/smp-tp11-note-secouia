/**
 * @file Joueur.cpp
 * @author Thomas Metais (metais.thomas92@gmail.com)
 * @brief Implémentation de la classe @ref Joueur.
 * @version 1.0
 * @date 2026-03-17
 *
 * @details
 * Toute la documentation contractuelle (pré/post-conditions, invariants,
 * complexité, valeurs de retour) se trouve dans `Joueur.hpp`. Ce fichier
 * ajoute, via des blocs `@details`, les notes d'implémentation pertinentes
 * (choix d'algorithme, idiomes STL employés, subtilités de manipulation).
 *
 * @see Joueur.hpp
 */
#include "Joueur.hpp"
#include "Plateau.hpp"
#include <algorithm>

/**
 * @details
 * `missions` et `mainCartes` sont laissés vides grâce au constructeur par
 * défaut de `std::vector`. L'ordre des initialiseurs respecte l'ordre de
 * déclaration des membres dans `Joueur.hpp` afin d'éviter l'avertissement
 * `-Wreorder-ctor`.
 */
Joueur::Joueur(const std::string &nom, couleurJoueur couleur)
    : ticketFini(0), nbWagons(20), couleur(couleur), nom(nom) {}

// ===== Accesseurs =====

const std::string &Joueur::getNom() const { return nom; }

couleurJoueur Joueur::getCouleur() const { return couleur; }

int Joueur::getNbWagons() const { return nbWagons; }

int Joueur::getTicketFini() const { return ticketFini; }

const std::vector<Ticket *> &Joueur::getMissions() const { return missions; }

const std::vector<Train *> &Joueur::getMainCartes() const { return mainCartes; }

/**
 * @details
 * Parcours linéaire de la main : aucune structure indexée par couleur n'est
 * maintenue, la main restant suffisamment petite en pratique pour que la
 * simplicité d'implémentation l'emporte sur l'optimisation.
 */
int Joueur::getNbCartes(couleurTrain couleur) const
{
  int count = 0;
  for (const auto &carte : mainCartes)
    if (carte->getCouleurTrain() == couleur)
      count++;
  return count;
}

int Joueur::getNbCartesTotales() const
{
  // NOTE: cast explicite size_t -> int pour éviter le warning -Wsign-compare.
  return static_cast<int>(mainCartes.size());
}

// ===== Gestion des cartes =====

void Joueur::ajouterCarte(Train *carte)
{
  mainCartes.push_back(carte);
}

/**
 * @details
 * Itère une unique fois sur la main et s'arrête dès que `quantite` cartes
 * ont été retirées. `std::vector::erase` invalide les itérateurs suivants :
 * on réutilise donc la valeur de retour de `erase`, qui pointe sur l'élément
 * suivant, pour continuer le parcours sans revalider.
 */
bool Joueur::retirerCartes(couleurTrain couleur, int quantite)
{
  int count = 0;
  for (auto it = mainCartes.begin(); it != mainCartes.end() && count < quantite;)
  {
    if ((*it)->getCouleurTrain() == couleur)
    {
      it = mainCartes.erase(it);
      count++;
    }
    else
      ++it;
  }
  return count == quantite;
}

bool Joueur::peutPrendreVoie(couleurTrain couleurVoie, int longueur) const
{
  return getNbCartes(couleurVoie) + getNbCartes(couleurTrain::MULTI) >= longueur;
}

/**
 * @details
 * Stratégie : consommer d'abord les cartes de la couleur demandée (dans la
 * limite de `longueur`) puis compléter avec des @ref couleurTrain::MULTI.
 * Les MULTI étant des jokers rares, ils sont ainsi préservés au maximum.
 */
bool Joueur::defausserCartesVoie(couleurTrain couleurVoie, int longueur)
{
  if (!peutPrendreVoie(couleurVoie, longueur))
    return false;

  int nbCouleur = std::min(getNbCartes(couleurVoie), longueur);
  retirerCartes(couleurVoie, nbCouleur);
  retirerCartes(couleurTrain::MULTI, longueur - nbCouleur);
  return true;
}

// ===== Gestion des wagons =====

bool Joueur::utiliserWagons(int nb)
{
  if (nbWagons < nb)
    return false;
  nbWagons -= nb;
  return true;
}

// ===== Gestion des tickets =====

void Joueur::ajouterTicket(Ticket *ticket)
{
  missions.push_back(ticket);
}

/**
 * @details
 * Copie les pointeurs dans le vecteur retourné puis vide `missions`. Aucun
 * @ref Ticket n'est détruit : la propriété reste à @ref Partie.
 */
std::vector<Ticket *> Joueur::defausserTickets()
{
  std::vector<Ticket *> defausse = missions;
  missions.clear();
  return defausse;
}

/**
 * @details
 * Utilise l'idiome *erase-remove* pour retirer `ticket` de @ref missions.
 * Si le pointeur n'y figure pas, `std::remove` renvoie `missions.end()` et
 * aucun élément n'est supprimé — la précondition documentée dans le header
 * empêche toutefois ce cas.
 */
bool Joueur::validerTicket(Ticket *ticket, Plateau &plateau)
{
  if (!plateau.existeChemin(ticket->getVilleDepart()->getNom(),
                            ticket->getVilleArrivee()->getNom(),
                            this))
    return false;

  ticketFini++;
  missions.erase(std::remove(missions.begin(), missions.end(), ticket), missions.end());
  return true;
}

// ===== Fin de partie =====

bool Joueur::aGagne() const { return ticketFini >= 6; }

bool Joueur::naPlusDeWagons() const { return nbWagons <= 0; }

// ===== Affichage =====

/**
 * @details
 * Chaque couleur est représentée par la valeur numérique de l'énumération
 * @ref couleurTrain (l'affichage détaillé par nom reste à implémenter côté UI).
 */
void Joueur::afficherMain(std::ostream &os) const
{
  os << "Main de " << nom << " (" << mainCartes.size() << " cartes) : ";
  for (const auto &carte : mainCartes)
    os << static_cast<int>(carte->getCouleurTrain()) << " ";
  os << std::endl;
}

std::ostream &operator<<(std::ostream &os, const Joueur &joueur)
{
  os << "Joueur: " << joueur.getNom()
     << " | Couleur: " << static_cast<int>(joueur.getCouleur())
     << " | Wagons: " << joueur.getNbWagons()
     << " | Tickets: " << joueur.getTicketFini();
  return os;
}
