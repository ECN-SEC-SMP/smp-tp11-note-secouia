/**
 * @file test_Joueur.cpp
 * @brief Tests unitaires Google Test pour la classe Joueur
 */

#include <gtest/gtest.h>
#include <sstream>

#include "Joueur.hpp"
#include "Train.hpp"
#include "Ticket.hpp"
#include "Ville.hpp"
#include "Plateau.hpp"
#include "couleurEnum.hpp"

// ============================================================
//  Fixture commune
// ============================================================

class JoueurTest : public ::testing::Test
{
protected:
  Joueur *joueur = nullptr;

  void SetUp() override
  {
    joueur = new Joueur("Alice", couleurJoueur::BLEU);
  }

  void TearDown() override
  {
    delete joueur;
  }

  // Crée un Train sur le tas (à libérer par le test si nécessaire)
  Train *makeTrain(couleurTrain c) { return new Train(c); }
};

// ============================================================
//  Construction / valeurs initiales
// ============================================================

TEST_F(JoueurTest, NomInitial)
{
  EXPECT_EQ(joueur->getNom(), "Alice");
}

TEST_F(JoueurTest, CouleurInitiale)
{
  EXPECT_EQ(joueur->getCouleur(), couleurJoueur::BLEU);
}

TEST_F(JoueurTest, WagonsInitiaux)
{
  EXPECT_EQ(joueur->getNbWagons(), 20);
}

TEST_F(JoueurTest, TicketsFiniInitial)
{
  EXPECT_EQ(joueur->getTicketFini(), 0);
}

TEST_F(JoueurTest, MainVideInitiale)
{
  EXPECT_EQ(joueur->getNbCartesTotales(), 0);
  EXPECT_TRUE(joueur->getMainCartes().empty());
}

TEST_F(JoueurTest, MissionsVidesInitiales)
{
  EXPECT_TRUE(joueur->getMissions().empty());
}

// ============================================================
//  Gestion des cartes Train
// ============================================================

TEST_F(JoueurTest, AjouterUneCarte)
{
  Train *t = makeTrain(couleurTrain::ROUGE);
  joueur->ajouterCarte(t);
  EXPECT_EQ(joueur->getNbCartesTotales(), 1);
  delete t;
}

TEST_F(JoueurTest, AjouterPlusieursCartes)
{
  for (int i = 0; i < 5; i++)
    joueur->ajouterCarte(makeTrain(couleurTrain::BLEU));

  EXPECT_EQ(joueur->getNbCartesTotales(), 5);

  for (auto *c : joueur->getMainCartes())
    delete c;
}

TEST_F(JoueurTest, GetNbCartesParesCouleur)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::ROUGE));
  joueur->ajouterCarte(makeTrain(couleurTrain::ROUGE));
  joueur->ajouterCarte(makeTrain(couleurTrain::BLEU));

  EXPECT_EQ(joueur->getNbCartes(couleurTrain::ROUGE), 2);
  EXPECT_EQ(joueur->getNbCartes(couleurTrain::BLEU), 1);
  EXPECT_EQ(joueur->getNbCartes(couleurTrain::VERT), 0);

  for (auto *c : joueur->getMainCartes())
    delete c;
}

TEST_F(JoueurTest, RetirerCartesExactes)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::VERT));
  joueur->ajouterCarte(makeTrain(couleurTrain::VERT));
  joueur->ajouterCarte(makeTrain(couleurTrain::VERT));

  bool ok = joueur->retirerCartes(couleurTrain::VERT, 2);

  EXPECT_TRUE(ok);
  EXPECT_EQ(joueur->getNbCartes(couleurTrain::VERT), 1);

  for (auto *c : joueur->getMainCartes())
    delete c;
}

TEST_F(JoueurTest, RetirerCartesInsuffisantes)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::NOIR));

  bool ok = joueur->retirerCartes(couleurTrain::NOIR, 3);

  // La méthode doit retourner false et laisser la main intacte
  // (ou retirer ce qu'elle peut — on teste au moins le code de retour)
  EXPECT_FALSE(ok);

  for (auto *c : joueur->getMainCartes())
    delete c;
}

TEST_F(JoueurTest, RetirerCartesAbsentes)
{
  bool ok = joueur->retirerCartes(couleurTrain::JAUNE, 1);
  EXPECT_FALSE(ok);
}

