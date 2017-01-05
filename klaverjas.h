#ifndef __klaverjas_h__
#define __klaverjas_h__

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>
#include <limits>

using namespace std;

const static int aantalspelers = 4;
const static int aantalslagen = 8;
const static int aantalkaarten = 8;
const static int aantalrandompotjes = 1000;
const static int maximumdelingen = 1000;
const static double maximumkans = 0.98;
const static float kans = 0.1;
const static bool rotterdams = true;
const static bool metroem = true;
const static bool tienennietweggooien = true;
const static bool semirandom_bijmaat = true;
const static int aantalkolommen = aantalspelers + 4;

enum Kaarten {
  S7 = 0,  S8, SV, SH, S10, SA, S9, SB,
  H7 = 10, H8, HV, HH, H10, HA, H9, HB,
  K7 = 20, K8, KV, KH, K10, KA, K9, KB,
  R7 = 30, R8, RV, RH, R10, RA, R9, RB,
  X = -1
};

enum Kleuren {
  Schoppen,
  Harten,
  Klaver,
  Ruiten
};

ostream& operator<<(ostream& os, const Kaarten kaart);
ostream& operator<<(ostream& os, const Kleuren kleur);

void parseargv(int argc, char* argv[], int spelers[aantalspelers], int &komtuit, 
               bool &experiment, bool &competitite, bool &file, string &filename, int &seed);
bool leesbestand(string filename, int spelers[aantalspelers], int spelerskaarten[aantalspelers][aantalkaarten], 
                 int &troef, int opgegooid[aantalspelers + 1][aantalkolommen], int &slag, int &komtuit);
string naarstring(int input);
int kleurvankaart(int kaart);
bool istroef(int kaart, int opgegooid[aantalslagen + 1][aantalkolommen]);
bool istroef(int kaart, int troefkleur);
bool gedekt(int kaart, int kaarten[]);
int waardeerkaart(int kaart, int troefkleur);
int roemvolgorde(int kaart);
int maat(int speler);
int wiespeelt(int opgegooid[aantalslagen + 1][aantalkolommen]);
void insertionsort(int input[], int lengte);
void printkaarten(int spelerskaarten[aantalspelers][aantalkaarten]);
int winnaar(int kaarten[aantalspelers], int komtuit, int troefkleur);
void printspel(int opgegooid[aantalslagen + 1][aantalkolommen]);
void printarray(int input[], int maxkaart);
int zoekelement(int element, int input[], int arraysize);
void wisselelement(int element, int input[], int arraysize);
void deleteelement(int element, int input[], int arraysize);
void deelkaarten(int spelerskaarten[aantalspelers][aantalkaarten]);
void berekenheeftniet(int opgegooid[aantalslagen + 1][aantalkolommen],
                      int slag, bool heeftniet[4][aantalspelers]);
void berekenheefttroefniet(int opgegooid[aantalslagen + 1][aantalkolommen],
                           int slag, bool heeftniet[aantalspelers][aantalkaarten]);
bool checkdeling(int spelerskaarten[aantalspelers][aantalslagen], bool heeftniet[4][aantalspelers], 
                 bool heefttroefniet[aantalspelers][aantalkaarten], int maxkaart, int troefkleur);
bool checkvierdezelfde(int kaarten[aantalspelers]);
bool checkstuk(int kaarten[aantalspelers], int troefkleur);
int checkroem(int originelekaarten[aantalspelers], int troefkleur);
int geefroem(int kaarten[aantalspelers], int troefkleur, bool output);
int teamroem(int opgegooid[aantalslagen + 1][aantalkolommen], int speler);
int waardeerkaarten(int kaarten[], int maxkaart, int troefkleur, bool output);
int totaalwinnaar(int kaarten[aantalslagen + 1][aantalkolommen], bool percentage);
void speelslag(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalkolommen],
          int spelerskaarten[aantalspelers][aantalkaarten], int slag, int &huidigespeler, 
          const int komtuit, bool output, bool experiment, bool competitite);
int speel(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalkolommen],
          int spelerskaarten[aantalspelers][aantalkaarten], int slag, int huidigespeler, 
          int komtuit, bool output, bool experiment, bool competitite);
void bepaaltroef(int spelerskaarten[aantalspelers][aantalkaarten], int spelers[aantalspelers],
                 int opgegooid[aantalslagen + 1][aantalkolommen], int komtuit, bool output, bool competitite);

#endif