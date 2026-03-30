/**
 * @file test_Ville.cpp
 * @brief Tests unitaires Google Test pour la classe Ville
 */

#include <gtest/gtest.h>

#include "Ville.hpp"

// ============================================================
//  Construction et getters de base
// ============================================================

TEST(VilleConstructor, NomSimple)
{
    Ville v("Paris");
    EXPECT_EQ(v.getNom(), "Paris");
}

TEST(VilleConstructor, NomDifferent)
{
    Ville v("Lyon");
    EXPECT_EQ(v.getNom(), "Lyon");
}

TEST(VilleConstructor, NomVide)
{
    Ville v("");
    EXPECT_EQ(v.getNom(), "");
}

// ============================================================
//  Plusieurs villes
// ============================================================

TEST(VilleMultiples, DeuxVillesDifferentes)
{
    Ville v1("Paris");
    Ville v2("Lyon");

    EXPECT_NE(v1.getNom(), v2.getNom());
}

TEST(VilleMultiples, DeuxVillesMemeNom)
{
    Ville v1("Paris");
    Ville v2("Paris");

    EXPECT_EQ(v1.getNom(), v2.getNom());
}

// ============================================================
//  Allocation sur le tas
// ============================================================

TEST(VilleHeap, AllocationSimple)
{
    Ville *v = new Ville("Paris");
    EXPECT_EQ(v->getNom(), "Paris");
    delete v;
}

TEST(VilleHeap, AllocationMultiple)
{
    Ville *v1 = new Ville("Paris");
    Ville *v2 = new Ville("Lyon");

    EXPECT_EQ(v1->getNom(), "Paris");
    EXPECT_EQ(v2->getNom(), "Lyon");

    delete v1;
    delete v2;
}

// ============================================================
//  Stabilité du getter
// ============================================================

TEST(VilleStabilite, GetNomRepete)
{
    Ville v("Paris");
    EXPECT_EQ(v.getNom(), "Paris");
    EXPECT_EQ(v.getNom(), "Paris");
}
