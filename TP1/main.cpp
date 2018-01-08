/**
 HA David
 ECOLLAN Charles
 LECAILLE Quentin
 NOUBISSI NKONGTCHOU Eva Mylene
 GROUPE A
 **/
#include <fstream>
#include <iostream>
#include <map>
using namespace std;
typedef struct {
    int nbTaches;       // Les tâches seront numérotées de 1 à nbTaches
    char* nomTaches;    // Chaque tâche est représentée par un caractère son indice dans ce tableau
    multimap<char,char> contraintes; // Stockage des contraintes pour chaque tâche
    map<char,int> duree; // Stockage des durées, préférable à un simple tableau dynamique pour trouver directement la valeur correspondante à une clé
} tt_contraintes;
// Stockage du graphe construit à partir des contraintes
typedef struct {
    int nbSommets; // Nombre de tâches + 2
    bool** adj; // Matrice d'adjacence
    int** val; // Matrice des valeurs
    // Valeur val[x][y] significative uniquement lorsque adj[x][y]=true
} tt_graphe;


void ajouterContrainte(tt_contraintes ** lesContrainte, char tacheCourante, char contrainteCourante);

void supprimerContrainte(tt_contraintes ** lesContrainte, char tacheCourante, char contrainteCourante);

void calculerRang(tt_contraintes * lesContrainte, char nomTache, map<char,int>& rang);

void plusTot(tt_contraintes * lesContrainte, char nomTache, map<char,int> rang, map<char,int>& datetot);

void plusTard(tt_contraintes * lesContrainte, char nomTache, map<char,int> rang, map<char, int>& datetard, map<char,int> datetot);

bool DFS (map<pair<char,char>,char>& status, char tache, char contrainte, tt_contraintes ** lesContrainte);

void clean_status (map<pair<char,char>,char> &status);

