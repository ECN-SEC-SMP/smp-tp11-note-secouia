// Aventuriers du Rail – main.cpp
// Chargement depuis CSV, règles complètes :
//   - Mise en place (4 cartes train + 2 tickets par joueur)
//   - Piocher 2 cartes train
//   - Prendre une voie ferrée (locomotives jokers)
//   - Passer son tour (défausser tickets + reprendre 2)
//   - Validation automatique des tickets via BFS
//   - Pioche un nouveau ticket après en avoir réussi un
//   - Recyclage de la défausse quand la pioche est vide
//   - Grande traversée (relier côte ouest à côte est)
//   - Fin de partie : 6e ticket ou dernier wagon


// Note : voir pour refaire la pioche de trains car pau de cartes, voir pour mélanger les cartes
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <random>
#include <limits>

#include "Partie.hpp"
#include "Joueur.hpp"
#include "Ticket.hpp"
#include "Train.hpp"
#include "VoieFerree.hpp"
#include "Ville.hpp"
#include "couleurEnum.hpp"

using namespace std;

// ── couleurs ─────────────────────────────────────────────────────────────────

static couleurTrain couleurFromString(const string& s)
{
    if (s == "black")  return couleurTrain::NOIR;
    if (s == "red")    return couleurTrain::ROUGE;
    if (s == "yellow") return couleurTrain::JAUNE;
    if (s == "blue")   return couleurTrain::BLEU;
    if (s == "white")  return couleurTrain::BLANC;
    if (s == "green")  return couleurTrain::VERT;
    if (s == "orange") return couleurTrain::JAUNE; // pas d'orange dans l'enum
    return couleurTrain::NOIR;
}

static string nomTrain(couleurTrain c)
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

static string nomJoueur(couleurJoueur c)
{
    switch (c) {
        case couleurJoueur::JAUNE: return "Jaune";
        case couleurJoueur::BLEU:  return "Bleu";
        case couleurJoueur::ROUGE: return "Rouge";
        case couleurJoueur::VERT:  return "Vert";
        default:                   return "?";
    }
}

// ── affichage ────────────────────────────────────────────────────────────────

static void afficherMain(const Joueur* j)
{
    vector<couleurTrain> cs = {
        couleurTrain::JAUNE, couleurTrain::ROUGE, couleurTrain::VERT,
        couleurTrain::BLEU,  couleurTrain::BLANC, couleurTrain::NOIR,
        couleurTrain::MULTI
    };
    cout << "Cartes [" << j->getNbCartesTotales() << "] : ";
    for (auto c : cs) {
        int n = j->getNbCartes(c);
        if (n > 0) cout << nomTrain(c) << "x" << n << "  ";
    }
    cout << "\n";
}

static void afficherMissions(const Joueur* j)
{
    const auto& ms = j->getMissions();
    if (ms.empty()) { cout << "Missions : aucune\n"; return; }
    cout << "Missions (" << ms.size() << ") :\n";
    for (auto t : ms)
        cout << "  - " << t->getVilleDepart()->getNom()
             << " -> " << t->getVilleArrivee()->getNom() << "\n";
}

static void afficherPlateau(const vector<VoieFerree*>& voies)
{
    cout << "\n=== Plateau ===\n";
    for (auto v : voies) {
        auto lv     = v->getListeVille();
        string prop = v->getJoueur() ? v->getJoueur()->getNom() : "libre";
        cout << "  " << lv[0]->getNom() << " <-> " << lv[1]->getNom()
             << "  [" << nomTrain(v->getCouleur()) << "/" << v->getPoids() << "]"
             << "  " << prop << "\n";
    }
    cout << "\n";
}

// ── BFS connectivité ────────────────────────────────────────────────────────

