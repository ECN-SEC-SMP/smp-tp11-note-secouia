/**
 * @file Joueur.hpp
 * @author Thomas Metais (metais.thomas92@gmail.com)
 * @brief Déclaration de la classe @ref Joueur, participant à une partie des
 *        *Aventuriers du Rail*.
 * @version 1.0
 * @date 2026-03-17
 *
 * @details
 * Ce fichier définit l'entité @ref Joueur qui encapsule l'état d'un joueur au
 * cours d'une @ref Partie : sa main de cartes @ref Train, ses missions
 * (@ref Ticket), son stock de wagons, et les actions disponibles
 * (prise de voie, validation de ticket, fin de partie…).
 *
 * @see Partie
 * @see Plateau
 * @see Ticket
 * @see Train
 */
#pragma once

#include "couleurEnum.hpp"
#include "Ticket.hpp"
#include "Train.hpp"
#include "Plateau.hpp"

#include <vector>
#include <string>
#include <ostream>

/**
 * @class Joueur
 * @brief Représente un joueur d'une partie des *Aventuriers du Rail*.
 *
 * @details
 * Un @ref Joueur maintient l'intégralité de son état personnel :
 * - un **nom** et une **couleur d'équipe**, immuables après construction ;
 * - un stock de **wagons** (initialement `20`) décrémenté à chaque voie prise ;
 * - une **main** de cartes @ref Train (ressources nécessaires à la prise de voies) ;
 * - une liste de **missions** (@ref Ticket) en cours et un compteur de missions validées.
 *
 * La classe expose les opérations de :
 * - gestion de main : ajouterCarte(), retirerCartes(), peutPrendreVoie(), defausserCartesVoie() ;
 * - gestion des wagons : utiliserWagons() ;
 * - gestion des tickets : ajouterTicket(), defausserTickets(), validerTicket() ;
 * - détection de fin de partie : aGagne(), naPlusDeWagons().
 *
 * @par Conditions de victoire
 * - @ref aGagne renvoie `true` dès que @ref getTicketFini atteint `6`.
 * - @ref naPlusDeWagons signale l'épuisement de la réserve de wagons, qui
 *   déclenche le dernier tour côté @ref Partie.
 *
 * @invariant `getNbWagons() >= 0` durant toute la partie.
 * @invariant `getTicketFini() >= 0` et ne décroît jamais (monotone croissant).
 * @invariant `getNom()` et `getCouleur()` ne changent jamais après la construction.
 * @invariant Tous les pointeurs contenus dans `getMissions()` et `getMainCartes()` sont non nuls.
 *
 * @warning Le @ref Joueur **ne possède pas** les @ref Train ni les @ref Ticket
 *          qu'il référence : leur durée de vie est gérée par @ref Partie qui détient
 *          les pioches. Retirer une carte ou défausser un ticket n'appelle donc
 *          jamais `delete` sur ces objets.
 *
 * @note Les cartes @ref couleurTrain::MULTI font office de joker et sont consommées
 *       en dernier par @ref defausserCartesVoie afin de préserver cette ressource rare.
 *
 * @code{.cpp}
 * Joueur alice{"Alice", couleurJoueur::BLEU};
 * alice.ajouterCarte(new Train(couleurTrain::ROUGE));
 * if (alice.peutPrendreVoie(couleurTrain::ROUGE, 1)) {
 *     alice.defausserCartesVoie(couleurTrain::ROUGE, 1);
 *     alice.utiliserWagons(1);
 * }
 * @endcode
 *
 * @see Partie
 * @see Plateau
 * @see Ticket
 * @see Train
 * @see VoieFerree
 */
class Joueur
{
private:
    int ticketFini;                  ///< Nombre de @ref Ticket validés (monotone croissant, jamais décrémenté).
    int nbWagons;                    ///< Wagons restants dans la réserve du joueur (initialement `20`).
    std::vector<Ticket *> missions;  ///< Missions en cours : @ref Ticket détenus mais non encore validés. **Pointeurs non propriétaires.**
    couleurJoueur couleur;           ///< Couleur d'équipe du joueur, fixée à la construction.
    std::string nom;                 ///< Nom affiché du joueur, fixé à la construction.
    std::vector<Train *> mainCartes; ///< Main courante de cartes @ref Train. **Pointeurs non propriétaires.**

public:
    /**
     * @brief Construit un nouveau joueur prêt à entrer en partie.
     *
     * @details Initialise la main et la liste de missions vides, le compteur de
     *          tickets validés à `0` et le stock de wagons à `20`.
     *
     * @param[in] nom     Nom du joueur (attendu non vide).
     * @param[in] couleur Couleur d'équipe du joueur.
     *
     * @post `getNbWagons() == 20`
     * @post `getTicketFini() == 0`
     * @post `getNbCartesTotales() == 0`
     * @post `getMissions().empty()`
     *
     * @par Complexité
     * O(1).
     */
    Joueur(const std::string &nom, couleurJoueur couleur);

