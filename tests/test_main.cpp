/**
 * @file test_main.cpp
 * @brief Tests unitaires pour la logique de jeu de main.cpp
 *
 * main.cpp ne définit que des fonctions static et une fonction main() :
 * ces symboles ne sont pas exportés par aventuriers_lib et ne peuvent donc
 * pas être appelés directement depuis un fichier de test sans refactorisation.
 *
 * Ces tests couvrent :
 *   1. La classe Partie (initialisation, pioche train/ticket, accesseurs)
 *      qui constitue le cœur de la logique de main.cpp.
 *   2. Les helpers pure-logic répliqués ici (couleurFromString, nomTrain,
 *      nomJoueur, estConnecte, creerTrains) pour valider leur comportement
 *      sans modifier le code source.
 *   3. Les interactions Joueur ↔ VoieFerree telles qu'orchestrées par main.cpp.
 */

#include <gtest/gtest.h>

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Joueur.hpp"
#include "Partie.hpp"
#include "Ticket.hpp"
#include "Train.hpp"
#include "Ville.hpp"
#include "VoieFerree.hpp"
#include "couleurEnum.hpp"

// ── Répliques des helpers static de main.cpp ────────────────────────────────

static couleurTrain couleurFromString(const std::string &s)
{
    if (s == "black")  return couleurTrain::NOIR;
    if (s == "red")    return couleurTrain::ROUGE;
    if (s == "yellow") return couleurTrain::JAUNE;
    if (s == "blue")   return couleurTrain::BLEU;
    if (s == "white")  return couleurTrain::BLANC;
    if (s == "green")  return couleurTrain::VERT;
    if (s == "orange") return couleurTrain::JAUNE; // pas d'orange dans l'enum
    return couleurTrain::NOIR;                     // défaut identique à main.cpp
}

static std::string nomTrain(couleurTrain c)
{
    switch (c) {
        case couleurTrain::JAUNE: return "Jaune";
        case couleurTrain::ROUGE: return "Rouge";
        case couleurTrain::VERT:  return "Vert";
        case couleurTrain::BLEU:  return "Bleu";
        case couleurTrain::BLANC: return "Blanc";
        case couleurTrain::NOIR:  return "Noir";
        case couleurTrain::MULTI: return "Loco";
        default:                  return "?";
    }
}

static std::string nomJoueur(couleurJoueur c)
{
    switch (c) {
        case couleurJoueur::JAUNE: return "Jaune";
        case couleurJoueur::BLEU:  return "Bleu";
        case couleurJoueur::ROUGE: return "Rouge";
        case couleurJoueur::VERT:  return "Vert";
        default:                   return "?";
    }
}

// Réplique exacte du BFS de main.cpp
static bool estConnecte(const std::string &a, const std::string &b,
                        const std::vector<VoieFerree *> &voies, Joueur *joueur)
{
    if (a == b) return true;
    std::map<std::string, std::vector<std::string>> adj;
    for (auto v : voies) {
        if (v->getJoueur() != joueur) continue;
        auto lv = v->getListeVille();
        for (size_t i = 0; i < lv.size(); i++)
            for (size_t k = 0; k < lv.size(); k++)
                if (i != k)
                    adj[lv[i]->getNom()].push_back(lv[k]->getNom());
    }
    std::set<std::string> vis;
    std::vector<std::string> q = {a};
    vis.insert(a);
    while (!q.empty()) {
        std::string cur = q.back(); q.pop_back();
        if (cur == b) return true;
        for (const auto &nxt : adj[cur])
            if (!vis.count(nxt)) { vis.insert(nxt); q.push_back(nxt); }
    }
    return false;
}

static void trim(std::string &s)
{
    while (!s.empty() && (s.front() == ' ' || s.front() == '\r')) s.erase(s.begin());
    while (!s.empty() && (s.back()  == ' ' || s.back()  == '\r')) s.pop_back();
}

// ── Helpers de construction partagés ────────────────────────────────────────