static bool estConnecte(const string& a, const string& b,
                        const vector<VoieFerree*>& voies, Joueur* joueur)
{
    if (a == b) return true;
    map<string, vector<string>> adj;
    for (auto v : voies) {
        if (v->getJoueur() != joueur) continue;
        auto lv = v->getListeVille();
        for (size_t i = 0; i < lv.size(); i++)
            for (size_t k = 0; k < lv.size(); k++)
                if (i != k)
                    adj[lv[i]->getNom()].push_back(lv[k]->getNom());
    }
    set<string> vis;
    vector<string> q = {a};
    vis.insert(a);
    while (!q.empty()) {
        string cur = q.back(); q.pop_back();
        if (cur == b) return true;
        for (const auto& nxt : adj[cur])
            if (!vis.count(nxt)) { vis.insert(nxt); q.push_back(nxt); }
    }
    return false;
}

// ── Grande traversée ─────────────────────────────────────────────────────────

static const vector<string> COTE_OUEST = {"Seattle", "San Francisco", "Los Angeles"};
static const vector<string> COTE_EST   = {"New York", "Washington", "Miami", "Montreal"};

static bool verifierGrandeTraversee(Joueur* j, const vector<VoieFerree*>& voies)
{
    for (const auto& ouest : COTE_OUEST)
        for (const auto& est : COTE_EST)
            if (estConnecte(ouest, est, voies, j)) return true;
    return false;
}

// ── Gestion de la pioche de tickets ──────────────────────────────────────────
// Deux niveaux : pioche de Partie (via partie.piocherTicket) puis
// pioche locale alimentée par la défausse recyclée.

struct PiocheTickets {
    int             partieLeft; // tickets restants dans Partie
    vector<Ticket*> locale;     // pioche locale (recyclage)
    vector<Ticket*> defausse;   // tickets défaussés en attente
    mt19937&        rng;

    PiocheTickets(int n, mt19937& g) : partieLeft(n), rng(g) {}

    bool vide() const
    {
        return partieLeft <= 0 && locale.empty() && defausse.empty();
    }

    // Recycle la défausse si nécessaire puis pioche 1 ticket pour j.
    // Retourne true si un ticket a été pioché.
    bool piocher(Joueur& j, Partie& partie)
    {
        // Recyclage si la pioche principale et locale sont vides
        if (partieLeft <= 0 && locale.empty()) {
            if (defausse.empty()) return false;
            locale = defausse;
            defausse.clear();
            shuffle(locale.begin(), locale.end(), rng);
            cout << "  [Pioche épuisée – défausse recyclée ("
                 << locale.size() << " tickets)]\n";
        }

        if (partieLeft > 0) {
            size_t avant = j.getMissions().size();
            partie.piocherTicket(j);
            partieLeft--;
            if (j.getMissions().size() > avant) {
                auto t = j.getMissions().back();
                cout << "  Ticket pioché : "
                     << t->getVilleDepart()->getNom()
                     << " -> " << t->getVilleArrivee()->getNom() << "\n";
            }
        } else {
            Ticket* t = locale.back(); locale.pop_back();
            j.ajouterTicket(t);
            cout << "  Ticket pioché : "
                 << t->getVilleDepart()->getNom()
                 << " -> " << t->getVilleArrivee()->getNom() << "\n";
        }
        return true;
    }

    void defausser(vector<Ticket*>& tickets)
    {
        for (auto t : tickets) defausse.push_back(t);
    }
};

// ── Validation des tickets + auto-pioche ─────────────────────────────────────

static void validerTickets(Joueur* j,
                           const vector<VoieFerree*>& voies,
                           Plateau* plateau,
                           PiocheTickets& pioche,
                           Partie& partie)
{
    // Copie car validerTicket() modifie missions
    vector<Ticket*> ms = j->getMissions();
    for (auto t : ms) {
        if (estConnecte(t->getVilleDepart()->getNom(),
                        t->getVilleArrivee()->getNom(), voies, j)) {
            // Le Plateau vide retourne toujours vrai dans existeChemin
            if (j->validerTicket(t, *plateau)) {
                cout << "  [Ticket!] "
                     << t->getVilleDepart()->getNom()
                     << " -> " << t->getVilleArrivee()->getNom() << "\n";
                // Pioche automatique d'un nouveau ticket
                pioche.piocher(*j, partie);
            }
        }
    }
}