    /**
     * @brief Destructeur par défaut.
     *
     * @warning Ne libère ni les @ref Train ni les @ref Ticket pointés : leur
     *          durée de vie est entièrement gérée par @ref Partie.
     */
    ~Joueur() = default;

    // ===== Accesseurs =====

    /**
     * @brief Renvoie le nom du joueur.
     * @return Référence constante sur le nom stocké.
     * @par Complexité O(1).
     */
    const std::string &getNom() const;

    /**
     * @brief Renvoie la couleur d'équipe du joueur.
     * @return Valeur de l'énumération @ref couleurJoueur.
     * @par Complexité O(1).
     */
    couleurJoueur getCouleur() const;

    /**
     * @brief Renvoie le nombre de wagons restants dans la réserve.
     * @return Entier positif ou nul.
     * @par Complexité O(1).
     * @see utiliserWagons, naPlusDeWagons
     */
    int getNbWagons() const;

    /**
     * @brief Renvoie le nombre de @ref Ticket que le joueur a validés.
     * @return Compteur monotone croissant de missions terminées.
     * @par Complexité O(1).
     * @see validerTicket, aGagne
     */
    int getTicketFini() const;

    /**
     * @brief Renvoie la liste des missions en cours.
     * @return Référence constante sur le vecteur des @ref Ticket non encore validés.
     * @par Complexité O(1).
     */
    const std::vector<Ticket *> &getMissions() const;

    /**
     * @brief Compte les cartes d'une couleur précise dans la main.
     * @param[in] couleur Couleur cherchée (y compris @ref couleurTrain::MULTI).
     * @return Nombre exact de cartes de cette couleur dans la main.
     * @par Complexité O(n) où n = `getNbCartesTotales()`.
     */
    int getNbCartes(couleurTrain couleur) const;

    /**
     * @brief Renvoie la taille totale de la main, toutes couleurs confondues.
     * @return Entier positif ou nul.
     * @par Complexité O(1).
     */
    int getNbCartesTotales() const;

    /**
     * @brief Renvoie la main complète du joueur.
     * @return Référence constante sur le vecteur des cartes @ref Train détenues.
     * @par Complexité O(1).
     */
    const std::vector<Train *> &getMainCartes() const;

    // ===== Gestion des cartes =====

    /**
     * @brief Ajoute une carte @ref Train à la main du joueur.
     *
     * @param[in] carte Pointeur non nul sur la carte à ajouter. Le joueur ne devient
     *                  pas propriétaire de l'objet.
     *
     * @pre `carte != nullptr`.
     * @post `getNbCartesTotales()` est incrémenté de `1`.
     *
     * @par Complexité O(1) amorti.
     */
    void ajouterCarte(Train *carte);

    /**
     * @brief Retire jusqu'à `quantite` cartes d'une couleur donnée.
     *
     * @param[in] couleur  Couleur ciblée.
     * @param[in] quantite Nombre exact de cartes à retirer (≥ 0).
     *
     * @return `true` si exactement `quantite` cartes ont été retirées,
     *         `false` sinon (il n'y en avait pas assez).
     * @retval true  `quantite` cartes retirées avec succès.
     * @retval false Nombre insuffisant ; la main est **partiellement** modifiée.
     *
     * @warning En cas d'échec, les cartes déjà retirées ne sont **pas restaurées**.
     *          Vérifier en amont avec @ref peutPrendreVoie ou @ref getNbCartes.
     * @warning Les cartes retirées ne sont pas détruites (pointeurs non propriétaires).
     *
     * @par Complexité O(n) où n = `getNbCartesTotales()`.
     * @see defausserCartesVoie
     */
    bool retirerCartes(couleurTrain couleur, int quantite);

    /**
     * @brief Teste si le joueur dispose d'assez de cartes pour prendre une voie.
     *
     * @details Les cartes @ref couleurTrain::MULTI complètent la couleur demandée
     *          à la manière d'un joker.
     *
     * @param[in] couleurVoie Couleur exigée par la @ref VoieFerree.
     * @param[in] longueur    Longueur de la voie (≥ 1).
     *
     * @return `true` si `getNbCartes(couleurVoie) + getNbCartes(MULTI) >= longueur`,
     *         `false` sinon.
     *
     * @par Complexité O(n).
     * @see defausserCartesVoie
     */
    bool peutPrendreVoie(couleurTrain couleurVoie, int longueur) const;

