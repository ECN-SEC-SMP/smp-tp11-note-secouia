#include "Joueur.hpp"

Joueur::Joueur(const std::string &nom, couleurJoueur couleur) : nom(nom), couleur(couleur), nbWagons(20), ticketFini(0) {}

Joueur::~Joueur() = default;

const std::string &Joueur::getNom() const
{
  return nom;
}

couleurJoueur Joueur::getCouleur() const
{
  return couleur;
}

int Joueur::getNbWagons() const
{
  return nbWagons;
}

int Joueur::getTicketFini() const
{
  return ticketFini;
}

const std::vector<Ticket *> &Joueur::getMissions() const
{
  return missions;
}

int Joueur::getNbCartes(couleurTrain couleur) const
{
  int count = 0;
  for (const auto &carte : mainCartes)
  {
    if (carte->getCouleurTrain() == couleur)
    {
      count++;
    }
  }
  return count;
}

int Joueur::getNbCartesTotales() const
{
  return mainCartes.size();
}

const std::vector<Train *> &Joueur::getMainCartes() const
{
  return mainCartes;
}

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
    {
      it++;
    }
  }
  return count == quantite;
}

bool Joueur::peutPrendreVoie(couleurTrain couleurVoie, int longueur) const
{
  int cartesDisponibles = getNbCartes(couleurVoie) + getNbCartes(couleurTrain::MULTI);
  return cartesDisponibles >= longueur;
}

bool Joueur::defausserCartesVoie(couleurTrain couleurVoie, int longueur)
{
  if (!peutPrendreVoie(couleurVoie, longueur))
  {
    return false;
  }

  int cartesAUtiliser = std::min(getNbCartes(couleurVoie), longueur);
  int cartesMultiAUtiliser = longueur - cartesAUtiliser;

  retirerCartes(couleurVoie, cartesAUtiliser);
  retirerCartes(couleurTrain::MULTI, cartesMultiAUtiliser);

  return true;
}

bool Joueur::utiliserWagons(int nb)
{
  if (nbWagons >= nb)
  {
    nbWagons -= nb;
    return true;
  }
  return false;
}

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

bool Joueur::validerTicket(Ticket *ticket, Plateau &plateau) const
{
  return plateau.existeChemin(ticket->getVilleDepart()->getNom(), ticket->getVilleArrivee()->getNom());
}

bool Joueur::aGagne() const
{
  return ticketFini >= 3; // Par exemple, un joueur gagne s'il a complété au moins 3 tickets
}

bool Joueur::naPlusDeWagons() const
{
  return nbWagons <= 0;
}

std::ostream &operator<<(std::ostream &os, const Joueur &joueur)
{
  os << "Joueur: " << joueur.getNom() << " | Couleur: " << static_cast<int>(joueur.getCouleur()) << " | Wagons restants: " << joueur.getNbWagons() << " | Tickets complétés: " << joueur.getTicketFini();
  return os;
}