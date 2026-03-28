/**
 * @file test_Train.cpp
 * @brief Tests unitaires Google Test pour la classe Train
 */

#include <gtest/gtest.h>

#include "Train.hpp"
#include "couleurEnum.hpp"

// ============================================================
//  Tests par couleur – construction & getCouleurTrain
// ============================================================

TEST(TrainCouleur, Rouge)
{
    Train t(couleurTrain::ROUGE);
    EXPECT_EQ(t.getCouleurTrain(), couleurTrain::ROUGE);
}

TEST(TrainCouleur, Bleu)
{
    Train t(couleurTrain::BLEU);
    EXPECT_EQ(t.getCouleurTrain(), couleurTrain::BLEU);
}

TEST(TrainCouleur, Vert)
{
    Train t(couleurTrain::VERT);
    EXPECT_EQ(t.getCouleurTrain(), couleurTrain::VERT);
}

TEST(TrainCouleur, Jaune)
{
    Train t(couleurTrain::JAUNE);
    EXPECT_EQ(t.getCouleurTrain(), couleurTrain::JAUNE);
}

TEST(TrainCouleur, Blanc)
{
    Train t(couleurTrain::BLANC);
    EXPECT_EQ(t.getCouleurTrain(), couleurTrain::BLANC);
}

TEST(TrainCouleur, Noir)
{
    Train t(couleurTrain::NOIR);
    EXPECT_EQ(t.getCouleurTrain(), couleurTrain::NOIR);
}

TEST(TrainCouleur, Multi)
{
    Train t(couleurTrain::MULTI);
    EXPECT_EQ(t.getCouleurTrain(), couleurTrain::MULTI);
}

// ============================================================
//  Trains alloués sur le tas
// ============================================================

TEST(TrainHeap, AllocationEtLiberation)
{
    Train *t = new Train(couleurTrain::ROUGE);
    EXPECT_EQ(t->getCouleurTrain(), couleurTrain::ROUGE);
    delete t; // ne doit pas crasher
}

// ============================================================
//  Plusieurs trains – indépendance des couleurs
// ============================================================

TEST(TrainMultiples, CouleursDifferentes)
{
    Train t1(couleurTrain::ROUGE);
    Train t2(couleurTrain::BLEU);
    Train t3(couleurTrain::MULTI);

    EXPECT_NE(t1.getCouleurTrain(), t2.getCouleurTrain());
    EXPECT_NE(t1.getCouleurTrain(), t3.getCouleurTrain());
    EXPECT_NE(t2.getCouleurTrain(), t3.getCouleurTrain());
}

TEST(TrainMultiples, MememeCouleur)
{
    Train t1(couleurTrain::VERT);
    Train t2(couleurTrain::VERT);

    EXPECT_EQ(t1.getCouleurTrain(), t2.getCouleurTrain());
}

// ============================================================
//  Couverture complète de l'enum couleurTrain
// ============================================================

TEST(TrainEnum, ToutesLesCouleurs)
{
    const std::vector<couleurTrain> toutes = {
        couleurTrain::JAUNE,
        couleurTrain::ROUGE,
        couleurTrain::VERT,
        couleurTrain::BLEU,
        couleurTrain::BLANC,
        couleurTrain::NOIR,
        couleurTrain::MULTI
    };

    for (auto c : toutes) {
        Train t(c);
        EXPECT_EQ(t.getCouleurTrain(), c);
    }
}

// ============================================================
//  Pioche simulée (vecteur de trains)
// ============================================================

TEST(TrainPioche, CompterParCouleur)
{
    // Reproduit la logique de creerTrains() de main.cpp :
    // 10 cartes par couleur (6 couleurs) + 12 locomotives = 72 cartes
    std::vector<Train *> pile;

    const std::vector<couleurTrain> couleurs = {
        couleurTrain::JAUNE, couleurTrain::ROUGE, couleurTrain::VERT,
        couleurTrain::BLEU,  couleurTrain::BLANC, couleurTrain::NOIR
    };

    for (auto c : couleurs)
        for (int i = 0; i < 10; i++)
            pile.push_back(new Train(c));

    for (int i = 0; i < 12; i++)
        pile.push_back(new Train(couleurTrain::MULTI));

    EXPECT_EQ(pile.size(), 72u);

    int nbMulti = 0;
    for (auto *t : pile)
        if (t->getCouleurTrain() == couleurTrain::MULTI) nbMulti++;

    EXPECT_EQ(nbMulti, 12);

    int nbRouge = 0;
    for (auto *t : pile)
        if (t->getCouleurTrain() == couleurTrain::ROUGE) nbRouge++;

    EXPECT_EQ(nbRouge, 10);

    for (auto *t : pile) delete t;
}
