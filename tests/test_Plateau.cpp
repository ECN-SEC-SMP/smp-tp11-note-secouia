/**
 * @file test_Plateau.cpp
 * @brief Tests unitaires Google Test pour la classe Plateau
 */

#include <gtest/gtest.h>

#include "Plateau.hpp"
#include "Ville.hpp"
#include "VoieFerree.hpp"
#include "Joueur.hpp"
#include "couleurEnum.hpp"

// ============================================================
//  Fixture commune
// ============================================================

class PlateauTest : public ::testing::Test
{
protected:
    Plateau *plateau = nullptr;
    Joueur *joueur   = nullptr;

    void SetUp() override
    {
        plateau = new Plateau();
        joueur  = new Joueur("Alice", couleurJoueur::ROUGE);
    }

    void TearDown() override
    {
        delete joueur;
        delete plateau;
    }
};

// ============================================================
//  Tests des getters
// ============================================================

TEST_F(PlateauTest, ConstructorInitialize)
{
    EXPECT_NE(plateau, nullptr);
}

TEST_F(PlateauTest, GetListeVille)
{
    vector<Ville*> villes = plateau->getListeVille();
    // Les villes peuvent être vides au début
    EXPECT_GE(villes.size(), 0);
}

TEST_F(PlateauTest, GetListeVoieFerree)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    // Les voies peuvent être vides au début
    EXPECT_GE(voies.size(), 0);
}

// ============================================================
//  Tests des pioches
// ============================================================

TEST_F(PlateauTest, GetPiocheTrain)
{
    vector<Train*> trains = plateau->getPiocheTrain();
    // getPiocheTrain crée 60 trains + 12 locomotives = 72 trains
    EXPECT_EQ(trains.size(), 72);
}

TEST_F(PlateauTest, GetPiocheTrainNonNull)
{
    vector<Train*> trains = plateau->getPiocheTrain();
    for (auto train : trains) {
        EXPECT_NE(train, nullptr);
    }
}

TEST_F(PlateauTest, GetPiocheTrainCouleurs)
{
    vector<Train*> trains = plateau->getPiocheTrain();
    bool hasJaune = false, hasVert = false, hasRouge = false, hasMulti = false;
    
    for (auto train : trains) {
        couleurTrain couleur = train->getCouleurTrain();
        if (couleur == couleurTrain::JAUNE) hasJaune = true;
        if (couleur == couleurTrain::VERT) hasVert = true;
        if (couleur == couleurTrain::ROUGE) hasRouge = true;
        if (couleur == couleurTrain::MULTI) hasMulti = true;
    }
    
    EXPECT_TRUE(hasJaune);
    EXPECT_TRUE(hasVert);
    EXPECT_TRUE(hasRouge);
    EXPECT_TRUE(hasMulti);
}

TEST_F(PlateauTest, GetPiocheTickets)
{
    vector<Ticket*> tickets = plateau->getPiocheTickets();
    // Les tickets sont chargés depuis le fichier CSV
    EXPECT_GE(tickets.size(), 0);
}

// ============================================================
//  Tests de stabilité
// ============================================================

TEST_F(PlateauTest, GetListeVilleStable)
{
    vector<Ville*> villes1 = plateau->getListeVille();
    vector<Ville*> villes2 = plateau->getListeVille();
    
    EXPECT_EQ(villes1.size(), villes2.size());
}

TEST_F(PlateauTest, GetListeVoieFerreeStable)
{
    vector<VoieFerree*> voies1 = plateau->getListeVoieFerree();
    vector<VoieFerree*> voies2 = plateau->getListeVoieFerree();
    
    EXPECT_EQ(voies1.size(), voies2.size());
}

// ============================================================
//  Tests de destruction
// ============================================================

TEST(PlateauDestruction, AllocationEtLiberation)
{
    Plateau *p = new Plateau();
    EXPECT_NE(p, nullptr);
    
    // Accès basique aux méthodes
    vector<Ville*> villes = p->getListeVille();
    vector<VoieFerree*> voies = p->getListeVoieFerree();
    
    delete p; // ne doit pas crasher
}

// ============================================================
//  Tests de contenu des pioches
// ============================================================

TEST_F(PlateauTest, PiocheTrainContientJaune)
{
    vector<Train*> trains = plateau->getPiocheTrain();
    bool found = false;
    for (auto train : trains) {
        if (train->getCouleurTrain() == couleurTrain::JAUNE) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(PlateauTest, PiocheTrainContientMulti)
{
    vector<Train*> trains = plateau->getPiocheTrain();
    int countMulti = 0;
    for (auto train : trains) {
        if (train->getCouleurTrain() == couleurTrain::MULTI) {
            countMulti++;
        }
    }
    // Il doit y avoir 12 locomotives MULTI
    EXPECT_EQ(countMulti, 12);
}

// ============================================================
//  Tests d'indépendance des instances
// ============================================================

TEST(PlateauMultiples, DeuxPlateauIndependants)
{
    Plateau *p1 = new Plateau();
    Plateau *p2 = new Plateau();

    vector<Train*> trains1 = p1->getPiocheTrain();
    vector<Train*> trains2 = p2->getPiocheTrain();

    // Les deux pioches doivent avoir la même taille
    EXPECT_EQ(trains1.size(), trains2.size());
    // Mais les pointeurs doivent être différents
    EXPECT_NE(trains1[0], trains2[0]);

    delete p2;
    delete p1;
}
