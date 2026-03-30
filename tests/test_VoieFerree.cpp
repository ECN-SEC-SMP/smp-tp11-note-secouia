/**
 * @file test_VoieFerree.cpp
 * @brief Tests unitaires Google Test pour la classe VoieFerree
 */

#include <gtest/gtest.h>

#include "VoieFerree.hpp"
#include "Ville.hpp"
#include "Joueur.hpp"
#include "couleurEnum.hpp"

// ============================================================
//  Fixture commune
// ============================================================

class VoieFerreeTest : public ::testing::Test
{
protected:
    Ville *paris      = nullptr;
    Ville *lyon       = nullptr;
    Ville *marseille  = nullptr;
    VoieFerree *voie  = nullptr;
    Joueur *joueur    = nullptr;

    void SetUp() override
    {
        paris      = new Ville("Paris");
        lyon       = new Ville("Lyon");
        marseille  = new Ville("Marseille");
        voie       = new VoieFerree({paris, lyon}, couleurTrain::ROUGE, 2);
        joueur     = new Joueur("Alice", couleurJoueur::ROUGE);
    }

    void TearDown() override
    {
        delete voie;
        delete joueur;
        delete paris;
        delete lyon;
        delete marseille;
    }
};

// ============================================================
//  Construction / getters de base
// ============================================================

TEST_F(VoieFerreeTest, GetListeVille)
{
    vector<Ville*> listeVille = voie->getListeVille();
    EXPECT_EQ(listeVille.size(), 2);
    EXPECT_EQ(listeVille[0], paris);
    EXPECT_EQ(listeVille[1], lyon);
}

TEST_F(VoieFerreeTest, GetCouleur)
{
    EXPECT_EQ(voie->getCouleur(), couleurTrain::ROUGE);
}

TEST_F(VoieFerreeTest, GetPoids)
{
    EXPECT_EQ(voie->getPoids(), 2);
}

TEST_F(VoieFerreeTest, GetJoueurInitial)
{
    EXPECT_EQ(voie->getJoueur(), nullptr);
}

// ============================================================
//  Setters – gestion du joueur
// ============================================================

TEST_F(VoieFerreeTest, SetJoueur)
{
    voie->setJoueur(joueur);
    EXPECT_EQ(voie->getJoueur(), joueur);
}

TEST_F(VoieFerreeTest, SetJoueurRemplacement)
{
    Joueur *joueur2 = new Joueur("Bob", couleurJoueur::BLEU);

    voie->setJoueur(joueur);
    EXPECT_EQ(voie->getJoueur(), joueur);

    voie->setJoueur(joueur2);
    EXPECT_EQ(voie->getJoueur(), joueur2);

    delete joueur2;
}

TEST_F(VoieFerreeTest, SetJoueurNull)
{
    voie->setJoueur(joueur);
    EXPECT_EQ(voie->getJoueur(), joueur);

    voie->setJoueur(nullptr);
    EXPECT_EQ(voie->getJoueur(), nullptr);
}

// ============================================================
//  Voies avec différents poids
// ============================================================

TEST_F(VoieFerreeTest, PoidsPetit)
{
    VoieFerree v({paris}, couleurTrain::BLEU, 1);
    EXPECT_EQ(v.getPoids(), 1);
}

TEST_F(VoieFerreeTest, PoidsGrand)
{
    VoieFerree v({paris, lyon, marseille}, couleurTrain::VERT, 6);
    EXPECT_EQ(v.getPoids(), 6);
}

TEST_F(VoieFerreeTest, PoidsMoyen)
{
    VoieFerree v({paris, lyon}, couleurTrain::JAUNE, 3);
    EXPECT_EQ(v.getPoids(), 3);
}

// ============================================================
//  Voies avec différentes couleurs
// ============================================================

TEST_F(VoieFerreeTest, CouleurRouge)
{
    VoieFerree v({paris, lyon}, couleurTrain::ROUGE, 2);
    EXPECT_EQ(v.getCouleur(), couleurTrain::ROUGE);
}

TEST_F(VoieFerreeTest, CouleurBleu)
{
    VoieFerree v({paris, lyon}, couleurTrain::BLEU, 2);
    EXPECT_EQ(v.getCouleur(), couleurTrain::BLEU);
}

TEST_F(VoieFerreeTest, CouleurVert)
{
    VoieFerree v({paris, lyon}, couleurTrain::VERT, 2);
    EXPECT_EQ(v.getCouleur(), couleurTrain::VERT);
}

TEST_F(VoieFerreeTest, CouleurJaune)
{
    VoieFerree v({paris, lyon}, couleurTrain::JAUNE, 2);
    EXPECT_EQ(v.getCouleur(), couleurTrain::JAUNE);
}

