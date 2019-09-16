#include "bitstream.h"
#include "bits.h"

/*
 * On écrit les "nb" bits de droite de "v"
 * dans le fichier (toujours du poids fort au faible).
 *
 * Pour v=11 nb=8 on va écrire les bits : 00001011 dans le fichier
 */

void put_bits(struct bitstream *b, unsigned int nb, unsigned long v)
{
	Booleen bit;
	while(nb) {
		nb--;
		//bit = (v >> nb) & 1;
		bit = prend_bit(v, nb);
		put_bit(b, bit);
	}
}


/*
 * Lecture de "nb" bits venant du fichier.
 * et on retourne un entier contenant ces bits cadrés à droite (poids faibles)
 * Par exemple pour nb=2 on peut retourner des valeurs de 0 à 3 inclu.
 * Suivant les 2 bits dans le fichier on obtiendra :
 * 00->0 01->1 10->2 11->3
 */

unsigned int get_bits(struct bitstream *b, unsigned int nb)
{
	unsigned int res = 0;
	Booleen bit;
	while(nb) {
		nb--;
		bit = get_bit(b);
		res += bit * pow2(nb);
	}
	return res;
}

/*
 * Pour vous simplifier la programmation.
 * Cette fonction stocke une chaine de la forme "0011010101010111010101001"
 * dans le flot de bit sous la forme d'une suite de bit 0 et 1.
 *
 * Comme d'habitude le caractère '0' c'est Faux les autres sont vrai
 */

void put_bit_string(struct bitstream *b, const char *bits)
{
	Booleen bit;
	for(const char* t = bits; *t != '\0'; t++) {
		// if (*t == '0') bit = Faux;
		// else bit = Vrai;
		bit = *t != '0';
		put_bit(b, bit);
	}

}
