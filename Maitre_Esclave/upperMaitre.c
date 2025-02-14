#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <strings.h>
#include "pvm3.h"
#include "point.h"
#include <string.h>

// Déclaration de la pile de problèmes et de l'index de tête de pile
static pb_t *my_file[PB];
static int tete;

// Initialise la file de problèmes avec des points générés aléatoirement
void init_queue(point *pts)
{
    int i;
    for (i=0; i<PB; i++) {
        my_file[i] = pb_alloc();
        my_file[i]->nbPoints1 = N;
        my_file[i]->nbPoints2 = 0;
        my_file[i]->pts1 = point_alloc();
        memcpy(my_file[i]->pts1, pts + i*N, N*sizeof(point));
        my_file[i]->pts2 = NULL;
        my_file[i]->type = PB_UH;
    }
    tete = PB;    
}

// Empile un problème dans la pile globale my_file
void empile(pb_t *pb)
{
    if (tete >= PB) {
        printf("La pile est pleine, impossible d'empiler un nouveau problème.\n");
        pvm_exit();
        exit(-1);
    }
    my_file[tete++] = pb;
}

// Dépile un problème de la pile globale my_file
pb_t *depile()
{
    if(tete > 0){
        return my_file[--tete];
    }
    else {
        return NULL;
    }
}

// Programme principal qui gère la distribution des problèmes aux esclaves
int main(int argc, char **argv)
{

    if (argc != 2) {
		fprintf(stderr, "usage: %s <nb points>\n", *argv);
		exit(-1);
	}

    int i;
    int tids[P]; // Identifiants des processus esclaves
    point *pts; // Données à traiter
    pb_t *pb; // Problème actuellement traité
    int sender[1];
    pts = point_random(atoi(argv[1])); // Génération aléatoire des données
    int nbPoints = point_nb(pts); 

    init_queue(pts); // Initialisation de la pile avec les données générées
    
    // Création de P processus esclaves
    pvm_spawn(EPATH "/upperEsclave", (char**)0, 0, "", P, tids);

    // Distribution des problèmes aux esclaves
    for (i=0; tete>0 && i<P; i++) send_pb(tids[i], depile());

    while (1) {
        pb_t *pb2;

        // Réception d'une solution (type fusion)
        pb = receive_pb(-1, sender);
        empile(pb);

        // Vérification de la fin du traitement
        if (pb->nbPoints1 == nbPoints) break;

        pb = depile(); // Récupération d'un problème à traiter
        // Traitement en fonction du type de problème
        if (pb->type == PB_UH) 
            // Envoi du problème à l'esclave pour calcul d'enveloppe
            send_pb(sender[0], pb);
        else { // Si le problème est de type fusion
            pb2 = depile(); // Récupération d'un deuxième problème pour fusion
            if (pb2==NULL) // Si la pile est vide
                empile(pb); // Remplacement du problème dans la pile
            else {
                if (pb2->type == PB_UH) { // Si le deuxième problème est de type calcul d'enveloppe
                    empile(pb); // Remplacement du problème actuel dans la pile
                    send_pb(sender[0], pb2); // Envoi du deuxième problème à l'esclave
                }
                else { // Si le deuxième problème est de type fusion
                    // Fusion des deux problèmes en un seul
                    pb->nbPoints2 = pb2->nbPoints1;
                    pb->pts2 = pb2->pts1;
                    send_pb(sender[0], pb); // Envoi du problème fusionné à l'esclave
                    pb_free(pb2); // Libération de la mémoire du deuxième problème
                }
            }
        }
    }

    pvm_initsend(PvmDataDefault);
    pvm_mcast(tids, P, MSG_END); // Notification des esclaves de la fin du traitement
    point_print_gnuplot(pb->pts1, 0, "version_parallele"); // Affichage des points restants
    point_print_gnuplot(pb->pts1, 1, "version_parallele"); 
    pb_free(pb); // Libération de la mémoire du dernier problème
    pvm_exit();
    exit(0);
}