int main(int argc, const char * argv[]) {
    // Ebauche de code pour démarrer votre TP / mini-projet
    // Stockage des durées et contraintes
    // ---------------------------------------------
    // Lecture des tâches et contraintes sur fichier
    // Attention :
    // Il n'est fait ici aucune vérification de syntaxe dans le fichier. Toute erreur entraînera un dysfonctionnement du programme
    printf("Lecture de tableau de contraintes\n");
    ifstream F("ha-ecollan-lacaille-noubissi-c03.txt");
    tt_contraintes * lesContraintes = new tt_contraintes;
    char tacheCourante, contrainteCourante;
    int dureeTache;
    map<pair<char,char>, char> status; // Status des différents verticies
    
    // Ligne 1 : nombre de tâches
    F >> lesContraintes->nbTaches;
    printf("Nombre de taches : %2d\n", lesContraintes->nbTaches);
    lesContraintes->nomTaches = new char[lesContraintes->nbTaches + 1]; // Indices 1 à nbTaches utilisés. Case 0 non utilisée
    
    // Lignes 2 à nbTaches+1 : durées des tâches
    
    printf("Durees : \n");
    for (int t = 1; t <= lesContraintes->nbTaches; t++) {
        F >> tacheCourante;
        lesContraintes->nomTaches[t] = tacheCourante;
        F >> dureeTache;
        lesContraintes->duree.insert(pair<char,int>(tacheCourante, dureeTache));
    };
    printf("\n");
    
    for(map<char,int>::iterator it = lesContraintes->duree.begin(); it != lesContraintes->duree.end(); ++it)
        printf("%c ", it->first);
    printf("\n");
    for(map<char,int>::iterator it = lesContraintes->duree.begin(); it != lesContraintes->duree.end(); ++it)
        printf("%i ", it->second);
    printf("\n\n");
    // Ligne suivantes : contraintes
    // Une ligne par tâche
    printf("Contraintes :\n");
    for (int t = 1; t <= lesContraintes->nbTaches; t++) {
        F >> tacheCourante;
        F >> contrainteCourante;
        while (contrainteCourante != '.') {
            // Ajout des status des différentes arêtes pour détecter le circuit
            status.insert(make_pair(make_pair(tacheCourante, contrainteCourante), 'N')); // On distinguera B : Being explored, N : Not explored, F : Fully explored
            ajouterContrainte(&lesContraintes, tacheCourante, contrainteCourante); // Ajout des contraintes dans le conteneur
            DFS(status, tacheCourante, contrainteCourante, &lesContraintes); // Après ajout, détecte si l'arête crée un circuit, si oui on la supprime
            clean_status(status);
            F >> contrainteCourante;
        };
    };
    // ------------------
    // Création du graphe
    // Initialisation des structures de données
    tt_graphe* leGraphe = new tt_graphe;
    leGraphe->nbSommets = lesContraintes->nbTaches + 2;
    // début = lesContraintes->nbTaches+1
    // fin = lesContraintes->nbTaches+2
    leGraphe->adj = new bool*[leGraphe->nbSommets + 1]; // ligne 0 non utilisée
    leGraphe->val = new int*[leGraphe->nbSommets + 1]; // ligne 0 non utilisée
    for (int ligne = 1; ligne <= leGraphe->nbSommets; ligne++) {
        leGraphe->adj[ligne] = new bool[leGraphe->nbSommets + 1];   // colonne 0 non utilisée
        leGraphe->val[ligne] = new int[leGraphe->nbSommets + 1];    // colonne 0 non utilisée
        // Initialisation : aucun arc
        for (int colonne = 1; colonne <= leGraphe->nbSommets; colonne++){
            leGraphe->adj[ligne][colonne] = false;
        };
    };
    
    
    // Transformation contraintes et durées --> graphe
    //Création de la ligne du sommet a, tous les sommets qui n'ont pas de contraintes
    lesContraintes->nbTaches++;
    lesContraintes->nomTaches[lesContraintes->nbTaches] = 'a';
    lesContraintes->duree.insert(pair<char,int>('a', 0));
    for(int colonne = 1; colonne < leGraphe->nbSommets; colonne++) {
        multimap<char,char>::iterator it = lesContraintes->contraintes.find(lesContraintes->nomTaches[colonne]);
        if(it == lesContraintes->contraintes.end() && lesContraintes->nomTaches[colonne] != 'a') { // Si on ne trouve pas le sommet dans les contraintes
            leGraphe->adj[lesContraintes->nbTaches][colonne] = true;
            leGraphe->val[lesContraintes->nbTaches][colonne] = 0;
            ajouterContrainte(&lesContraintes, lesContraintes->nomTaches[colonne], 'a');
        }
    }
    
    //Création de la colonne pour le sommet z, tous les sommets qui n'ont pas de fleches vers d'autres sommets
    lesContraintes->nbTaches++;
    lesContraintes->nomTaches[lesContraintes->nbTaches] = 'z';
    for(int ligne = 1; ligne < leGraphe->nbSommets; ligne++) {
        bool etat_final = true;
        for(multimap<char,char>::iterator it = lesContraintes->contraintes.begin(); it != lesContraintes->contraintes.end(); it++)
            if(it->second == lesContraintes->nomTaches[ligne]) etat_final = false;
        if(etat_final && lesContraintes->nomTaches[ligne] != 'a') { // On ne veut pas inclure le sommet initial a
            leGraphe->adj[ligne][leGraphe->nbSommets] = true;
            leGraphe->val[ligne][leGraphe->nbSommets] = lesContraintes->duree.find(lesContraintes->nomTaches[ligne])->second;
            ajouterContrainte(&lesContraintes, 'z', lesContraintes->nomTaches[ligne]);
        }
    };
    
    //Création du reste de la matrice d'adjacence et de valeur pour tous les autres sommets
    for(int ligne = 1; ligne < leGraphe->nbSommets; ligne++){
        for(int colonne = 1; colonne < leGraphe->nbSommets; colonne++){
            //On va parcourir la multimap pour chercher à partir des valeurs
            for(multimap<char, char>::iterator it = lesContraintes->contraintes.begin(); it != lesContraintes->contraintes.end(); it++) {
                if(it->second == lesContraintes->nomTaches[ligne] && it->first == lesContraintes->nomTaches[colonne]) {
                    leGraphe->adj[ligne][colonne] = true;
                    leGraphe->val[ligne][colonne] = lesContraintes->duree.find(lesContraintes->nomTaches[ligne])->second;
                }
            }
        }
    }
    
    
    
    // Impression du graphe
    // Le code proposé ici imprime le contenu de la matrice d'adjacence.
    // A vous de l'améliorer pour afficher les valeurs des arcs, ainsi que les noms (caractères) de chaque tâche en en-tête de ligne et colonne
    // Et aussi, comme indiqué dans le sujet du TP, remplacer les "1" par la valeur de l'arc, et les "0" par rien
    // Attention : les sommets 'début' (nbTaches+1) et 'fin' (nbTaches+1) n'ont pas de libelle dans la structure "contraintes"
    printf("------------------\nMatrice d'adjacence\n------------------\n");
    printf("  ");
    for(int i = 1; i <= lesContraintes->nbTaches; i++) printf("%c ", lesContraintes->nomTaches[i]);
    printf("\n");
    
    for (int ligne = 1; ligne <= leGraphe->nbSommets; ligne++) {
        printf("%c ", lesContraintes->nomTaches[ligne]);
        for (int colonne = 1; colonne <= leGraphe->nbSommets; colonne++) {
            if (leGraphe->adj[ligne][colonne]) {
                printf("1 ");
            }
            else {
                printf("0 ");
            };
        };
        printf("\n");
    };
    
    // Impression du graphe des valeurs
    
    printf("------------------\nMatrice de valeurs\n------------------\n");
    printf("  ");
    for(int i = 1; i <= lesContraintes->nbTaches; i++) printf("%c ", lesContraintes->nomTaches[i]);
    printf("\n");
    
    for (int ligne = 1; ligne <= leGraphe->nbSommets; ligne++) {
        printf("%c ", lesContraintes->nomTaches[ligne]);
        for (int colonne = 1; colonne <= leGraphe->nbSommets; colonne++) {
            if(leGraphe->adj[ligne][colonne]) { // On compare avec la matrice d'adjacence pour pouvoir insérer les valeurs
                printf("%i ", leGraphe->val[ligne][colonne]);
            }
            else {
                printf("  ");
            };
        };
        printf("\n");
    };
    printf("Fin de lecture des taches et contraintes\n");
    
    /**
     PARTIE CALCUL DU CALENDRIER AU PLUS TÔT ET AU PLUS TARD
     Utilisation de la multimap des contraintes pour partir depuis a et mettre les sommets qui n'ont pas de contraintes en rang n = 1
     */
    
    printf("\n---------------------------------\nCalcul du calendrier au plus tot et au plus tard\n---------------------------------\n");
    map<char,int> rang; // Association sommet et rang
    map<char,int> dateTot; // Conteneur des dates au plus tot pour tous les sommets
    map<char,int> dateTard; // Conteneur des dates au plus tard pour tous les sommets
    
    rang.insert(pair<char,int>('a', 0)); // 'a' sera toujours fixé que ce soit pour le rang ou la date au plus tot
    dateTot.insert(pair<char,int>('a', 0));
    calculerRang(lesContraintes, 'a', rang); // on commence la récursivité dans l'ordre des rangs decroissants donc a partir de 'a'
    plusTot(lesContraintes, 'a', rang, dateTot);
    plusTard(lesContraintes, 'z', rang, dateTard, dateTot); // on commence par l'ordre des rangs decroissants donc a partir de 'z'
    

    // AFFICHAGE DES RANGS
    printf("\t\t\t\t\t");
    for(int i = 1; i <= lesContraintes->nbTaches; i++) printf("%c\t", lesContraintes->nomTaches[i]); // On affiche d'abord le nom des taches
    printf("\n");
    printf("Rang\t\t\t\t");
    for(map<char,int>::const_iterator it2 = rang.begin(); it2 != rang.end(); it2++) {
        printf("%i\t", it2->second); // Affichage des rangs
    };
    printf("\n");
    
    
    // AFFICHAGE DES DATES AU PLUS TOT ET AU PLUS TARD
    printf("Date au plus tot\t");
    for(map<char,int>::const_iterator it = dateTot.begin(); it != dateTot.end(); it++) printf("%i\t", it->second); // Affichage des dates au plus tot
    printf("\nDate au plus tard\t");
    for(map<char,int>::const_iterator it = dateTard.begin(); it != dateTard.end(); it++) printf("%i\t", it->second); // Affichage des dates au plus tard
    
    
    // DIAGRAMME DATE AU PLUS TOT
    printf("\n\n\n-----------------------\nDiagramme date au plus tot\n-----------------------\n");
    printf("\t ");
    for(int i = 0; i <= dateTard.find('z')->second; i++) {
        if(i < 10) printf("%i  ", i); // Pour le formatage du diagramme pour éviter les décalages chiffres
        else printf("%i ",i);
    }
    printf("\n");
    for(int ligne = 1; ligne <= lesContraintes->nbTaches -2; ligne++) { // On ne veut pas inclure 'a' et 'z'
        printf("%c\t ", lesContraintes->nomTaches[ligne]);
        for(int colonne = 0; colonne <= dateTard.find('z')->second; colonne++) { // On itère jusqu'à la date au plus tard de 'z'
            if(colonne >= dateTot.find(lesContraintes->nomTaches[ligne])->second && colonne < dateTot.find(lesContraintes->nomTaches[ligne])->second + lesContraintes->duree.find(lesContraintes->nomTaches[ligne])->second)
                printf("|==");
            else printf("|  ");
        }
        printf("\n");
    }
    
    
    // DIAGRAMME DATE AU PLUS TARD
    printf("\n\n\n-----------------------\nDiagramme date au plus tard\n-----------------------\n");
    printf("\t ");
    for(int i = 0; i <= dateTard.find('z')->second; i++) {
        if(i < 10) printf("%i  ", i); // Pour le formatage du diagramme afin éviter les décalages chiffres
        else printf("%i ",i);
    }
    printf("\n");
    for(int ligne = 1; ligne <= lesContraintes->nbTaches -2; ligne++) {
        printf("%c\t ", lesContraintes->nomTaches[ligne]);
        for(int colonne = 0; colonne <= dateTard.find('z')->second; colonne++) { // On itère jusqu'à la date au plus tard de 'z'
            if(colonne >= dateTard.find(lesContraintes->nomTaches[ligne])->second && colonne < dateTard.find(lesContraintes->nomTaches[ligne])->second + lesContraintes->duree.find(lesContraintes->nomTaches[ligne])->second)
                printf("|==");
            else printf("|  ");
        }
        printf("\n");
    }
    return 0;
}

