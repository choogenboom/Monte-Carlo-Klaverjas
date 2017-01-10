#include "competitie.h"

void schrijfenters(int speler, int aantalenters) {
  string filename = "Speler" + naarstring(speler) + ".spl";
  ofstream file(filename.c_str(), ios::out | ios::app);

  for (int i = 0; i < aantalenters; i++)
    file << endl;
  file.close();
}

bool schrijfbestand(int speler, int opgegooid[aantalslagen + 1][aantalkolommen], int vorigeslag[aantalspelers], 
                    int kaarten[aantalkaarten], int slag, string message) {
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];
  int komtuit = 0;
  int winnaarvorigeslag = -1;

  for (int i = 0; i < aantalspelers; i++) {
    komtuit = (opgegooid[slag][aantalspelers] + i) % aantalspelers;
    if (opgegooid[slag][komtuit] == -1)
      break;
  }

  if (slag > 0)
    winnaarvorigeslag = opgegooid[slag - 1][aantalspelers + 1];

  schrijfenters(speler, 30);
  string filename = "Speler" + naarstring(speler) + ".spl";

  ofstream file(filename.c_str(), ios::out | ios::app);

  if (file.is_open()) {
    file << "Speler " << naarstring(speler) << endl << "--------" << endl;
    if (!opgegooid[aantalslagen][aantalspelers + 2]) {
      file << Kleuren(troefkleur) << " is troef, " << opgegooid[aantalslagen][aantalspelers] << " speelt." 
           << endl << endl << endl << endl << endl;
    }
    else {
      file << Kleuren(troefkleur) << " is troef, " << opgegooid[aantalslagen][aantalspelers] << " speelt (verplicht)." 
           << endl << endl << endl << endl << endl;
    }

    file << "Opgegooid:" << endl << "----------" << endl;
    for (int i = 0; i < aantalspelers; i++)
      file << Kaarten(opgegooid[slag][i]) << " ";
    file << endl;

    // Pijltje laten zien bij de huidige speler
    for (int i = 0; i < 5 * komtuit; i++)
      file << " ";
    file << "^" << endl;

    file << endl << endl << endl;

    file << "Vorige slag:" << endl << "-------------" << endl;
    for (int i = 0; i < aantalspelers; i++)
      file << Kaarten(vorigeslag[i]) << " ";
    
    file << endl;
    if (winnaarvorigeslag != -1) {
      for (int i = 0; i < 5 * winnaarvorigeslag; i++)
        file << " ";
      file << "^" << endl;
    }

    file << endl << endl << endl << endl;

    file << "Je kaarten:" << endl << "-------------" << endl;
    for (int i = 0; i < aantalkaarten; i++)
      file << Kaarten(kaarten[i]) << " ";

    file << endl << endl;

    file << message << endl;

    file.close();

    return true;
  }
  else
    return false;
}

void schrijfbestanden(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalkolommen],
                      int spelerskaarten[aantalspelers][aantalkaarten], int slag, string bericht) {
  int legeslag[aantalspelers] = {-1, -1, -1, -1};
  int aandebeurt = -1;
  
  for (int i = 0; i < aantalspelers; i++) {
    aandebeurt = (opgegooid[slag][aantalspelers] + i) % aantalspelers;
    if (opgegooid[slag][aandebeurt] == -1)
      break;
  }

  for (int i = 0; i < aantalspelers; i++) {
    if (slag == 0)
      schrijfbestand(i, opgegooid, legeslag, spelerskaarten[i], slag, "");
    else
      schrijfbestand(i, opgegooid, opgegooid[slag - 1], spelerskaarten[i], slag, "");
    
    if (i != aandebeurt)
      appendbestand(i, "Speler " + naarstring(aandebeurt) + " is aan de beurt.", 0);
    else
      appendbestand(i, "Je bent aan de beurt.", 0);

    appendbestand(i, bericht, 2);
  }
}

bool appendbestand(int speler, string bericht, int entersvooraf) {
    // schrijfenters(speler, entersvooraf);
  string filename = "Speler" + naarstring(speler) + ".spl";

  ofstream file(filename.c_str(), ios::out | ios::app);

  if (file.is_open()) {
    for (int i = 0; i < entersvooraf; i++)
      file << endl;

    file << bericht << endl;

    file.close();
    return true;
  }
  else
    return false;
}

