// gui_main.cpp – Interface graphique Qt pour Aventuriers du Rail

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QPixmap>
#include <QRectF>
#include <QPointF>
#include <QString>
#include <QPen>
#include <QBrush>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <algorithm>
#include <random>

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
/**
 * @brief Convertit une chaîne de caractères en couleur de train
 * 
 * @param s 
 * @return couleurTrain 
 */
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
/**
 * @brief Retourne le nom d'un train en fonction de sa couleur
 * 
 * @param c 
 * @return string 
 */
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
/**
 * @brief Retourne le nom d'un joueur en fonction de sa couleur
 * 
 * @param c 
 * @return string 
 */
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
/**
 * @brief  Affiche la main d'un joueur
 * 
 * @param s 
 */
static void trimStr(string& s)
{
    while (!s.empty() && (s.front() == ' ' || s.front() == '\r')) s.erase(s.begin());
    while (!s.empty() && (s.back()  == ' ' || s.back()  == '\r')) s.pop_back();
}
/**
 * @brief Affiche la main d'un joueur
 * 
 * @param nom 
 * @param villes 
 * @return Ville* 
 */
static Ville* getOuCreer(const string& nom, vector<Ville*>& villes)
{
    for (auto v : villes) if (v->getNom() == nom) return v;
    auto* v = new Ville(nom); villes.push_back(v); return v;
}
/**
 * @brief Charge la carte à partir d'un fichier CSV et remplit les villes et voies
 * 
 * @param fichier 
 * @param villes 
 * @param voies 
 */
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
/**
 * @brief Charge les tickets à partir d'un fichier CSV
 * 
 * @param fichier 
 * @param villes 
 * @return vector<Ticket*> 
 */
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
/**
 * @brief Crée les trains pour la partie
 * 
 * @return vector<Train*> 
 */
static vector<Train*> creerTrains()
{
    vector<Train*> pile;
    for (auto c : {couleurTrain::JAUNE, couleurTrain::ROUGE, couleurTrain::VERT,
                   couleurTrain::BLEU,  couleurTrain::BLANC, couleurTrain::NOIR})
        for (int i = 0; i < 10; i++) pile.push_back(new Train(c));
    for (int i = 0; i < 12; i++) pile.push_back(new Train(couleurTrain::MULTI));
    return pile;
}
/**
 * @brief Vérifie si deux villes sont connectées par des voies de train appartenant à un joueur
 * 
 * @param a 
 * @param b 
 * @param voies 
 * @param joueur 
 * @return true 
 * @return false 
 */
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

/**
 * @brief Vérifie si un joueur a accompli une grande traversée
 * 
 * @param j 
 * @param voies 
 * @return true 
 * @return false 
 */
static bool verifierGrandeTraversee(Joueur* j, const vector<VoieFerree*>& voies)
{
    for (const auto& o : COTE_OUEST)
        for (const auto& e : COTE_EST)
            if (estConnecte(o, e, voies, j)) return true;
    return false;
}
/**
 * @brief Représente la pioche de tickets
 * 
 */
struct PiocheTickets {
    int             partieLeft;
    vector<Ticket*> locale;
    vector<Ticket*> defausse;
    mt19937&        rng;
    /**
     * @brief Construct a new Pioche Tickets object
     * 
     * @param n 
     * @param g 
     */
    PiocheTickets(int n, mt19937& g) : partieLeft(n), rng(g) {}
    /**
     * @brief Vérifie si la pioche est vide
     * 
     * @return true 
     * @return false 
     */
    bool vide() const { return partieLeft <= 0 && locale.empty() && defausse.empty(); }
    /**
     * @brief Pioche un ticket pour le joueur
     * 
     * @param j 
     * @param partie 
     * @param log 
     * @return true 
     * @return false 
     */
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
    /**
     * @brief Défausse des tickets
     * 
     * @param tickets 
     */
    void defausser(vector<Ticket*>& tickets)
    { for (auto t : tickets) defausse.push_back(t); }
};
/**
 * @brief Valide les tickets d'un joueur
 * 
 * @param j 
 * @param voies 
 * @param plateau 
 * @param pioche 
 * @param partie 
 * @param log 
 */
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
// SECTION 2 – Palette & positions
// ═══════════════════════════════════════════════════════════════════════════════

namespace BS {
    const QColor DarkBg    = {  6,  8, 20};
    const QColor Card      = { 16, 20, 46};
    const QColor CardBdr   = { 38, 50, 95};
    const QColor Dark      = { 22, 26, 48};
    const QColor Gold      = {212,168, 22};
    const QColor GoldLight = {255,220, 80};
    const QColor Warning   = {255,190,  0};
    const QColor Primary   = { 55,120,255};
    const QColor PrimaryH  = { 88,152,255};
    const QColor Success   = {  0,178,120};
    const QColor Danger    = {218, 50, 70};
    const QColor Secondary = { 72, 85,120};
    const QColor Light     = {215,222,245};
    const QColor Muted     = { 85, 98,135};
    const QColor MapBg1    = { 18, 52, 35};
    const QColor MapOcean  = { 12, 30, 65};
    const QColor MapMtn    = { 38, 55, 45};
}
/** @brief Interpole entre deux couleurs
 * 
 * @param a 
 * @param b 
 * @param t 
 * @return QColor 
 */
static QColor lerp(QColor a, QColor b, float t)
{
    t = qBound(0.f, t, 1.f);
    return QColor(
        int(a.red()   + t * (b.red()   - a.red())),
        int(a.green() + t * (b.green() - a.green())),
        int(a.blue()  + t * (b.blue()  - a.blue())),
        int(a.alpha() + t * (b.alpha() - a.alpha()))
    );
}
/** @brief Convertit une couleur de train en couleur Qt
 * 
 * @param c 
 * @return QColor 
 */
static QColor qRoute(couleurTrain c)
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
/** @brief Convertit une couleur de joueur en couleur Qt
 * 
 * @param c 
 * @return QColor 
 */
static QColor qJoueur(couleurJoueur c)
{
    switch (c) {
        case couleurJoueur::JAUNE: return {255,215,  0};
        case couleurJoueur::BLEU:  return { 30,144,255};
        case couleurJoueur::ROUGE: return {230, 55, 55};
        case couleurJoueur::VERT:  return { 50,210, 80};
        default:                   return {200,200,200};
    }
}
/** @brief Positions des villes sur la carte
 * 
 */
