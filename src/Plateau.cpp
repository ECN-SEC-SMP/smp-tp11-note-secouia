#include "Plateau.hpp"

// CORRECTION : tous les indices de boucle passent en size_t pour éviter
// le warning -Wsign-compare (comparaison int vs size_t non signé)

bool Plateau::isVilleStringInVector(string ville, vector<Ville *> listeVille) const
{
    for (size_t i = 0; i < listeVille.size(); i++)
    {
        if (listeVille[i]->getNom() == ville)
        {
            return true;
        }
    }
    return false;
}

bool Plateau::isVilleInVector(Ville *ville, vector<Ville *> listeVille) const
{
    for (size_t i = 0; i < listeVille.size(); i++)
    {
        if (listeVille[i] == ville)
        {
            return true;
        }
    }
    return false;
}

Plateau::Plateau()
{
    vector<VoieFerree *> v;
    this->listeVoieFerree = v;
}

Plateau::~Plateau()
{
    for (size_t i = 0; i < this->listeVoieFerree.size(); i++)
    {
        delete this->listeVoieFerree[i];
    }
}

void Plateau::placeWagon(string villeA, string villeB, Joueur *joueur, couleurTrain couleur)
{
    bool isAnEdit = false;
    for (size_t i = 0; i < this->getListeVoieFerree().size(); i++)
    {
        VoieFerree *current = this->getListeVoieFerree()[i];
        if (isVilleStringInVector(villeA, current->getListeVille()) && isVilleStringInVector(villeB, current->getListeVille()) && couleur == current->getCouleur())
        {

            if (current->getJoueur() != nullptr)
            {
                // TODO : RAISE AN ERROR
            }

            current->setJoueur(joueur);
            isAnEdit = true;
        }
    }

    if (!isAnEdit)
    {
        // TODO : RAISE AN ERROR
    }
}

Ville *Plateau::getVilleFromString(string ville, vector<Ville *> v) const
{
    for (size_t i = 0; i < v.size(); i++)
    {
        if (v[i]->getNom() == ville)
        {
            return v[i];
        }
    }

    // CORRECTION : retourner nullptr si aucune ville trouvée
    // (évite -Werror=return-type : "control reaches end of non-void function")
    return nullptr;
}

// CORRECTION : la fonction ne retournait rien → return !v.empty()
// v contient les sommets atteignables non encore visités ; tant qu'il en reste,
// Dijkstra doit continuer.
bool verifDiskstra(vector<Ville *> listeVille, map<Ville *, int> distance, vector<Ville *> sommets_visites)
{
    vector<Ville *> v;

    for (size_t i = 0; i < listeVille.size(); i++)
    {
        auto it = find(sommets_visites.begin(), sommets_visites.end(), listeVille[i]);
        if (distance[listeVille[i]] != numeric_limits<int>::max() && it == sommets_visites.end())
        {
            v.push_back(listeVille[i]);
        }
    }

    return !v.empty(); // ← retourne vrai s'il reste des sommets à explorer
}

bool Plateau::existeChemin(string villeA, string villeB, Joueur *joueur) const
{
    /* Initialisation du tableau distances */

    map<Ville *, int> distance;

    for (size_t i = 0; i < this->getListeVille().size(); i++)
    {
        distance[this->getListeVille()[i]] = numeric_limits<int>::max();
    }

    distance[getVilleFromString(villeA, this->getListeVille())] = 0;

    vector<Ville *> sommets_visites;
    while (verifDiskstra(this->getListeVille(), distance, sommets_visites))
    {
        int distance_minimale = numeric_limits<int>::max();
        Ville *t = nullptr;

        map<Ville *, int>::iterator it;
        for (it = distance.begin(); it != distance.end(); it++)
        {
            auto recherche = find(sommets_visites.begin(), sommets_visites.end(), it->first);

            if (recherche != sommets_visites.end() && distance[it->first] < distance_minimale)
            {
                distance_minimale = distance[it->first];
                t = it->first;
            }
        }

        vector<Ville *> voisins;
        map<Ville *, int> distanceDuVoisin;

        // Recherche des voisins
        for (size_t i = 0; i < this->getListeVille().size(); i++)
        {
            for (size_t j = 0; j < this->getListeVoieFerree().size(); j++)
            {
                if (isVilleInVector(t, this->getListeVoieFerree()[j]->getListeVille()) && isVilleInVector(this->getListeVille()[i], this->getListeVoieFerree()[j]->getListeVille()) && this->getListeVoieFerree()[j]->getJoueur() == joueur)
                {

                    voisins.push_back(this->getListeVille()[i]);
                    distanceDuVoisin[this->getListeVille()[i]] = this->getListeVoieFerree()[j]->getPoids();
                }
            }
        }

        for (size_t i = 0; i < voisins.size(); i++)
        {
            distance[voisins[i]] = min(distance[voisins[i]], distance[t] + distanceDuVoisin[voisins[i]]);
        }
        sommets_visites.push_back(t);
    }

    return distance[getVilleFromString(villeB, this->getListeVille())] != numeric_limits<int>::max();
}

vector<VoieFerree *> Plateau::getListeVoieFerree() const
{
    return this->listeVoieFerree;
}

vector<Ville *> Plateau::getListeVille() const
{
    return this->listeVille;
}

vector<Ticket*> Plateau::getPiocheTickets() {
    vector<Ticket*> piocheTickets;

    ifstream fichier("./files/ticket.csv");

    if(!fichier.is_open()){
        cerr << "Erreur dans l'ouverture du fichier *ticket.csv*" << endl;
    }

    string ligne;

    getline(fichier, ligne);


    while (getline(fichier, ligne))
    {
        stringstream ss(ligne);
        string ticketId;
        string nom_villeA; 
        string nom_villeB; 

        getline(ss, ticketId, ',');
        getline(ss, nom_villeA, ',');
        getline(ss, nom_villeB, ',');

        Ville* villeA = getVilleFromString(nom_villeA, this->getListeVille());
        Ville* villeB = getVilleFromString(nom_villeB, this->getListeVille());

        Ticket* tempTicket = new Ticket(villeA, villeB);

        piocheTickets.push_back(tempTicket);

    }

    return piocheTickets;
    
}


vector<Train*> Plateau::getPiocheTrain(){
    vector<Train*> tempPioche; 

    for(int i = 0; i < 10; i++){
        //Création des cartes :
        Train* trainJaune = new Train(couleurTrain::JAUNE);
        Train* trainVert = new Train(couleurTrain::VERT);
        Train* trainBlanc = new Train(couleurTrain::BLANC);
        Train* trainNoir = new Train(couleurTrain::NOIR);
        Train* trainBleu = new Train(couleurTrain::BLEU);
        Train* trainRouge = new Train(couleurTrain::ROUGE);

        tempPioche.push_back(trainJaune);
        tempPioche.push_back(trainVert);
        tempPioche.push_back(trainBlanc);
        tempPioche.push_back(trainNoir);
        tempPioche.push_back(trainBleu);
        tempPioche.push_back(trainRouge);
    }

    for(int i = 0; i < 12; i++){
        Train* locomotive = new Train(couleurTrain::MULTI);

        tempPioche.push_back(locomotive);
    }

    return tempPioche;
}
