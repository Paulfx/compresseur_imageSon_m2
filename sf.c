/*
 * Le but du shannon-fano dynamique est de ne pas transmettre la table
 * des occurrences. Pour ce faire, on ajoute à la table un symbole ESCAPE
 * qui permet l'ajout d'éléments à la table.
 * Le décompresseur sait qu'après un événement ESCAPE il trouvera
 * la valeur (et non le code) d'un événement à ajouter à la table.
 */


#include "bits.h"
#include "sf.h"

#define VALEUR_ESCAPE 0x7fffffff /* Plus grand entier positif */

struct evenement
 {
  int valeur ;
  int nb_occurrences ;
 } ;

struct shannon_fano
 {
  int nb_evenements ;
  struct evenement evenements[200000] ;
 } ;

/*
 * Allocation des la structure et remplissage des champs pour initialiser
 * le tableau des événements avec l'événement ESCAPE (avec une occurrence).
 */
struct shannon_fano* open_shannon_fano()
{
  struct shannon_fano *sf;
  ALLOUER(sf, 1);

  struct evenement e = { VALEUR_ESCAPE, 1};
  sf->nb_evenements = 1;
  sf->evenements[0] = e;
  return sf;
}

/*
 * Fermeture (libération mémoire)
 */
void close_shannon_fano(struct shannon_fano *sf)
{
  free(sf);
}

/*
 * En entrée l'événement (sa valeur, pas son code shannon-fano).
 * En sortie la position de l'événement dans le tableau "evenements"
 * Si l'événement n'est pas trouvé, on retourne la position
 * de l'événement ESCAPE.
 */

static int trouve_position(const struct shannon_fano *sf, int evenement)
{
  for (int i=0; i<sf->nb_evenements; ++i) {
    if (sf->evenements[i].valeur == evenement)
      return i;
  }
  return trouve_position(sf, VALEUR_ESCAPE);
}

/*
 * Soit le sous-tableau "evenements[position_min..position_max]"
 * Les bornes sont incluses.
 *
 * LES FORTES OCCURRENCES SONT LES PETITS INDICES DU TABLEAU
 *
 * Il faut trouver la séparation.
 * La séparation sera codée comme l'indice le plus fort
 * des fortes occurrences.
 *
 * La séparation minimise la valeur absolue de la différence
 * de la somme des occurrences supérieures et inférieures.
 *
 * L'algorithme (trivial) n'est pas facile à trouver, réfléchissez bien.
 */
static int trouve_separation(const struct shannon_fano *sf
			     , int position_min
			     , int position_max)
{
  // int nbEvent = position_max - position_min + 1;
  // int accumulateSum[nbEvent];
  // int sum = 0;

  // int i;
  // for (i=0; i<nbEvent; ++i) {
  //   sum += sf->evenements[position_min + i].nb_occurrences;
  //   accumulateSum[i] = sum;
  // }
   
  // int middle = accumulateSum[nbEvent-1] / 2;
    
  // for (i=0; i<nbEvent; ++i) {
  //   if (accumulateSum[i] >= middle)
  //     return position_min+i;
  // }

  //OU
  int sum_min = sf->evenements[position_min].nb_occurrences;    
  int sum_max = sf->evenements[position_max].nb_occurrences;
  while(position_min != position_max - 1) {
    if (sum_max < sum_min) {
      position_max -= 1;
      sum_max += sf->evenements[position_max].nb_occurrences;
    }
    else {
      position_min += 1;
      sum_min += sf->evenements[position_min].nb_occurrences;
    }
  }
  return position_min;

}

/*
 * Cette fonction (simplement itérative)
 * utilise "trouve_separation" pour générer les bons bit dans "bs"
 * le code de l'événement "sf->evenements[position]".
 */

static void encode_position(struct bitstream *bs,struct shannon_fano *sf,
		     int position)
{

  int position_min = 0;
  int position_max = sf->nb_evenements-1;
  int sep;
  while (position_min != position_max) {
    sep = trouve_separation(sf, position_min, position_max);
    if (position > sep) {
      position_min = sep+1;
      put_bit(bs, 1);
    }
    else {
      position_max = sep;
      put_bit(bs, 0);
    }
  }
}

