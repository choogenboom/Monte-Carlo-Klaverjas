#include "klaverjas.h"
#include "montecarlo.h"

#ifndef __speelpas_h__
#define __speelpas_h__

int speelpasrondje(int spelerskaarten[aantalspelers][aantalkaarten], int spelers[aantalspelers], int kleur, int komtuit);

bool montecarlospeelt(int kaarten[aantalkaarten], int komtuit, int troefkleur);
bool puntenspeelt(int kaarten[aantalkaarten], int troefkleur);
bool troefspeelt(int kaarten[aantalkaarten], int troefkleur);
bool userspeelt(int kaarten[aantalkaarten], int kleur);

#endif