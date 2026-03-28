/**
 * @file test_Ticket.cpp
 * @brief Tests unitaires Google Test pour la classe Ticket
 */

#include <gtest/gtest.h>

#include "Ticket.hpp"
#include "Ville.hpp"

// ============================================================
//  Fixture commune
// ============================================================

class TicketTest : public ::testing::Test
{
protected:
    Ville *paris  = nullptr;
    Ville *lyon   = nullptr;
    Ticket *ticket = nullptr;

    void SetUp() override
    {
        paris  = new Ville("Paris");
        lyon   = new Ville("Lyon");
        ticket = new Ticket(paris, lyon);
    }

    void TearDown() override
    {
        delete ticket;
        delete paris;
        delete lyon;
    }
};

// ============================================================
//  Construction / getters de base
// ============================================================

TEST_F(TicketTest, GetVilleDepart)
{
    EXPECT_EQ(ticket->getVilleDepart(), paris);
}

TEST_F(TicketTest, GetVilleArrivee)
{
    EXPECT_EQ(ticket->getVilleArrivee(), lyon);
}

TEST_F(TicketTest, GetVilleDepartNom)
{
    EXPECT_EQ(ticket->getVilleDepart()->getNom(), "Paris");
}

TEST_F(TicketTest, GetVilleArriveeNom)
{
    EXPECT_EQ(ticket->getVilleArrivee()->getNom(), "Lyon");
}

// ============================================================
//  Villes distinctes
// ============================================================

TEST_F(TicketTest, VillesDifferentes)
{
    EXPECT_NE(ticket->getVilleDepart(), ticket->getVilleArrivee());
}

TEST_F(TicketTest, VillesDifferentesNoms)
{
    EXPECT_NE(ticket->getVilleDepart()->getNom(),
              ticket->getVilleArrivee()->getNom());
}

// ============================================================
//  Ticket avec la même ville départ et arrivée
// ============================================================

TEST(TicketMemeVille, DepartEgalArrivee)
{
    Ville *v = new Ville("Seattle");
    Ticket *t = new Ticket(v, v);

    EXPECT_EQ(t->getVilleDepart(),  v);
    EXPECT_EQ(t->getVilleArrivee(), v);
    EXPECT_EQ(t->getVilleDepart()->getNom(), "Seattle");

    delete t;
    delete v;
}

// ============================================================
//  Plusieurs tickets indépendants
// ============================================================

TEST(TicketMultiples, DeuxTicketsDifferents)
{
    Ville *a = new Ville("New York");
    Ville *b = new Ville("Miami");
    Ville *c = new Ville("Chicago");

    Ticket *t1 = new Ticket(a, b);
    Ticket *t2 = new Ticket(b, c);

    EXPECT_EQ(t1->getVilleDepart()->getNom(),  "New York");
    EXPECT_EQ(t1->getVilleArrivee()->getNom(), "Miami");

    EXPECT_EQ(t2->getVilleDepart()->getNom(),  "Miami");
    EXPECT_EQ(t2->getVilleArrivee()->getNom(), "Chicago");

    // Les tickets ne partagent pas les mêmes pointeurs de villes extrêmes
    EXPECT_NE(t1->getVilleDepart(), t2->getVilleDepart());

    delete t1;
    delete t2;
    delete a;
    delete b;
    delete c;
}

TEST(TicketMultiples, PointeursIndependants)
{
    Ville *x = new Ville("X");
    Ville *y = new Ville("Y");
    Ticket *t1 = new Ticket(x, y);
    Ticket *t2 = new Ticket(y, x);

    // t1 : X -> Y
    EXPECT_EQ(t1->getVilleDepart()->getNom(),  "X");
    EXPECT_EQ(t1->getVilleArrivee()->getNom(), "Y");

    // t2 : Y -> X (inversé)
    EXPECT_EQ(t2->getVilleDepart()->getNom(),  "Y");
    EXPECT_EQ(t2->getVilleArrivee()->getNom(), "X");

    delete t1;
    delete t2;
    delete x;
    delete y;
}

// ============================================================
//  Noms de villes avec espaces et accents (robustesse)
// ============================================================

TEST(TicketNoms, NomAvecEspace)
{
    Ville *a = new Ville("Los Angeles");
    Ville *b = new Ville("San Francisco");
    Ticket *t = new Ticket(a, b);

    EXPECT_EQ(t->getVilleDepart()->getNom(),  "Los Angeles");
    EXPECT_EQ(t->getVilleArrivee()->getNom(), "San Francisco");

    delete t;
    delete a;
    delete b;
}