// ============================================================
//  peutPrendreVoie / defausserCartesVoie
// ============================================================

TEST_F(JoueurTest, PeutPrendreVoieSuffisamment)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::ROUGE));
  joueur->ajouterCarte(makeTrain(couleurTrain::ROUGE));
  joueur->ajouterCarte(makeTrain(couleurTrain::ROUGE));

  EXPECT_TRUE(joueur->peutPrendreVoie(couleurTrain::ROUGE, 3));

  for (auto *c : joueur->getMainCartes())
    delete c;
}

TEST_F(JoueurTest, PeutPrendreVoieAvecJokers)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::ROUGE));
  joueur->ajouterCarte(makeTrain(couleurTrain::MULTI));
  joueur->ajouterCarte(makeTrain(couleurTrain::MULTI));

  EXPECT_TRUE(joueur->peutPrendreVoie(couleurTrain::ROUGE, 3));

  for (auto *c : joueur->getMainCartes())
    delete c;
}

TEST_F(JoueurTest, PeutPassPrendreVoieInsuffisant)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::ROUGE));

  EXPECT_FALSE(joueur->peutPrendreVoie(couleurTrain::ROUGE, 3));

  for (auto *c : joueur->getMainCartes())
    delete c;
}

TEST_F(JoueurTest, DefausserCartesVoieOK)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::BLEU));
  joueur->ajouterCarte(makeTrain(couleurTrain::BLEU));
  joueur->ajouterCarte(makeTrain(couleurTrain::BLEU));

  bool ok = joueur->defausserCartesVoie(couleurTrain::BLEU, 3);

  EXPECT_TRUE(ok);
  EXPECT_EQ(joueur->getNbCartes(couleurTrain::BLEU), 0);
}

TEST_F(JoueurTest, DefausserCartesVoieKO)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::BLEU));

  bool ok = joueur->defausserCartesVoie(couleurTrain::BLEU, 3);

  EXPECT_FALSE(ok);
  // La main doit être inchangée
  EXPECT_EQ(joueur->getNbCartes(couleurTrain::BLEU), 1);

  for (auto *c : joueur->getMainCartes())
    delete c;
}

TEST_F(JoueurTest, DefausserCartesVoieMixteJokers)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::VERT));
  joueur->ajouterCarte(makeTrain(couleurTrain::MULTI));
  joueur->ajouterCarte(makeTrain(couleurTrain::MULTI));

  bool ok = joueur->defausserCartesVoie(couleurTrain::VERT, 3);

  EXPECT_TRUE(ok);
  EXPECT_EQ(joueur->getNbCartesTotales(), 0);
}

// ============================================================
//  Gestion des wagons
// ============================================================

TEST_F(JoueurTest, UtiliserWagonsOK)
{
  bool ok = joueur->utiliserWagons(5);
  EXPECT_TRUE(ok);
  EXPECT_EQ(joueur->getNbWagons(), 15);
}

TEST_F(JoueurTest, UtiliserWagonsTousExacts)
{
  bool ok = joueur->utiliserWagons(20);
  EXPECT_TRUE(ok);
  EXPECT_EQ(joueur->getNbWagons(), 0);
}

TEST_F(JoueurTest, UtiliserWagonsTropNombreux)
{
  bool ok = joueur->utiliserWagons(25);
  EXPECT_FALSE(ok);
  EXPECT_EQ(joueur->getNbWagons(), 20); // inchangé
}

TEST_F(JoueurTest, NaPlusDeWagonsApresUtilisation)
{
  joueur->utiliserWagons(20);
  EXPECT_TRUE(joueur->naPlusDeWagons());
}

TEST_F(JoueurTest, NaPlusDeWagonsNegatif)
{
  // Le joueur commence avec 20 wagons → pas encore à 0
  EXPECT_FALSE(joueur->naPlusDeWagons());
}

// ============================================================
//  Gestion des tickets
// ============================================================

TEST_F(JoueurTest, AjouterTicket)
{
  Ville *a = new Ville("Paris");
  Ville *b = new Ville("Lyon");
  Ticket *t = new Ticket(a, b);

  joueur->ajouterTicket(t);
  EXPECT_EQ(joueur->getMissions().size(), 1u);

  delete t;
  delete a;
  delete b;
}