static const map<string, QPointF> CITY_POS = {
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
// SECTION 3 – Helpers de dessin (QPainter)
// ═══════════════════════════════════════════════════════════════════════════════
/** @brief Crée une police de caractères
 * 
 * @param pixelSize 
 * @return QFont 
 */
static QFont mkFont(int pixelSize)
{
    QFont f;
    f.setPixelSize(pixelSize);
    return f;
}
/** @brief Dessine une ombre portée
 * 
 * @param p 
 * @param x 
 * @param y 
 * @param bw 
 * @param bh 
 * @param alpha 
 */
static void drawShadow(QPainter& p, float x, float y, float bw, float bh, int alpha = 90)
{
    for (int i = 5; i >= 1; i--) {
        float fi = float(i);
        QColor c(0, 0, 0, int(float(alpha) / (fi + 1.f)));
        p.fillRect(QRectF(x - fi + fi*2.f, y + fi*1.5f, bw + fi*2.f, bh + fi*2.f), c);
    }
}
/** @brief Vérifie si un point est dans un bouton
 * 
 * @param bx 
 * @param by 
 * @param bw 
 * @param bh 
 * @param pt 
 * @return true 
 * @return false 
 */
static bool btnHit(float bx, float by, float bw, float bh, QPoint pt)
{
    return pt.x() >= int(bx) && pt.x() <= int(bx+bw) &&
           pt.y() >= int(by) && pt.y() <= int(by+bh);
}
/** @brief Dessine une carte
 * 
 * @param p 
 * @param x 
 * @param y 
 * @param cw 
 * @param ch 
 * @param bg 
 * @param topAccent 
 */
static void drawCard(QPainter& p, float x, float y, float cw, float ch,
                     QColor bg = BS::Card, QColor topAccent = Qt::transparent)
{
    drawShadow(p, x, y, cw, ch, 80);
    p.fillRect(QRectF(x, y, cw, ch), bg);
    p.save();
    p.setPen(QPen(BS::CardBdr, 1.f));
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRectF(x, y, cw, ch));
    p.restore();
    p.fillRect(QRectF(x, y, cw, ch/3.f), QColor(255,255,255, 6));
    if (topAccent.alpha() > 0)
        p.fillRect(QRectF(x, y, cw, 3.f), topAccent);
}
/** @brief Dessine un bouton
 * 
 * @param p 
 * @param x 
 * @param y 
 * @param bw 
 * @param bh 
 * @param label 
 * @param disabled 
 * @param mouse 
 * @param time 
 * @param base 
 * @param hoverCol 
 */
static void drawBtn(QPainter& p, float x, float y, float bw, float bh,
                    const QString& label, bool disabled,
                    QPoint mouse, float time,
                    QColor base = BS::Primary, QColor hoverCol = BS::PrimaryH)
{
    bool hovered = !disabled && btnHit(x, y, bw, bh, mouse);
    float pulse  = (std::sin(time * 3.5f) + 1.f) * 0.5f;
    float offY   = hovered ? 2.f : 0.f;

    QColor bgTop, bgBot;
    if (disabled) {
        bgTop = lerp(BS::Secondary, {100,110,150}, 0.3f);
        bgBot = BS::Secondary;
    } else if (hovered) {
        bgTop = lerp(hoverCol, Qt::white, 0.15f + 0.05f * pulse);
        bgBot = hoverCol;
    } else {
        bgTop = lerp(base, Qt::white, 0.12f);
        bgBot = lerp(base, Qt::black, 0.15f);
    }

    if (!disabled && !hovered) drawShadow(p, x, y + offY, bw, bh, 80);

    p.fillRect(QRectF(x, y + offY,         bw, bh / 2.f), bgTop);
    p.fillRect(QRectF(x, y + offY + bh/2.f, bw, bh / 2.f), bgBot);

    QColor border;
    if (disabled)     border = QColor(60, 70, 100, 120);
    else if (hovered) border = lerp(BS::GoldLight, BS::Gold, pulse);
    else              border = lerp(base, QColor(200,200,255), 0.35f);

    p.save();
    p.setPen(QPen(border, 1.5f));
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRectF(x, y + offY, bw, bh));
    p.restore();

    p.fillRect(QRectF(x+2.f, y+offY+2.f, bw-4.f, 2.f),
               QColor(255,255,255, disabled ? 0 : (hovered ? 55 : 28)));

    if (hovered) {
        float cs = 4.f;
        QColor cc = lerp(BS::Gold, BS::GoldLight, pulse);
        for (auto [cx2, cy2] : vector<pair<float,float>>{
                {x, y+offY}, {x+bw-cs, y+offY},
                {x, y+offY+bh-cs}, {x+bw-cs, y+offY+bh-cs}})
            p.fillRect(QRectF(cx2, cy2, cs, cs), cc);
    }

    p.save();
    QFont f = mkFont(14);
    p.setFont(f);
    QColor tc = disabled ? BS::Muted : Qt::white;
    if (!disabled && hovered) tc = BS::GoldLight;
    p.setPen(tc);
    p.drawText(QRectF(x, y+offY, bw, bh), Qt::AlignCenter, label);
    p.restore();
}

/**
 * @brief Dessine une ligne épaisse (rectangle tourné)
 * 
 * @param p 
 * @param a 
 * @param b 
 * @param color 
 * @param thick 
 * @param perpOff 
 */
static void drawThickLine(QPainter& p,
                          QPointF a, QPointF b,
                          QColor color, float thick, float perpOff = 0.f)
{
    QPointF d = b - a;
    double len = std::sqrt(d.x()*d.x() + d.y()*d.y());
    if (len < 1.0) return;
    double nx = -d.y() / len, ny = d.x() / len;
    QPointF start = QPointF(a.x() + nx * perpOff, a.y() + ny * perpOff);
    double angleDeg = std::atan2(d.y(), d.x()) * 180.0 / M_PI;

    p.save();
    p.translate(start);
    p.rotate(angleDeg);
    p.fillRect(QRectF(0, -thick/2.0, len, thick), color);
    p.restore();
}
/**
 * @brief Calcule la distance d'un point à un segment
 * 
 * @param pt 
 * @param a 
 * @param b 
 * @return double 
 */