/**
 Ajout des contraintes dans le conteneur
 
 @param lesContrainte
 Structure du graphe dont on aura besoin pour accéder aux contraintes
 
 @param tacheCourante
 Tache dont on veut ajouter une contrainte
 
 @param contrainteCourante
 Tache que l'on ajoute aux contraintes de la tache courante
 */
void ajouterContrainte(tt_contraintes ** lesContrainte, char tacheCourante, char contrainteCourante) {
    (*lesContrainte)->contraintes.insert(pair<char,char>(tacheCourante, contrainteCourante));
    map<char,int>::const_iterator it = (*lesContrainte)->duree.find(contrainteCourante);
    printf("%c --[%i]--> %c\n", contrainteCourante, it->second, tacheCourante);
}

/**
 Suppression d'une contrainte
 
 @param lesContrainte
 Structure du graphe dont on aura besoin pour accéder aux contraintes
 
 @param tacheCourante
 Tache courante
 
 @param contrainteCourante
 Contrainte à enlever pour le sommet donné
 */
void supprimerContrainte(tt_contraintes ** lesContrainte, char tacheCourante, char contrainteCourante) {
    pair<multimap<char,char>::iterator, multimap<char,char>::iterator> it = (*lesContrainte)->contraintes.equal_range(tacheCourante); // On cherche l'arête à supprimer
    for(multimap<char,char>::iterator it2 = it.first; it2 != it.second; ++it2) { // si la tache et la contrainte correspondent bien, on supprime la contrainte du conteneur
        if(it2->second == contrainteCourante){
            (*lesContrainte)->contraintes.erase(it2);
            break;
        }
    }
}

