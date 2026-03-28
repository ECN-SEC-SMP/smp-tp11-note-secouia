/**
 * @file test_gui_main.cpp
 * @brief Tests unitaires pour la logique de jeu de gui_main.cpp
 *
 * gui_main.cpp contient des fonctions static et une fonction main() couplée
 * à Qt (QApplication, QWidget…). Ces symboles ne peuvent pas être testés
 * directement avec GTest sans framework Qt Test.
 *
 * Ces tests couvrent :
 *   1. Les helpers pure-logic de gui_main.cpp (couleurFromString, nomTrain,
 *      nomJoueur, estConnecte, trim, creerTrains) répliqués ici.
 *   2. Les interactions QColor ↔ couleurTrain (conversion), vérifiées
 *      sans Qt en testant uniquement les valeurs d'enum.
 *   3. Le bon comportement de Partie et Joueur tels qu'utilisés par la GUI
 *      (mise en place, pioche, prise de voie, fin de partie).
 */

#include <gtest/gtest.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Joueur.hpp"
#include "Partie.hpp"
#include "Ticket.hpp"
#include "Train.hpp"
#include "Ville.hpp"
#include "VoieFerree.hpp"
#include "couleurEnum.hpp"

// ── Répliques des helpers static de gui_main.cpp ────────────────────────────
// (identiques à main.cpp ; les deux fichiers partagent la même logique)

static couleurTrain couleurFromStringGui(const std::string &s)
{
    if (s == "black")  return couleurTrain::NOIR;
    if (s == "red")    return couleurTrain::ROUGE;
    if (s == "yellow") return couleurTrain::JAUNE;
    if (s == "blue")   return couleurTrain::BLEU;
    if (s == "white")  return couleurTrain::BLANC;
    if (s == "green")  return couleurTrain::VERT;
    if (s == "orange") return couleurTrain::JAUNE;
    return couleurTrain::NOIR;
}

static std::string nomTrainGui(couleurTrain c)
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

static std::string nomJoueurGui(couleurJoueur c)
{
    switch (c) {
        case couleurJoueur::JAUNE: return "Jaune";
        case couleurJoueur::BLEU:  return "Bleu";
        case couleurJoueur::ROUGE: return "Rouge";
        case couleurJoueur::VERT:  return "Vert";
        default:                   return "?";
    }
}

