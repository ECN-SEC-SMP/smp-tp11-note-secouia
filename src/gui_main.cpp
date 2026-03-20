// gui_main.cpp – Interface graphique SFML 3 pour Aventuriers du Rail
// Exécutable séparé (aventuriers_gui), logique de jeu autonome.

#include <SFML/Graphics.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <algorithm>
#include <random>
#include <cmath>
#include <optional>

#include "Partie.hpp"
#include "Joueur.hpp"
#include "Ticket.hpp"
#include "Train.hpp"
#include "VoieFerree.hpp"
#include "Ville.hpp"
#include "couleurEnum.hpp"

using namespace std;

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 1 – Logique de jeu
// ═══════════════════════════════════════════════════════════════════════════════

static couleurTrain couleurFromString(const string& s)
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

static string nomJoueurCol(couleurJoueur c)
{
    switch (c) {
        case couleurJoueur::JAUNE: return "Jaune";
        case couleurJoueur::BLEU:  return "Bleu";
        case couleurJoueur::ROUGE: return "Rouge";
        case couleurJoueur::VERT:  return "Vert";
        default:                   return "?";
    }
}

static void trimStr(string& s)
{
    while (!s.empty() && (s.front() == ' ' || s.front() == '\r')) s.erase(s.begin());
    while (!s.empty() && (s.back()  == ' ' || s.back()  == '\r')) s.pop_back();
}

static Ville* getOuCreer(const string& nom, vector<Ville*>& villes)
{
    for (auto v : villes) if (v->getNom() == nom) return v;
    auto* v = new Ville(nom); villes.push_back(v); return v;
}

static void chargerMap(const string& fichier,
                       vector<Ville*>& villes, vector<VoieFerree*>& voies)
{
    ifstream f(fichier);
    if (!f.is_open()) return;
    string ligne;
    getline(f, ligne);
    while (getline(f, ligne)) {
        if (ligne.empty() || ligne[0] == '\r') continue;
        istringstream ss(ligne);
        string a, b, coul, len;
        if (!getline(ss,a,',') || !getline(ss,b,',') ||
            !getline(ss,coul,',') || !getline(ss,len,',')) continue;
        trimStr(a); trimStr(b); trimStr(coul); trimStr(len);
        if (a.empty()||b.empty()||coul.empty()||len.empty()) continue;
        voies.push_back(new VoieFerree(
            vector<Ville*>{getOuCreer(a,villes), getOuCreer(b,villes)},
            couleurFromString(coul), stoi(len)));
    }
}

static vector<Ticket*> chargerTickets(const string& fichier,
                                      const vector<Ville*>& villes)
{
    vector<Ticket*> pile;
    ifstream f(fichier);
    if (!f.is_open()) return pile;
    string ligne;
    getline(f, ligne);
    while (getline(f, ligne)) {
        if (ligne.empty() || ligne[0] == '\r') continue;
        istringstream ss(ligne);
        string id, a, b;
        if (!getline(ss,id,',') || !getline(ss,a,',') || !getline(ss,b,',')) continue;
        trimStr(a); trimStr(b);
        Ville *va=nullptr, *vb=nullptr;
        for (auto v : villes) { if(v->getNom()==a) va=v; if(v->getNom()==b) vb=v; }
        if (va && vb) pile.push_back(new Ticket(va, vb));
    }
    return pile;
}

static vector<Train*> creerTrains()
{
    vector<Train*> pile;
    for (auto c : {couleurTrain::JAUNE, couleurTrain::ROUGE, couleurTrain::VERT,
                   couleurTrain::BLEU,  couleurTrain::BLANC, couleurTrain::NOIR})
        for (int i = 0; i < 10; i++) pile.push_back(new Train(c));
    for (int i = 0; i < 12; i++) pile.push_back(new Train(couleurTrain::MULTI));
    return pile;
}

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
                if (i != k) adj[lv[i]->getNom()].push_back(lv[k]->getNom());
    }
    set<string> vis; vector<string> q = {a}; vis.insert(a);
    while (!q.empty()) {
        string cur = q.back(); q.pop_back();
        if (cur == b) return true;
        for (const auto& n : adj[cur])
            if (!vis.count(n)) { vis.insert(n); q.push_back(n); }
    }
    return false;
}

static const vector<string> COTE_OUEST = {"Seattle","San Francisco","Los Angeles"};
static const vector<string> COTE_EST   = {"New York","Washington","Miami","Montreal"};

static bool verifierGrandeTraversee(Joueur* j, const vector<VoieFerree*>& voies)
{
    for (const auto& o : COTE_OUEST)
        for (const auto& e : COTE_EST)
            if (estConnecte(o, e, voies, j)) return true;
    return false;
}

struct PiocheTickets {
    int             partieLeft;
    vector<Ticket*> locale;
    vector<Ticket*> defausse;
    mt19937&        rng;

    PiocheTickets(int n, mt19937& g) : partieLeft(n), rng(g) {}
    bool vide() const { return partieLeft <= 0 && locale.empty() && defausse.empty(); }

    bool piocher(Joueur& j, Partie& partie, vector<string>& log)
    {
        if (partieLeft <= 0 && locale.empty()) {
            if (defausse.empty()) { log.push_back("Pioche tickets vide !"); return false; }
            locale = defausse; defausse.clear();
            shuffle(locale.begin(), locale.end(), rng);
            log.push_back("[Défausse recyclée]");
        }
        if (partieLeft > 0) {
            size_t avant = j.getMissions().size();
            partie.piocherTicket(j); partieLeft--;
            if (j.getMissions().size() > avant) {
                auto t = j.getMissions().back();
                log.push_back("Ticket: " + t->getVilleDepart()->getNom()
                              + " > " + t->getVilleArrivee()->getNom());
            }
        } else {
            Ticket* t = locale.back(); locale.pop_back();
            j.ajouterTicket(t);
            log.push_back("Ticket: " + t->getVilleDepart()->getNom()
                          + " > " + t->getVilleArrivee()->getNom());
        }
        return true;
    }
    void defausser(vector<Ticket*>& tickets)
    { for (auto t : tickets) defausse.push_back(t); }
};

