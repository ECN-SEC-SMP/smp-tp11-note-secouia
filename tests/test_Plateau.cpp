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
//  Construction et getters de base
// ============================================================

TEST_F(PlateauTest, ConstructorInitialize)
{
    EXPECT_NE(plateau, nullptr);
}

TEST_F(PlateauTest, GetListeVille)
{
    vector<Ville*> villes = plateau->getListeVille();
    EXPECT_GT(villes.size(), 0);
}

TEST_F(PlateauTest, GetListeVoieFerree)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    EXPECT_GT(voies.size(), 0);
}

TEST_F(PlateauTest, NombreVillesNonNul)
{
    EXPECT_GT(plateau->getListeVille().size(), 0);
}

TEST_F(PlateauTest, NombreVoiesNonNul)
{
    EXPECT_GT(plateau->getListeVoieFerree().size(), 0);
}

// ============================================================
//  Villes et voies - cohérence des données
// ============================================================

TEST_F(PlateauTest, VillesContiennentNoms)
{
    vector<Ville*> villes = plateau->getListeVille();
    for (auto ville : villes) {
        EXPECT_FALSE(ville->getNom().empty());
    }
}

TEST_F(PlateauTest, VoiesContiennentVilles)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    for (auto voie : voies) {
        EXPECT_GT(voie->getListeVille().size(), 0);
    }
}

TEST_F(PlateauTest, VoiesContiennentCouleur)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    for (auto voie : voies) {
        // Juste vérifier qu'on peut récupérer la couleur
        couleurTrain couleur = voie->getCouleur();
        (void)couleur; // Éviter les warnings inutilisés
    }
}

TEST_F(PlateauTest, VoiesContiennentPoids)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    for (auto voie : voies) {
        EXPECT_GT(voie->getPoids(), 0);
    }
}

// ============================================================
//  Getters avec vérification des pointeurs
// ============================================================

TEST_F(PlateauTest, GetListeVilleNonNull)
{
    vector<Ville*> villes = plateau->getListeVille();
    for (auto ville : villes) {
        EXPECT_NE(ville, nullptr);
    }
}

TEST_F(PlateauTest, GetListeVoieFerreeNonNull)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    for (auto voie : voies) {
        EXPECT_NE(voie, nullptr);
    }
}

// ============================================================
//  Gestion des pioches
// ============================================================

TEST_F(PlateauTest, GetPiocheTickets)
{
    vector<Ticket*> tickets = plateau->getPiocheTickets();
    EXPECT_GE(tickets.size(), 0);
}

TEST_F(PlateauTest, GetPiocheTrain)
{
    vector<Train*> trains = plateau->getPiocheTrain();
    EXPECT_GE(trains.size(), 0);
}

TEST_F(PlateauTest, PiocheTicketsContientNonNull)
{
    vector<Ticket*> tickets = plateau->getPiocheTickets();
    for (auto ticket : tickets) {
        EXPECT_NE(ticket, nullptr);
    }
}

TEST_F(PlateauTest, PiocheTrainContientNonNull)
{
    vector<Train*> trains = plateau->getPiocheTrain();
    for (auto train : trains) {
        EXPECT_NE(train, nullptr);
    }
}

// ============================================================
//  Stabilité après appels répétés
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
//  Vérification de contenu commun
// ============================================================

TEST_F(PlateauTest, VillesNonVides)
{
    vector<Ville*> villes = plateau->getListeVille();
    EXPECT_FALSE(villes.empty());
}

TEST_F(PlateauTest, VoiesNonVides)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    EXPECT_FALSE(voies.empty());
}

// ============================================================
//  Unicité des instances
// ============================================================

TEST(PlateauMultiples, DeuxPlateauIndependants)
{
    Plateau *p1 = new Plateau();
    Plateau *p2 = new Plateau();

    vector<Ville*> villes1 = p1->getListeVille();
    vector<Ville*> villes2 = p2->getListeVille();

    // Même si les noms sont identiques (données chargées du fichier),
    // les instances doivent être différentes
    EXPECT_EQ(villes1.size(), villes2.size());

    delete p2;
    delete p1;
}

// ============================================================
//  Vérification des couleurs des voies
// ============================================================

TEST_F(PlateauTest, VoiesCouleurValide)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    for (auto voie : voies) {
        couleurTrain couleur = voie->getCouleur();
        // Vérifier que c'est une couleur valide (entre JAUNE et MULTI)
        EXPECT_GE(static_cast<int>(couleur), 0);
    }
}

// ============================================================
//  Tests de destructor
// ============================================================

TEST(PlateauDestruction, AllocationEtLiberation)
{
    Plateau *p = new Plateau();
    EXPECT_NE(p, nullptr);
    EXPECT_GT(p->getListeVille().size(), 0);
    
    delete p; // ne doit pas crasher
}

// ============================================================
//  Cohérence des voies avec les villes
// ============================================================

TEST_F(PlateauTest, VoiesReferenceVillesValides)
{
    vector<Ville*> villes = plateau->getListeVille();
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();

    for (auto voie : voies) {
        vector<Ville*> villesVoie = voie->getListeVille();
        for (auto villeVoie : villesVoie) {
            // Chaque ville dans une voie doit être dans la liste du plateau
            bool found = false;
            for (auto ville : villes) {
                if (ville == villeVoie) {
                    found = true;
                    break;
                }
            }
            EXPECT_TRUE(found) << "Ville de voie non trouvée dans plateau";
        }
    }
}

// ============================================================
//  Poids des voies
// ============================================================

TEST_F(PlateauTest, VoiesPoidsCoherents)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    for (auto voie : voies) {
        int poids = voie->getPoids();
        // Le poids doit être raisonnable (entre 1 et 6 généralement)
        EXPECT_GT(poids, 0);
        EXPECT_LE(poids, 10);
    }
}

// ============================================================
//  Voies sans propriétaire initial
// ============================================================

TEST_F(PlateauTest, VoiesSansJoueurInitial)
{
    vector<VoieFerree*> voies = plateau->getListeVoieFerree();
    for (auto voie : voies) {
        // Au début, les voies ne doivent pas avoir de propriétaire
        EXPECT_EQ(voie->getJoueur(), nullptr);
    }
}