static double distSeg(QPointF pt, QPointF a, QPointF b)
{
    QPointF ab = b - a, ap = pt - a;
    double t = (ap.x()*ab.x() + ap.y()*ab.y()) / (ab.x()*ab.x() + ab.y()*ab.y() + 1e-6);
    t = qBound(0.0, t, 1.0);
    QPointF cl = a + ab * t, dv = pt - cl;
    return std::sqrt(dv.x()*dv.x() + dv.y()*dv.y());
}
/**
 * @brief Calcule les offsets pour les voies parallèles
 * 
 * @param voies 
 * @return map<VoieFerree*, float> 
 */
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
        float start = -(float(g.size()) - 1.f) * 7.f / 2.f;
        for (size_t i = 0; i < g.size(); i++)
            offsets[g[i]] = start + float(i) * 7.f;
    }
    return offsets;
}

// ═════════════════════════════════
// SECTION 4 – État global
// ═════════════════════════════════

enum class AppState { SETUP_NB, SETUP_NAME, GAME, GAME_OVER };
enum class ActionSt { MENU, SELECT_ROUTE };
/**
 * @brief Structure contenant l'état global du jeu
 * 
 */
struct GS {
    AppState app     = AppState::SETUP_NB;
    int      nbJ     = 2;
    int      nameIdx = 0;
    string   inputBuf;

    vector<Joueur*>     joueurs;
    vector<VoieFerree*> voies;
    vector<Ville*>      villes;
    vector<Train*>      trains;
    vector<Ticket*>     tickets;
    Partie*             partie    = nullptr;
    PiocheTickets*      pioche    = nullptr;
    mt19937             rng       {random_device{}()};
    int                 trainLeft = 0;
    size_t              tourIdx   = 0;
    bool                grandeTrav= false;
    ActionSt            action    = ActionSt::MENU;
    Joueur*             gagnant   = nullptr;
    string              statusMsg;
    bool                shouldQuit= false;

    vector<string> log;
    vector<float>  logTimes;
    float          now = 0.f;
    /**
     * @brief Ajoute un message au journal
     * 
     * @param s 
     */
    void addLog(const string& s) {
        log.push_back(s);
        logTimes.push_back(now);
        if (log.size() > 7) { log.erase(log.begin()); logTimes.erase(logTimes.begin()); }
    }

    VoieFerree*             hovered = nullptr;
    map<VoieFerree*, float> offsets;

    QPixmap logoPix;