// ── Chargement CSV ────────────────────────────────────────────────────────────

static void trim(string& s)
{
    while (!s.empty() && (s.front() == ' ' || s.front() == '\r')) s.erase(s.begin());
    while (!s.empty() && (s.back()  == ' ' || s.back()  == '\r')) s.pop_back();
}

static Ville* getOuCreerVille(const string& nom, vector<Ville*>& villes)
{
    for (auto v : villes) if (v->getNom() == nom) return v;
    auto* v = new Ville(nom); villes.push_back(v); return v;
}

static void chargerMap(const string& fichier,
                       vector<Ville*>& villes,
                       vector<VoieFerree*>& voies)
{
    ifstream f(fichier);
    if (!f.is_open()) { cerr << "Erreur : impossible d'ouvrir " << fichier << "\n"; return; }
    string ligne;
    getline(f, ligne); // skip header
    while (getline(f, ligne)) {
        if (ligne.empty() || ligne[0] == '\r') continue;
        istringstream ss(ligne);
        string a, b, coul, len;
        if (!getline(ss, a, ',') || !getline(ss, b, ',') ||
            !getline(ss, coul, ',') || !getline(ss, len, ',')) continue;
        trim(a); trim(b); trim(coul); trim(len);
        if (a.empty() || b.empty() || coul.empty() || len.empty()) continue;
        int poids = stoi(len);
        Ville* va = getOuCreerVille(a, villes);
        Ville* vb = getOuCreerVille(b, villes);
        voies.push_back(new VoieFerree(vector<Ville*>{va, vb}, couleurFromString(coul), poids));
    }
}

static vector<Ticket*> chargerTickets(const string& fichier,
                                      const vector<Ville*>& villes)
{
    vector<Ticket*> pile;
    ifstream f(fichier);
    if (!f.is_open()) { cerr << "Erreur : impossible d'ouvrir " << fichier << "\n"; return pile; }
    string ligne;
    getline(f, ligne); // skip header
    while (getline(f, ligne)) {
        if (ligne.empty() || ligne[0] == '\r') continue;
        istringstream ss(ligne);
        string id, a, b;
        if (!getline(ss, id, ',') || !getline(ss, a, ',') || !getline(ss, b, ',')) continue;
        trim(a); trim(b);
        if (a.empty() || b.empty()) continue;
        Ville* va = nullptr; Ville* vb = nullptr;
        for (auto v : villes) {
            if (v->getNom() == a) va = v;
            if (v->getNom() == b) vb = v;
        }
        if (va && vb) pile.push_back(new Ticket(va, vb));
    }
    return pile;
}

// ── Pioche de trains (72 cartes : 10 par couleur + 12 locos) ─────────────────

static vector<Train*> creerTrains()
{
    vector<Train*> pile;
    vector<couleurTrain> cs = {
        couleurTrain::JAUNE, couleurTrain::ROUGE, couleurTrain::VERT,
        couleurTrain::BLEU,  couleurTrain::BLANC, couleurTrain::NOIR
    };
    for (auto c : cs)
        for (int i = 0; i < 10; i++)
            pile.push_back(new Train(c));
    for (int i = 0; i < 12; i++)
        pile.push_back(new Train(couleurTrain::MULTI));
    return pile;
}

// ── Tour d'un joueur ─────────────────────────────────────────────────────────