/*
 * Cette fonction incrémente le nombre d'occurrence de
 * "sf->evenements[position]"
 * Puis elle modifie le tableau pour qu'il reste trié par nombre
 * d'occurrence (un simple échange d'événement suffit)
 *
 * Les faibles indices correspondent aux grand nombres d'occurrences
 */

static void incremente_et_ordonne(struct shannon_fano *sf, int position)
{
  sf->evenements[position].nb_occurrences++;
  if (position == 0) 
    return;

  for (int i=0; i<position; ++i) {
    //Le premier event qu'on trouve avec un nb occurence inférieur ou égal au nbOcc
    //Est échangé avec l'event actuel
    if (sf->evenements[i].nb_occurrences < sf->evenements[position].nb_occurrences) {
      struct evenement tmp = sf->evenements[position];
      sf->evenements[position] = sf->evenements[i];
      sf->evenements[i] = tmp; 
      return;
    }
  }
}

/*
 * Cette fonction trouve la position de l'événement puis l'encode.
 * Si la position envoyée est celle de ESCAPE, elle fait un "put_bits"
 * de "evenement" pour envoyer le code du nouvel l'événement.
 * Elle termine en appelant "incremente_et_ordonne" pour l'événement envoyé.
 */
void put_entier_shannon_fano(struct bitstream *bs
			     ,struct shannon_fano *sf, int evenement)
{ 
  int position = trouve_position(sf, evenement);
  encode_position(bs, sf, position);

  if (sf->evenements[position].valeur == VALEUR_ESCAPE) {
    //position ESCAPE, event pas encore dans la table

    put_bits(bs, sizeof(int) * 8, evenement);

    //On ajoute l'event à sf
    struct evenement s_evenement = { evenement, 1 };
    sf->evenements[sf->nb_evenements++] = s_evenement;

  }
  //On incrémente après car l'event peut changer de position
  incremente_et_ordonne(sf, position);
  
}


/*
 * Fonction inverse de "encode_position"
 */
static int decode_position(struct bitstream *bs,struct shannon_fano *sf)
{
  int position_min = 0;
  int position_max = sf->nb_evenements-1;
  int sep;
  while(position_min != position_max) {
    sep = trouve_separation(sf, position_min, position_max);
  
    if (get_bit(bs)) {
      //position > sep
      position_min = sep+1;
    }
    else {
      //position <= sep
      position_max = sep;
    }
  }

  return position_min;

}

/*
 * Fonction inverse de "put_entier_shannon_fano"
 *
 * Attention au piège : "incremente_et_ordonne" change le tableau
 * donc l'événement trouvé peut changer de position.
 */
int get_entier_shannon_fano(struct bitstream *bs, struct shannon_fano *sf)
{
  int position = decode_position(bs, sf);
  //On se souvient de l'évenement car il peut changer de position
  int evenement = sf->evenements[position].valeur;
  incremente_et_ordonne(sf, position);

  if (evenement == VALEUR_ESCAPE) {
    //L'évenement est l'entier en valeur
    evenement = get_bits(bs, sizeof(int) * 8);

    //On doit l'ajouter à la table
    struct evenement e = {evenement, 1};
    sf->evenements[sf->nb_evenements++] = e;
  }

  return evenement;
}

/*
 * Fonctions pour les tests, NE PAS MODIFIER, NE PAS UTILISER.
 */
int sf_get_nb_evenements(struct shannon_fano *sf)
 {
   return sf->nb_evenements ;
 }
void sf_get_evenement(struct shannon_fano *sf, int i, int *valeur, int *nb_occ)
 {
   *valeur = sf->evenements[i].valeur ;
   *nb_occ = sf->evenements[i].nb_occurrences ;
 }
int sf_table_ok(const struct shannon_fano *sf)
 {
  int i, escape ;

  escape = 0 ;
  for(i=0;i<sf->nb_evenements;i++)
    {
    if ( i != 0
        && sf->evenements[i-1].nb_occurrences<sf->evenements[i].nb_occurrences)
	{
	   fprintf(stderr, "La table des événements n'est pas triée\n") ;
	   return(0) ;
	}
    if ( sf->evenements[i].valeur == VALEUR_ESCAPE )
	escape = 1 ;
    }
 if ( escape == 0 )
	{
	   fprintf(stderr, "Pas de ESCAPE dans la table !\n") ;
	   return(0) ;
	}
 return 1 ;
 }