/**
 Ajout des rangs dans le conteneur recursivement
 
 @param lesContrainte
 Structure du graphe dont on aura besoin pour accéder aux contraintes
 
 @param nomTache
 Nom de la tache actuelle
 
 @param rang
 Conteneur des rangs pour insérer les rangs des différentes taches
 */
void calculerRang(tt_contraintes * lesContrainte, char nomTache, map<char,int> &rang) {
    if(nomTache == 'z') { // Condition d'arrêt
        return;
    }
    int n_rang = rang.find(nomTache)->second; // Rang du sommet actuel
    for(multimap<char,char>::iterator it = lesContrainte->contraintes.begin(); it != lesContrainte->contraintes.end(); it++) {
        if(it->second == nomTache) { // On cherche les successeurs du sommet actuel
            map<char,int>::iterator it2 = rang.find(it->first);
            if(it2 == rang.end()) { // S'il n'existe pas dans le conteneur de rang on l'ajoute
                rang.insert(pair<char,int>(it->first, n_rang+1));
            }else if(it2->second < n_rang+1) { // S'il existe et que le rang qu'on veut lui mettre est supérieur, on modifie son rang sans ajouter une nouvelle association
                it2->second = n_rang+1;
            }
            calculerRang(lesContrainte, it->first, rang); // On l'applique à tous les autres sommets qui ont pour contrainte le sommet actuel
        }
    }
}

