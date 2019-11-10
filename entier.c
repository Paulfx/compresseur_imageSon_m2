#include "bits.h"
#include "entier.h"

/*
 * Les fonctions de ce fichier permette d'encoder et de décoder
 * des entiers en utilisant des codes statiques.
 */

/*
 * Codage d'un entier (entre 0 et 32767 inclus) en une chaine de bits
 * qui est écrite dans le bitstream.
 *
 * Le nombre est codé par la concaténation du PREFIXE et SUFFIXE
 * Le suffixe est en fait le nombre entier sauf le premier bit a 1
 * 
 * Nombre de bits |    PRÉFIXE     | nombres codés | SUFFIXE
 *       0        |       00       |      0        |
 *     	 1        |       01 0      |  1 (pas 0)    |
 *     	 2        |       01 1      |     2-3       | 2=0 3=1
 *     	 3        |      100 0      |     4-7       | 4=00 5=01 6=10 7=11
 *     	 4        |      100 1      |     8-15      | 8=000 ... 15=111
 *     	 5        |      101 0      |    16-31      | 16=0000 ... 31=1111
 *     	 6        |      101 1      |    32-63      |
 *     	 7        |      110 00     |    64-127     |
 *     	 8        |      110 01     |   128-255     |
 *     	 9        |      110 10     |   256-511     |
 *     	 10       |      110 11     |   512-1023    |
 *     	 11       |      111 00     |  1024-2047    |
 *     	 12       |      111 01     |  2048-4097    |
 *     	 13       |      111 10     |  4096-8191    |
 *     	 14       |      111 110    |  8192-16383   |
 *     	 15       |      111 111    | 16384-32767   |
 *
 * Je vous conseille de faire EXIT si l'entier est trop grand.
 *
 */

static char *prefixes[] = { "00", "010", "011", "1000", "1001", "1010", "1011",
			    "11000", "11001", "11010", "11011", "11100",
			    "11101", "11110", "111110", "111111" } ;

void put_entier(struct bitstream *b, unsigned int f)
{
	unsigned int nbBitsUtile = nb_bits_utile(f);
	if (nbBitsUtile >= TAILLE(prefixes) ) 
		EXIT;

	//On put le préfixe
	const char* prefixe = prefixes[nbBitsUtile];
	put_bit_string(b, prefixe);

	//On put le suffixe	
	if (nbBitsUtile > 1)
		put_bits(b, nbBitsUtile-1, f);
}

/*
 * Cette fonction fait l'inverse de la précédente.
 *
 * Un implémentation propre, extensible serait d'utiliser
 * un arbre binaire comme pour le décodage d'Huffman.
 * Ou bien parcourir l'arbre des états 8 bits par 8 bits (voir le cours)
 * Mais je ne vous le demande pas
 */

unsigned int get_entier(struct bitstream *b)
{

	//Surement mauvais....
	//On lit e = 2 bits, puis on lira e bits
	//Si e est à la valeur max (en ayant lu 3 bits....) on relit 1 bit
	//Variable en dur => c'est moche
	//Trouver une logique ? Pourquoi 6 bits au max dans le suffixe?

	//unsigned int maxValue = pow2(2) - 1;
	
	unsigned int beginPrefixe = get_bits(b, 2);
	unsigned int endPrefixe = get_bits(b,beginPrefixe);
	unsigned int indexPrefixe = (pow2(beginPrefixe) - 1) + endPrefixe;
	//Max value = 2**3-1
	if (endPrefixe == 7) {
		//On doit relire 1 bit..
		indexPrefixe += get_bit(b);
	}

	//On connait le préfixe, à l'indice indexPrefixe
	//On doit lire indexPrefixe-1 valeur après le préfixe
	if (indexPrefixe <= 1) return indexPrefixe;
	int nbBitsALire = indexPrefixe - 1;
	//On ajoute un 1 au poids fort
	return get_bits(b, nbBitsALire) + pow2(nbBitsALire);
}

/*
 * Operation sur des entiers signés
 *
 * Si l'entier est signé, il est précédé d'un bit à 1:negatif et 0:positif
 * On considère que l'entier 0 est positif donc on pourra ajouter
 * 1 aux nombres négatif pour récupérer la place du zero négatif.
 *    2 --> 0 2
 *    1 --> 0 1
 *    0 --> 0 0
 *   -1 --> 1 0
 *   -2 --> 1 1
 *   -3 --> 1 2
 *
 */

void put_entier_signe(struct bitstream *b, int i)
{

	if (i < 0) {
		put_bit(b,1);
		put_entier(b, -i - 1);
		return;
	}
	//i >= 0
	put_bit(b,0);
	put_entier(b, i);
}

/*
 *
 */
int get_entier_signe(struct bitstream *b)
{
	if (!get_bit(b))
		return get_entier(b);
	//else, entier négatif
	return -get_entier(b)-1;
}