int speelcompetitie(int spelers[aantalspelers], int komtuit) {
  int handjes = 16;
  int opgegooid[aantalslagen + 1][aantalkolommen];
  // Per handje: punten voor team 1 en team 2
  int scoreformulier[handjes][2];
  int spelerskaarten[aantalspelers][aantalkaarten];
  bool output = true;
  int slag;
  int huidigespeler;
  int troefkleur;

  for (int i = 0; i < handjes; i++) {
    for (int j = 0; j < 2; j++) {
      scoreformulier[i][j] = 0;
    }
  }

  string scorefilename = naarstring(spelers[0]) + naarstring(spelers[1]) + 
                         naarstring(spelers[2]) + naarstring(spelers[3]) +  ".txt";
  ofstream scorefile(scorefilename.c_str(), ios::out | ios::app);
  if (scorefile.is_open()) {
    scorefile << scorefilename << endl << endl;
    scorefile.close();
    scorefile.clear();
  }

  for (int handje = 0; handje < handjes; handje++) {

    // Initieer opgegooid
    for (int i = 0; i < aantalslagen + 1; i++)
      for (int j = 0; j < aantalkolommen; j++)
      opgegooid[i][j] = -1;
    // En initieer spelerskaarten
    for (int i = 0; i < aantalspelers; i++)
      for (int j = 0; j < aantalkaarten; j++)
        spelerskaarten[i][j] = -1;

    deelkaarten(spelerskaarten);
    bepaaltroef(spelerskaarten, spelers, opgegooid, komtuit, output, true);
    troefkleur = opgegooid[aantalslagen][aantalspelers + 1];
    opgegooid[0][aantalspelers] = komtuit;

    slag = 0;
    huidigespeler = komtuit;
    for (int i = 0; i < aantalspelers; i++) {
      schrijfbestanden(spelers, opgegooid, spelerskaarten, slag, "");
      schrijfenters(i, 200);
    }
    while (slag < aantalslagen) {
      speelslag(spelers, opgegooid, spelerskaarten, slag, huidigespeler, komtuit, output, false, true);

      komtuit = winnaar(opgegooid[slag], komtuit, troefkleur);
      opgegooid[slag][aantalspelers + 1] = komtuit;
      opgegooid[slag][aantalspelers + 2] = waardeerkaarten(opgegooid[slag], aantalspelers, troefkleur, output);
      opgegooid[slag][aantalspelers + 3] = geefroem(opgegooid[slag], troefkleur, output);

      if (slag < aantalslagen - 1)
        opgegooid[slag + 1][aantalspelers] = komtuit;

      // 10 punten extra voor de laatste slag
      if (slag == aantalslagen - 1)
        opgegooid[slag][aantalspelers + 2] = opgegooid[slag][aantalspelers + 2] + 10;

      printkaarten(spelerskaarten);
      printspel(opgegooid);

      // Schrijf resultaten naar de bestanden...
      string punten = naarstring(opgegooid[slag][aantalspelers + 2]);

      string roem = naarstring(opgegooid[slag][aantalspelers + 3]);
      
      string appendstr = "Slag gewonnen door speler " + naarstring(komtuit) + " met " + punten + " punten en " + roem + " roem.";
      schrijfbestanden(spelers, opgegooid, spelerskaarten, slag + 1, appendstr);

      cout << "Druk enter om door te gaan.";
      cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
      cin.ignore(std::numeric_limits<streamsize>::max(), '\n');

      // En klaarmaken voor de volgende slag
      huidigespeler = komtuit;
      slag++;
    }
    
    string resultaat = "";
    if (totaalwinnaar(opgegooid, false) == 0) {
      resultaat = "Team 0 en 2 hebben gewonnen met " + naarstring(opgegooid[aantalslagen][0]) + " punten, inclusief roem.\n" + 
                  "Team 1 en 3 hadden er " + naarstring(opgegooid[aantalslagen][1]);
    }
    else {
      resultaat = "Team 1 en 3 hebben gewonnen met " + naarstring(opgegooid[aantalslagen][1]) + " punten, inclusief roem.\n" + 
                  "Team 0 en 2 hadden er " + naarstring(opgegooid[aantalslagen][0]);
    }

    scoreformulier[handje][0] = opgegooid[aantalslagen][0];
    scoreformulier[handje][1] = opgegooid[aantalslagen][1];
    
    scorefile.open(scorefilename.c_str(), std::ios_base::app);

    if (scorefile.is_open()) {
      scorefile << handje << ": " << opgegooid[aantalslagen][0] << " " << opgegooid[aantalslagen][1] << endl;
      scorefile.close();
      scorefile.clear();
    }

    for (int i = 0; i < aantalspelers; i++)
      appendbestand(i, resultaat, 200);

    cout << "Druk enter om door te gaan.";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }

  cout << endl << endl << "Boompje voorbij! Scores: " << endl;
  int totaalteam1 = 0;
  int totaalteam2 = 0;
  for (int i = 0; i < handjes; i++) {
    totaalteam1 += scoreformulier[i][0];
    totaalteam2 += scoreformulier[i][1];
    cout << i << ": " << scoreformulier[i][0] << " " << scoreformulier[i][1] << endl;
  }
  cout << "---------- +" << endl << totaalteam1 << "  " << totaalteam2 << endl;
  return 0;
}