static std::vector<Train *> creerTrains()
{
    std::vector<Train *> pile;
    for (auto c : {couleurTrain::JAUNE, couleurTrain::ROUGE, couleurTrain::VERT,
                   couleurTrain::BLEU,  couleurTrain::BLANC, couleurTrain::NOIR})
        for (int i = 0; i < 10; i++) pile.push_back(new Train(c));
    for (int i = 0; i < 12; i++) pile.push_back(new Train(couleurTrain::MULTI));
    return pile;
}

// ── Helper pour créer une Partie minimale ────────────────────────────────────
// 4 trains + 2 tickets distribués automatiquement par le constructeur Partie.

struct SetupPartie {
    Ville *va, *vb, *vc;
    Ticket *t1, *t2, *t3, *t4;
    Train  *tr1, *tr2, *tr3, *tr4, *tr5, *tr6, *tr7, *tr8;
    Joueur *j1, *j2;
    std::vector<Ticket *> tickets;
    std::vector<Train *>  trains;
    std::vector<Joueur *> joueurs;

    SetupPartie()
    {
        va = new Ville("A"); vb = new Ville("B"); vc = new Ville("C");
        t1 = new Ticket(va, vb); t2 = new Ticket(vb, vc);
        t3 = new Ticket(va, vc); t4 = new Ticket(vb, va);
        tickets = {t1, t2, t3, t4};

        for (auto c : {couleurTrain::ROUGE, couleurTrain::BLEU,
                       couleurTrain::VERT,  couleurTrain::NOIR,
                       couleurTrain::BLANC, couleurTrain::JAUNE,
                       couleurTrain::MULTI, couleurTrain::ROUGE})
            trains.push_back(new Train(c));

        j1 = new Joueur("Alice", couleurJoueur::BLEU);
        j2 = new Joueur("Bob",   couleurJoueur::ROUGE);
        joueurs = {j1, j2};
    }