    Joueur* cur() const { return joueurs[tourIdx % joueurs.size()]; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 5 – Initialisation
// ═══════════════════════════════════════════════════════════════════════════════
/**
 * @brief Initialise le jeu
 * 
 * @param gs 
 */
static void initGame(GS& gs)
{
    chargerMap(string(FILES_DIR) + "/map.csv", gs.villes, gs.voies);
    gs.trains  = creerTrains();
    gs.tickets = chargerTickets(string(FILES_DIR) + "/ticket.csv", gs.villes);
    gs.partie  = new Partie(gs.tickets, false, gs.trains, gs.joueurs);
    gs.trainLeft = int(gs.trains.size())  - gs.nbJ * 4;
    int tLeft    = int(gs.tickets.size()) - gs.nbJ * 2;
    gs.pioche    = new PiocheTickets(tLeft, gs.rng);
    gs.offsets   = calcOffsets(gs.voies);
    gs.app       = AppState::GAME;
    gs.addLog("=== Partie lancée ! ===");
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 6 – Dessin de la carte
// ═══════════════════════════════════════════════════════════════════════════════
/**
 * @brief   Dessine un champ d'étoiles en arrière-plan
 * 
 * @param p 
 * @param time 
 * @param areaW 
 * @param areaH 
 */
static void drawStarfield(QPainter& p, float time, float areaW = 808.f, float areaH = 720.f)
{
    for (int i = 0; i < 90; i++) {
        float seed    = float(i) * 137.508f;
        float sx      = std::fmod(seed * 4.13f, areaW);
        float sy      = std::fmod(seed * 2.97f, areaH);
        float twinkle = (std::sin(time * 1.2f + seed * 0.05f) + 1.f) * 0.5f;
        float r       = 0.8f + twinkle * 1.8f;
        int   a       = int(25.f + twinkle * 110.f);
        QColor c = (i % 11 == 0) ? QColor(255,220,100,a) : QColor(180,200,255,a);
        p.setBrush(c);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QRectF(sx-r, sy-r, 2*r, 2*r));
    }
}
/** @brief Dessine les rails d'un train
 * 
 * @param p 
 * @param y 
 * @param time 
 * @param alpha01 
 */
static void drawTrainTrack(QPainter& p, float y, float time, float alpha01)
{
    int a = int(alpha01 * 30.f);
    if (a == 0) return;
    QColor c(200,170,100,a);
    p.fillRect(QRectF(0.f, y-5.f, 808.f, 2.f), c);
    p.fillRect(QRectF(0.f, y+5.f, 808.f, 2.f), c);
    float offset = std::fmod(time * 40.f, 44.f);
    p.save();
    p.setBrush(QColor(170,130,70, a*9/10));
    p.setPen(Qt::NoPen);
    for (float tx = -44.f + offset; tx < 808.f; tx += 44.f)
        p.drawRect(QRectF(tx-3.f, y-7.f, 6.f, 14.f));
    p.restore();
}
/** @brief Dessine la carte
 * 
 * @param p 
 * @param gs 
 * @param time 
 */
static void drawMap(QPainter& p, const GS& gs, float time)
{
    // Fond
    p.fillRect(QRectF(0,0,800,720), BS::MapBg1);
    p.fillRect(QRectF(230,160,340,440), QColor(24,62,40,180));
    p.fillRect(QRectF(0,0,90,720),   BS::MapOcean);
    p.fillRect(QRectF(720,0,80,720), BS::MapOcean);
    p.fillRect(QRectF(200,100,120,500), QColor(BS::MapMtn.red(), BS::MapMtn.green(), BS::MapMtn.blue(),120));

    // Vignette
    for (int edge = 0; edge < 4; edge++) {
        for (int i = 0; i < 5; i++) {
            float fi = float(i);
            int a = int(((5.f-fi)/5.f) * ((5.f-fi)/5.f) * 90.f);
            float dim = fi * 12.f;
            if      (edge==0) p.fillRect(QRectF(0,0,800,dim), QColor(0,0,0,a));
            else if (edge==1) p.fillRect(QRectF(0,720-dim,800,dim), QColor(0,0,0,a));
            else if (edge==2) p.fillRect(QRectF(0,0,dim,720), QColor(0,0,0,a));
            else              p.fillRect(QRectF(800-dim,0,dim,720), QColor(0,0,0,a));
        }
    }

    // Grille topographique
    p.save();
    p.setPen(QPen(QColor(255,255,255,7), 1.f));
    for (int i = 0; i <= 16; i++) p.drawLine(QPointF(i*50.f,0), QPointF(i*50.f,720));
    for (int i = 0; i <= 14; i++) p.drawLine(QPointF(0,i*52.f), QPointF(800,i*52.f));
    p.restore();

    // Routes
    p.save();
    p.setPen(Qt::NoPen);
    for (auto v : gs.voies) {
        auto lv = v->getListeVille();
        const string& na = lv[0]->getNom(), &nb = lv[1]->getNom();
        if (!CITY_POS.count(na) || !CITY_POS.count(nb)) continue;

        QPointF pa = CITY_POS.at(na), pb = CITY_POS.at(nb);
        float off   = gs.offsets.count(v) ? gs.offsets.at(v) : 0.f;
        int   poids = v->getPoids();
        float thick = 7.f;
        bool  owned = v->getJoueur() != nullptr;
        bool  isHov = (v == gs.hovered && gs.action == ActionSt::SELECT_ROUTE);
        float pulse = (std::sin(time * 5.f) + 1.f) * 0.5f;

        QColor col;
        if (owned)      col = qJoueur(v->getJoueur()->getCouleur());
        else if (isHov) col = lerp({255,225,30}, {255,140,0}, pulse);
        else            col = qRoute(v->getCouleur());

        if (isHov) thick = 9.f + pulse * 3.f;
        if (owned) thick = 10.f;

        QPointF d  = pb - pa;
        double  len = std::sqrt(d.x()*d.x() + d.y()*d.y());
        QPointF ud = len > 0 ? d / len : d;
        QPointF perp = {-ud.y(), ud.x()};

        // Halo sélection
        if (isHov)
            drawThickLine(p, pa, pb,
                          QColor(255,220,50, int(40.f + pulse*60.f)),
                          thick + 10.f, off);

        // Ombre + ballast
        drawThickLine(p, pa, pb, QColor(0,0,0,100), thick + 4.f, off + 2.f);
        drawThickLine(p, pa, pb, lerp(col, Qt::black, 0.5f), thick + 2.f, off);

        // Wagons individuels
        for (int seg = 0; seg < poids; seg++) {
            float t0 = (float(seg) + 0.08f) / float(poids);
            float t1 = (float(seg) + 0.92f) / float(poids);
            QPointF a2 = pa + d*t0 + perp*off;
            QPointF b2 = pa + d*t1 + perp*off;
            double segLen = std::sqrt((b2-a2).x()*(b2-a2).x()+(b2-a2).y()*(b2-a2).y());

            QColor slotCol = col;
            if (owned) {
                float shimmer = (std::sin(time*2.f + float(seg)*0.8f)+1.f)*0.5f;
                slotCol = lerp(col, lerp(col, Qt::white, 0.3f), shimmer*0.4f);
            }
            if (isHov) slotCol = lerp(col, QColor(255,255,200), 0.3f + 0.2f*pulse);

            double angleDeg = std::atan2(d.y(), d.x()) * 180.0 / M_PI;
            p.save();
            p.translate(a2);
            p.rotate(angleDeg);
            p.fillRect(QRectF(0, -thick/2.0, segLen, thick), slotCol);
            // Reflet
            p.fillRect(QRectF(0, -thick/2.0, segLen, 2.5),
                       QColor(255,255,255, owned ? 50 : 25));
            p.restore();
        }
    }
    p.restore();

    // Villes
    p.save();
    p.setPen(Qt::NoPen);
    for (auto& kv : CITY_POS) {
        QPointF pos = kv.second;
        bool isOuest = std::find(COTE_OUEST.begin(), COTE_OUEST.end(), kv.first) != COTE_OUEST.end();
        bool isEst   = std::find(COTE_EST.begin(),   COTE_EST.end(),   kv.first) != COTE_EST.end();
        bool isTicket= false;
        if (!gs.joueurs.empty()) {
            for (auto t : gs.cur()->getMissions())
                if (t->getVilleDepart()->getNom()==kv.first ||
                    t->getVilleArrivee()->getNom()==kv.first)
                    { isTicket=true; break; }
        }

        float pp = (std::sin(time*2.f + float(pos.x())*0.04f)+1.f)*0.5f;

        if (isTicket) {
            float r1 = 18.f + pp*8.f;
            QColor hc = qJoueur(gs.cur()->getCouleur());
            hc.setAlpha(int(80.f + pp*100.f));
            p.setBrush(hc);
            p.drawEllipse(QPointF(pos.x(), pos.y()), double(r1), double(r1));
        }

        // Anneau extérieur
        p.setPen(QPen(QColor(180,155,80,200), 2.f));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(pos.x(), pos.y()), 11.0, 11.0);

        // Disque principal
        p.setPen(QPen(QColor(220,195,120), 1.5f));
        QColor dc = isOuest ? QColor(30,100,200) : isEst ? QColor(180,130,0) : QColor(60,90,55);
        p.setBrush(dc);
        p.drawEllipse(QPointF(pos.x(), pos.y()), 8.0, 8.0);

        // Point central
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255,248,210,230));
        p.drawEllipse(QPointF(pos.x(), pos.y()), 3.5, 3.5);

        // Étiquette
        QString lbl = QString::fromStdString(kv.first);
        QFont f = mkFont(10);
        p.setFont(f);
        QFontMetrics fm(f);
        int tw = fm.horizontalAdvance(lbl);
        float lx = float(pos.x()) - tw/2.f;
        float ly = float(pos.y()) + 13.f;
        // Ombre triple
        for (auto [dx,dy] : vector<pair<float,float>>{{1,1},{-1,1},{0,1.5f}}) {
            p.setPen(QColor(0,0,0,160));
            p.drawText(QPointF(lx+dx, ly+dy+fm.ascent()), lbl);
        }
        p.setPen(QColor(230,220,185));
        p.drawText(QPointF(lx, ly+fm.ascent()), lbl);
    }
    p.restore();
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 7 – Panneau latéral
// ═══════════════════════════════════════════════════════════════════════════════
/**
 * @brief Dessine le panneau latéral
 * 
 * @param p 
 * @param gs 
 * @param click 
 * @param mouse 
 * @param time 
 */
