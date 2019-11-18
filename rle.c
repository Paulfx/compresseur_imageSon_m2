#include "bases.h"
#include "intstream.h"
#include "rle.h"

/*
 * Avant propos sur les "intstream"
 *
 * Un "intstream" permet de stocker des entiers dans un fichier.
 * A son ouverture (faite par les programmes de test) une méthode
 * de codage de l'entier lui est associée.
 * Dans notre cas, cela sera les algorithmes développés
 * dans "entier.c" ou "sf.c"
 *
 * Il faut remarquer que deux "intstream" peuvent permettre
 * d'écrire dans le même "bitstream". Les données sont intercalées
 * dans le fichier, pour les récupérer il faut les relire en faisant
 * les lectures dans le même ordre que les écritures.
 */

/*
 * RLE : Run Length Encoding
 *
 * Version spécifique pour stocker les valeurs de la DCT.
 *
 * Au lieu de coder un entier par valeur
 * on va coder le nombre d'entiers qui sont nuls (entier positif)
 * suivi de la valeur du premier entier non nul.
 *
 * Par exemple pour coder le tableau :
 * 	5 8 0 0 4 0 0 0 0 2 1 0 0 0
 *    On stocke dans les deux "intstream"
 *      0 0     2         4 0       3      Nombres de 0
 *      5 8     4         2 1              La valeur différentes de 0
 * Comme les deux "intstream" sont stockés dans le même fichier
 * il faut absolument lire et écrire les valeurs dans le même ordre.
 *     (0,5) (0,8) (2,4) (4,2) (0,1) (3)
 */

/*
 * Stocker le tableau de flottant dans les deux "instream"
 * En perdant le moins d'information possible.
 */

void compresse(struct intstream *entier, struct intstream *entier_signe
	       , int nbe, const float *dct)
{
	int count_zero = 0;
	int val;
	for (int i=0; i<nbe; ++i) {
		//arrondir à l'entier le plus proche
		val = rint(dct[i]);

		if (val != 0) {
			//Stocker le nombre de 0
			put_entier_intstream(entier, count_zero);
			count_zero = 0;
			//Stocker l'entier
			put_entier_intstream(entier_signe, val);
		}
		else
			count_zero++; //On peut le faire systématiquement, est ce mieux au niveau du pipeline?
	}
	//S'il reste des 0 non écrits, on les écrits
	if (count_zero)
		put_entier_intstream(entier, count_zero);
}

/*
 * Lit le tableau de flottant qui est dans les deux "instream"
 */

void decompresse(struct intstream *entier, struct intstream *entier_signe
		 , int nbe, float *dct)
{
	
	int count_val = 0;
	int count_zero, i;
	while(count_val != nbe) {

		//On lit le nombre de 0
		count_zero = get_entier_intstream(entier);
		//On met autant de 0 dans le tableau
		for (i=0; i<count_zero; ++i)
			dct[count_val++] = 0;

		//On peut finir sur des 0
		if (count_val == nbe)
			break;

		//On lit l'entier signé et on le stocke
		dct[count_val++] = get_entier_intstream(entier_signe);
	}

}