    ~SetupPartie()
    {
        delete va; delete vb; delete vc;
        // tickets et trains sont gérés par Partie (ne pas double-free)
        delete j1; delete j2;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//  1. Tests couleurFromString
// ═══════════════════════════════════════════════════════════════════════════

TEST(CouleurFromString, Black)   { EXPECT_EQ(couleurFromString("black"),  couleurTrain::NOIR);  }
TEST(CouleurFromString, Red)     { EXPECT_EQ(couleurFromString("red"),    couleurTrain::ROUGE); }
TEST(CouleurFromString, Yellow)  { EXPECT_EQ(couleurFromString("yellow"), couleurTrain::JAUNE); }
TEST(CouleurFromString, Blue)    { EXPECT_EQ(couleurFromString("blue"),   couleurTrain::BLEU);  }
TEST(CouleurFromString, White)   { EXPECT_EQ(couleurFromString("white"),  couleurTrain::BLANC); }
TEST(CouleurFromString, Green)   { EXPECT_EQ(couleurFromString("green"),  couleurTrain::VERT);  }
TEST(CouleurFromString, OrangeMappeJaune)
{
    // orange n'existe pas dans l'enum → mappé sur JAUNE
    EXPECT_EQ(couleurFromString("orange"), couleurTrain::JAUNE);
}
TEST(CouleurFromString, InconnuMappeNoir)
{
    EXPECT_EQ(couleurFromString("purple"), couleurTrain::NOIR);
}

// ═══════════════════════════════════════════════════════════════════════════
//  2. Tests nomTrain
// ═══════════════════════════════════════════════════════════════════════════

TEST(NomTrain, Jaune)  { EXPECT_EQ(nomTrain(couleurTrain::JAUNE), "Jaune"); }
TEST(NomTrain, Rouge)  { EXPECT_EQ(nomTrain(couleurTrain::ROUGE), "Rouge"); }
TEST(NomTrain, Vert)   { EXPECT_EQ(nomTrain(couleurTrain::VERT),  "Vert");  }
TEST(NomTrain, Bleu)   { EXPECT_EQ(nomTrain(couleurTrain::BLEU),  "Bleu");  }
TEST(NomTrain, Blanc)  { EXPECT_EQ(nomTrain(couleurTrain::BLANC), "Blanc"); }
TEST(NomTrain, Noir)   { EXPECT_EQ(nomTrain(couleurTrain::NOIR),  "Noir");  }
TEST(NomTrain, Multi)  { EXPECT_EQ(nomTrain(couleurTrain::MULTI), "Loco");  }

// ═══════════════════════════════════════════════════════════════════════════
//  3. Tests nomJoueur
// ═══════════════════════════════════════════════════════════════════════════

TEST(NomJoueur, Jaune) { EXPECT_EQ(nomJoueur(couleurJoueur::JAUNE), "Jaune"); }
TEST(NomJoueur, Bleu)  { EXPECT_EQ(nomJoueur(couleurJoueur::BLEU),  "Bleu");  }
TEST(NomJoueur, Rouge) { EXPECT_EQ(nomJoueur(couleurJoueur::ROUGE), "Rouge"); }
TEST(NomJoueur, Vert)  { EXPECT_EQ(nomJoueur(couleurJoueur::VERT),  "Vert");  }

// ═══════════════════════════════════════════════════════════════════════════
//  4. Tests trim
// ═══════════════════════════════════════════════════════════════════════════

TEST(Trim, SansEspace)
{
    std::string s = "Paris";
    trim(s);
    EXPECT_EQ(s, "Paris");
}

TEST(Trim, EspaceDebut)
{
    std::string s = "  Paris";
    trim(s);
    EXPECT_EQ(s, "Paris");
}

TEST(Trim, EspaceFin)
{
    std::string s = "Paris  ";
    trim(s);
    EXPECT_EQ(s, "Paris");
}

TEST(Trim, EspacesDesBordsCR)
{
    std::string s = " Paris\r";
    trim(s);
    EXPECT_EQ(s, "Paris");
}

TEST(Trim, ChaineVide)
{
    std::string s = "";
    trim(s);
    EXPECT_EQ(s, "");
}

TEST(Trim, QueSsEspaces)
{
    std::string s = "   ";
    trim(s);
    EXPECT_EQ(s, "");
}

// ═══════════════════════════════════════════════════════════════════════════
//  5. Tests estConnecte (BFS)
// ═══════════════════════════════════════════════════════════════════════════

class ConnectiviteTest : public ::testing::Test
{
protected:
    Joueur *joueur = nullptr;
    Ville  *seattle = nullptr, *portland = nullptr,
           *sf      = nullptr, *la       = nullptr;
    std::vector<VoieFerree *> voies;

    void SetUp() override
    {
        joueur   = new Joueur("Test", couleurJoueur::BLEU);
        seattle  = new Ville("Seattle");
        portland = new Ville("Portland");
        sf       = new Ville("San Francisco");
        la       = new Ville("Los Angeles");
    }

    void TearDown() override
    {
        for (auto v : voies) delete v;
        delete seattle; delete portland; delete sf; delete la;
        delete joueur;
    }

    VoieFerree *makeVoie(Ville *a, Ville *b, bool appartient)
    {
        auto *v = new VoieFerree({a, b}, couleurTrain::BLEU, 1);
        if (appartient) v->setJoueur(joueur);
        voies.push_back(v);
        return v;
    }
};

TEST_F(ConnectiviteTest, MemeVille)
{
    EXPECT_TRUE(estConnecte("Seattle", "Seattle", voies, joueur));
}

TEST_F(ConnectiviteTest, VoisinsDirectsConnectes)
{
    makeVoie(seattle, portland, true);
    EXPECT_TRUE(estConnecte("Seattle", "Portland", voies, joueur));
}

TEST_F(ConnectiviteTest, CheminIndirect)
{
    makeVoie(seattle, portland, true);
    makeVoie(portland, sf, true);
    EXPECT_TRUE(estConnecte("Seattle", "San Francisco", voies, joueur));
}

TEST_F(ConnectiviteTest, PasDeChemin)
{
    makeVoie(seattle, portland, true);
    // sf et la non connectés
    EXPECT_FALSE(estConnecte("Seattle", "San Francisco", voies, joueur));
}

TEST_F(ConnectiviteTest, VoieAppartientAutreJoueur)
{
    Joueur *autre = new Joueur("Autre", couleurJoueur::ROUGE);
    auto *v = new VoieFerree({seattle, portland}, couleurTrain::ROUGE, 1);
    v->setJoueur(autre);
    voies.push_back(v);

    // La voie n'appartient pas à joueur → pas de chemin
    EXPECT_FALSE(estConnecte("Seattle", "Portland", voies, joueur));

    delete autre;
}

TEST_F(ConnectiviteTest, VoieLibreNonComptee)
{
    makeVoie(seattle, portland, false); // voie libre, pas de joueur
    EXPECT_FALSE(estConnecte("Seattle", "Portland", voies, joueur));
}

TEST_F(ConnectiviteTest, CheminLong)
{
    makeVoie(seattle, portland, true);
    makeVoie(portland, sf, true);
    makeVoie(sf, la, true);
    EXPECT_TRUE(estConnecte("Seattle", "Los Angeles", voies, joueur));
}

// ═══════════════════════════════════════════════════════════════════════════
//  6. Tests creerTrains
// ═══════════════════════════════════════════════════════════════════════════

TEST(CreerTrains, TailleTotal)
{
    auto pile = creerTrains();
    EXPECT_EQ(pile.size(), 72u);
    for (auto *t : pile) delete t;
}

TEST(CreerTrains, NbLocomotives)
{
    auto pile = creerTrains();
    int nb = 0;
    for (auto *t : pile)
        if (t->getCouleurTrain() == couleurTrain::MULTI) nb++;
    EXPECT_EQ(nb, 12);
    for (auto *t : pile) delete t;
}

TEST(CreerTrains, NbParCouleurNormale)
{
    auto pile = creerTrains();
    for (auto c : {couleurTrain::JAUNE, couleurTrain::ROUGE, couleurTrain::VERT,
                   couleurTrain::BLEU,  couleurTrain::BLANC, couleurTrain::NOIR})
    {
        int nb = 0;
        for (auto *t : pile)
            if (t->getCouleurTrain() == c) nb++;
        EXPECT_EQ(nb, 10) << "Couleur " << nomTrain(c) << " devrait avoir 10 cartes";
    }
    for (auto *t : pile) delete t;
}

// ═══════════════════════════════════════════════════════════════════════════
//  7. Tests Partie – construction et distribution initiale
// ═══════════════════════════════════════════════════════════════════════════

TEST(PartieInit, DistributionTrainsJoueurs)
{
    SetupPartie s;
    Partie p(s.tickets, false, s.trains, s.joueurs);

    // Chaque joueur reçoit 4 cartes train au démarrage
    EXPECT_EQ(s.j1->getNbCartesTotales(), 4);
    EXPECT_EQ(s.j2->getNbCartesTotales(), 4);
}

TEST(PartieInit, DistributionTicketsJoueurs)
{
    SetupPartie s;
    Partie p(s.tickets, false, s.trains, s.joueurs);

    // Chaque joueur reçoit 2 tickets au démarrage
    EXPECT_EQ(s.j1->getMissions().size(), 2u);
    EXPECT_EQ(s.j2->getMissions().size(), 2u);
}

TEST(PartieInit, PlateauNonNul)
{
    SetupPartie s;
    Partie p(s.tickets, false, s.trains, s.joueurs);
    EXPECT_NE(p.getPlateau(), nullptr);
}

TEST(PartieInit, GetJoueurParIndex)
{
    SetupPartie s;
    Partie p(s.tickets, false, s.trains, s.joueurs);
    EXPECT_EQ(p.getJoueur(0)->getNom(), "Alice");
    EXPECT_EQ(p.getJoueur(1)->getNom(), "Bob");
}

TEST(PartieInit, GetJoueurVecteur)
{
    SetupPartie s;
    Partie p(s.tickets, false, s.trains, s.joueurs);
    EXPECT_EQ(p.getJoueur().size(), 2u);
}

// ═══════════════════════════════════════════════════════════════════════════
//  8. Tests Partie – piocherTrain
// ═══════════════════════════════════════════════════════════════════════════

TEST(PartiePiocheTrain, PiocherAugmenteMain)
{
    Joueur *j = new Joueur("Toto", couleurJoueur::VERT);
    std::vector<Train *>  trains  = {new Train(couleurTrain::ROUGE)};
    std::vector<Ticket *> tickets = {};
    std::vector<Joueur *> joueurs = {j};

    // Partie() par défaut (sans distribution) – on utilise le constructeur
    // complet avec 0 joueur pour éviter la distribution, puis on pioche manuellement
    Joueur *j2 = new Joueur("Solo", couleurJoueur::JAUNE);
    std::vector<Train *>  trains2  = {new Train(couleurTrain::BLEU),
                                      new Train(couleurTrain::VERT)};
    std::vector<Ticket *> tickets2 = {};
    std::vector<Joueur *> joueurs2 = {};

    Partie p(tickets2, false, trains2, joueurs2);
    EXPECT_EQ(j2->getNbCartesTotales(), 0);
    p.piocherTrain(*j2);
    EXPECT_EQ(j2->getNbCartesTotales(), 1);
    p.piocherTrain(*j2);
    EXPECT_EQ(j2->getNbCartesTotales(), 2);

    delete j; delete j2;
    for (auto *t : trains)  delete t;
    for (auto *t : tickets) delete t;
}

// ═══════════════════════════════════════════════════════════════════════════
//  9. Tests Partie – piocherTicket
// ═══════════════════════════════════════════════════════════════════════════

TEST(PartiePiocheTicket, PiocherAjouteMission)
{
    Ville *a = new Ville("P"); Ville *b = new Ville("Q");
    std::vector<Ticket *> tks = {new Ticket(a, b)};
    std::vector<Train *>  trs = {new Train(couleurTrain::ROUGE)};
    Joueur *j = new Joueur("Solo", couleurJoueur::BLEU);
    std::vector<Joueur *> js = {};

    Partie p(tks, false, trs, js);
    EXPECT_EQ(j->getMissions().size(), 0u);
    p.piocherTicket(*j);
    EXPECT_EQ(j->getMissions().size(), 1u);
    EXPECT_EQ(j->getMissions()[0]->getVilleDepart()->getNom(),  "P");
    EXPECT_EQ(j->getMissions()[0]->getVilleArrivee()->getNom(), "Q");

    delete j;
    delete a; delete b;
}

// ═══════════════════════════════════════════════════════════════════════════
//  10. Intégration : pose d'une voie et validation de ticket
// ═══════════════════════════════════════════════════════════════════════════

TEST(IntegrationMain, PrendreVoieEtValiderTicket)
{
    Ville *seattle = new Ville("Seattle");
    Ville *portland = new Ville("Portland");

    Joueur *j = new Joueur("Test", couleurJoueur::BLEU);

    // Donner 3 cartes bleues au joueur
    for (int i = 0; i < 3; i++)
        j->ajouterCarte(new Train(couleurTrain::BLEU));

    VoieFerree *voie = new VoieFerree({seattle, portland}, couleurTrain::BLEU, 3);
    std::vector<VoieFerree *> voies = {voie};

    // Vérifier qu'il peut prendre la voie
    EXPECT_TRUE(j->peutPrendreVoie(couleurTrain::BLEU, 3));

    // Poser ses wagons
    EXPECT_TRUE(j->defausserCartesVoie(couleurTrain::BLEU, 3));
    EXPECT_TRUE(j->utiliserWagons(3));
    voie->setJoueur(j);

    // La voie est maintenant possédée par j
    EXPECT_EQ(voie->getJoueur(), j);

    // Le BFS doit trouver le chemin
    EXPECT_TRUE(estConnecte("Seattle", "Portland", voies, j));

    // Nettoyage
    delete voie;
    delete seattle; delete portland;
    delete j;
}