static void drawPanel(QPainter& p, GS& gs, QPoint click, QPoint mouse, float time)
{
    const float PX = 808.f, PW = 464.f;

    // Fond panneau
    p.fillRect(QRectF(PX, 0, PW, 720), BS::DarkBg);
    p.fillRect(QRectF(PX, 0, 2, 720), BS::CardBdr);

    // ── SETUP : choix du nombre de joueurs ──────────────────────────────────
    if (gs.app == AppState::SETUP_NB) {
        p.fillRect(QRectF(0,0,808,720), BS::DarkBg);
        drawStarfield(p, time);
        drawTrainTrack(p, 160.f, time, 0.7f);
        drawTrainTrack(p, 560.f, time, 0.5f);

        float pulse = (std::sin(time*1.5f)+1.f)*0.5f;

        // Halos concentriques
        p.save();
        p.setPen(Qt::NoPen);
        for (int ring = 4; ring >= 0; ring--) {
            float fr = float(ring);
            float pr = (std::sin(time*1.2f + fr*0.6f)+1.f)*0.5f;
            float r  = 80.f + fr*42.f + pr*12.f;
            p.setBrush(QColor(55-ring*8, 120-ring*10, 255-ring*12, int(8.f+pr*10.f)));
            p.drawEllipse(QPointF(404, 290), double(r), double(r));
        }
        p.restore();

        // Logo
        if (!gs.logoPix.isNull()) {
            float maxW=460.f, maxH=420.f;
            float scale = qMin(maxW/gs.logoPix.width(), maxH/gs.logoPix.height());
            float lw = gs.logoPix.width()*scale, lh = gs.logoPix.height()*scale;
            QPixmap scaled = gs.logoPix.scaled(int(lw), int(lh), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            p.setOpacity(240.0/255.0);
            p.drawPixmap(QPointF(404.f-lw/2.f, 285.f-lh/2.f), scaled);
            p.setOpacity(1.0);
        }

        // Séparateur doré
        float sw = 180.f + pulse*30.f;
        p.fillRect(QRectF(404.f-sw/2.f, 535.f, sw, 2.f), lerp(BS::Gold, BS::GoldLight, pulse));

        // Titre
        {
            QFont f = mkFont(32);
            p.setFont(f);
            QFontMetrics fm(f);
            QString title = "Aventuriers du Rail";
            int tw = fm.horizontalAdvance(title);
            QColor tc = lerp(BS::Gold, BS::GoldLight, pulse);
            p.setPen(QColor(0,0,0,120));
            p.drawText(QPointF(404.f - tw/2.f + 2, 548.f + fm.ascent() + 2), title);
            p.setPen(tc);
            p.drawText(QPointF(404.f - tw/2.f, 548.f + fm.ascent()), title);
        }

        // Sous-titre
        {
            QFont f = mkFont(13); p.setFont(f);
            QFontMetrics fm(f);
            QString sub = "Tickets \u2022 Wagons \u2022 Victoire";
            int sw2 = fm.horizontalAdvance(sub);
            p.setPen(QColor(BS::Muted.red(), BS::Muted.green(), BS::Muted.blue(), 200));
            p.drawText(QPointF(404.f - sw2/2.f, 592.f + fm.ascent()), sub);
        }

        // Panneau droit
        drawCard(p, PX+20.f, 60.f, PW-40.f, 90.f, QColor(20,26,44), BS::Gold);
        p.setFont(mkFont(22)); p.setPen(BS::GoldLight);
        p.drawText(QPointF(PX+30.f, 76.f + QFontMetrics(mkFont(22)).ascent()), "Aventuriers du Rail");
        p.setFont(mkFont(12)); p.setPen(BS::Muted);
        p.drawText(QPointF(PX+30.f, 108.f + QFontMetrics(mkFont(12)).ascent()), "Choisissez le nombre de joueurs");

        p.setFont(mkFont(15)); p.setPen(BS::Light);
        p.drawText(QPointF(PX+20.f, 178.f + QFontMetrics(mkFont(15)).ascent()), "Nombre de joueurs");

        for (int n = 2; n <= 4; n++) {
            float bx = PX + 20.f + float(n-2) * 130.f;
            drawBtn(p, bx, 205.f, 115.f, 55.f,
                    QString::number(n) + "  joueurs", false, mouse, time);
            if (btnHit(bx, 205.f, 115.f, 55.f, click)) {
                gs.nbJ     = n;
                gs.app     = AppState::SETUP_NAME;
                gs.nameIdx = 0;
                gs.inputBuf.clear();
            }
        }
        return;
    }

    // ── SETUP : saisie des noms ──────────────────────────────────────────────
    if (gs.app == AppState::SETUP_NAME) {
        p.fillRect(QRectF(0,0,808,720), BS::DarkBg);
        float pulse2 = (std::sin(time*1.5f)+1.f)*0.5f;
        QColor hc2 = qJoueur(static_cast<couleurJoueur>(gs.nameIdx));
        hc2.setAlpha(int(15.f + pulse2*20.f));
        float hr2 = 160.f + pulse2*10.f;
        p.save(); p.setPen(Qt::NoPen); p.setBrush(hc2);
        p.drawEllipse(QPointF(404,280), double(hr2), double(hr2));
        p.restore();

        if (!gs.logoPix.isNull()) {
            float maxW=340.f, maxH=320.f;
            float scale = qMin(maxW/gs.logoPix.width(), maxH/gs.logoPix.height());
            float lw = gs.logoPix.width()*scale, lh = gs.logoPix.height()*scale;
            QPixmap scaled = gs.logoPix.scaled(int(lw), int(lh), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            p.setOpacity(200.0/255.0);
            p.drawPixmap(QPointF(404.f-lw/2.f, 260.f-lh/2.f), scaled);
            p.setOpacity(1.0);
        }

        {
            QFont f = mkFont(22); p.setFont(f);
            QFontMetrics fm(f);
            QString inv = "Joueur " + QString::number(gs.nameIdx+1) + " / " + QString::number(gs.nbJ);
            int tw = fm.horizontalAdvance(inv);
            p.setPen(qJoueur(static_cast<couleurJoueur>(gs.nameIdx)));
            p.drawText(QPointF(404.f - tw/2.f, 440.f + fm.ascent()), inv);
        }

        // Progression
        p.save(); p.setPen(QPen(QColor(255,255,255,60), 1.f));
        for (int i = 0; i < gs.nbJ; i++) {
            float px2 = 404.f - float(gs.nbJ)*14.f + float(i)*28.f;
            QColor dc2 = (i < gs.nameIdx) ? qJoueur(static_cast<couleurJoueur>(i))
                       : (i == gs.nameIdx) ? lerp(qJoueur(static_cast<couleurJoueur>(i)), BS::Dark, 0.3f+0.2f*pulse2)
                       : BS::CardBdr;
            p.setBrush(dc2);
            p.drawEllipse(QPointF(px2, 490.f), 10.0, 10.0);
        }
        p.restore();

        drawCard(p, PX+20.f, 50.f, PW-40.f, 220.f, QColor(20,26,44));
        p.fillRect(QRectF(PX+20.f, 50.f, PW-40.f, 4.f),
                   qJoueur(static_cast<couleurJoueur>(gs.nameIdx)));

        {
            QFont f = mkFont(20); p.setFont(f); p.setPen(qJoueur(static_cast<couleurJoueur>(gs.nameIdx)));
            p.drawText(QPointF(PX+30.f, 66.f + QFontMetrics(f).ascent()),
                       "Joueur " + QString::number(gs.nameIdx+1) + " / " + QString::number(gs.nbJ));
        }
        p.setFont(mkFont(13)); p.setPen(BS::Muted);
        p.drawText(QPointF(PX+30.f, 96.f + QFontMetrics(mkFont(13)).ascent()), "Entrez votre nom :");

        // Champ de saisie
        p.save();
        p.fillRect(QRectF(PX+30.f, 118.f, PW-60.f, 38.f), QColor(30,38,60));
        p.setPen(QPen(BS::Primary, 2.f)); p.setBrush(Qt::NoBrush);
        p.drawRect(QRectF(PX+30.f, 118.f, PW-60.f, 38.f));
        p.restore();

        bool showCur = (int(time * 2.f) % 2 == 0);
        QString buf = QString::fromStdString(gs.inputBuf) + (showCur ? "|" : " ");
        p.setFont(mkFont(15)); p.setPen(Qt::white);
        p.drawText(QPointF(PX+38.f, 126.f + QFontMetrics(mkFont(15)).ascent()), buf);

        drawBtn(p, PX+30.f, 172.f, PW-60.f, 42.f, "Valider  [Entrée]",
                gs.inputBuf.empty(), mouse, time, BS::Success);

        bool ok = !gs.inputBuf.empty() && btnHit(PX+30.f, 172.f, PW-60.f, 42.f, click);
        if (ok) {
            gs.joueurs.push_back(new Joueur(gs.inputBuf, static_cast<couleurJoueur>(gs.nameIdx)));
            gs.inputBuf.clear();
            gs.nameIdx++;
            if (gs.nameIdx >= gs.nbJ) initGame(gs);
        }
        return;
    }

    // ── FIN DE PARTIE ────────────────────────────────────────────────────────
    if (gs.app == AppState::GAME_OVER) {
        float pp = (std::sin(time*2.f)+1.f)*0.5f;
        drawCard(p, PX+10.f, 30.f, PW-20.f, 80.f,
                 lerp(QColor(40,30,5), QColor(20,26,44), pp));
        p.fillRect(QRectF(PX+10.f, 30.f, PW-20.f, 4.f), lerp(BS::Warning, BS::Success, pp));

        p.setFont(mkFont(20)); p.setPen(BS::Warning);
        p.drawText(QPointF(PX+20.f, 44.f + QFontMetrics(mkFont(20)).ascent()), "=== FIN DE PARTIE ===");
        p.setFont(mkFont(13)); p.setPen(BS::Muted);
        p.drawText(QPointF(PX+20.f, 74.f + QFontMetrics(mkFont(13)).ascent()), "Résultats finaux");

        if (gs.gagnant) {
            drawCard(p, PX+10.f, 125.f, PW-20.f, 60.f);
            p.setFont(mkFont(18)); p.setPen(qJoueur(gs.gagnant->getCouleur()));
            p.drawText(QPointF(PX+20.f, 134.f + QFontMetrics(mkFont(18)).ascent()),
                       "Vainqueur : " + QString::fromStdString(gs.gagnant->getNom()));
            p.setFont(mkFont(13)); p.setPen(BS::Muted);
            p.drawText(QPointF(PX+20.f, 160.f + QFontMetrics(mkFont(13)).ascent()),
                       QString::number(gs.gagnant->getTicketFini()) + " tickets complétés");
        }

        float ys = 210.f;
        for (auto j : gs.joueurs) {
            drawCard(p, PX+10.f, ys, PW-20.f, 32.f);
            p.fillRect(QRectF(PX+10.f, ys, 4.f, 32.f), qJoueur(j->getCouleur()));
            p.setFont(mkFont(13)); p.setPen(qJoueur(j->getCouleur()));
            p.drawText(QPointF(PX+22.f, ys+8.f + QFontMetrics(mkFont(13)).ascent()),
                       QString::fromStdString(j->getNom()) + "  :  " +
                       QString::number(j->getTicketFini()) + " ticket(s)");
            ys += 38.f;
        }

        drawBtn(p, PX+20.f, 540.f, PW-40.f, 48.f, "Quitter", false, mouse, time, BS::Danger);
        if (btnHit(PX+20.f, 540.f, PW-40.f, 48.f, click))
            gs.shouldQuit = true;
        return;
    }

    // ── JEU EN COURS ─────────────────────────────────────────────────────────
    Joueur* j  = gs.cur();
    QColor  jc = qJoueur(j->getCouleur());

    // En-tête joueur
    drawCard(p, PX+8.f, 6.f, PW-16.f, 72.f);
    p.fillRect(QRectF(PX+8.f, 6.f, PW-16.f, 4.f), jc);

    // Logo petit
    if (!gs.logoPix.isNull()) {
        float maxW=52.f, maxH=52.f;
        float scale = qMin(maxW/gs.logoPix.width(), maxH/gs.logoPix.height());
        float lw = gs.logoPix.width()*scale, lh = gs.logoPix.height()*scale;
        QPixmap scaled = gs.logoPix.scaled(int(lw), int(lh), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        p.setOpacity(160.0/255.0);
        p.drawPixmap(QPointF(PX+PW-38.f-lw/2.f, 42.f-lh/2.f), scaled);
        p.setOpacity(1.0);
    }

    p.setFont(mkFont(17)); p.setPen(jc);
    p.drawText(QPointF(PX+16.f, 16.f + QFontMetrics(mkFont(17)).ascent()),
               QString::fromStdString(j->getNom() + "  (" + nomJoueurCol(j->getCouleur()) + ")"));

    // Badges
    auto drawBadge = [&](float bx, float by, const QString& txt, QColor col) {
        p.fillRect(QRectF(bx, by, 70.f, 22.f), col);
        p.setFont(mkFont(12)); p.setPen(Qt::white);
        p.drawText(QPointF(bx+5.f, by+4.f + QFontMetrics(mkFont(12)).ascent()), txt);
    };
    drawBadge(PX+16.f, 48.f, "Wagons: " + QString::number(j->getNbWagons()),
              j->getNbWagons() <= 5 ? BS::Danger : BS::Secondary);
    drawBadge(PX+100.f, 48.f, "Tickets: " + QString::number(j->getTicketFini()) + "/6",
              j->getTicketFini() >= 5 ? BS::Warning : BS::Success);

    // Cartes en main
    drawCard(p, PX+8.f, 84.f, PW-16.f, 68.f);
    p.setFont(mkFont(11)); p.setPen(BS::Muted);
    p.drawText(QPointF(PX+16.f, 88.f + QFontMetrics(mkFont(11)).ascent()), "Main");

    float cx = PX + 16.f;
    for (auto c : {couleurTrain::JAUNE, couleurTrain::ROUGE, couleurTrain::VERT,
                   couleurTrain::BLEU,  couleurTrain::BLANC, couleurTrain::NOIR,
                   couleurTrain::MULTI}) {
        int n = j->getNbCartes(c);
        if (n == 0) continue;
        drawShadow(p, cx, 100.f, 42.f, 40.f, 60);
        p.save();
        p.fillRect(QRectF(cx, 100.f, 42.f, 40.f), qRoute(c));
        p.setPen(QPen(QColor(255,255,255,40), 1.f));
        p.setBrush(Qt::NoBrush);
        p.drawRect(QRectF(cx, 100.f, 42.f, 40.f));
        p.fillRect(QRectF(cx, 100.f, 42.f, 4.f), QColor(255,255,255,30));
        QColor tc2 = (c == couleurTrain::BLANC) ? Qt::black : Qt::white;
        p.setFont(mkFont(14)); p.setPen(tc2);
        p.drawText(QPointF(cx+4.f, 100.f + QFontMetrics(mkFont(14)).ascent()), QString::number(n));
        p.setFont(mkFont(9)); p.setPen(tc2);
        p.drawText(QPointF(cx+2.f, 116.f + QFontMetrics(mkFont(9)).ascent()),
                   QString::fromStdString(nomTrain(c).substr(0,3)));
        p.restore();
        cx += 48.f;
        if (cx > PX + PW - 55.f) cx = PX + 16.f;
    }

    // Missions
    const auto& ms = j->getMissions();
    float mh = 24.f + float(ms.size()) * 18.f;
    drawCard(p, PX+8.f, 158.f, PW-16.f, mh);
    p.setFont(mkFont(11)); p.setPen(BS::Muted);
    p.drawText(QPointF(PX+16.f, 162.f + QFontMetrics(mkFont(11)).ascent()), "Missions");
    float my = 178.f;
    for (auto tk : ms) {
        p.setFont(mkFont(11)); p.setPen(BS::Warning);
        p.drawText(QPointF(PX+16.f, my + QFontMetrics(mkFont(11)).ascent()), "> ");
        p.setPen(QColor(230,230,130));
        p.drawText(QPointF(PX+30.f, my + QFontMetrics(mkFont(12)).ascent()),
                   QString::fromStdString(tk->getVilleDepart()->getNom() + "  \u2192  " + tk->getVilleArrivee()->getNom()));
        my += 18.f;
    }

    // Boutons d'actions
    float by2 = qMax(my + 10.f, 200.f);
    bool noTrains   = gs.trainLeft <= 0;
    bool noMissions = j->getMissions().empty();

    if (gs.action == ActionSt::MENU) {
        drawBtn(p, PX+8.f, by2, PW-16.f, 42.f,
                "  Piocher 2 cartes train",
                noTrains, mouse, time,
                noTrains ? BS::Secondary : BS::Primary);

        drawBtn(p, PX+8.f, by2+50.f, PW-16.f, 42.f,
                "  Poser wagons  (cliquer sur la carte)",
                false, mouse, time, BS::Success);

        drawBtn(p, PX+8.f, by2+100.f, PW-16.f, 42.f,
                "  Passer son tour  (défausser tickets)",
                noMissions, mouse, time,
                noMissions ? BS::Secondary : BS::Secondary);

        if (!noTrains && btnHit(PX+8.f, by2, PW-16.f, 42.f, click)) {
            int nb = std::min(2, gs.trainLeft);
            for (int i = 0; i < nb; i++) { gs.partie->piocherTrain(*j); gs.trainLeft--; }
            gs.addLog(j->getNom() + " pioche " + to_string(nb) + " carte(s).");
            gs.statusMsg.clear(); gs.tourIdx++;
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
            gs.statusMsg.clear(); gs.tourIdx++;
        }
    } else {
        float pulse = (std::sin(time*3.f)+1.f)*0.5f;
        p.save();
        p.fillRect(QRectF(PX+8.f, by2, PW-16.f, 38.f), lerp(QColor(60,55,5), QColor(40,38,5), pulse));
        p.setPen(QPen(lerp(BS::Warning, QColor(200,140,0), pulse), 1.5f));
        p.setBrush(Qt::NoBrush);
        p.drawRect(QRectF(PX+8.f, by2, PW-16.f, 38.f));
        p.setFont(mkFont(13)); p.setPen(BS::Warning);
        p.drawText(QPointF(PX+12.f, by2+10.f + QFontMetrics(mkFont(13)).ascent()),
                   "  Cliquez sur une voie libre...");
        p.restore();

        drawBtn(p, PX+8.f, by2+46.f, PW-16.f, 38.f, "  Annuler", false, mouse, time, BS::Danger);
        if (btnHit(PX+8.f, by2+46.f, PW-16.f, 38.f, click)) {
            gs.action = ActionSt::MENU;
            gs.hovered = nullptr;
            gs.statusMsg.clear();
        }
    }

    // Message d'erreur
    if (!gs.statusMsg.empty()) {
        float ey = by2 + 152.f;
        drawCard(p, PX+8.f, ey, PW-16.f, 30.f, BS::Danger);
        p.setFont(mkFont(11)); p.setPen(QColor(255,200,200));
        p.drawText(QPointF(PX+12.f, ey+7.f + QFontMetrics(mkFont(11)).ascent()),
                   "  " + QString::fromStdString(gs.statusMsg));
    }

    // Journal
    const float logY = 575.f;
    drawCard(p, PX+8.f, logY, PW-16.f, 140.f, QColor(14,18,30));
    p.fillRect(QRectF(PX+8.f, logY, PW-16.f, 3.f), BS::Primary);
    p.setFont(mkFont(11)); p.setPen(BS::Muted);
    p.drawText(QPointF(PX+16.f, logY+5.f + QFontMetrics(mkFont(11)).ascent()), "Journal");

    for (size_t i = 0; i < gs.log.size(); i++) {
        float age     = gs.now - (i < gs.logTimes.size() ? gs.logTimes[i] : 0.f);
        float alpha01 = qMin(1.f, age / 0.4f);
        int   alpha   = int(alpha01 * 210.f);
        float slideX  = (1.f - alpha01) * 20.f;

        QColor col(180,190,210,alpha);
        if (gs.log[i].find("[Ticket!]") != string::npos) col = QColor(255,215,0,alpha);
        if (gs.log[i].find("[Grande")   != string::npos) col = QColor(50,210,80,alpha);
        if (gs.log[i].find("vide")      != string::npos) col = QColor(220,60,60,alpha);

        p.setFont(mkFont(11)); p.setPen(col);
        p.drawText(QPointF(PX+16.f+slideX,
                           logY+20.f + float(i)*17.f + QFontMetrics(mkFont(11)).ascent()),
                   QString::fromStdString(gs.log[i]));
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 8 – Interaction carte
// ═══════════════════════════════════════════════════════════════════════════════
/**
 * @brief Trouve la voie ferée la plus proche d'un point donné, dans un rayon de 14 pixels. Ignore les voies déjà prises ou mal positionnées.
 * 
 * @param mp 
 * @param gs 
 * @return VoieFerree* 
 */
static VoieFerree* findRoute(QPointF mp, const GS& gs)
{
    VoieFerree* best = nullptr;
    double bestD = 14.0;
    for (auto v : gs.voies) {
        if (v->getJoueur()) continue;
        auto lv = v->getListeVille();
        if (!CITY_POS.count(lv[0]->getNom()) || !CITY_POS.count(lv[1]->getNom())) continue;
        QPointF pa = CITY_POS.at(lv[0]->getNom());
        QPointF pb = CITY_POS.at(lv[1]->getNom());
        float off = gs.offsets.count(v) ? gs.offsets.at(v) : 0.f;
        QPointF d  = pb - pa;
        double  len = std::sqrt(d.x()*d.x() + d.y()*d.y());
        QPointF perp = len > 0 ? QPointF(-d.y()/len, d.x()/len) : QPointF(0,0);
        double dist = distSeg(mp, pa + perp*off, pb + perp*off);
        if (dist < bestD) { bestD = dist; best = v; }
    }
    return best;
}
/** @brief Gère le clic sur la carte
 * 
 * @param mousePos 
 * @param gs 
 */
static void handleMapClick(QPoint mousePos, GS& gs)
{
    if (gs.app != AppState::GAME || gs.action != ActionSt::SELECT_ROUTE) return;
    QPointF mp = {double(mousePos.x()), double(mousePos.y())};
    VoieFerree* v = findRoute(mp, gs);
    if (!v) return;

    Joueur* j  = gs.cur();
    int  poids = v->getPoids();
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
/**
 * @brief Met à jour l'élément survolé par la souris
 * 
 * @param mp 
 * @param gs 
 */
static void updateHover(QPoint mp, GS& gs)
{
    if (gs.app != AppState::GAME || gs.action != ActionSt::SELECT_ROUTE)
        { gs.hovered = nullptr; return; }
    gs.hovered = findRoute({double(mp.x()), double(mp.y())}, gs);
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION 9 – Widget Qt + main()
// ═══════════════════════════════════════════════════════════════════════════════
/** @brief Widget principal du jeu
 * 
 */
class GameWidget : public QWidget {
public:
    explicit GameWidget(QWidget* parent = nullptr) : QWidget(parent)
    {
        setFixedSize(1272, 720);
        setWindowTitle("Aventuriers du Rail");
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);

        gs.logoPix.load(QString(FILES_DIR) + "/logo.png");

        clock.start();

        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this](){
            update();
            if (gs.shouldQuit) close();
        });
        timer->start(16);
    }
/**
 * @brief 
 * 
 */
protected:
    void paintEvent(QPaintEvent*) override
    {
        gs.now = float(clock.elapsed()) / 1000.f;

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::TextAntialiasing);

        p.fillRect(rect(), BS::DarkBg);

        if (gs.app == AppState::GAME || gs.app == AppState::GAME_OVER)
            drawMap(p, gs, gs.now);

        drawPanel(p, gs, pendingClick, mousePos, gs.now);
        pendingClick = QPoint(-1, -1);
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton) {
            pendingClick = e->pos();
            if (e->pos().x() < 808)
                handleMapClick(e->pos(), gs);
        }
    }

    void mouseMoveEvent(QMouseEvent* e) override
    {
        mousePos = e->pos();
        updateHover(e->pos(), gs);
    }

    void keyPressEvent(QKeyEvent* e) override
    {
        if (gs.app != AppState::SETUP_NAME) return;
        if (e->key() == Qt::Key_Backspace) {
            if (!gs.inputBuf.empty()) gs.inputBuf.pop_back();
        } else if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
            if (!gs.inputBuf.empty()) {
                gs.joueurs.push_back(
                    new Joueur(gs.inputBuf, static_cast<couleurJoueur>(gs.nameIdx)));
                gs.inputBuf.clear();
                gs.nameIdx++;
                if (gs.nameIdx >= gs.nbJ) initGame(gs);
            }
        } else {
            QString text = e->text();
            if (!text.isEmpty()) {
                QChar ch = text[0];
                if (ch.unicode() >= 32 && ch.unicode() < 127)
                    gs.inputBuf += static_cast<char>(ch.toLatin1());
            }
        }
        update();
    }
/** @brief Widget principal du jeu
 * 
 */
private:
    GS            gs;
    QElapsedTimer clock;
    QPoint        mousePos   {0, 0};
    QPoint        pendingClick{-1, -1};
};
/**
 * @brief Fonction principale
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    GameWidget w;
    w.show();

    // Cleanup on exit handled by destructor / process end
    return app.exec();
}
