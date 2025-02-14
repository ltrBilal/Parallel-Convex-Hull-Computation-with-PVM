#include <stdio.h>
#include <stdlib.h>
#include "point.h"
#include "pvm3.h"
	
/*
   static int compareX(i,j)

   comparaison des abscisses de 2 points
   pour la fonction qsort
 */

static int compareX(point **i, point **j)
{ return (*i)->x - (*j)->x; }

/*
	fpnction pour creer une ensemble utilisant deux tableau des cordonnée 
	qui sont passés en paramètre
*/
point *creer_S(int nbPoints, int table_x[], int table_y[])
{
	point **pts;
	pts = (point **)malloc(nbPoints * sizeof(point*));
	for(int i = 0; i < nbPoints; i++){
		pts[i] = point_alloc();
		pts[i]->x = table_x[i];
		pts[i]->y = table_y[i];
		
	}
	qsort(pts, nbPoints, sizeof(point *), (__compar_fn_t) compareX);
	// Pour lier les points entre eux avec l'attribut next
	for(int i= 0; i < nbPoints - 1; i++){
		pts[i]->next = pts[i+1];
	}
	//retourner la premier point de l'ensemble
	return pts[0];
	//return (point *) *pts;
}

/*
	copie les cordonnée des point dans deux tableau:
		les x dans table_x
		les y dans table_y
*/
void copier_cordonnee(point *pts, int table_x[], int table_y[])
{
	point *p;
    int i=0;

	for (p=pts; p!=NULL; p=p->next) {
		table_x[i] = p->x;
		table_y[i] = p->y;
		i++;
	}
    
	/* for (int i = 0; i < point_nb(pts); i++) {
                 table_x[i]=p->x;
				 table_y[i]=p->y;
                 p = p->next;
	} */
}

int main(int argc, char **argv)
{     	
	int nbPoints;	       
	int tids[2];		
    point *pts, *pts2;       
       
	int parent = pvm_parent();
	if (parent == PvmNoParent) { // Si le père ( le processus principale )

		// initialisation de l'ensemble de point utilisant la valeur passé en paramètre
		pts = point_random(atoi(argv[1]));
		point_print_gnuplot(pts, 0); //affichage des points 
		nbPoints = point_nb(pts); // le nombre de points
    }
	else { // si les fils
		// ils reçoient les information suivant :
		pvm_recv(parent, MSG_DATA); // un message de père
		pvm_upkint(&nbPoints, 1, 1); //  le nombre de points
		int table_x [nbPoints];
		int table_y [nbPoints]; 
		pvm_upkint(table_x, nbPoints, 1); // les cordonnées x de toutes les points
		pvm_upkint(table_y, nbPoints, 1); // les cordonnées y de toutes les points
			//creation de l'ensemble du points utilisant les deux tableaux 
		pts = creer_S(nbPoints,table_x,table_y);
       }

	

	if (nbPoints > 4) {
		// Création de 2 fils
		pvm_spawn(BPWD "/upper_par", (char**)0, 0, "", 2, tids);
		// couper l'ensemble à deux sous ensembles
		pts2 = point_part(pts); 
		int nbPoints1=point_nb(pts); //le nombre de points de la prémiere ensemble 
		int nbPoints2=point_nb(pts2); //le nombre de points de la deuxiéme ensemble 

		int table_x1 [nbPoints1], table_x2 [nbPoints2]; 
		int table_y1 [nbPoints1], table_y2 [nbPoints2];	
		copier_cordonnee(pts , table_x1, table_y1); 
		copier_cordonnee(pts2 , table_x2, table_y2);

		// Envoyer les information vers le premier fils
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&nbPoints1, 1, 1); // envoyer le nombre de points
		pvm_pkint(table_x1,nbPoints1, 1); // envoyer les x de sous ensemble
		pvm_pkint(table_y1,nbPoints1, 1); // envoyer les y de sous ensemble
		pvm_send(tids[0], MSG_DATA);  // envoyer vers le premier fils
		// Envoyer vers le fils 2
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&nbPoints2, 1, 1); // envoyer le nombre de points
		pvm_pkint(table_x2,nbPoints2, 1); // envoyer les x de sous ensemble
		pvm_pkint(table_y2,nbPoints2, 1); // envoyer les y de sous ensemble
		pvm_send(tids[1], MSG_DATA); // envoyer vers le deuxième fils


		// reception des deux envelopes       
		// reception du Fils 1
		pvm_recv(-1, MSG_SORT);
		pvm_upkint(&nbPoints, 1, 1); // recevoir le nombre de points de sous ensemble
		int tab_x1 [nbPoints];
		int tab_y1 [nbPoints];
		pvm_upkint(tab_x1, nbPoints, 1); // recevoir les x de sous ensemble
		pvm_upkint(tab_y1, nbPoints, 1); // recevoir les y de sous ensemble
		pts = creer_S(nbPoints,tab_x1,tab_y1); // construire l'ensemble de points a partir les X et les Y
		
		// reception de la part de Fils 2
		pvm_recv(-1, MSG_SORT);
		pvm_upkint(&nbPoints, 1, 1); // recevoir la taille de sous ensemble
		int tab_x2 [nbPoints];
		int tab_y2 [nbPoints];
		pvm_upkint(tab_x2, nbPoints, 1); // recevoir les x de sous ensemble
		pvm_upkint(tab_y2, nbPoints, 1);// recevoir les y de sous ensemble
		pts2 = creer_S(nbPoints,tab_x2,tab_y2);// construire l'ensemble de points a partir les X et les Y
		// fusionner les deux sous ensebmle
		point_merge_UH(pts,pts2); 
				
	}
			

	else{
		pts = point_UH(pts);
	}

	if (parent == PvmNoParent) {
		point_print_gnuplot(pts, 1, "version_parallele");
	}
	else { // renvoi les enveloppe au parents
		pvm_initsend(PvmDataDefault);
		int taille= point_nb(pts);
		int table_x [taille];
		int table_y [taille];
		copier_cordonnee(pts , table_x, table_y); //copie_abs: permettre au tabPointsX1 de conserver les x

		pvm_pkint(&taille, 1, 1);
		pvm_pkint(table_x,taille, 1); 
		pvm_pkint(table_y,taille, 1);
		pvm_send(parent, MSG_SORT);

	}






	pvm_exit();
	exit(0);
}

