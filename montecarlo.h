#include "klaverjas.h"

#ifndef __montecarlo_h__
#define __montecarlo_h__


int deelrestkaarten(int opgegooid[aantalslagen + 1][aantalkolommen], int slag, int komtuit, int huidigespeler,
                    int spelerskaarten[aantalspelers][aantalkaarten], bool metkans);
int deelkansverdeling(int opgegooid[aantalslagen + 1][aantalkolommen], int slag, int komtuit, int huidigespeler,
                      int spelerskaarten[aantalspelers][aantalkaarten], double multiplier, double multiplier2);                  
bool checkkansdeling(int aantalgedeeld[aantalspelers - 1], int zoumoetenhebben[aantalspelers - 1]);
void berekenheeftnietmetkans(int opgegooid[aantalslagen + 1][aantalkolommen], int slag,
                             bool heeftniet[4][aantalspelers]);
void berekenkansverdeling(int opgegooid[aantalslagen + 1][aantalkolommen], int slag, int komtuit,
                          double kansverdeling[aantalspelers - 1][4], int huidigespeler,
                          double multiplier, int totaalvankleur[4]);
void berekentroefverdeling(int opgegooid[aantalslagen + 1][aantalkolommen], int slag, int komtuit, 
                           double troefverdeling[aantalspelers - 1][aantalkaarten], int huidigespeler,
                           double kansverdeling[aantalspelers - 1][4], double multiplier, int totaaltroeven);
int montecarlokansmove(int kaarten[aantalkaarten], int opgegooid[aantalslagen + 1][aantalkolommen],
                       int slag, int komtuit, int huidigespeler, int niveaurandom, bool output);
int montecarlomove(int kaarten[aantalkaarten], int opgegooid[aantalslagen + 1][aantalkolommen],
                   int slag, int komtuit, int huidigespeler, int niveaurandom, bool output, bool metkans, float kans, bool experiment);


#endif