static void validerTickets(Joueur* j, const vector<VoieFerree*>& voies,
                           Plateau* plateau, PiocheTickets& pioche,
                           Partie& partie, vector<string>& log)
{
    vector<Ticket*> ms = j->getMissions();
    for (auto t : ms)
        if (estConnecte(t->getVilleDepart()->getNom(),
                        t->getVilleArrivee()->getNom(), voies, j))
            if (j->validerTicket(t, *plateau)) {
                log.push_back("[Ticket!] " + t->getVilleDepart()->getNom()
                              + " > " + t->getVilleArrivee()->getNom());
                pioche.piocher(*j, partie, log);
            }
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 2 – Palette Bootstrap-dark & positions
// ═══════════════════════════════════════════════════════════════════════════════

namespace BS {
    // Fond & surfaces
    const sf::Color DarkBg    = {  6,  8, 20};
    const sf::Color Card      = { 16, 20, 46};
    const sf::Color CardBdr   = { 38, 50, 95};
    const sf::Color Dark      = { 22, 26, 48};
    // Accents aventure
    const sf::Color Gold      = {212,168, 22};
    const sf::Color GoldLight = {255,220, 80};
    const sf::Color Warning   = {255,190,  0};
    // Actions
    const sf::Color Primary   = { 55,120,255};
    const sf::Color PrimaryH  = { 88,152,255};
    const sf::Color Success   = {  0,178,120};
    const sf::Color Danger    = {218, 50, 70};
    const sf::Color Secondary = { 72, 85,120};
    // Texte
    const sf::Color Light     = {215,222,245};
    const sf::Color Muted     = { 85, 98,135};
    // Carte géographique (utilisées dans drawMap)
    const sf::Color MapBg1    = { 18, 52, 35};   // terrain forêt
    const sf::Color MapOcean  = { 12, 30, 65};   // océan
    const sf::Color MapMtn    = { 38, 55, 45};   // montagnes
}

static sf::Color lerp(sf::Color a, sf::Color b, float t)
{
    t = std::max(0.f, std::min(1.f, t));
    return {
        static_cast<uint8_t>(a.r + t * (static_cast<float>(b.r) - static_cast<float>(a.r))),
        static_cast<uint8_t>(a.g + t * (static_cast<float>(b.g) - static_cast<float>(a.g))),
        static_cast<uint8_t>(a.b + t * (static_cast<float>(b.b) - static_cast<float>(a.b))),
        static_cast<uint8_t>(a.a + t * (static_cast<float>(b.a) - static_cast<float>(a.a)))
    };
}

static sf::Color sfRoute(couleurTrain c)
{
    switch (c) {
        case couleurTrain::JAUNE: return {230,190,  0};
        case couleurTrain::ROUGE: return {210, 45, 45};
        case couleurTrain::VERT:  return { 35,170, 65};
        case couleurTrain::BLEU:  return { 30,110,230};
        case couleurTrain::BLANC: return {210,220,230};
        case couleurTrain::NOIR:  return { 70, 70, 80};
        case couleurTrain::MULTI: return {180, 80,220};
        default:                  return {128,128,128};
    }
}

static sf::Color sfJoueur(couleurJoueur c)
{
    switch (c) {
        case couleurJoueur::JAUNE: return {255,215,  0};
        case couleurJoueur::BLEU:  return { 30,144,255};
        case couleurJoueur::ROUGE: return {230, 55, 55};
        case couleurJoueur::VERT:  return { 50,210, 80};
        default:                   return {200,200,200};
    }
}

static const map<string, sf::Vector2f> CITY_POS = {
    {"Seattle",         { 85.f,  90.f}},
    {"Calgary",         {215.f,  60.f}},
    {"Helena",          {295.f, 155.f}},
    {"Winnipeg",        {425.f,  70.f}},
    {"Duluth",          {525.f, 180.f}},
    {"Montreal",        {650.f, 100.f}},
    {"San Francisco",   { 70.f, 295.f}},
    {"Salt Lake City",  {235.f, 270.f}},
    {"Denver",          {315.f, 335.f}},
    {"Kansas City",     {440.f, 340.f}},
    {"Chicago",         {535.f, 245.f}},
    {"Washington",      {660.f, 280.f}},
    {"Los Angeles",     { 80.f, 420.f}},
    {"Albuquerque",     {250.f, 425.f}},
    {"Dallas",          {380.f, 470.f}},
    {"New Orleans",     {485.f, 510.f}},
    {"Atlanta",         {565.f, 450.f}},
    {"New York",        {675.f, 220.f}},
    {"Miami",           {620.f, 560.f}}
};

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 3 – Helpers de dessin
// ═══════════════════════════════════════════════════════════════════════════════

// Fix accents : convertit une std::string UTF-8 en sf::String correctement
static sf::String sfStr(const string& s)
{
    return sf::String::fromUtf8(s.begin(), s.end());
}

static sf::Text mkText(const sf::Font& font, const string& s,
                       unsigned sz, sf::Color col, float x, float y)
{
    sf::Text t(font, sfStr(s), sz);
    t.setFillColor(col);
    t.setPosition({x, y});
    return t;
}

static bool btnHit(float bx, float by, float bw, float bh, sf::Vector2i p)
{
    return p.x >= static_cast<int>(bx) && p.x <= static_cast<int>(bx+bw) &&
           p.y >= static_cast<int>(by) && p.y <= static_cast<int>(by+bh);
}

// Dessine une ombre portée multicouche
static void drawShadow(sf::RenderWindow& w, float x, float y, float bw, float bh,
                       uint8_t alpha = 90u)
{
    for (int i = 5; i >= 1; i--) {
        float fi = static_cast<float>(i);
        sf::RectangleShape s({bw + fi*2.f, bh + fi*2.f});
        s.setPosition({x - fi + fi*2.f, y + fi*1.5f});
        s.setFillColor({0, 0, 0, static_cast<uint8_t>(static_cast<float>(alpha) / (fi + 1.f))});
        w.draw(s);
    }
}

// Dessine un bouton aventure : dégradé, bordure dorée au hover, effet pressé
static void drawBtn(sf::RenderWindow& w, const sf::Font& font,
                    float x, float y, float bw, float bh,
                    const string& label,
                    bool disabled,
                    sf::Vector2i mouse,
                    float time,
                    sf::Color base = BS::Primary,
                    sf::Color hoverCol = BS::PrimaryH)
{
    bool hovered = !disabled && btnHit(x, y, bw, bh, mouse);
    float pulse  = (std::sin(time * 3.5f) + 1.f) * 0.5f;

    // Effet pressé au hover : léger décalage vers le bas
    float offY = hovered ? 2.f : 0.f;

    sf::Color bgTop, bgBot;
    if (disabled) {
        bgTop = lerp(BS::Secondary, {100,110,150}, 0.3f);
        bgBot = BS::Secondary;
    } else if (hovered) {
        bgTop = lerp(hoverCol, {255,255,255}, 0.15f + 0.05f * pulse);
        bgBot = hoverCol;
    } else {
        bgTop = lerp(base, {255,255,255}, 0.12f);
        bgBot = lerp(base, {0,0,0}, 0.15f);
    }

    // Ombre
    if (!disabled && !hovered) drawShadow(w, x, y + offY, bw, bh, 80u);

    // Corps du bouton : demi-haut + demi-bas
    sf::RectangleShape top({bw, bh / 2.f});
    top.setPosition({x, y + offY});
    top.setFillColor(bgTop);
    w.draw(top);
    sf::RectangleShape bot({bw, bh / 2.f});
    bot.setPosition({x, y + offY + bh / 2.f});
    bot.setFillColor(bgBot);
    w.draw(bot);

    // Bordure extérieure
    sf::Color border;
    if (disabled)     border = {60, 70, 100, 120};
    else if (hovered) border = lerp(BS::GoldLight, BS::Gold, pulse);
    else              border = lerp(base, {200,200,255}, 0.35f);
    sf::RectangleShape frame({bw, bh});
    frame.setPosition({x, y + offY});
    frame.setFillColor(sf::Color::Transparent);
    frame.setOutlineColor(border);
    frame.setOutlineThickness(1.5f);
    w.draw(frame);

    // Reflet supérieur
    sf::RectangleShape shine({bw - 4.f, 2.f});
    shine.setPosition({x + 2.f, y + offY + 2.f});
    shine.setFillColor({255, 255, 255, static_cast<uint8_t>(disabled ? 0u : (hovered ? 55u : 28u))});
    w.draw(shine);

    // Accents aux coins (petits carrés dorés si hover)
    if (hovered) {
        float cs = 4.f;
        for (auto [cx2, cy2] : vector<pair<float,float>>{
                {x, y+offY}, {x+bw-cs, y+offY},
                {x, y+offY+bh-cs}, {x+bw-cs, y+offY+bh-cs}}) {
            sf::RectangleShape corner({cs, cs});
            corner.setPosition({cx2, cy2});
            corner.setFillColor(lerp(BS::Gold, BS::GoldLight, pulse));
            w.draw(corner);
        }
    }

    // Texte centré
    sf::Text t(font, sfStr(label), 14u);
    t.setFillColor(disabled ? BS::Muted : sf::Color::White);
    if (!disabled && hovered) t.setFillColor(BS::GoldLight);
    auto b = t.getLocalBounds();
    t.setOrigin({b.position.x + b.size.x/2.f, b.position.y + b.size.y/2.f});
    t.setPosition({x + bw/2.f, y + offY + bh/2.f});
    w.draw(t);
}

// Dessine une carte avec dégradé de fond et trait coloré facultatif en haut
static void drawCard(sf::RenderWindow& w, float x, float y, float cw, float ch,
                     sf::Color bg = BS::Card,
                     sf::Color topAccent = sf::Color::Transparent)
{
    drawShadow(w, x, y, cw, ch, 80u);
    // Corps
    sf::RectangleShape r({cw, ch});
    r.setPosition({x, y});
    r.setFillColor(bg);
    r.setOutlineColor(BS::CardBdr);
    r.setOutlineThickness(1.f);
    w.draw(r);
    // Reflet intérieur subtil (coin supérieur gauche)
    sf::RectangleShape glow({cw, ch / 3.f});
    glow.setPosition({x, y});
    glow.setFillColor({255, 255, 255, 6});
    w.draw(glow);
    // Trait coloré en haut
    if (topAccent.a > 0) {
        sf::RectangleShape bar({cw, 3.f});
        bar.setPosition({x, y});
        bar.setFillColor(topAccent);
        w.draw(bar);
    }
}

// Dessine une ligne épaisse avec décalage perpendiculaire
static void drawLine(sf::RenderWindow& w,
                     sf::Vector2f a, sf::Vector2f b,
                     sf::Color color, float thick, float perpOff = 0.f)
{
    sf::Vector2f d = b - a;
    float len = std::sqrt(d.x*d.x + d.y*d.y);
    if (len < 1.f) return;
    sf::Vector2f perp = {-d.y / len, d.x / len};
    sf::RectangleShape r({len, thick});
    r.setOrigin({0.f, thick / 2.f});
    r.setPosition(a + perp * perpOff);
    r.setRotation(sf::radians(std::atan2(d.y, d.x)));
    r.setFillColor(color);
    w.draw(r);
}

static float distSeg(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b)
{
    sf::Vector2f ab = b - a, ap = p - a;
    float t = (ap.x*ab.x + ap.y*ab.y) / (ab.x*ab.x + ab.y*ab.y + 1e-6f);
    t = std::max(0.f, std::min(1.f, t));
    sf::Vector2f cl = a + ab * t, dv = p - cl;
    return std::sqrt(dv.x*dv.x + dv.y*dv.y);
}

static map<VoieFerree*, float> calcOffsets(const vector<VoieFerree*>& voies)
{
    map<VoieFerree*, float> offsets;
    map<pair<string,string>, vector<VoieFerree*>> grp;
    for (auto v : voies) {
        auto lv = v->getListeVille();
        string a = lv[0]->getNom(), b = lv[1]->getNom();
        if (a > b) swap(a, b);
        grp[{a, b}].push_back(v);
    }
    for (auto& kv : grp) {
        auto& g = kv.second;
        if (g.size() == 1) { offsets[g[0]] = 0.f; continue; }
        float start = -(static_cast<float>(g.size()) - 1.f) * 7.f / 2.f;
        for (size_t i = 0; i < g.size(); i++)
            offsets[g[i]] = start + static_cast<float>(i) * 7.f;
    }
    return offsets;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 4 – État global
// ═══════════════════════════════════════════════════════════════════════════════

enum class AppState { SETUP_NB, SETUP_NAME, GAME, GAME_OVER };
enum class ActionSt { MENU, SELECT_ROUTE };

struct GS {
    AppState app       = AppState::SETUP_NB;
    int      nbJ       = 2;
    int      nameIdx   = 0;
    string   inputBuf;

    vector<Joueur*>     joueurs;
    vector<VoieFerree*> voies;
    vector<Ville*>      villes;
    vector<Train*>      trains;
    vector<Ticket*>     tickets;
    Partie*             partie     = nullptr;
    PiocheTickets*      pioche     = nullptr;
    mt19937             rng        {random_device{}()};
    int                 trainLeft  = 0;
    size_t              tourIdx    = 0;
    bool                grandeTrav = false;
    ActionSt            action     = ActionSt::MENU;
    Joueur*             gagnant    = nullptr;
    string              statusMsg;
    bool                shouldQuit = false;

    // Log avec timestamps pour animation de fondu
    vector<string>  log;
    vector<float>   logTimes;     // temps d'ajout de chaque entrée
    float           now = 0.f;    // mis à jour chaque frame

    void addLog(const string& s)
    {
        log.push_back(s);
        logTimes.push_back(now);
        if (log.size() > 7) {
            log.erase(log.begin());
            logTimes.erase(logTimes.begin());
        }
    }

    VoieFerree*             hovered = nullptr;
    map<VoieFerree*, float> offsets;

    std::optional<sf::Texture> logoTex;  // logo SEC/AI

    Joueur* cur() const { return joueurs[tourIdx % joueurs.size()]; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 5 – Initialisation
// ═══════════════════════════════════════════════════════════════════════════════

static void initGame(GS& gs)
{
    chargerMap(string(FILES_DIR) + "/map.csv", gs.villes, gs.voies);
    gs.trains  = creerTrains();
    gs.tickets = chargerTickets(string(FILES_DIR) + "/ticket.csv", gs.villes);
    gs.partie  = new Partie(gs.tickets, false, gs.trains, gs.joueurs);
    gs.trainLeft = static_cast<int>(gs.trains.size())  - gs.nbJ * 4;
    int tLeft    = static_cast<int>(gs.tickets.size()) - gs.nbJ * 2;
    gs.pioche    = new PiocheTickets(tLeft, gs.rng);
    gs.offsets   = calcOffsets(gs.voies);
    gs.app       = AppState::GAME;
    gs.addLog("=== Partie lancée ! ===");
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 6 – Dessin de la carte (map)
// ═══════════════════════════════════════════════════════════════════════════════

static void drawMap(sf::RenderWindow& w, const sf::Font& font,
                    const GS& gs, float time)
{
    // ── Fond terrain multicouche ────────────────────────────────────────────
    // Base : vert forêt
    sf::RectangleShape bgBase({800.f, 720.f});
    bgBase.setFillColor(BS::MapBg1);
    w.draw(bgBase);

    // Zone centrale légèrement plus claire (plaines)
    sf::RectangleShape plains({340.f, 440.f});
    plains.setPosition({230.f, 160.f});
    plains.setFillColor({24, 62, 40, 180});
    w.draw(plains);

    // Bande océan Pacifique (côte ouest)
    sf::RectangleShape pacific({90.f, 720.f});
    pacific.setFillColor(BS::MapOcean);
    w.draw(pacific);

    // Bande océan Atlantique (côte est)
    sf::RectangleShape atlantic({80.f, 720.f});
    atlantic.setPosition({720.f, 0.f});
    atlantic.setFillColor(BS::MapOcean);
    w.draw(atlantic);

    // Zone montagneuse (Rocheuses)
    sf::RectangleShape rockies({120.f, 500.f});
    rockies.setPosition({200.f, 100.f});
    rockies.setFillColor({BS::MapMtn.r, BS::MapMtn.g, BS::MapMtn.b, 120});
    w.draw(rockies);

    // Vignette (bords assombris)
    for (int edge = 0; edge < 4; edge++) {
        for (int i = 0; i < 5; i++) {
            float fi = static_cast<float>(i);
            float t  = (5.f - fi) / 5.f;
            uint8_t a = static_cast<uint8_t>(t * t * 90.f);
            float dim = fi * 12.f;
            if (edge == 0) {
                sf::RectangleShape v2({800.f, dim});
                v2.setPosition({0.f, 0.f});
                v2.setFillColor({0, 0, 0, a}); w.draw(v2);
            } else if (edge == 1) {
                sf::RectangleShape v2({800.f, dim});
                v2.setPosition({0.f, 720.f - dim});
                v2.setFillColor({0, 0, 0, a}); w.draw(v2);
            } else if (edge == 2) {
                sf::RectangleShape v2({dim, 720.f});
                v2.setPosition({0.f, 0.f});
                v2.setFillColor({0, 0, 0, a}); w.draw(v2);
            } else {
                sf::RectangleShape v2({dim, 720.f});
                v2.setPosition({800.f - dim, 0.f});
                v2.setFillColor({0, 0, 0, a}); w.draw(v2);
            }
        }
    }

    // Grille topographique fine
    for (int i = 0; i <= 16; i++) {
        float xg = static_cast<float>(i) * 50.f;
        sf::RectangleShape gl({1.f, 720.f});
        gl.setPosition({xg, 0.f});
        gl.setFillColor({255, 255, 255, 7});
        w.draw(gl);
    }
    for (int i = 0; i <= 14; i++) {
        float yg = static_cast<float>(i) * 52.f;
        sf::RectangleShape gl({800.f, 1.f});
        gl.setPosition({0.f, yg});
        gl.setFillColor({255, 255, 255, 7});
        w.draw(gl);
    }

    // ── Routes ──────────────────────────────────────────────────────────────
    for (auto v : gs.voies) {
        auto lv = v->getListeVille();
        const string& na = lv[0]->getNom();
        const string& nb = lv[1]->getNom();
        if (!CITY_POS.count(na) || !CITY_POS.count(nb)) continue;

        sf::Vector2f pa = CITY_POS.at(na), pb = CITY_POS.at(nb);
        float off    = gs.offsets.count(v) ? gs.offsets.at(v) : 0.f;
        int   poids  = v->getPoids();
        float thick  = 7.f;
        bool  owned  = v->getJoueur() != nullptr;
        bool  isHov  = (v == gs.hovered && gs.action == ActionSt::SELECT_ROUTE);
        float pulse  = (std::sin(time * 5.f) + 1.f) * 0.5f;

        sf::Color col;
        if (owned)       col = sfJoueur(v->getJoueur()->getCouleur());
        else if (isHov)  col = lerp({255,225,30}, {255,140,0}, pulse);
        else             col = sfRoute(v->getCouleur());

        if (isHov) thick = 9.f + pulse * 3.f;
        if (owned) thick = 10.f;

        sf::Vector2f d  = pb - pa;
        float len       = std::sqrt(d.x*d.x + d.y*d.y);
        sf::Vector2f ud = d / len;
        sf::Vector2f perp = {-ud.y, ud.x};

        // Rail de fond (ombre portée)
        drawLine(w, pa, pb, {0, 0, 0, 100}, thick + 4.f, off + 2.f);
        // Rail de ballast (bord extérieur foncé)
        drawLine(w, pa, pb, lerp(col, {0,0,0}, 0.5f), thick + 2.f, off);

        // Wagons individuels (slots)
        for (int seg = 0; seg < poids; seg++) {
            float t0 = (static_cast<float>(seg) + 0.08f) / static_cast<float>(poids);
            float t1 = (static_cast<float>(seg) + 0.92f) / static_cast<float>(poids);
            sf::Vector2f a2 = pa + d * t0 + perp * off;
            sf::Vector2f b2 = pa + d * t1 + perp * off;
            float segLen = std::sqrt((b2-a2).x*(b2-a2).x + (b2-a2).y*(b2-a2).y);

            sf::RectangleShape slot({segLen, thick});
            slot.setOrigin({0.f, thick / 2.f});
            slot.setPosition(a2);
            slot.setRotation(sf::radians(std::atan2(d.y, d.x)));

            sf::Color slotCol = col;
            if (owned) {
                // Effet brillant sur wagon posé
                float shimmer = (std::sin(time * 2.f + static_cast<float>(seg) * 0.8f) + 1.f) * 0.5f;
                slotCol = lerp(col, lerp(col, {255,255,255}, 0.3f), shimmer * 0.4f);
            }
            if (isHov) slotCol = lerp(col, {255,255,200}, 0.3f + 0.2f * pulse);
            slot.setFillColor(slotCol);

            // Reflet supérieur sur chaque wagon
            sf::RectangleShape shine({segLen, 2.5f});
            shine.setOrigin({0.f, 0.f});
            shine.setPosition(a2 + perp * (thick / 2.f - 3.f));
            shine.setRotation(sf::radians(std::atan2(d.y, d.x)));
            shine.setFillColor({255, 255, 255, static_cast<uint8_t>(owned ? 50u : 25u)});
            w.draw(shine);
            w.draw(slot);
        }

        // Halo de sélection
        if (isHov) {
            drawLine(w, pa, pb,
                     {255, 220, 50, static_cast<uint8_t>(40.f + pulse * 60.f)},
                     thick + 10.f, off);
        }
    }

    // ── Villes ──────────────────────────────────────────────────────────────
    for (auto& kv : CITY_POS) {
        const sf::Vector2f& pos = kv.second;
        bool isOuest = std::find(COTE_OUEST.begin(), COTE_OUEST.end(), kv.first)
                       != COTE_OUEST.end();
        bool isEst   = std::find(COTE_EST.begin(),   COTE_EST.end(),   kv.first)
                       != COTE_EST.end();
        bool isTicket = false;
        for (auto t : gs.cur()->getMissions()){
            if (t->getVilleDepart()->getNom() == kv.first ||
                t->getVilleArrivee()->getNom() == kv.first)
                { isTicket = true; break; }
        }

        float p = (std::sin(time * 2.f + pos.x * 0.04f) + 1.f) * 0.5f;

        // Halo animé grande amplitude pour villes côtières
        if (isTicket) {
            float r1 = 18.f + p * 8.f;
            sf::CircleShape outer(r1);
            outer.setOrigin({r1, r1});
            outer.setPosition(pos);
            sf::Color hc = sfJoueur(gs.cur()->getCouleur());
            hc.a = static_cast<uint8_t>(80.f + p * 100.f);
            outer.setFillColor(hc);
            w.draw(outer);
        }

        // Anneau extérieur
        sf::CircleShape ring(11.f);
        ring.setOrigin({11.f, 11.f});
        ring.setPosition(pos);
        ring.setFillColor({0, 0, 0, 0});
        ring.setOutlineColor({180, 155, 80, 200});
        ring.setOutlineThickness(2.f);
        w.draw(ring);

        // Disque principal
        sf::CircleShape disc(8.f);
        disc.setOrigin({8.f, 8.f});
        disc.setPosition(pos);
        disc.setFillColor(isOuest ? sf::Color{30, 100, 200} :
                          isEst   ? sf::Color{180, 130, 0}  :
                                    sf::Color{60, 90, 55});
        disc.setOutlineColor({220, 195, 120});
        disc.setOutlineThickness(1.5f);
        w.draw(disc);

        // Point central brillant
        {
            sf::CircleShape dot3(3.5f);
            dot3.setOrigin({3.5f, 3.5f});
            dot3.setPosition(pos);
            dot3.setFillColor({255, 248, 210, 230});
            w.draw(dot3);
        }

        // Étiquette : ombre triple + texte blanc
        auto drawLabel = [&](const string& s, float lx, float ly) {
            for (auto [dx, dy] : vector<pair<float,float>>{
                    {1.f,1.f}, {-1.f,1.f}, {0.f,1.5f}}) {
                sf::Text sh(font, sfStr(s), 10u);
                sh.setFillColor({0, 0, 0, 160});
                auto b = sh.getLocalBounds();
                sh.setOrigin({b.position.x + b.size.x/2.f, 0.f});
                sh.setPosition({lx + dx, ly + dy});
                w.draw(sh);
            }
            sf::Text lbl(font, sfStr(s), 10u);
            lbl.setFillColor({230, 220, 185});
            auto b = lbl.getLocalBounds();
            lbl.setOrigin({b.position.x + b.size.x/2.f, 0.f});
            lbl.setPosition({lx, ly});
            w.draw(lbl);
        };
        drawLabel(kv.first, pos.x, pos.y + 13.f);
    }
}

// Champ d'étoiles déterministe (aucun état mutable)
static void drawStarfield(sf::RenderWindow& w, float time,
                          float areaW = 808.f, float areaH = 720.f)
{
    for (int i = 0; i < 90; i++) {
        float seed = static_cast<float>(i) * 137.508f;
        float sx   = std::fmod(seed * 4.13f, areaW);
        float sy   = std::fmod(seed * 2.97f, areaH);
        float twinkle = (std::sin(time * 1.2f + seed * 0.05f) + 1.f) * 0.5f;
        float r2   = 0.8f + twinkle * 1.8f;
        sf::CircleShape star(r2);
        star.setOrigin({r2, r2});
        star.setPosition({sx, sy});
        uint8_t a = static_cast<uint8_t>(25.f + twinkle * 110.f);
        // Quelques étoiles dorées
        if (i % 11 == 0) star.setFillColor({255, 220, 100, a});
        else              star.setFillColor({180, 200, 255, a});
        w.draw(star);
    }
}

// Rails de train décoratifs (animation de défilement)
static void drawTrainTrack(sf::RenderWindow& w, float y, float time, float alpha01)
{
    uint8_t a = static_cast<uint8_t>(alpha01 * 30.f);
    if (a == 0) return;
    // Rail haut
    sf::RectangleShape r1({808.f, 2.f});
    r1.setPosition({0.f, y - 5.f});
    r1.setFillColor({200, 170, 100, a});
    w.draw(r1);
    // Rail bas
    sf::RectangleShape r2({808.f, 2.f});
    r2.setPosition({0.f, y + 5.f});
    r2.setFillColor({200, 170, 100, a});
    w.draw(r2);
    // Traverses (défilent)
    float offset = std::fmod(time * 40.f, 44.f);
    for (float tx = -44.f + offset; tx < 808.f; tx += 44.f) {
        sf::RectangleShape tie({6.f, 14.f});
        tie.setOrigin({3.f, 7.f});
        tie.setPosition({tx, y});
        tie.setFillColor({170, 130, 70, static_cast<uint8_t>(a * 9 / 10)});
        w.draw(tie);
    }
}

// Cercle centré (helper)
static void drawCircle(sf::RenderWindow& w, float cx, float cy,
                       float r, sf::Color col)
{
    sf::CircleShape c(r);
    c.setOrigin({r, r});
    c.setPosition({cx, cy});
    c.setFillColor(col);
    w.draw(c);
}

// Dessine le logo SEC/AI centré dans une zone donnée (cx,cy = centre)
static void drawLogo(sf::RenderWindow& w, const GS& gs,
                     float cx, float cy, float maxW, float maxH,
                     uint8_t alpha = 255u)
{
    if (!gs.logoTex.has_value()) return;
    sf::Sprite logo(*gs.logoTex);
    auto sz = gs.logoTex->getSize();
    float scale = std::min(maxW / static_cast<float>(sz.x),
                           maxH / static_cast<float>(sz.y));
    logo.setScale({scale, scale});
    float lw = static_cast<float>(sz.x) * scale;
    float lh = static_cast<float>(sz.y) * scale;
    logo.setPosition({cx - lw / 2.f, cy - lh / 2.f});
    logo.setColor({255, 255, 255, alpha});
    w.draw(logo);
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 7 – Panneau latéral Bootstrap-dark
// ═══════════════════════════════════════════════════════════════════════════════

static void drawPanel(sf::RenderWindow& w, const sf::Font& font,
                      GS& gs, sf::Vector2i click,
                      sf::Vector2i mouse, float time, const sf::Clock& clk)
{
    const float PX = 808.f;
    const float PW = 464.f;

    // Fond panneau
    sf::RectangleShape bg({PW, 720.f});
    bg.setPosition({PX, 0.f});
    bg.setFillColor(BS::DarkBg);
    w.draw(bg);
    // Séparateur gauche
    sf::RectangleShape sep({2.f, 720.f});
    sep.setPosition({PX, 0.f});
    sep.setFillColor(BS::CardBdr);
    w.draw(sep);

    // ── SETUP : choix du nombre de joueurs ────────────────────────────────────
    if (gs.app == AppState::SETUP_NB) {
        // ── Zone gauche : splash animé ────────────────────────────────────────
        sf::RectangleShape leftBg({808.f, 720.f});
        leftBg.setFillColor(BS::DarkBg);
        w.draw(leftBg);

        // Étoiles
        drawStarfield(w, time);

        // Rails décoratifs qui défilent
        drawTrainTrack(w, 160.f, time, 0.7f);
        drawTrainTrack(w, 560.f, time, 0.5f);

        float pulse = (std::sin(time * 1.5f) + 1.f) * 0.5f;

        // Halos concentriques pulsants derrière le logo
        for (int ring = 4; ring >= 0; ring--) {
            float fr = static_cast<float>(ring);
            float pr = (std::sin(time * 1.2f + fr * 0.6f) + 1.f) * 0.5f;
            float r  = 80.f + fr * 42.f + pr * 12.f;
            drawCircle(w, 404.f, 290.f, r,
                {static_cast<uint8_t>(55 - ring*8),
                 static_cast<uint8_t>(120 - ring*10),
                 static_cast<uint8_t>(255 - ring*12),
                 static_cast<uint8_t>(8.f + pr * 10.f)});
        }

        // Logo centré grand format
        drawLogo(w, gs, 404.f, 285.f, 460.f, 420.f, 240u);

        // Séparateur doré
        {
            float sw = 180.f + pulse * 30.f;
            sf::RectangleShape sep({sw, 2.f});
            sep.setOrigin({sw/2.f, 1.f});
            sep.setPosition({404.f, 535.f});
            sep.setFillColor(lerp(BS::Gold, BS::GoldLight, pulse));
            w.draw(sep);
        }

        // Titre principal
        {
            sf::Text title(font, sfStr("Aventuriers du Rail"), 32u);
            title.setFillColor(lerp(BS::Gold, BS::GoldLight, pulse));
            auto tb = title.getLocalBounds();
            title.setOrigin({tb.position.x + tb.size.x/2.f, 0.f});
            title.setPosition({404.f, 548.f});
            // Ombre titre
            sf::Text shadow(font, sfStr("Aventuriers du Rail"), 32u);
            shadow.setFillColor({0, 0, 0, 120});
            shadow.setOrigin({tb.position.x + tb.size.x/2.f, 0.f});
            shadow.setPosition({406.f, 550.f});
            w.draw(shadow);
            w.draw(title);
        }

        // Sous-titre
        {
            sf::Text sub(font, sfStr("Tickets \xe2\x80\xa2 Wagons \xe2\x80\xa2 Victoire"), 13u);
            sub.setFillColor({BS::Muted.r, BS::Muted.g, BS::Muted.b, 200});
            auto sb = sub.getLocalBounds();
            sub.setOrigin({sb.position.x + sb.size.x/2.f, 0.f});
            sub.setPosition({404.f, 592.f});
            w.draw(sub);
        }

        // ── Zone droite : boutons ─────────────────────────────────────────────
        drawCard(w, PX+20.f, 60.f, PW-40.f, 90.f, {20,26,44}, BS::Gold);
        w.draw(mkText(font, "Aventuriers du Rail", 22u, BS::GoldLight, PX+30.f, 76.f));
        w.draw(mkText(font, "Choisissez le nombre de joueurs", 12u, BS::Muted, PX+30.f, 108.f));

        w.draw(mkText(font, "Nombre de joueurs", 15u, BS::Light, PX+20.f, 178.f));
        for (int n = 2; n <= 4; n++) {
            float bx = PX + 20.f + static_cast<float>(n-2) * 130.f;
            drawBtn(w, font, bx, 205.f, 115.f, 55.f, to_string(n) + "  joueurs",
                    false, mouse, time);
            if (btnHit(bx, 205.f, 115.f, 55.f, click)) {
                gs.nbJ     = n;
                gs.app     = AppState::SETUP_NAME;
                gs.nameIdx = 0;
                gs.inputBuf.clear();
            }
        }
        return;
    }

    // ── SETUP : saisie des noms ───────────────────────────────────────────────
    if (gs.app == AppState::SETUP_NAME) {
        // ── Zone gauche : logo petit format + progression ─────────────────────
        sf::RectangleShape leftBg2({808.f, 720.f});
        leftBg2.setFillColor(BS::DarkBg);
        w.draw(leftBg2);

        float pulse2 = (std::sin(time * 1.5f) + 1.f) * 0.5f;
        sf::CircleShape halo2(160.f + pulse2 * 10.f);
        halo2.setOrigin({160.f + pulse2*10.f, 160.f + pulse2*10.f});
        halo2.setPosition({404.f, 280.f});
        halo2.setFillColor(sfJoueur(static_cast<couleurJoueur>(gs.nameIdx)));
        halo2.setFillColor({
            sfJoueur(static_cast<couleurJoueur>(gs.nameIdx)).r,
            sfJoueur(static_cast<couleurJoueur>(gs.nameIdx)).g,
            sfJoueur(static_cast<couleurJoueur>(gs.nameIdx)).b,
            static_cast<uint8_t>(15.f + pulse2 * 20.f)
        });
        w.draw(halo2);

        drawLogo(w, gs, 404.f, 260.f, 340.f, 320.f, 200u);

        // Texte d'invitation centré
        sf::Text inv(font, sfStr("Joueur " + to_string(gs.nameIdx+1) + " / " + to_string(gs.nbJ)), 22u);
        inv.setFillColor(sfJoueur(static_cast<couleurJoueur>(gs.nameIdx)));
        auto ib = inv.getLocalBounds();
        inv.setOrigin({ib.position.x + ib.size.x/2.f, 0.f});
        inv.setPosition({404.f, 440.f});
        w.draw(inv);

        // Barre de progression des joueurs
        for (int i = 0; i < gs.nbJ; i++) {
            float px2 = 404.f - (static_cast<float>(gs.nbJ)*28.f)/2.f + static_cast<float>(i)*28.f;
            sf::CircleShape dot2(10.f);
            dot2.setOrigin({10.f, 10.f});
            dot2.setPosition({px2, 490.f});
            dot2.setFillColor(i < gs.nameIdx
                ? sfJoueur(static_cast<couleurJoueur>(i))
                : (i == gs.nameIdx ? lerp(sfJoueur(static_cast<couleurJoueur>(i)),
                                         BS::Dark, 0.3f + 0.2f*pulse2)
                                   : BS::CardBdr));
            dot2.setOutlineColor({255,255,255, 60u});
            dot2.setOutlineThickness(1.f);
            w.draw(dot2);
        }

        drawCard(w, PX+20.f, 50.f, PW-40.f, 220.f, {20,26,44});

        // Badge couleur joueur
        sf::RectangleShape badge({PW-40.f, 4.f});
        badge.setPosition({PX+20.f, 50.f});
        badge.setFillColor(sfJoueur(static_cast<couleurJoueur>(gs.nameIdx)));
        w.draw(badge);

        w.draw(mkText(font,
            "Joueur " + to_string(gs.nameIdx+1) + " / " + to_string(gs.nbJ),
            20u, sfJoueur(static_cast<couleurJoueur>(gs.nameIdx)), PX+30.f, 66.f));
        w.draw(mkText(font, "Entrez votre nom :", 13u, BS::Muted, PX+30.f, 96.f));

        // Champ de saisie animé
        bool active = true;
        sf::RectangleShape field({PW-60.f, 38.f});
        field.setPosition({PX+30.f, 118.f});
        field.setFillColor({30, 38, 60});
        field.setOutlineColor(active ? BS::Primary : BS::CardBdr);
        field.setOutlineThickness(2.f);
        w.draw(field);

        bool showCur = static_cast<int>(clk.getElapsedTime().asSeconds() * 2) % 2 == 0;
        w.draw(mkText(font, gs.inputBuf + (showCur ? "|" : " "),
                      15u, sf::Color::White, PX+38.f, 126.f));

        drawBtn(w, font, PX+30.f, 172.f, PW-60.f, 42.f,
                "Valider  [Entrée]",
                gs.inputBuf.empty(), mouse, time, BS::Success);
        bool ok = !gs.inputBuf.empty() && btnHit(PX+30.f, 172.f, PW-60.f, 42.f, click);
        if (ok) {
            gs.joueurs.push_back(
                new Joueur(gs.inputBuf, static_cast<couleurJoueur>(gs.nameIdx)));
            gs.inputBuf.clear();
            gs.nameIdx++;
            if (gs.nameIdx >= gs.nbJ) initGame(gs);
        }
        return;
    }

    // ── FIN DE PARTIE ─────────────────────────────────────────────────────────
    if (gs.app == AppState::GAME_OVER) {
        // Bannière victoire animée
        float p = (std::sin(time * 2.f) + 1.f) * 0.5f;
        drawCard(w, PX+10.f, 30.f, PW-20.f, 80.f,
                 lerp({40,30,5}, {20,26,44}, p));
        sf::RectangleShape top({PW-20.f, 4.f});
        top.setPosition({PX+10.f, 30.f});
        top.setFillColor(lerp(BS::Warning, BS::Success, p));
        w.draw(top);
        w.draw(mkText(font, "=== FIN DE PARTIE ===", 20u, BS::Warning, PX+20.f, 44.f));
        w.draw(mkText(font, "Résultats finaux", 13u, BS::Muted, PX+20.f, 74.f));

        if (gs.gagnant) {
            drawCard(w, PX+10.f, 125.f, PW-20.f, 60.f);
            w.draw(mkText(font,
                "Vainqueur : " + gs.gagnant->getNom(), 18u,
                sfJoueur(gs.gagnant->getCouleur()), PX+20.f, 134.f));
            w.draw(mkText(font,
                to_string(gs.gagnant->getTicketFini()) + " tickets complétés",
                13u, BS::Muted, PX+20.f, 160.f));
        }
        float ys = 210.f;
        for (auto j : gs.joueurs) {
            drawCard(w, PX+10.f, ys, PW-20.f, 32.f);
            // Badge couleur
            sf::RectangleShape badge2({4.f, 32.f});
            badge2.setPosition({PX+10.f, ys});
            badge2.setFillColor(sfJoueur(j->getCouleur()));
            w.draw(badge2);
            w.draw(mkText(font,
                j->getNom() + "  :  " + to_string(j->getTicketFini()) + " ticket(s)",
                13u, sfJoueur(j->getCouleur()), PX+22.f, ys+8.f));
            ys += 38.f;
        }
        drawBtn(w, font, PX+20.f, 540.f, PW-40.f, 48.f, "Quitter",
                false, mouse, time, BS::Danger);
        if (btnHit(PX+20.f, 540.f, PW-40.f, 48.f, click))
            gs.shouldQuit = true;
        return;
    }

    // ── JEU EN COURS ──────────────────────────────────────────────────────────
    Joueur* j   = gs.cur();
    sf::Color jc = sfJoueur(j->getCouleur());

    // ── En-tête joueur (card avec bande couleur) ──────────────────────────────
    drawCard(w, PX+8.f, 6.f, PW-16.f, 72.f);
    sf::RectangleShape jband({PW-16.f, 4.f});
    jband.setPosition({PX+8.f, 6.f});
    jband.setFillColor(jc);
    w.draw(jband);

    // Petit logo SEC/AI dans le coin droit de l'en-tête
    drawLogo(w, gs, PX + PW - 38.f, 42.f, 52.f, 52.f, 160u);

    w.draw(mkText(font, j->getNom() + "  (" + nomJoueurCol(j->getCouleur()) + ")",
                  17u, jc, PX+16.f, 16.f));

    // Wagons & tickets (badges)
    auto drawBadge = [&](float bx, float by, const string& txt, sf::Color col) {
        sf::RectangleShape b({70.f, 22.f});
        b.setPosition({bx, by});
        b.setFillColor(col);
        w.draw(b);
        w.draw(mkText(font, txt, 12u, sf::Color::White, bx+5.f, by+4.f));
    };
    drawBadge(PX+16.f,  48.f, "Wagons: " + to_string(j->getNbWagons()),
              j->getNbWagons() <= 5 ? BS::Danger : BS::Secondary);
    drawBadge(PX+100.f, 48.f, "Tickets: " + to_string(j->getTicketFini()) + "/6",
              j->getTicketFini() >= 5 ? BS::Warning : BS::Success);

    // ── Cartes en main ────────────────────────────────────────────────────────
    drawCard(w, PX+8.f, 84.f, PW-16.f, 68.f);
    w.draw(mkText(font, "Main", 11u, BS::Muted, PX+16.f, 88.f));
    float cx = PX + 16.f;
    for (auto c : {couleurTrain::JAUNE, couleurTrain::ROUGE, couleurTrain::VERT,
                   couleurTrain::BLEU,  couleurTrain::BLANC, couleurTrain::NOIR,
                   couleurTrain::MULTI}) {
        int n = j->getNbCartes(c);
        if (n == 0) continue;

        // Carte avec ombre
        drawShadow(w, cx, 100.f, 42.f, 40.f, 60u);
        sf::RectangleShape card({42.f, 40.f});
        card.setPosition({cx, 100.f});
        card.setFillColor(sfRoute(c));
        card.setOutlineColor({255,255,255, 40u});
        card.setOutlineThickness(1.f);
        w.draw(card);
        // Reflet en haut
        sf::RectangleShape shine({42.f, 4.f});
        shine.setPosition({cx, 100.f});
        shine.setFillColor({255,255,255,30u});
        w.draw(shine);

        sf::Color tc = (c == couleurTrain::BLANC) ? sf::Color::Black : sf::Color::White;
        w.draw(mkText(font, to_string(n), 14u, tc, cx + 4.f, 100.f));
        w.draw(mkText(font, nomTrain(c).substr(0,3), 9u, tc, cx + 2.f, 116.f));
        cx += 48.f;
        if (cx > PX + PW - 55.f) cx = PX + 16.f;
    }

    // ── Missions ──────────────────────────────────────────────────────────────
    const auto& ms = j->getMissions();
    float mh = 24.f + static_cast<float>(ms.size()) * 18.f;
    drawCard(w, PX+8.f, 158.f, PW-16.f, mh);
    w.draw(mkText(font, "Missions", 11u, BS::Muted, PX+16.f, 162.f));
    float my = 178.f;
    for (auto tk : ms) {
        // Icône flèche
        w.draw(mkText(font, "> ", 11u, BS::Warning, PX+16.f, my));
        w.draw(mkText(font,
            tk->getVilleDepart()->getNom() + "  →  " + tk->getVilleArrivee()->getNom(),
            12u, {230,230,130}, PX+30.f, my));
        my += 18.f;
    }

    // ── Boutons d'actions ─────────────────────────────────────────────────────
    float by2 = std::max(my + 10.f, 200.f);
    bool noTrains   = gs.trainLeft <= 0;
    bool noMissions = j->getMissions().empty();

    if (gs.action == ActionSt::MENU) {
        drawBtn(w, font, PX+8.f, by2,       PW-16.f, 42.f,
                "  Piocher 2 cartes train",
                noTrains, mouse, time,
                noTrains ? BS::Secondary : BS::Primary);

        drawBtn(w, font, PX+8.f, by2+50.f,  PW-16.f, 42.f,
                "  Poser wagons  (cliquer sur la carte)",
                false, mouse, time, BS::Success);

        drawBtn(w, font, PX+8.f, by2+100.f, PW-16.f, 42.f,
                "  Passer son tour  (défausser tickets)",
                noMissions, mouse, time,
                noMissions ? BS::Secondary : BS::Secondary);

        if (!noTrains && btnHit(PX+8.f, by2, PW-16.f, 42.f, click)) {
            int nb = min(2, gs.trainLeft);
            for (int i = 0; i < nb; i++) {
                gs.partie->piocherTrain(*j);
                gs.trainLeft--;
            }
            gs.addLog(j->getNom() + " pioche " + to_string(nb) + " carte(s).");
            gs.statusMsg.clear();
            gs.tourIdx++;
        }
        if (btnHit(PX+8.f, by2+50.f, PW-16.f, 42.f, click)) {
            gs.action = ActionSt::SELECT_ROUTE;
            gs.addLog("Sélectionnez une voie sur la carte...");
        }
        if (!noMissions && btnHit(PX+8.f, by2+100.f, PW-16.f, 42.f, click)) {
            auto def = j->defausserTickets();
            gs.addLog(j->getNom() + " défausse " + to_string(def.size()) + " ticket(s).");
            gs.pioche->defausser(def);
            for (int i = 0; i < 2 && !gs.pioche->vide(); i++)
                gs.pioche->piocher(*j, *gs.partie, gs.log);
            gs.statusMsg.clear();
            gs.tourIdx++;
        }
    } else {
        // Mode sélection de voie
        float pulse = (std::sin(time * 3.f) + 1.f) * 0.5f;
        sf::RectangleShape hint({PW-16.f, 38.f});
        hint.setPosition({PX+8.f, by2});
        hint.setFillColor(lerp({60,55,5}, {40,38,5}, pulse));
        hint.setOutlineColor(lerp(BS::Warning, {200,140,0}, pulse));
        hint.setOutlineThickness(1.5f);
        w.draw(hint);
        w.draw(mkText(font, "  Cliquez sur une voie libre...",
                      13u, BS::Warning, PX+12.f, by2+10.f));

        drawBtn(w, font, PX+8.f, by2+46.f, PW-16.f, 38.f,
                "  Annuler", false, mouse, time, BS::Danger);
        if (btnHit(PX+8.f, by2+46.f, PW-16.f, 38.f, click)) {
            gs.action   = ActionSt::MENU;
            gs.hovered  = nullptr;
            gs.statusMsg.clear();
        }
    }

    // Message d'erreur
    if (!gs.statusMsg.empty()) {
        float ey = by2 + 152.f;
        drawCard(w, PX+8.f, ey, PW-16.f, 30.f, BS::Danger);
        w.draw(mkText(font, "  " + gs.statusMsg, 11u, {255,200,200}, PX+12.f, ey+7.f));
    }

    // ── Journal avec fondu animé ───────────────────────────────────────────────
    const float logY = 575.f;
    drawCard(w, PX+8.f, logY, PW-16.f, 140.f, {14,18,30});
    sf::RectangleShape logTop({PW-16.f, 3.f});
    logTop.setPosition({PX+8.f, logY});
    logTop.setFillColor(BS::Primary);
    w.draw(logTop);
    w.draw(mkText(font, "Journal", 11u, BS::Muted, PX+16.f, logY+5.f));

    for (size_t i = 0; i < gs.log.size(); i++) {
        float age     = gs.now - (i < gs.logTimes.size() ? gs.logTimes[i] : 0.f);
        float alpha01 = std::min(1.f, age / 0.4f);       // fondu entrée 0.4s
        auto  alpha   = static_cast<uint8_t>(alpha01 * 210.f);
        float slideX  = (1.f - alpha01) * 20.f;          // glissement horizontal

        sf::Color col = {180,190,210, alpha};
        if (gs.log[i].find("[Ticket!]") != string::npos) col = {255,215,0, alpha};
        if (gs.log[i].find("[Grande")   != string::npos) col = {50,210,80, alpha};
        if (gs.log[i].find("vide")      != string::npos) col = {220,60,60, alpha};

        w.draw(mkText(font, gs.log[i], 11u, col,
                      PX + 16.f + slideX,
                      logY + 20.f + static_cast<float>(i) * 17.f));
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 8 – Interaction carte
// ═══════════════════════════════════════════════════════════════════════════════

static VoieFerree* findRoute(sf::Vector2f mp, const GS& gs)
{
    VoieFerree* best = nullptr;
    float bestD = 14.f;
    for (auto v : gs.voies) {
        if (v->getJoueur()) continue;
        auto lv = v->getListeVille();
        if (!CITY_POS.count(lv[0]->getNom()) || !CITY_POS.count(lv[1]->getNom())) continue;
        sf::Vector2f pa = CITY_POS.at(lv[0]->getNom());
        sf::Vector2f pb = CITY_POS.at(lv[1]->getNom());
        float off = gs.offsets.count(v) ? gs.offsets.at(v) : 0.f;
        sf::Vector2f d = pb - pa;
        float len = std::sqrt(d.x*d.x + d.y*d.y);
        sf::Vector2f perp = {-d.y/len, d.x/len};
        float dist = distSeg(mp, pa + perp*off, pb + perp*off);
        if (dist < bestD) { bestD = dist; best = v; }
    }
    return best;
}

static void handleMapClick(sf::Vector2i mousePos, GS& gs)
{
    if (gs.app != AppState::GAME || gs.action != ActionSt::SELECT_ROUTE) return;
    sf::Vector2f mp = {static_cast<float>(mousePos.x),
                       static_cast<float>(mousePos.y)};
    VoieFerree* v = findRoute(mp, gs);
    if (!v) return;

    Joueur* j    = gs.cur();
    int  poids   = v->getPoids();
    couleurTrain c = v->getCouleur();

    if (j->getNbWagons() < poids) {
        gs.statusMsg = "Pas assez de wagons (besoin=" + to_string(poids) + ").";
        return;
    }
    if (!j->peutPrendreVoie(c, poids)) {
        gs.statusMsg = "Pas assez de cartes " + nomTrain(c)
                     + "  (" + to_string(j->getNbCartes(c))
                     + " + Loco×" + to_string(j->getNbCartes(couleurTrain::MULTI)) + ").";
        return;
    }

    j->defausserCartesVoie(c, poids);
    j->utiliserWagons(poids);
    v->setJoueur(j);
    gs.statusMsg.clear();

    auto lv = v->getListeVille();
    gs.addLog(j->getNom() + " prend " + lv[0]->getNom() + ">" + lv[1]->getNom());

    validerTickets(j, gs.voies, gs.partie->getPlateau(),
                   *gs.pioche, *gs.partie, gs.log);

    if (!gs.grandeTrav && verifierGrandeTraversee(j, gs.voies)) {
        gs.grandeTrav = true;
        Ticket bonus(gs.villes[0], gs.villes[0]);
        j->validerTicket(&bonus, *gs.partie->getPlateau());
        gs.addLog("[Grande Traversée!] " + j->getNom() + " +1 bonus !");
    }

    gs.action  = ActionSt::MENU;
    gs.hovered = nullptr;

    if (j->aGagne() || j->naPlusDeWagons()) {
        gs.gagnant = j;
        gs.app     = AppState::GAME_OVER;
    } else {
        gs.tourIdx++;
    }
}

static void updateHover(sf::Vector2i mp, GS& gs)
{
    if (gs.app != AppState::GAME || gs.action != ActionSt::SELECT_ROUTE)
        { gs.hovered = nullptr; return; }
    sf::Vector2f p = {static_cast<float>(mp.x), static_cast<float>(mp.y)};
    gs.hovered = findRoute(p, gs);
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 9 – main()
// ═══════════════════════════════════════════════════════════════════════════════

int main()
{
    sf::RenderWindow window(sf::VideoMode({1280u, 720u}), "Aventuriers du Rail");
    window.setFramerateLimit(60);

    sf::Font font;
    {
        const array<const char*, 5> paths = {
            "/Library/Fonts/Arial.ttf",
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
        };
        for (const char* p : paths)
            if (font.openFromFile(p)) break;
    }

    sf::Clock clock;
    GS gs;

    // Chargement du logo SEC/AI
    {
        sf::Texture tmpTex;
        if (tmpTex.loadFromFile(string(FILES_DIR) + "/logo.png"))
            gs.logoTex = std::move(tmpTex);
    }

    while (window.isOpen()) {
        gs.now = clock.getElapsedTime().asSeconds();
        sf::Vector2i clickPos(-1, -1);
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* e = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (e->button == sf::Mouse::Button::Left) {
                    clickPos = {e->position.x, e->position.y};
                    if (clickPos.x < 800)
                        handleMapClick(clickPos, gs);
                }
            }

            if (const auto* e = event->getIf<sf::Event::TextEntered>()) {
                if (gs.app == AppState::SETUP_NAME) {
                    auto ch = static_cast<uint32_t>(e->unicode);
                    if      (ch == 8u  && !gs.inputBuf.empty())  gs.inputBuf.pop_back();
                    else if (ch == 13u && !gs.inputBuf.empty()) {
                        gs.joueurs.push_back(
                            new Joueur(gs.inputBuf,
                                       static_cast<couleurJoueur>(gs.nameIdx)));
                        gs.inputBuf.clear();
                        gs.nameIdx++;
                        if (gs.nameIdx >= gs.nbJ) initGame(gs);
                    }
                    else if (ch >= 32u && ch < 127u)
                        gs.inputBuf += static_cast<char>(ch);
                }
            }
        }

        updateHover(mousePos, gs);

        window.clear(BS::DarkBg);
        if (gs.app == AppState::GAME || gs.app == AppState::GAME_OVER)
            drawMap(window, font, gs, gs.now);

        drawPanel(window, font, gs, clickPos, mousePos, gs.now, clock);
        window.display();

        if (gs.shouldQuit) window.close();
    }

    delete gs.partie;
    delete gs.pioche;
    for (auto v : gs.voies)   delete v;
    for (auto v : gs.villes)  delete v;
    for (auto j : gs.joueurs) delete j;
    for (auto t : gs.tickets) delete t;
    for (auto t : gs.trains)  delete t;
    return 0;
}
