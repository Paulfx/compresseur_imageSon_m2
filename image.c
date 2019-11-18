#include "image.h"


/*
 * Lecture d'une ligne du fichier.
 * On saute les lignes commençant par un "#" (commentaire)
 * On simplifie en considérant que les lignes ne dépassent pas MAXLIGNE
 */

void lire_ligne(FILE *f, char *ligne)
{
	while(fgets(ligne, MAXLIGNE, f) != NULL) {
		if (ligne[0]!='#')
			break;
	}
}

/*
 * Allocation d'une image
 */

struct image* allocation_image(int hauteur, int largeur)
{
	struct image* im;
	ALLOUER(im, 1);
	im->largeur = largeur;
	im->hauteur = hauteur;

	ALLOUER(im->pixels, hauteur);
	for (int i=0; i<hauteur; ++i)
		ALLOUER(im->pixels[i], largeur);

	return im;
}

/*
 * Libération image
 */

void liberation_image(struct image* image)
{
	for (int i=0; i<image->hauteur; ++i)
		free(image->pixels[i]);
	free(image->pixels);
	free(image);
}

/*
 * Allocation et lecture d'un image au format PGM.
 * (L'entête commence par "P5\nLargeur Hauteur\n255\n"
 * Avec des lignes de commentaire possibles avant la dernière.
 */

struct image* lecture_image(FILE *f)
{

	char ligne[MAXLIGNE];
//Ligne P5\n
	lire_ligne(f, ligne);
	if (strcmp(ligne, "P5\n")) {
		eprintf("Mauvais format de fichier PGM pour ligne 1, %s au lieu de P5\n", ligne);
		return NULL;
	}
//Ligne Largeur Hauteur\n
	int largeur,hauteur;
	lire_ligne(f, ligne);
	const char* delim = " \n";
	//On split la ligne avec les espaces
	char *split = strtok(ligne, delim);
	largeur = atoi(split);
	if (largeur == 0) {
		eprintf("Mauvaise lecture de largeur\n");
		return NULL;
	}
	hauteur = atoi(strtok(NULL, delim));
	if (hauteur == 0) {
		eprintf("Mauvaise lecture de hauteur\n");
		return NULL;
	}

//Ligne 255\n
	lire_ligne(f, ligne);
	if (strcmp(ligne, "255\n")) {
		eprintf("Mauvais format de fichier PGM pour ligne 3, %s au lieu de 255\n", ligne);
		return NULL;
	}

	//On peut allouer l'image
	struct image *res = allocation_image(hauteur, largeur);

	printf("Image de taille h=%d, l=%d\n", hauteur, largeur);

	for (int i=0; i<hauteur; ++i) {
		lire_ligne(f, ligne);
		//printf("Ligne %d, %s\n", strlen(ligne), ligne);
		//Erase last character == \n
		//ligne[strlen(ligne)-1]=0;
		res->pixels[i] = ligne;
	}
	
	return res;
}

/*
 * Écriture de l'image (toujours au format PGM)
 */

void ecriture_image(FILE *f, const struct image *image)
{










}
