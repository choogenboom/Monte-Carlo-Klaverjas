#include "speelpas.h"
#include "competitie.h"

// Speelt op basis van 1000 random potjes met deze hand
bool montecarlospeelt(int kaarten[aantalkaarten], int komtuit, int troefkleur) {
  int niveaurandom = 4;
  int spelers[aantalspelers] = {niveaurandom, niveaurandom, niveaurandom, niveaurandom};
  int opgegooid[aantalslagen + 1][aantalkolommen];
  int spelerskaarten[aantalspelers][aantalkaarten];
  int wij = 0;
  int zij = 0;

  for (int k = 0; k < aantalrandompotjes; k++) {
    // Initieer een lege opgegooid
    for (int i = 0; i < aantalslagen + 1; i++)
      for (int j = 0; j < aantalkolommen; j++)
        opgegooid[i][j] = -1;

    // Eigen kaarten zetten we in speler 0
    for (int i = 0; i < aantalkaarten; i++)
      spelerskaarten[0][i] = kaarten[i];

    opgegooid[aantalslagen][aantalspelers] = 0;
    opgegooid[aantalslagen][aantalspelers + 1] = troefkleur;
    opgegooid[aantalslagen][aantalspelers + 2] = 0;
    // De rest van de kaarten worden willekeurig verdeeld
    deelrestkaarten(opgegooid, 0, komtuit, 0, spelerskaarten, false);

    speel(spelers, opgegooid, spelerskaarten, 0, komtuit, komtuit, false, false, false);
    wij += opgegooid[aantalslagen][0];
    zij += opgegooid[aantalslagen][1];
  }

  // cout << "Punten: " << wij << " op " << Kleuren(troefkleur) << endl;
  return (wij > zij);
}

/* Speelt op basis van een aangepast handwaarderingssysteem voor Bridge.
 * - A krijgt 3 punten
 * - 10 krijgt 2 punten
 * - H krijgt 1 punt
 * - Troefboer krijgt 5 punten
 * - Troefnel krijgt 4 punten
 * - Elke andere troef krijgt 1 punt
 *
 * Als dit in toaal meer dan de helft van alle mogelijke punten (39) zijn spelen we.
 */
bool puntenspeelt(int kaarten[aantalkaarten], int troefkleur) {
  int punten = 0;

  for (int i = 0; i < aantalkaarten; i++) {
    int kaart = kaarten[i] - (10 * kleurvankaart(kaarten[i]));

    if (kaart == 5)
      punten += 3;
    else if (kaart == 4 && gedekt(kaarten[i], kaarten))
      punten += 2;
    else if (kaart == 3)
      punten += 1;

    if (istroef(kaarten[i], troefkleur)) {
      if (kaart == 7)
        punten += 5;
      else if (kaart == 6)
        punten += 4;
      else
        punten += 1;
    }
  }
  // cout << punten << endl;
  return (punten > 10);
}

// Speelt op basis van aantal troeven
bool troefspeelt(int kaarten[aantalkaarten], int troefkleur) {
  int troeven = 0;
  int azen = 0;
  bool boer = false;
  bool nel = false;

  for (int i = 0; i < aantalkaarten; i++) {
    int kaart = kaarten[i] - (10 * kleurvankaart(kaarten[i]));
    if (istroef(kaarten[i], troefkleur)) {
      troeven++;
      if (kaart == 7)
        boer = true;
      else if (kaart == 6)
        nel = true;
    }

    if (kaart == 5)
      azen++;
  }

  if (boer && nel)
    return true;
  else if (boer) {
    // Alleen de boer, niet de nel
    if (troeven > 32)
      return true;
    else {
      // Boer + 1 extra troef of alleen boer
      if (azen >= 2)
        return true;
    }
    // B A . .?
  }
  else if (nel) {
    // Alleen de nel, niet de boer
    // Met kale nel spelen we sowieso niet
    if (troeven == 1)
      return false;
    if (troeven > 3)
      return true;
    else {
      // Nel en max 2 troeven ernaast
      if (azen >= 2)
        return true;
    }
  }

  return false;
}

bool userspeelt(int kaarten[aantalkaarten], int kleur) {
  char input = 'N';
  cout << "Kaarten: " << endl;

  for (int i = 0; i < aantalkaarten; i++) {
    cout << Kaarten(kaarten[i]);
  }

  cout << endl << "Voer in of je wilt spelen [J] of passen [N] op " << Kleuren(kleur) << ": ";
  cin >> input;

  if (input == 'Y' || input == 'y' || input == 'J' || input == 'j')
    return true;

  else
    return false;
}

int speelpasrondje(int spelerskaarten[aantalspelers][aantalkaarten], int spelers[aantalspelers], int kleur, int komtuit, bool competitie) {
  int speelt = -1;

  for (int i = 0; i < aantalspelers; i++) {
    int maghetzeggen = (komtuit + i) % aantalspelers;

    if (spelers[maghetzeggen] == 0) {
      // Voor het overzicht worden de kaarten van mensen gesorteerd
      cout << maghetzeggen << ": " << endl;

      if (competitie) {
        for (int i = 0; i < aantalspelers; i++) {
          insertionsort(spelerskaarten[i], aantalkaarten);
          stringstream ss;
          ss << "Je kaarten:" << endl;
          
          for (int j = 0; j < aantalkaarten; j++)
            ss << Kaarten(spelerskaarten[i][j]);

          ss << endl << endl;
          if (i == maghetzeggen)
            ss << "Geef aan of je wilt spelen op " << Kleuren(kleur) << "." << endl;
          else
            ss << Kleuren(kleur) << "is gedraaid. " << maghetzeggen << " mag het zeggen." << endl;
          appendbestand(i, ss.str(), 200);
        }
      }

      if (userspeelt(spelerskaarten[maghetzeggen], kleur))
        speelt = maghetzeggen;
    }
    else if ((spelers[maghetzeggen] >= 1 && spelers[maghetzeggen] < 4) || spelers[maghetzeggen] == 5) {
      if (montecarlospeelt(spelerskaarten[maghetzeggen], komtuit, kleur))
        speelt = maghetzeggen;
    }
    else {
      // if (troefspeelt(spelerskaarten[maghetzeggen]))
      if (puntenspeelt(spelerskaarten[maghetzeggen], kleur))
        speelt = maghetzeggen;
    }

    if (speelt != -1)
      break;
  }

  return speelt;
}