    /**
     * @brief Défausse les cartes nécessaires à la prise d'une voie.
     *
     * @details Consomme d'abord les cartes de la couleur demandée (dans la limite
     *          de `longueur`), puis complète avec des @ref couleurTrain::MULTI.
     *          **Ne décrémente pas** le stock de wagons : appeler @ref utiliserWagons
     *          séparément après cet appel.
     *
     * @param[in] couleurVoie Couleur exigée par la voie.
     * @param[in] longueur    Nombre de cartes à défausser (≥ 1).
     *
     * @return `true` si la défausse a été effectuée, `false` si le joueur n'avait
     *         pas assez de cartes.
     * @retval true  Cartes défaussées avec succès.
     * @retval false Cartes insuffisantes ; main inchangée.
     *
     * @par Complexité O(n).
     * @see peutPrendreVoie, utiliserWagons
     */
    bool defausserCartesVoie(couleurTrain couleurVoie, int longueur);

    // ===== Gestion des wagons =====

    /**
     * @brief Consomme un certain nombre de wagons dans la réserve du joueur.
     *
     * @param[in] nb Nombre de wagons à utiliser (≥ 0).
     *
     * @return `true` si la réserve contenait assez de wagons, `false` sinon.
     * @retval true  Wagons décomptés ; `getNbWagons()` diminué de `nb`.
     * @retval false Réserve insuffisante ; état inchangé.
     *
     * @post En cas de succès, `getNbWagons()` est décrémenté de `nb`.
     * @par Complexité O(1).
     * @see naPlusDeWagons
     */
    bool utiliserWagons(int nb);

    // ===== Gestion des tickets =====

    /**
     * @brief Ajoute un @ref Ticket à la liste des missions en cours.
     *
     * @param[in] ticket Pointeur non nul sur le ticket à ajouter
     *                   (non possédé par le joueur).
     *
     * @pre `ticket != nullptr`.
     * @post `getMissions().size()` est incrémenté de `1`.
     *
     * @par Complexité O(1) amorti.
     */
    void ajouterTicket(Ticket *ticket);

    /**
     * @brief Vide la liste des missions en cours et la renvoie à l'appelant.
     *
     * @details Utile lors du choix initial de tickets en début de partie : les
     *          tickets non conservés sont renvoyés à la pioche via le vecteur retourné.
     *
     * @return Vecteur contenant tous les @ref Ticket défaussés (éventuellement vide).
     * @post `getMissions().empty()`.
     *
     * @par Complexité O(n) où n = `getMissions().size()`.
     */
    std::vector<Ticket *> defausserTickets();

    /**
     * @brief Tente de valider un ticket en vérifiant la connexion de ses deux villes.
     *
     * @details Délègue à @ref Plateau::existeChemin : si un chemin composé
     *          uniquement des voies possédées par ce joueur relie la ville de
     *          départ à la ville d'arrivée du ticket, celui-ci est marqué comme
     *          terminé, retiré de @ref missions et le compteur @ref getTicketFini
     *          est incrémenté.
     *
     * @param[in]     ticket  Pointeur non nul sur un ticket contenu dans `getMissions()`.
     * @param[in,out] plateau Plateau de jeu courant (nécessaire pour le pathfinding).
     *
     * @return `true` si le ticket est validé, `false` si aucun chemin n'existe.
     * @retval true  Ticket validé et retiré de `missions` ; `ticketFini` incrémenté.
     * @retval false Pas de chemin complet ; état inchangé.
     *
     * @pre `ticket != nullptr`.
     * @pre `ticket` appartient à @ref getMissions (sinon le compteur peut être
     *      incrémenté sans qu'aucun élément ne soit réellement retiré).
     *
     * @par Complexité
     * O(V + E) dominée par le parcours de graphe dans @ref Plateau::existeChemin.
     *
     * @see Plateau::existeChemin, aGagne
     */
    bool validerTicket(Ticket *ticket, Plateau &plateau);

    // ===== Fin de partie =====

    /**
     * @brief Indique si le joueur a satisfait la condition de victoire principale.
     * @return `true` dès que @ref getTicketFini atteint `6`, `false` sinon.
     * @par Complexité O(1).
     * @see getTicketFini
     */
    bool aGagne() const;

    /**
     * @brief Indique si le joueur n'a plus de wagons disponibles.
     * @return `true` si `getNbWagons() <= 0`, `false` sinon.
     * @note Signale généralement le dernier tour côté @ref Partie.
     * @par Complexité O(1).
     */
    bool naPlusDeWagons() const;

    // ===== Affichage =====

    /**
     * @brief Affiche la main du joueur (taille et couleurs) sur un flux.
     * @param[in,out] os Flux de sortie à alimenter.
     * @par Complexité O(n) où n = `getNbCartesTotales()`.
     */
    void afficherMain(std::ostream &os) const;
};

/**
 * @brief Opérateur d'insertion stream : résumé compact d'un @ref Joueur.
 *
 * @details Format produit :
 *          `Joueur: <nom> | Couleur: <id> | Wagons: <n> | Tickets: <k>`.
 *
 * @param[in,out] os     Flux de sortie cible.
 * @param[in]     joueur Joueur à afficher.
 * @return Référence sur `os` pour chaînage d'appels.
 *
 * @par Complexité O(1).
 */
std::ostream &operator<<(std::ostream &os, const Joueur &joueur);
