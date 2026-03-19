#include "Plateau.hpp"

bool Plateau::isVilleStringInVector(string ville, vector<Ville*> listeVille) const {
    for(int i = 0; i < listeVille.size(); i++){
        if(listeVille[i]->getNom() == ville){
            return true;
        }
    }
    return false;
}

bool Plateau::isVilleInVector(Ville* ville, vector<Ville*> listeVille) const {
    for(int i = 0; i < listeVille.size(); i++){
        if(listeVille[i] == ville){
            return true;
        }
    }
    return false;
}


Plateau::Plateau(){
    vector<VoieFerree*> temp_listeVoieFerree;

    vector<Ville*> temp_listeVille;
    
    ifstream fichier("./files/map.csv"); //city_a, city_b, color, length

    if(!fichier.is_open()){
        cerr << "Erreur lors de l'ouverture du fichier *map.csv* !!" << endl;
    }

    string ligne;
    getline(fichier, ligne);

    while(getline(fichier, ligne)){
        stringstream ss(ligne);

        // GET VILLE A
        string nom_villeA;
        getline(ss, nom_villeA, ',');
        Ville* villeA;

        if(!isVilleStringInVector(nom_villeA, temp_listeVille)){
            villeA = new Ville(nom_villeA);
        }else{
            villeA = getVilleFromString(nom_villeA, temp_listeVille);
        }

        //Get VILLE B
        string nom_villeB;
        getline(ss, nom_villeB, ',');
        Ville* villeB;

        if(!isVilleStringInVector(nom_villeB, temp_listeVille)){
            villeB = new Ville(nom_villeB);
        }else{
            villeB = getVilleFromString(nom_villeB, temp_listeVille);
        }


        //Get COLOR 
        string color; 
        getline(ss, color, ',');

        couleurTrain tempColor; 

        if(color == "black"){
            tempColor = couleurTrain::NOIR;
        }
        if(color == "red"){
            tempColor = couleurTrain::ROUGE;
        }
        if(color == "yellow"){
            tempColor = couleurTrain::JAUNE;
        }
        if(color == "blue"){
            tempColor = couleurTrain::BLEU;
        }
        if(color == "white"){
            tempColor = couleurTrain::BLANC;
        }
        if(color == "green"){
            tempColor = couleurTrain::VERT;
        }

        // Get LENGHT
        int poids; 
        string tempPoids;

        getline(ss, tempPoids, ',');

        poids = stoi(tempPoids);
        
        vector<Ville*> tempVillePourVF; 

        tempVillePourVF.push_back(villeA);
        tempVillePourVF.push_back(villeB);

        VoieFerree* tempVoieFerree = new VoieFerree(tempVillePourVF, tempColor, poids);
        temp_listeVoieFerree.push_back(tempVoieFerree);
    }
    fichier.close();

    this->listeVoieFerree = temp_listeVoieFerree;
    this->listeVille = temp_listeVille; 


}

Plateau::~Plateau() {
    for(int i = 0; i < this->listeVoieFerree.size() ; i++){
        delete this->listeVoieFerree[i];
    }
}

void Plateau::placeWagon(string villeA, string villeB, Joueur* joueur, couleurTrain couleur){
    bool isAnEdit = false;
    for(int i = 0; i < this->getListeVoieFerree().size(); i++){
        VoieFerree* current = this->getListeVoieFerree()[i];
        if(isVilleStringInVector(villeA, current->getListeVille())
            && isVilleStringInVector(villeB, current->getListeVille())
            && couleur == current->getCouleur()){

            if(current->getJoueur() != nullptr){
                // TODO : RAISE AN ERROR
            }

            current->setJoueur(joueur);
            isAnEdit = true;
        }
    }

    if(!isAnEdit){
        //TODO : RAISE AN ERROR
    }
}

Ville* Plateau::getVilleFromString(string ville, vector<Ville*> v) const {
    for(int i = 0; i < v.size(); i ++){
        if(v[i]->getNom() == ville){
            return v[i];
        }
    }

    //TODO : RAISE AN ERROR
}

bool verifDiskstra(vector<Ville*> listeVille, map<Ville*, int> distance, vector<Ville*> sommets_visites){
    vector<Ville*> v; 
    
    for(int i = 0; i < listeVille.size(); i++){
        auto it = find(sommets_visites.begin(), sommets_visites.end(), listeVille[i]);
        if(distance[listeVille[i]] != numeric_limits<int>::max()
            && it == sommets_visites.end()){
                v.push_back(listeVille[i]);
            }
    }
}

bool Plateau::existeChemin(string villeA, string villeB, Joueur* joueur) const {
    /* Initialisation du tableau distances */

    map<Ville*, int> distance;

    for(int i = 0; i < this->getListeVille().size() ; i++){
        distance[this->getListeVille()[i]] = numeric_limits<int>::max();
    }

    distance[getVilleFromString(villeA, this->getListeVille())] = 0; 

    vector<Ville*> sommets_visites; 
    while(verifDiskstra(this->getListeVille(), distance, sommets_visites)){
        int distance_minimale = numeric_limits<int>::max();
        Ville* t = nullptr;


        map<Ville*, int>::iterator it;
        for(it = distance.begin(); it != distance.end() ; it++){
            auto recherche = find(sommets_visites.begin(), sommets_visites.end(), it->first);

            if(recherche != sommets_visites.end() && distance[it->first] < distance_minimale){
                distance_minimale = distance[it->first];
                t = it->first;
            }
        }

        vector<Ville*> voisins; 
        map<Ville*, int> distanceDuVoisin; 


        // Recherche des voisins
        for(int i = 0 ; i < this->getListeVille().size(); i++){
            for(int j = 0 ; j < this->getListeVoieFerree().size(); j++){
                if(isVilleInVector(t, this->getListeVoieFerree()[j]->getListeVille()) 
                    && isVilleInVector(this->getListeVille()[i], this->getListeVoieFerree()[j]->getListeVille())
                    && this->getListeVoieFerree()[j]->getJoueur() == joueur){
                    
                    //La ville est dans les villes d'adjances
                    voisins.push_back(this->getListeVille()[i]);
                    distanceDuVoisin[this->getListeVille()[i]] = this->getListeVoieFerree()[j]->getPoids();
                }
            }
        }


        for(int i = 0 ; i < voisins.size(); i++){
            distance[voisins[i]] = min(distance[voisins[i]], distance[t] + distanceDuVoisin[voisins[i]]);
        }
        sommets_visites.push_back(t);

    }

    return distance[getVilleFromString(villeB, this->getListeVille())];
}

vector<VoieFerree*> Plateau::getListeVoieFerree() const{
    return this->listeVoieFerree;
}

vector<Ville*> Plateau::getListeVille() const {
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