/**
 Calcul la date au plus tot pour chaque sommet a partir de leur rang
 
 @param lesContrainte
 Structure du graphe dont on aura besoin pour accéder aux contraintes.
 
 @param nomTache
 Nom de la tache actuelle.
 
 @param rang
 Conteneur des rangs pour modifier la date au plus tot
 
 @param dateTot
 Conteneur des dates au plus tôt des différentes tâches
 
 */
void plusTot(tt_contraintes * lesContrainte, char nomTache, map<char,int> rang, map<char,int>& dateTot) {
    if(nomTache == 'z') {
        return;
    }
    map<char,int>::iterator it2 = dateTot.find(nomTache); // Cherche la date au plus tot du rang actuel
    for(multimap<char,char>::iterator it = lesContrainte->contraintes.begin(); it != lesContrainte->contraintes.end(); it++) {
        if(it->second == nomTache && rang.find(nomTache)->second < rang.find(it->first)->second) { // On cherche le successeur et s'il a un rang supérieur au sommet actuel
            map<char,int>::iterator it3 = dateTot.find(it->first);
            if(it3 == dateTot.end())  // pas encore de date au plus tôt
                dateTot.insert(pair<char,int>(it->first, lesContrainte->duree.find(nomTache)->second + it2->second));
            
            if(it3 != dateTot.end() && it3->second < lesContrainte->duree.find(nomTache)->second + it2->second) // si la date que l'on veut modifier est supérieure à celle déjà existante
                it3->second = lesContrainte->duree.find(nomTache)->second + it2->second;
            
            plusTot(lesContrainte, it->first, rang, dateTot);
        }
    }
}

/**
 Calcul de la date au plus tard pour chaque sommet à partir de leur rang
 
 @param lesContrainte
 Structure du graphe dont on aura besoin pour accéder aux contraintes
 
 @param nomTache
 Tache courante
 
 @param rang
 Conteneur des rangs des différents sommets
 
 @param dateTard
 Conteneur des dates au plus tard de chaque sommet
 
 @param dateTot
 Conteneur des dates au plus tot de chaque sommet
 */