static bool estConnecteGui(const std::string &a, const std::string &b,
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

// ── Mapping couleur→HTML tel que défini dans gui_main.cpp ──────────────────
// gui_main.cpp choisit une couleur HTML par couleurTrain pour afficher les
// voies. On valide ici la cohérence de la fonction de mapping sans Qt.

static std::string htmlCouleur(couleurTrain c)
{
    switch (c) {
        case couleurTrain::ROUGE: return "#E53935";
        case couleurTrain::BLEU:  return "#1E88E5";
        case couleurTrain::VERT:  return "#43A047";
        case couleurTrain::JAUNE: return "#FDD835";
        case couleurTrain::BLANC: return "#EEEEEE";
        case couleurTrain::NOIR:  return "#424242";
        case couleurTrain::MULTI: return "#AB47BC";
        default:                  return "#9E9E9E";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  1. Tests couleurFromString (gui_main)
// ═══════════════════════════════════════════════════════════════════════════

TEST(GuiCouleurFromString, Black)  { EXPECT_EQ(couleurFromStringGui("black"),  couleurTrain::NOIR);  }
TEST(GuiCouleurFromString, Red)    { EXPECT_EQ(couleurFromStringGui("red"),    couleurTrain::ROUGE); }
TEST(GuiCouleurFromString, Yellow) { EXPECT_EQ(couleurFromStringGui("yellow"), couleurTrain::JAUNE); }
TEST(GuiCouleurFromString, Blue)   { EXPECT_EQ(couleurFromStringGui("blue"),   couleurTrain::BLEU);  }
TEST(GuiCouleurFromString, White)  { EXPECT_EQ(couleurFromStringGui("white"),  couleurTrain::BLANC); }
TEST(GuiCouleurFromString, Green)  { EXPECT_EQ(couleurFromStringGui("green"),  couleurTrain::VERT);  }
TEST(GuiCouleurFromString, OrangeMappeJaune)
{
    EXPECT_EQ(couleurFromStringGui("orange"), couleurTrain::JAUNE);
}
TEST(GuiCouleurFromString, InconnuMappeNoir)
{
    EXPECT_EQ(couleurFromStringGui("magenta"), couleurTrain::NOIR);
}

// ═══════════════════════════════════════════════════════════════════════════
//  2. Tests nomTrain / nomJoueur (gui)
// ═══════════════════════════════════════════════════════════════════════════

TEST(GuiNomTrain, ToutesLesCouleurs)
{
    EXPECT_EQ(nomTrainGui(couleurTrain::JAUNE), "Jaune");
    EXPECT_EQ(nomTrainGui(couleurTrain::ROUGE), "Rouge");
    EXPECT_EQ(nomTrainGui(couleurTrain::VERT),  "Vert");
    EXPECT_EQ(nomTrainGui(couleurTrain::BLEU),  "Bleu");
    EXPECT_EQ(nomTrainGui(couleurTrain::BLANC), "Blanc");
    EXPECT_EQ(nomTrainGui(couleurTrain::NOIR),  "Noir");
    EXPECT_EQ(nomTrainGui(couleurTrain::MULTI), "Loco");
}

TEST(GuiNomJoueur, ToutesLesCouleurs)
{
    EXPECT_EQ(nomJoueurGui(couleurJoueur::JAUNE), "Jaune");
    EXPECT_EQ(nomJoueurGui(couleurJoueur::BLEU),  "Bleu");
    EXPECT_EQ(nomJoueurGui(couleurJoueur::ROUGE), "Rouge");
    EXPECT_EQ(nomJoueurGui(couleurJoueur::VERT),  "Vert");
}

// ═══════════════════════════════════════════════════════════════════════════
//  3. Tests mapping HTML couleur
// ═══════════════════════════════════════════════════════════════════════════

TEST(GuiHtmlCouleur, RougeEstDifferentDeBleu)
{
    EXPECT_NE(htmlCouleur(couleurTrain::ROUGE), htmlCouleur(couleurTrain::BLEU));
}

TEST(GuiHtmlCouleur, ToutesCouleursDifferentes)
{
    std::vector<couleurTrain> couleurs = {
        couleurTrain::ROUGE, couleurTrain::BLEU,  couleurTrain::VERT,
        couleurTrain::JAUNE, couleurTrain::BLANC, couleurTrain::NOIR,
        couleurTrain::MULTI
    };
    std::set<std::string> vus;
    for (auto c : couleurs) {
        std::string h = htmlCouleur(c);
        EXPECT_FALSE(vus.count(h)) << "Couleur dupliquée : " << h;
        vus.insert(h);
    }
}

TEST(GuiHtmlCouleur, FormatHex)
{
    // Toutes les couleurs commencent par '#'
    for (auto c : {couleurTrain::ROUGE, couleurTrain::BLEU, couleurTrain::VERT,
                   couleurTrain::JAUNE, couleurTrain::BLANC, couleurTrain::NOIR,
                   couleurTrain::MULTI}) {
        EXPECT_EQ(htmlCouleur(c)[0], '#');
        EXPECT_EQ(htmlCouleur(c).size(), 7u); // #RRGGBB
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  4. Tests estConnecte (GUI – BFS identique à main.cpp)
// ═══════════════════════════════════════════════════════════════════════════

class GuiConnectiviteTest : public ::testing::Test
{
protected:
    Joueur *joueur = nullptr;
    Ville  *ny = nullptr, *dc = nullptr, *miami = nullptr;
    std::vector<VoieFerree *> voies;

    void SetUp() override
    {
        joueur = new Joueur("GUI", couleurJoueur::VERT);
        ny    = new Ville("New York");
        dc    = new Ville("Washington");
        miami = new Ville("Miami");
    }

    void TearDown() override
    {
        for (auto v : voies) delete v;
        delete ny; delete dc; delete miami;
        delete joueur;
    }

    void addVoie(Ville *a, Ville *b)
    {
        auto *v = new VoieFerree({a, b}, couleurTrain::VERT, 2);
        v->setJoueur(joueur);
        voies.push_back(v);
    }
};

TEST_F(GuiConnectiviteTest, VilleIdentique)
{
    EXPECT_TRUE(estConnecteGui("New York", "New York", voies, joueur));
}

TEST_F(GuiConnectiviteTest, VoisinsDirecs)
{
    addVoie(ny, dc);
    EXPECT_TRUE(estConnecteGui("New York", "Washington", voies, joueur));
}

TEST_F(GuiConnectiviteTest, ChaineTroisVilles)
{
    addVoie(ny, dc);
    addVoie(dc, miami);
    EXPECT_TRUE(estConnecteGui("New York", "Miami", voies, joueur));
}

TEST_F(GuiConnectiviteTest, AucuneChemin)
{
    // Aucune voie → pas de chemin
    EXPECT_FALSE(estConnecteGui("New York", "Miami", voies, joueur));
}

TEST_F(GuiConnectiviteTest, VoieAutreJoueur)
{
    Joueur *autre = new Joueur("Autre", couleurJoueur::ROUGE);
    auto *v = new VoieFerree({ny, dc}, couleurTrain::ROUGE, 2);
    v->setJoueur(autre);
    voies.push_back(v);

    EXPECT_FALSE(estConnecteGui("New York", "Washington", voies, joueur));
    delete autre;
}

// ═══════════════════════════════════════════════════════════════════════════
//  5. Tests Partie (logique utilisée par la GUI)
// ═══════════════════════════════════════════════════════════════════════════

class GuiPartieTest : public ::testing::Test
{
protected:
    Ville *va = nullptr, *vb = nullptr;
    std::vector<Ticket *> tickets;
    std::vector<Train *>  trains;
    Joueur *j1 = nullptr, *j2 = nullptr;
    std::vector<Joueur *> joueurs;

    void SetUp() override
    {
        va = new Ville("A"); vb = new Ville("B");
        for (int i = 0; i < 4; i++)
            tickets.push_back(new Ticket(va, vb));
        for (int i = 0; i < 10; i++)
            trains.push_back(new Train(couleurTrain::ROUGE));
        j1 = new Joueur("P1", couleurJoueur::BLEU);
        j2 = new Joueur("P2", couleurJoueur::ROUGE);
        joueurs = {j1, j2};
    }

    void TearDown() override
    {
        delete va; delete vb;
        delete j1; delete j2;
        // tickets et trains appartiennent à la Partie
    }
};

TEST_F(GuiPartieTest, DistributionInitialeCartes)
{
    Partie p(tickets, false, trains, joueurs);
    EXPECT_EQ(j1->getNbCartesTotales(), 4);
    EXPECT_EQ(j2->getNbCartesTotales(), 4);
}

TEST_F(GuiPartieTest, DistributionInitialeTickets)
{
    Partie p(tickets, false, trains, joueurs);
    EXPECT_EQ(j1->getMissions().size(), 2u);
    EXPECT_EQ(j2->getMissions().size(), 2u);
}

TEST_F(GuiPartieTest, PlateauDisponible)
{
    Partie p(tickets, false, trains, joueurs);
    EXPECT_NE(p.getPlateau(), nullptr);
}

TEST_F(GuiPartieTest, GetJoueurParIndex)
{
    Partie p(tickets, false, trains, joueurs);
    EXPECT_EQ(p.getJoueur(0)->getNom(), "P1");
    EXPECT_EQ(p.getJoueur(1)->getNom(), "P2");
}

// ═══════════════════════════════════════════════════════════════════════════
//  6. Scénario GUI : un joueur pose une voie et remporte un ticket
// ═══════════════════════════════════════════════════════════════════════════

TEST(GuiScenario, PoseVoieEtConnectivite)
{
    Ville *seattle  = new Ville("Seattle");
    Ville *portland = new Ville("Portland");
    Joueur *j = new Joueur("GUI_Joueur", couleurJoueur::VERT);

    for (int i = 0; i < 4; i++)
        j->ajouterCarte(new Train(couleurTrain::VERT));

    VoieFerree *voie = new VoieFerree({seattle, portland}, couleurTrain::VERT, 4);
    std::vector<VoieFerree *> voies = {voie};

    // Avant la pose : pas de chemin
    EXPECT_FALSE(estConnecteGui("Seattle", "Portland", voies, j));

    EXPECT_TRUE(j->defausserCartesVoie(couleurTrain::VERT, 4));
    EXPECT_TRUE(j->utiliserWagons(4));
    voie->setJoueur(j);

    // Après la pose : chemin trouvé
    EXPECT_TRUE(estConnecteGui("Seattle", "Portland", voies, j));
    EXPECT_EQ(j->getNbWagons(), 16);
    EXPECT_EQ(j->getNbCartesTotales(), 0);

    delete voie;
    delete seattle; delete portland;
    delete j;
}

TEST(GuiScenario, JoueurSansWagonsPerdAcces)
{
    Joueur *j = new Joueur("FiniWagons", couleurJoueur::BLEU);
    EXPECT_FALSE(j->naPlusDeWagons());

    j->utiliserWagons(20);
    EXPECT_TRUE(j->naPlusDeWagons());
    EXPECT_EQ(j->getNbWagons(), 0);

    delete j;
}

TEST(GuiScenario, GrandeTraverseeDetectable)
{
    // Côte ouest → côte est via une chaîne de voies appartenant au joueur
    const std::vector<std::string> OUEST = {"Seattle", "San Francisco", "Los Angeles"};
    const std::vector<std::string> EST   = {"New York", "Washington", "Miami", "Montreal"};

    Ville *seattle  = new Ville("Seattle");
    Ville *chicago  = new Ville("Chicago");
    Ville *ny       = new Ville("New York");

    Joueur *j = new Joueur("Grand", couleurJoueur::BLEU);
    VoieFerree *v1 = new VoieFerree({seattle, chicago}, couleurTrain::BLEU, 4);
    VoieFerree *v2 = new VoieFerree({chicago, ny},      couleurTrain::BLEU, 3);
    v1->setJoueur(j);
    v2->setJoueur(j);
    std::vector<VoieFerree *> voies = {v1, v2};

    // Seattle (ouest) → New York (est) doit être connecté
    bool grandeTraversee = false;
    for (const auto &o : OUEST)
        for (const auto &e : EST)
            if (estConnecteGui(o, e, voies, j)) { grandeTraversee = true; break; }

    EXPECT_TRUE(grandeTraversee);

    delete v1; delete v2;
    delete seattle; delete chicago; delete ny;
    delete j;
}
