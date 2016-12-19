#include "klaverjas.h"

#ifndef __spelers_h__
#define __spelers_h__

void geefmogelijkheden(int opgegooid[aantalkolommen], int maxkaart, int komtuit,
                       int huidigespeler, int mijnkaarten[aantalkaarten], int troefkleur,
                       int mogelijkekaarten[aantalkaarten], int & aantalmogelijkheden);
int semiramdommove(int kaarten[aantalkaarten], int opgegooid[aantalkolommen],
                   int slag, int komtuit, int huidigespeler, int troefkleur, bool output);
int tactiekmove(int kaarten[aantalkaarten], int opgegooid[aantalslagen + 1][aantalkolommen],
                int slag, int komtuit, int huidigespeler, bool output);
int randommove(int kaarten[aantalkaarten], int opgegooid[aantalkolommen],
               int slag, int komtuit, int huidigespeler, int troefkleur, bool output);
int usermove(int kaarten[aantalkaarten], int slag);

#endif