void plusTard(tt_contraintes * lesContrainte, char nomTache, map<char,int>rang, map<char,int>& dateTard, map<char,int> dateTot) {
    if(nomTache == 'a') return; // Condition d'arrêt
    if(nomTache == 'z'){ // On met directement la valeur au plus tot de z dans sa valeur au plus tard car on part de lui
        map<char,int>::const_iterator it = dateTot.find(nomTache);
        dateTard.insert(pair<char,int>(nomTache, it->second));
    }
    pair<multimap<char,char>::iterator, multimap<char,char>::iterator> it_predecesseur = lesContrainte->contraintes.equal_range(nomTache); // On cherche les prédecesseurs du sommet actuel
    map<char,int>::const_iterator it_courant = dateTard.find(nomTache); // On cherche sa date au plus tard
    int rang_courant = rang.find(nomTache)->second;
    for(multimap<char,char>::iterator it2 = it_predecesseur.first; it2 != it_predecesseur.second; ++it2) {
        map<char,int>::iterator it_plusT = dateTard.find(it2->second);
        map<char,int>::const_iterator it_duree = lesContrainte->duree.find(it2->second);
        if(it_plusT == dateTard.end() && it_courant->second - it_duree->second >= 0 && rang_courant >= rang.find(it2->first)->second) { // S'il n'existe aucune date au plus tard pour le prédecesseur, on l'ajoute
            dateTard.insert(pair<char,int>(it2->second, it_courant->second - it_duree->second));
        }
        else if((it_plusT->second > (it_courant->second - it_duree->second)) && ((it_courant->second - it_duree->second) >= 0) && (rang_courant >= rang.find(it2->first)->second))
        { // Si on peut appliquer une date plus petite on le fait
            it_plusT->second = it_courant->second - it_duree->second;
        }
        plusTard(lesContrainte, it2->second, rang, dateTard, dateTot); // On l'applique ensuite récursivement au prédecesseur du prédecesseur jusqu'à atteindre 'a'
    }
}

/**
 Vérifie par un parcours en profondeur s'il existe un circuit en partant d'un sommet donné

 @param status
 Conteneur des status des différentes arêtes
 
 @param tache
 Tache courante
 
 @param contrainte
 Contrainte courante
 
 @param lesContrainte
 Structure du graphe dont on aura besoin pour accéder aux contraintes
 */
bool DFS (map<pair<char,char>,char>& status, char tache, char contrainte, tt_contraintes ** lesContrainte) {
    map<pair<char,char>, char>::iterator arete = status.find(make_pair(tache,contrainte)); // Qui va de contrainte à tache dans un graphe orienté
    arete->second = 'B'; // Change l'etat du sommet sur lequel on se situe en Being Explored
    for(multimap<char,char>::iterator it = (*lesContrainte)->contraintes.begin(); it != (*lesContrainte)->contraintes.end(); it++) {
        if(it->second == tache) { // on veut continuer le parcours sur la prochaine arête partant de la tache courante
            map<pair<char,char>, char>::iterator res = status.find(make_pair(it->first, it->second));
            if(res->second == 'N') { // Si on n'a pas encore exploré l'arête, on va l'explorer
                DFS(status, it->first, it->second, lesContrainte);
            }
            else if(res->second == 'B'){ // Si l'arête a été explorée mais pas complètement, alors on a un circuit
                supprimerContrainte(lesContrainte, it->first, it->second);
                printf("Creation de circuit detecte, suppression de l'arc de %c a %c\n", it->second, it->first);
                return true;
            }
        }
    }
    arete->second = 'F'; // Cette arête a été complètement explorée
    return false;
}
/**
 Remet les status des arêtes à 'Not explored' afin que l'appel recursif fonctionne à chaque fois qu'on appelle l'algorithme de detection de circuit
 
 @param status
 Conteneur des status des arêtes
 */
void clean_status (map<pair<char,char>,char> &status) {
    for(map<pair<char,char>,char>::iterator it = status.begin(); it != status.end(); it++) {
        it->second = 'N';
    }
}
