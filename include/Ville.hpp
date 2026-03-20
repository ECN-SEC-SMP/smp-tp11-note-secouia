#pragma once
#include <string>

using namespace std;

class Ville {
    private:
        string nom;
    
    public:
         /**
         * @brief Constructeur ville
         * @param nom Une chaîne de charactère
         * @return Ville
         */
        Ville(string nom);

        /**
        * @brief Getter nom de la ville
        * @return string
        */
        string getNom() const; 

};