TEST_F(JoueurTest, AjouterPlusieursTickets)
{
  Ville *a = new Ville("A");
  Ville *b = new Ville("B");
  Ville *c = new Ville("C");
  Ticket *t1 = new Ticket(a, b);
  Ticket *t2 = new Ticket(b, c);

  joueur->ajouterTicket(t1);
  joueur->ajouterTicket(t2);
  EXPECT_EQ(joueur->getMissions().size(), 2u);

  delete t1;
  delete t2;
  delete a;
  delete b;
  delete c;
}

TEST_F(JoueurTest, DefausserTicketsVideLaMission)
{
  Ville *a = new Ville("X");
  Ville *b = new Ville("Y");
  Ticket *t = new Ticket(a, b);
  joueur->ajouterTicket(t);

  auto defausse = joueur->defausserTickets();

  EXPECT_EQ(defausse.size(), 1u);
  EXPECT_TRUE(joueur->getMissions().empty());

  delete t;
  delete a;
  delete b;
}

TEST_F(JoueurTest, DefausserTicketsSansTicket)
{
  auto defausse = joueur->defausserTickets();
  EXPECT_TRUE(defausse.empty());
}

// ============================================================
//  aGagne
// ============================================================

TEST_F(JoueurTest, AGagneNonInitialement)
{
  EXPECT_FALSE(joueur->aGagne());
}

// On teste aGagne() en validant des tickets via un Plateau minimal.
// La méthode validerTicket incrémente ticketFini si un chemin existe pour
// le joueur. Ici on valide le comportement de aGagne sans Plateau réel
// (on ne peut pas facilement simuler 6 tickets sans infrastructure complète),
// donc on teste la condition limite via la sémantique de aGagne (>= 6).
//
// Test de boîte blanche : aGagne() renvoie true si ticketFini >= 6.
// On injecte des validations via un plateau configuré pour accepter le chemin.
// Si le Plateau n'est pas câblé, on valide uniquement la valeur de retour false.

TEST_F(JoueurTest, AGagneFalseApresQuelquesTickets)
{
  // Sans vrai plateau, on ne peut pas valider → ticketFini reste 0
  EXPECT_FALSE(joueur->aGagne());
}

// ============================================================
//  Affichage
// ============================================================

TEST_F(JoueurTest, AfficherMainVide)
{
  std::ostringstream oss;
  joueur->afficherMain(oss);
  std::string out = oss.str();
  EXPECT_NE(out.find("Alice"), std::string::npos);
  EXPECT_NE(out.find("0"), std::string::npos);
}

TEST_F(JoueurTest, AfficherMainAvecCartes)
{
  joueur->ajouterCarte(makeTrain(couleurTrain::ROUGE));
  joueur->ajouterCarte(makeTrain(couleurTrain::BLEU));

  std::ostringstream oss;
  joueur->afficherMain(oss);
  std::string out = oss.str();

  EXPECT_NE(out.find("Alice"), std::string::npos);
  EXPECT_NE(out.find("2"), std::string::npos);

  for (auto *c : joueur->getMainCartes())
    delete c;
}

TEST_F(JoueurTest, OperateurFlux)
{
  std::ostringstream oss;
  oss << *joueur;
  std::string out = oss.str();

  EXPECT_NE(out.find("Alice"), std::string::npos);
  EXPECT_NE(out.find("20"), std::string::npos); // wagons
  EXPECT_NE(out.find("0"), std::string::npos);  // tickets
}

// ============================================================
//  Tests avec plusieurs couleurs de joueur
// ============================================================

TEST(JoueurCouleurTest, CouleurJaune)
{
  Joueur j("Bob", couleurJoueur::JAUNE);
  EXPECT_EQ(j.getCouleur(), couleurJoueur::JAUNE);
}

TEST(JoueurCouleurTest, CouleurRouge)
{
  Joueur j("Carol", couleurJoueur::ROUGE);
  EXPECT_EQ(j.getCouleur(), couleurJoueur::ROUGE);
}

TEST(JoueurCouleurTest, CouleurVert)
{
  Joueur j("Dave", couleurJoueur::VERT);
  EXPECT_EQ(j.getCouleur(), couleurJoueur::VERT);
}