static bool tourJoueur(Joueur* j, Partie& partie,
                       vector<VoieFerree*>& voies,
                       int& trainsLeft,
                       PiocheTickets& pioche,
                       bool& grandeTraverseeGagnee,
                       const vector<Ville*>& villes)
{
    cout << "\n--- " << j->getNom()
         << " (" << nomJoueur(j->getCouleur()) << ")"
         << "  wagons=" << j->getNbWagons()
         << "  tickets=" << j->getTicketFini() << "/6 ---\n";
    afficherMain(j);
    afficherMissions(j);

    while (true) {
        cout << "Actions :\n"
             << "  1. Piocher 2 cartes train"
             << (trainsLeft <= 0 ? " [vide]" : "") << "\n"
             << "  2. Poser ses wagons (prendre une voie)\n"
             << "  3. Passer son tour (defausser tickets et en reprendre 2)\n"
             << "  4. Voir le plateau\n"
             << "Choix : ";

        int choix = 0;
        if (!(cin >> choix)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        // ── 1. Piocher 2 cartes train ─────────────────────────────────────
        if (choix == 1) {
            if (trainsLeft <= 0) { cout << "Pioche de trains vide.\n"; continue; }
            int nb = min(2, trainsLeft);
            for (int i = 0; i < nb; i++) { partie.piocherTrain(*j); trainsLeft--; }
            cout << nb << " carte(s) piochee(s).\n";
            afficherMain(j);
            return false;

        // ── 2. Prendre une voie ───────────────────────────────────────────
        } else if (choix == 2) {
            vector<VoieFerree*> dispo;
            for (auto v : voies) if (!v->getJoueur()) dispo.push_back(v);
            if (dispo.empty()) { cout << "Aucune voie libre.\n"; continue; }

            cout << "Voies disponibles :\n";
            for (size_t i = 0; i < dispo.size(); i++) {
                auto lv = dispo[i]->getListeVille();
                cout << "  " << i << ". "
                     << lv[0]->getNom() << " <-> " << lv[1]->getNom()
                     << "  [" << nomTrain(dispo[i]->getCouleur())
                     << "/" << dispo[i]->getPoids() << " wagons]\n";
            }
            cout << "Voie (-1 pour annuler) ? ";

            int idx = -1;
            if (!(cin >> idx)) { cin.clear(); cin.ignore(1000, '\n'); continue; }
            if (idx == -1) continue;
            if (idx < 0 || idx >= static_cast<int>(dispo.size())) {
                cout << "Index invalide.\n"; continue;
            }

            VoieFerree*  voie    = dispo[static_cast<size_t>(idx)];
            int          poids   = voie->getPoids();
            couleurTrain couleur = voie->getCouleur();

            if (j->getNbWagons() < poids) {
                cout << "Pas assez de wagons (besoin=" << poids << ").\n";
                continue;
            }
            if (!j->peutPrendreVoie(couleur, poids)) {
                cout << "Pas assez de cartes (besoin=" << poids << ", "
                     << "vous avez " << nomTrain(couleur) << "x"
                     << j->getNbCartes(couleur)
                     << " + Locox" << j->getNbCartes(couleurTrain::MULTI)
                     << " = " << j->getNbCartes(couleur) + j->getNbCartes(couleurTrain::MULTI)
                     << ").\n";
                continue;
            }

            j->defausserCartesVoie(couleur, poids);
            j->utiliserWagons(poids);
            voie->setJoueur(j);
            cout << "Voie prise !\n";

            // Validation des tickets
            validerTickets(j, voies, partie.getPlateau(), pioche, partie);

            // Grande traversée
            if (!grandeTraverseeGagnee && verifierGrandeTraversee(j, voies)) {
                grandeTraverseeGagnee = true;
                // Ticket bonus : validerTicket sur Plateau vide incrémente
                // ticketFini sans supprimer quoi que ce soit des missions
                Ticket bonus(villes[0], villes[0]);
                j->validerTicket(&bonus, *partie.getPlateau());
                cout << "  [Grande Traversee!] " << j->getNom()
                     << " relie la cote ouest a la cote est ! (+1 ticket bonus)\n";
            }

            return j->aGagne();

        // ── 3. Passer son tour ────────────────────────────────────────────
        } else if (choix == 3) {
            if (j->getMissions().empty()) {
                cout << "Vous n'avez aucun ticket a defausser.\n"; continue;
            }
            vector<Ticket*> defausses = j->defausserTickets();
            cout << defausses.size() << " ticket(s) defausse(s).\n";
            pioche.defausser(defausses);

            // Pioche 2 nouveaux tickets
            int nb = 0;
            for (int i = 0; i < 2 && !pioche.vide(); i++) {
                if (pioche.piocher(*j, partie)) nb++;
            }
            if (nb == 0) cout << "Pioche de tickets vide, aucun ticket repris.\n";
            return false;

        // ── 4. Voir le plateau ────────────────────────────────────────────
        } else if (choix == 4) {
            afficherPlateau(voies);

        } else {
            cout << "Choix invalide.\n";
        }
    }
}

// ── main ─────────────────────────────────────────────────────────────────────

int main()
{
    cout << "=== Aventuriers du Rail ===\n";
    {
        ifstream f(ASCII_ART);
        if (f.is_open()) { string l; while (getline(f, l)) cout << l << "\n"; f.close(); }
    }

    // Joueurs
    int nbJ = 0;
    do {
        cout << "Nombre de joueurs (2-4) : ";
        cin >> nbJ;
        if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); nbJ = 0; }
    } while (nbJ < 2 || nbJ > 4);

    vector<Joueur*> joueurs;
    for (int i = 0; i < nbJ; i++) {
        cout << "Nom du joueur " << (i + 1) << " : ";
        string nom; cin >> nom;
        joueurs.push_back(new Joueur(nom, static_cast<couleurJoueur>(i)));
    }

    // Chargement depuis CSV (lancer depuis la racine du projet)
    vector<Ville*>      villes;
    vector<VoieFerree*> voies;
    chargerMap(string(FILES_DIR) + "/map.csv", villes, voies);

    vector<Train*>  trains  = creerTrains();
    vector<Ticket*> tickets = chargerTickets(string(FILES_DIR) + "/ticket.csv", villes);

    if (voies.empty())   { cerr << "Erreur : carte vide\n";    return 1; }
    if (tickets.empty()) { cerr << "Erreur : pas de tickets\n"; return 1; }

    // RNG partagé
    mt19937 rng(random_device{}());

    // Initialisation : Partie distribue 4 trains + 2 tickets par joueur
    Partie partie(tickets, false, trains, joueurs);

    int trainsLeft  = static_cast<int>(trains.size())  - nbJ * 4;
    int ticketsLeft = static_cast<int>(tickets.size()) - nbJ * 2;
    PiocheTickets pioche(ticketsLeft, rng);

    bool   grandeTraverseeGagnee = false;
    Joueur* gagnant = nullptr;
    size_t  tour    = 0;

    cout << "\n--- Debut de la partie ---\n";
    afficherPlateau(voies);

    while (!gagnant) {
        Joueur* j = joueurs[tour % static_cast<size_t>(nbJ)];

        if (tourJoueur(j, partie, voies, trainsLeft, pioche,
                       grandeTraverseeGagnee, villes)) {
            gagnant = j;
        } else if (j->naPlusDeWagons()) {
            cout << j->getNom() << " n'a plus de wagons !\n";
            gagnant = j;
        }
        tour++;
    }

    cout << "\n=== Fin de partie ===\n"
         << "Vainqueur : " << gagnant->getNom()
         << "  (" << gagnant->getTicketFini() << " ticket(s) completes)\n";

    // Nettoyage
    for (auto v : voies)   delete v;
    for (auto v : villes)  delete v;
    for (auto j : joueurs) delete j;
    for (auto t : tickets) delete t;
    for (auto t : trains)  delete t;

    return 0;
}
