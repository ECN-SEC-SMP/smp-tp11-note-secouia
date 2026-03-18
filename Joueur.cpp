#include "Joueur.hpp"
#include "Plateau.hpp"
#include <algorithm>

Joueur::Joueur(const std::string &nom, couleurJoueur couleur)
    : nom(nom), couleur(couleur), nbWagons(20), ticketFini(0) {}

Joueur::~Joueur() = default;

// ---- Getters ----

const std::string &Joueur::getNom() const { return nom; }

couleurJoueur Joueur::getCouleur() const { return couleur; }

int Joueur::getNbWagons() const { return nbWagons; }

int Joueur::getTicketFini() const { return ticketFini; }

const std::vector<Ticket *> &Joueur::getMissions() const { return missions; }

const std::vector<Train *> &Joueur::getMainCartes() const { return mainCartes; }

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
  return mainCartes.size();
}

// ---- Gestion des cartes ----

void Joueur::ajouterCarte(Train *carte)
{
  mainCartes.push_back(carte);
}

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

bool Joueur::defausserCartesVoie(couleurTrain couleurVoie, int longueur)
{
  if (!peutPrendreVoie(couleurVoie, longueur))
    return false;

  int nbCouleur = std::min(getNbCartes(couleurVoie), longueur);
  retirerCartes(couleurVoie, nbCouleur);
  retirerCartes(couleurTrain::MULTI, longueur - nbCouleur);
  return true;
}

// ---- Gestion des wagons ----

bool Joueur::utiliserWagons(int nb)
{
  if (nbWagons < nb)
    return false;
  nbWagons -= nb;
  return true;
}

// ---- Gestion des tickets ----

void Joueur::ajouterTicket(Ticket *ticket)
{
  missions.push_back(ticket);
}

std::vector<Ticket *> Joueur::defausserTickets()
{
  std::vector<Ticket *> defausse = missions;
  missions.clear();
  return defausse;
}

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

// ---- Fin de partie ----

bool Joueur::aGagne() const { return ticketFini >= 6; }

bool Joueur::naPlusDeWagons() const { return nbWagons <= 0; }

// ---- Affichage ----

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