TEST_F(VoieFerreeTest, CouleurBlanc)
{
    VoieFerree v({paris, lyon}, couleurTrain::BLANC, 2);
    EXPECT_EQ(v.getCouleur(), couleurTrain::BLANC);
}

TEST_F(VoieFerreeTest, CouleurNoir)
{
    VoieFerree v({paris, lyon}, couleurTrain::NOIR, 2);
    EXPECT_EQ(v.getCouleur(), couleurTrain::NOIR);
}

// ============================================================
//  Voies avec une seule ville
// ============================================================

TEST(VoieFerreeSingleVille, UneSeuleVille)
{
    Ville *paris = new Ville("Paris");
    VoieFerree *v = new VoieFerree({paris}, couleurTrain::ROUGE, 1);

    EXPECT_EQ(v->getListeVille().size(), 1);
    EXPECT_EQ(v->getListeVille()[0], paris);
    EXPECT_EQ(v->getPoids(), 1);

    delete v;
    delete paris;
}

// ============================================================
//  Voies avec plusieurs villes
// ============================================================

TEST(VoieFerreeMultipleVilles, DeuxVilles)
{
    Ville *paris = new Ville("Paris");
    Ville *lyon  = new Ville("Lyon");
    VoieFerree *v = new VoieFerree({paris, lyon}, couleurTrain::BLEU, 2);

    EXPECT_EQ(v->getListeVille().size(), 2);
    EXPECT_EQ(v->getPoids(), 2);

    delete v;
    delete lyon;
    delete paris;
}

TEST(VoieFerreeMultipleVilles, TroisVilles)
{
    Ville *paris     = new Ville("Paris");
    Ville *lyon      = new Ville("Lyon");
    Ville *marseille = new Ville("Marseille");
    VoieFerree *v = new VoieFerree({paris, lyon, marseille}, couleurTrain::VERT, 3);

    EXPECT_EQ(v->getListeVille().size(), 3);
    EXPECT_EQ(v->getPoids(), 3);

    delete v;
    delete marseille;
    delete lyon;
    delete paris;
}

// ============================================================
//  Plusieurs voies – indépendance
// ============================================================

TEST(VoieFerreeMultiples, VoiesDifferentes)
{
    Ville *paris     = new Ville("Paris");
    Ville *lyon      = new Ville("Lyon");
    Ville *marseille = new Ville("Marseille");

    VoieFerree v1({paris, lyon}, couleurTrain::ROUGE, 2);
    VoieFerree v2({lyon, marseille}, couleurTrain::BLEU, 3);

    EXPECT_NE(v1.getCouleur(), v2.getCouleur());
    EXPECT_NE(v1.getPoids(), v2.getPoids());

    delete marseille;
    delete lyon;
    delete paris;
}

TEST(VoieFerreeMultiples, JoueurIndependant)
{
    Ville *paris = new Ville("Paris");
    Ville *lyon  = new Ville("Lyon");
    Joueur *joueur1 = new Joueur("Alice", couleurJoueur::ROUGE);
    Joueur *joueur2 = new Joueur("Bob", couleurJoueur::BLEU);

    VoieFerree v1({paris, lyon}, couleurTrain::ROUGE, 2);
    VoieFerree v2({paris, lyon}, couleurTrain::BLEU, 2);

    v1.setJoueur(joueur1);
    v2.setJoueur(joueur2);

    EXPECT_EQ(v1.getJoueur(), joueur1);
    EXPECT_EQ(v2.getJoueur(), joueur2);
    EXPECT_NE(v1.getJoueur(), v2.getJoueur());

    delete joueur2;
    delete joueur1;
    delete lyon;
    delete paris;
}

// ============================================================
//  Allocation / libération sur le tas
// ============================================================

TEST(VoieFerreeHeap, AllocationEtLiberation)
{
    Ville *paris = new Ville("Paris");
    Ville *lyon  = new Ville("Lyon");
    VoieFerree *v = new VoieFerree({paris, lyon}, couleurTrain::ROUGE, 2);

    EXPECT_EQ(v->getCouleur(), couleurTrain::ROUGE);
    EXPECT_EQ(v->getPoids(), 2);

    delete v;
    delete lyon;
    delete paris;
}

// ============================================================
//  Noms de Ville dans une VoieFerree
// ============================================================

TEST_F(VoieFerreeTest, NomsVilles)
{
    vector<Ville*> listeVille = voie->getListeVille();
    EXPECT_EQ(listeVille[0]->getNom(), "Paris");
    EXPECT_EQ(listeVille[1]->getNom(), "Lyon");
}

TEST_F(VoieFerreeTest, JoueurNom)
{
    voie->setJoueur(joueur);
    EXPECT_EQ(voie->getJoueur()->getNom(), "Alice");
}
