/*
 * upper.h
 * Définition du programme upper
 * SR 13/04/94
 * LL 14/10/97
 */

// Constantes définies pour le programme
#define N 			4 // Taille maximale d'un problème de calcul d'enveloppe
#define PB 			11 // Nombre de problèmes dans la pile
#define P 			5 // Nombre d'esclaves
#define MSG_PB		0 // Type de message pour le problème
#define MSG_END 	1 // Type de message pour la fin
#define PB_UH 		0 // Type de problème pour le calcul d'enveloppe
#define PB_FUSION 	1 // Type de problème pour la fusion
#define XMAX 		1024 // Largeur maximale de l'espace de travail
#define YMAX 		1024 // Hauteur maximale de l'espace de travail

// Structure pour représenter un point
typedef struct st_point point;
struct st_point {
    int x, y; // Coordonnées du point
    point *next; // Pointeur vers le prochain point dans la liste chaînée
};

// Structure pour représenter un problème
typedef struct st_pb pb_t;
struct st_pb {    
    int nbPoints1, nbPoints2; // Nombre de points dans les deux ensembles
    int type; // Type de problème (calcul d'enveloppe ou fusion)
    point *pts1; // Premier ensemble de points
    point *pts2; // Deuxième ensemble de points
};

// Déclarations de fonctions externes dans point.c
extern point *point_alloc(); // Alloue de la mémoire pour un point
extern void point_free(); // Libère la mémoire allouée pour un point
extern void point_print(); // Affiche un point
extern void point_print_gnuplot(); // Affiche le gnuplot
extern point *point_random(); // Génère un point aléatoire
extern point *point_UH(); // Calcule l'enveloppe convexe de 4 point en plus
extern int point_nb(); // Calcule le nombre de points dans un ensemble
extern point *point_part(); // Partitionne un ensemble de points en deux sous ensembles
extern point *point_merge_UH(); // Fusionne deux ensembles de points
extern void upper_hull(); // Calcule l'enveloppe supérieure

// Déclarations de fonctions externes pour le problème
extern pb_t *pb_alloc(); // Alloue de la mémoire pour un problème
extern void pb_free(); // Libère la mémoire allouée pour un problème
extern void send_pb(); // Envoie un problème à un processus
extern pb_t *receive_pb(); // Reçoit un problème d'un processus