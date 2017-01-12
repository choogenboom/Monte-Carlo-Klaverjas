#include "klaverjas.h"

#ifndef __competitie_h__
#define __competite_h__

const int aantalrandompotjes = 1000;

void schrijfenters(int speler, int aantalenters);
bool schrijfbestand(int speler, int opgegooid[aantalslagen + 1][aantalkolommen], int vorigeslag[aantalspelers], 
                    int kaarten[aantalkaarten], int slag, string message);
void schrijfbestanden(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalkolommen],
                      int spelerskaarten[aantalspelers][aantalkaarten], int slag, string message);
bool appendbestand(int speler, string message, int entersvooraf);
int speelcompetitie(int spelers[aantalspelers], int komtuit);

#endif