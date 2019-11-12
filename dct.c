#include "bases.h"
#include "matrice.h"
#include "dct.h"

/*
 * La fonction calculant les coefficients de la DCT (et donc de l'inverse)
 * car la matrice de l'inverse DCT est la transposée de la matrice DCT
 *
 * Cette fonction prend beaucoup de temps.
 * il faut que VOUS l'utilisiez le moins possible (UNE SEULE FOIS)
 *
 * FAITES LES CALCULS EN "double"
 *
 * La valeur de Pi est : M_PI
 *
 * Pour ne pas avoir de problèmes dans la suite du TP, indice vos tableau
 * avec [j][i] et non [i][j].
 */

#define M_PI           3.14159265358979323846

void coef_dct(Matrice *table)
{
	double n = table->width;
	double sqrt_n = sqrt(n);
	double moyenne = 1.f / sqrt_n;

	//printf("Moyenne %f\n", moyenne);

	int i;
	int j;
	for (i=0;i<table->width;++i)
		table->t[0][i] = moyenne;

	double sqrt_2_n = sqrt(2) / sqrt_n;

	for(j=1;j<table->height;++j)
		for(i=0;i<table->width;++i) {
			table->t[j][i] = sqrt_2_n * cos(j * M_PI * (2 * i + 1) / (2*n)); 
		}

}

/*
 * La fonction calculant la DCT ou son inverse.
 *
 * Cette fonction va être appelée très souvent pour faire
 * la DCT du son ou de l'image (nombreux paquets).
 */

void dct(int   inverse,		/* ==0: DCT, !=0 DCT inverse */
	 int nbe,		/* Nombre d'échantillons  */
	 const float *entree,	/* Le son avant transformation (DCT/INVDCT) */
	 float *sortie		/* Le son après transformation */
	 )
{
	static Matrice* dct; //TODO liberer, surement nul mais comment ne pas la recreer
	//Calculer les coefficients de la dct
	dct = allocation_matrice_float(nbe,nbe);
	coef_dct(dct);

	Matrice *dct_calc;

	if (inverse) {
		dct_calc = allocation_matrice_float(nbe,nbe);
		transposition_matrice(dct,dct_calc);
	}
	else
		dct_calc = dct;

	produit_matrice_vecteur(dct_calc, entree, sortie);
}
