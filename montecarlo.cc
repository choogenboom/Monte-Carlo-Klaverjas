#include "montecarlo.h"
#include "spelers.h"

int deelrestkaarten(int opgegooid[aantalslagen + 1][aantalkolommen], int slag, int komtuit, int huidigespeler,
                    int spelerskaarten[aantalspelers][aantalkaarten], bool metkans) 
{
  int allekaarten[aantalkaarten * aantalspelers];
  int maxkaart = aantalkaarten * aantalspelers;
  int aantalgedelete = 0;
  bool foutedeling = false;
  bool heeftniet[4][aantalspelers];
  bool heefttroefniet[aantalspelers][aantalkaarten];
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];

  // Initieer alle kaarten
  for (int i = 0; i < aantalkaarten * aantalspelers ; i++)
    allekaarten[i] = (10 * floor(i / aantalkaarten) + i % aantalkaarten);

  // Delete kaarten die al opgegooid zijn uit allekaarten
  //                  // +1 omdat we ook kaarten halverwege de slag meenemen
  for (int i = 0; i < (slag + 1); i++) {
    for (int j = 0; j < aantalspelers; j++) {
      int k = (komtuit + j) % 4;
      if (opgegooid[i][k] != -1) {
        int index = (aantalkaarten * floor(opgegooid[i][k] / 10) + opgegooid[i][k] % 10);

        if (allekaarten[index] != opgegooid[i][k])
          index = zoekelement(opgegooid[i][k], allekaarten, maxkaart - aantalgedelete);

        allekaarten[index] = -1;
        aantalgedelete++;
        wisselelement(index, allekaarten, maxkaart - (aantalgedelete));
      }
    }
  }

  // Delete eigen overige kaarten
  for (int i = 0; i < aantalkaarten - slag; i++) {
    int index = (aantalkaarten * floor(spelerskaarten[huidigespeler][i] / 10) + spelerskaarten[huidigespeler][i] % 10);

    if (allekaarten[index] != spelerskaarten[huidigespeler][i])
      index = zoekelement(spelerskaarten[huidigespeler][i], allekaarten, maxkaart - aantalgedelete);

    allekaarten[index] = -1;
    aantalgedelete++;
    wisselelement(index, allekaarten, maxkaart - aantalgedelete);
  }

  maxkaart -= aantalgedelete;

  if (metkans)
    berekenheeftnietmetkans(opgegooid, slag, heeftniet);
  else
    berekenheeftniet(opgegooid, slag, heeftniet);

  berekenheefttroefniet(opgegooid, slag, heefttroefniet);

  int aantalgedeeld[aantalspelers] = {0, 0, 0, 0};
  // Hierna verdelen we alle kaarten over de spelers:
  // Eerst verdelen we de kaarten die een bepaalde speler moet hebben
  for (int i = 0; i < 4; i++) {
    int aantalheeftniet = 0;
    int heeftwel = -1;

    // Check hoeveel spelers een kleur niet hebben, exclusief jezelf
    for (int j = 0; j < aantalspelers; j++)
      if (j != huidigespeler && heeftniet[i][j] == true)
        aantalheeftniet++;
      else if (j != huidigespeler)
        heeftwel = j;

    // Als 2 spelers een kleur niet hebben zit alles bij de 3e
    if (aantalheeftniet == 2) {
      for (int k = 0; k < maxkaart; k++) {
        if (kleurvankaart(allekaarten[k]) == i) {
          spelerskaarten[heeftwel][aantalgedeeld[heeftwel]] = allekaarten[k];
          allekaarten[k] = -1;
          wisselelement(k, allekaarten, maxkaart - 1);
          maxkaart--;
          aantalgedeeld[heeftwel]++;
          // Degene waarmee gewisseld is kan ook die kleur zijn
          k--;
        }
      }
    }
  }

  int maxorig = maxkaart;
  int aantaldelingen = 0;

  // Daarna delen we de rest van de kaarten
  bool goededeling = false;
  while (!goededeling) {
    bool debug = false;
    if (debug) {
      if (metkans)
        berekenheeftnietmetkans(opgegooid, slag, heeftniet);
      else
        berekenheeftniet(opgegooid, slag, heeftniet);

      berekenheefttroefniet(opgegooid, slag, heefttroefniet);
    }

    maxkaart = maxorig;

    for (int i = 1; i <= aantalspelers - 2; i++) {
      for (int j = aantalgedeeld[(huidigespeler + i) % 4]; j < aantalkaarten - slag - 1; j++) {
          // Door de kansdelingen kan maxkaart 0 worden, en komt er een FPE
          if (maxkaart > 0) {
            int randomkaart = rand() % (maxkaart);
            spelerskaarten[(huidigespeler + i) % 4][j] = allekaarten[randomkaart];
            wisselelement(randomkaart, allekaarten, maxkaart - 1);
            maxkaart--;
          }
          else {
            foutedeling = true;
            break;
          }
      }
    }

    // Deel speler links van huidigespeler (+3 %4) de rest van de kaarten
    for (int i = aantalgedeeld[(huidigespeler + 3) % 4]; i < aantalkaarten - slag - 1; i++) {
      // Door de kansdelingen kan maxkaart 0 worden, en komt er een FPE
      if (maxkaart > 0) {
        int randomkaart = rand() % (aantalkaarten - slag - i);

        spelerskaarten[(huidigespeler + 3) % 4][i] = allekaarten[randomkaart];
        wisselelement(randomkaart, allekaarten, maxkaart - 1);
        maxkaart--;
      }
      else {
        foutedeling = true;
        break;
      }
    }

    // Overige kaarten verdelen over spelers die nog niet opgegooid hebben
    int i = 0;
    while (maxkaart > 0) {
      if (opgegooid[slag][i] == -1 && huidigespeler != i && aantalgedeeld[i] != (aantalkaarten - slag)) {
        int randomkaart = rand() % (maxkaart);
        spelerskaarten[i][aantalkaarten - slag - 1] = allekaarten[randomkaart];
        wisselelement(randomkaart, allekaarten, maxkaart - 1);
        maxkaart--;
      }
      i++;
    }

    goededeling = checkdeling(spelerskaarten, heeftniet, heefttroefniet, aantalkaarten - slag, troefkleur);
    aantaldelingen++;

    // Als we in een onmogelijke deling zitten door de kans, ga terug naar normale deling
    if ((aantaldelingen > maximumdelingen && metkans) || foutedeling) {
      // if (output)
      //   cout << "Onmogelijke deling..." << endl;
      // Maak spelerskaarten weer leeg...
      for (int k = 0; k < aantalspelers; k++) {
        if (k != huidigespeler) {
          for (int l = 0; l < aantalkaarten; l++) {
            spelerskaarten[k][l] = -1;
          }
        }
      }

      // En roep de functie aan zonder kans
      return deelrestkaarten(opgegooid, slag, komtuit, huidigespeler, spelerskaarten, false);
    } 
  }

  return aantaldelingen;
}

/* Verdeelt de rest van de kaarten op basis van een kansverdeling in de volgende format:
 * 
 *   |  S  H  K  R
 * --|-------------
 * 0 |  x 
 * 1 |  y
 * 2 |  x
 *
 * Waarin horizontaal de kleuren staan en verticaal de spelers, exclusief de huidige speler.
 * x, y en z zijn kansen op een kleur, waarvan de som gelijk is aan het aantal kaarten van die 
 * kleur nog te verdelen.
*/
int deelkansverdeling(int opgegooid[aantalslagen + 1][aantalkolommen], int slag, int komtuit, int huidigespeler,
                      int spelerskaarten[aantalspelers][aantalkaarten], double multiplier, double multiplier2) 
{
  int allekaarten[aantalkaarten * aantalspelers];
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];
  int maxkaart = aantalkaarten * aantalspelers;
  int maxorig = 0;
  int delingen = 0;
  int herverdeling[aantalspelers - 1][aantalkaarten * 2];
  // int zoumoetenhebben[aantalspelers - 1];
  int aantalgedeeld[aantalspelers - 1] = {0, 0, 0};
  int totaalvankleur[4] = {0, 0, 0, 0};
  int aantalgedelete = 0;
  double kansverdeling[aantalspelers - 1][4];
  double troefverdeling[aantalspelers - 1][aantalkaarten];

  // Initieer alle kaarten
  for (int i = 0; i < aantalkaarten * aantalspelers ; i++)
    allekaarten[i] = (10 * floor(i / aantalkaarten) + i % aantalkaarten);

  // Initieer herverdeling
  for (int i = 0; i < aantalspelers - 1; i++)
    for (int j = 0; j < aantalkaarten * 2; j++)
      herverdeling[i][j] = -1;

  // Delete kaarten die al opgegooid zijn uit allekaarten
  for (int i = 0; i < (slag + 1); i++) {
    for (int j = 0; j < aantalspelers; j++) {
      int k = (komtuit + j) % 4;
      if (opgegooid[i][k] != -1) {
        int index = (aantalkaarten * floor(opgegooid[i][k] / 10) + opgegooid[i][k] % 10);

        if (allekaarten[index] != opgegooid[i][k])
          index = zoekelement(opgegooid[i][k], allekaarten, maxkaart - aantalgedelete);

        allekaarten[index] = -1;
        aantalgedelete++;
        wisselelement(index, allekaarten, maxkaart - (aantalgedelete));
      }
    }
  }

  // Delete eigen overige kaarten
  for (int i = 0; i < aantalkaarten - slag; i++) {
    int index = (aantalkaarten * floor(spelerskaarten[huidigespeler][i] / 10) + spelerskaarten[huidigespeler][i] % 10);

    if (allekaarten[index] != spelerskaarten[huidigespeler][i])
      index = zoekelement(spelerskaarten[huidigespeler][i], allekaarten, maxkaart - aantalgedelete);

    allekaarten[index] = -1;
    aantalgedelete++;
    wisselelement(index, allekaarten, maxkaart - aantalgedelete);
  }

  maxkaart -= aantalgedelete;

  // Initieer totaalvankleur
  for (int i = 0; i < maxkaart; i++) {
    if (kleurvankaart(allekaarten[i]) == 0)
      totaalvankleur[0]++;
    else if (kleurvankaart(allekaarten[i]) == 1)
      totaalvankleur[1]++;
    else if (kleurvankaart(allekaarten[i]) == 2)
      totaalvankleur[2]++;
    else if (kleurvankaart(allekaarten[i]) == 3)
      totaalvankleur[3]++;
  }

  // Maak kansverdeling
  berekenkansverdeling(opgegooid, slag, komtuit, kansverdeling, huidigespeler, multiplier, totaalvankleur);
  berekentroefverdeling(opgegooid, slag, komtuit, troefverdeling, huidigespeler, kansverdeling, multiplier2, totaalvankleur[troefkleur]);

  if (slag == 6)
    cout << " ";

  // Initieer zoumoetenhebben
  int zoumoetenhebben[aantalspelers - 1] = {aantalkaarten - slag, aantalkaarten - slag, aantalkaarten - slag};
  int verwerkt = 0;
  for (int i = 0; i < aantalspelers; i++) {
    if (huidigespeler != i) {
      if (opgegooid[slag][i] != -1)
        zoumoetenhebben[verwerkt]--;
      verwerkt++;
    }
  }

  maxorig = maxkaart;

  // Maak de verdeling op basis van de kansverdeling
  while (!checkkansdeling(aantalgedeeld, zoumoetenhebben)) {
    maxkaart = maxorig;
    aantalgedeeld[0] = 0;
    aantalgedeeld[1] = 0;
    aantalgedeeld[2] = 0;

    bool debug = false;
    if (debug) {
      berekenkansverdeling(opgegooid, slag, komtuit, kansverdeling, huidigespeler, multiplier, totaalvankleur);
      berekentroefverdeling(opgegooid, slag, komtuit, troefverdeling, huidigespeler, kansverdeling, multiplier2, totaalvankleur[troefkleur]);
    }

    for (int i = maxkaart - 1; i >= 0; i--) {
      int kleur = kleurvankaart(allekaarten[i]);

      // Als het niet de boer of nel is kijken we alleen naar de kleur voor de kansverdeling
      if (!istroef(allekaarten[i], troefkleur)) {
        double randomnum = (double)(rand() % (10 * totaalvankleur[kleur])) / 10;
        if (randomnum < kansverdeling[0][kleur]) {
          // Deel kaart aan speler 0
          herverdeling[0][aantalgedeeld[0]] = allekaarten[i];
          aantalgedeeld[0]++;
        }
        else if (randomnum < kansverdeling[1][kleur] + kansverdeling[0][kleur]) {
          // Deel kaart aan speler 1
          herverdeling[1][aantalgedeeld[1]] = allekaarten[i];
          aantalgedeeld[1]++;
        }
        else {
          // Deel kaart aan speler 2
          herverdeling[2][aantalgedeeld[2]] = allekaarten[i];
          aantalgedeeld[2]++;
        }
      }
      else {
        // De huidige kaart is troef, kijk naar de troefverdeling
        double randomnum = (double)(rand() % 100) / 100;
        int kaart = allekaarten[i] - 10 * kleurvankaart(allekaarten[i]);

        if (randomnum < troefverdeling[0][kaart]) {
          // Deel kaart aan speler 0
          herverdeling[0][aantalgedeeld[0]] = allekaarten[i];
          aantalgedeeld[0]++;
        }
        else if (randomnum < troefverdeling[1][kaart] + troefverdeling[0][kaart]) {
          // Deel kaart aan speler 1
          herverdeling[1][aantalgedeeld[1]] = allekaarten[i];
          aantalgedeeld[1]++;
        }
        else {
          // Deel kaart aan speler 2
          herverdeling[2][aantalgedeeld[2]] = allekaarten[i];
          aantalgedeeld[2]++;
        }
      }

      // Als een van de spelers het maximum heeft bereikt beginnen we opnieuw
      if (aantalgedeeld[0] > aantalkaarten || aantalgedeeld[1] > aantalkaarten || aantalgedeeld[2] > aantalkaarten)
        break;

      maxkaart--;
    }
    // Kijken of een kleine wisseling het wel goed maakt?
    delingen++;
  }



  // De herverdeling wordt in spelerskaarten geplaatst en zo gereturned
  int k = 0;
  for (int i = 0; i < aantalspelers; i++) {
    if (i != huidigespeler) {
      for (int j = 0; j < aantalgedeeld[k]; j++) {
        spelerskaarten[i][j] = herverdeling[k][j];
      }
      k++;
    }
  }

  return delingen;
}

bool checkkansdeling(int aantalgedeeld[aantalspelers - 1], int zoumoetenhebben[aantalspelers - 1]) {
  for (int i = 0; i < aantalspelers - 1; i++) {
    if (aantalgedeeld[i] != zoumoetenhebben[i])
      return false;
  }

  return true;
}

void berekenheeftnietmetkans(int opgegooid[aantalslagen + 1][aantalkolommen], int slag,
                             bool heeftniet[4][aantalspelers]) 
{
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];
  // Initieer de arrays
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < aantalspelers; j++) {
      heeftniet[i][j] = false;
    }
  }

  for (int i = 0; i <= slag; i++) {
    int komtuit = opgegooid[i][aantalspelers];

    for (int k = 0; k < aantalspelers; k++) {
      int j = (komtuit + k) % 4;
      int kaart = opgegooid[i][j];

      if (kaart != -1) {
        if (j != komtuit && kleurvankaart(opgegooid[i][komtuit]) != kleurvankaart(kaart)) {
          // Kleur is niet bekend en deze speler kwam niet uit
          heeftniet[kleurvankaart(opgegooid[i][komtuit])][j] = true;
          if (!istroef(kaart, opgegooid))
            heeftniet[troefkleur][j] = true;
        }
        else if (j != komtuit && k == 3) {
          // j komt niet uit, heeft wel kleur bekend en is de laatste speler
          int zonderj[aantalspelers];

          // Checken:
          for (int l = 0; l < aantalspelers; l++) {
            if (j == l)
              zonderj[l] = -1;
            else
              zonderj[l] = opgegooid[i][l];
          }

          if (checkroem(opgegooid[i], troefkleur) > checkroem(zonderj, troefkleur) && 
              winnaar(zonderj, komtuit, troefkleur) != maat(j) &&
              winnaar(opgegooid[i], komtuit, troefkleur) != j) {
            heeftniet[kleurvankaart(opgegooid[i][komtuit])][j] = true;
          }
        }
      }
    }
  }
}

/* Maakt een kansverdeling in de volgende format:
 * 
 *   |  S  H  K  R
 * --|-------------
 * 0 |  x 
 * 1 |  y
 * 2 |  x
 *
 * Waarin horizontaal de kleuren staan en verticaal de spelers, exclusief de huidige speler.
 * x, y en z zijn kansen op een kleur, waarvan de som gelijk is aan het aantal kaarten van die 
 * kleur nog te verdelen. Kans is 0 als een speler die kleur niet kan hebben.
 *
 * Speelt is de speler die speelt, of -1 als de huidige speler speelt of iemand verplicht moet.
*/
void berekenkansverdeling(int opgegooid[aantalslagen + 1][aantalkolommen], int slag, int komtuit,
                          double kansverdeling[aantalspelers - 1][4], int huidigespeler,
                          double multiplier, int totaalvankleur[4]) 
{
  int speelt = opgegooid[aantalslagen][aantalspelers];
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];
  double heeftnietkansmultiplier = 0.5;
  bool heeftniet[4][aantalspelers];
  bool heeftnietkans[4][aantalspelers];
  
  // Als de speler zelf speelt of als een speler verplicht moet hebben we hier geen kennis over
  if (speelt == huidigespeler || opgegooid[aantalslagen][6] == 1)
    speelt = -1;
  // Correctie doordat de kansverdeling de huidigespeler niet bevat
  else if (huidigespeler < speelt)
    speelt--;
  
  // Initieer de kansverdeling op -1
  for (int i = 0; i < aantalspelers - 1; i++) {
    for (int j = 0; j < 4; j++) {
      kansverdeling[i][j] = -1;
    }
  }

  berekenheeftniet(opgegooid, slag, heeftniet);
  berekenheeftnietmetkans(opgegooid, slag, heeftnietkans);

  for (int i = 0; i < 4; i++) {
    int k = 0;

    for (int j = 0; j < aantalspelers; j ++) {
      if (j != huidigespeler) {
        if (heeftniet[i][j])
          kansverdeling[k][i] = 0;
        k++;
      }
    }
  }

  // Kansen worden verdeeld over spelers die wel die kleur mogen hebben
  for (int i = 0; i < 4; i++) {
    double hebbenwel = 0;
    for (int j = 0; j < aantalspelers - 1; j++) {
      if (kansverdeling[j][i] != 0)
        hebbenwel++;
    }

    if (hebbenwel > 1) {
      double kansperspeler = (double)totaalvankleur[i] / hebbenwel;
      if (speelt == -1 || i != troefkleur) {
        for (int j = 0; j < aantalspelers - 1; j++) {
          if (kansverdeling[j][i] != 0)
            kansverdeling[j][i] = kansperspeler;
        }
      }
      else {
        double kansmultiplier = kansperspeler * multiplier;
        double kansrest = (double)(totaalvankleur[troefkleur] - kansmultiplier) / (hebbenwel - 1);

        if (kansmultiplier > totaalvankleur[troefkleur]) {
          // Kans groter dan 1, maak gelijk aan maximumkans
          kansmultiplier = maximumkans * totaalvankleur[troefkleur];
          kansrest = (1 - kansmultiplier) / (hebbenwel - 1);
        }
        if (kansrest < 0) {
          kansmultiplier = maximumkans * totaalvankleur[troefkleur];
          kansrest = (1 - kansmultiplier) / (hebbenwel - 1);
        }

        for (int j = 0; j < aantalspelers - 1; j++) {
          if (j == speelt && kansverdeling[j][troefkleur] != 0)
            kansverdeling[j][troefkleur] = kansmultiplier;
          else if (kansverdeling[j][troefkleur] != 0)
            kansverdeling[j][troefkleur] = kansrest;
        }
      }
    }
    else {
      for (int j = 0; j < aantalspelers - 1; j++) {
        if (kansverdeling[j][i] != 0)
          kansverdeling[j][i] = totaalvankleur[i];
      }
    }
  }

  // Check verschill tussen heeftniet en heeftnietkans
  for (int i = 0; i < 4; i++) {
    int k = 0;

    for (int j = 0; j < aantalspelers; j ++) {
      if (j != huidigespeler) {
        if (!heeftniet[i][j] && heeftnietkans[i][j]) {
          // Niet in heeftniet, wel in heeftnietkans --> speler heeft ingetroefd
          // Kans wordt verlaagd, overige deel verdeeld over spelers die wel hebben.
          double hebbenwel = 0;

          for (int l = 0; l < aantalspelers - 1; l++) {
            if (kansverdeling[l][i] > 0 && l != k)
              hebbenwel++;
          }

          double kansoverig = kansverdeling[k][i] - (heeftnietkansmultiplier * kansverdeling[k][i]);
          double kansextra = kansoverig / hebbenwel;
          kansverdeling[k][i] = heeftnietkansmultiplier * kansverdeling[k][i];

          for (int l = 0; l < aantalspelers - 1; l++) {
            if (l != k && kansverdeling[l][i] > 0)
              kansverdeling[l][i] = kansverdeling[l][i] + kansextra;
          }
        }

        k++;
      }
    }
  }
}

/* Maakt een kansverdeling over alle troefkaarten in de volgende format:
 * 
 *   |  7 8 V H 10 A 9 B
 * --|-------------
 * 0 |  x 
 * 1 |  y
 * 2 |  x
 *
 * Waarin horizontaal de troefkaarten staan en verticaal de spelers, exclusief de huidige speler.
 * x, y en z zijn kansen op een kaart, waarvan de som gelijk is aan het aantal kaarten van die 
 * kleur nog te verdelen. Kans is 0 als een speler een kaart niet mag hebben door ondertroeven.
 *
 * Speelt is de speler die speelt, of -1 als de huidige speler speelt of iemand verplicht moet.
*/
void berekentroefverdeling(int opgegooid[aantalslagen + 1][aantalkolommen], int slag, int komtuit, 
                           double troefverdeling[aantalspelers - 1][aantalkaarten], int huidigespeler,
                           double kansverdeling[aantalspelers - 1][4], double multiplier, int totaaltroeven) {
  bool alopgegooid[aantalkaarten] = {false, false, false, false, false, false, false, false};
  int speelt = opgegooid[aantalslagen][aantalspelers];
  int troefkleur = opgegooid[aantalkaarten][aantalspelers + 1];
  double hebbenwel = 0;
  double kansspeelt = 0;
  double restkans = 0;

  // Als de speler zelf speelt of als een speler verplicht moet hebben we hier geen kennis over
  if (speelt == huidigespeler || opgegooid[aantalspelers][6] == 1)
    speelt = -1;
  // Correctie doordat de kansverdeling de huidigespeler niet bevat
  else if (huidigespeler < speelt)
    speelt--;
  // Als de speler die speelt geen troeven heeft, doe alsof niemand speelt.
  if (kansverdeling[speelt][troefkleur] == 0)
    speelt = -1;

  // Initieer de kansverdeling op -1
  for (int i = 0; i < aantalspelers - 1; i++) {
    for (int j = 0; j < aantalkaarten; j++) {
      troefverdeling[i][j] = -1;
    }
  }

  for (int i = 0; i < aantalspelers - 1; i++) {
    if (kansverdeling[i][troefkleur] != 0)
      hebbenwel++;
  }
  
  if (hebbenwel == 1) {
    // Nog maar 1 speler heeft troeven, hij krijgt alle kansen.
    int heeftwel = -1;
    for (int i = 0; i < aantalspelers - 1; i++)
      if (kansverdeling[i][troefkleur] != 0)
        heeftwel = i;

    for (int i = 0; i < aantalspelers - 1; i++) {
      for (int j = 0; j < aantalkaarten; j++) {
        if (i == heeftwel)
          troefverdeling[heeftwel][j] = 1;
        else 
          troefverdeling[i][j] = 0;
      }
    }
    
    return;
  }
  // Geen troeven meer in het spel, exit functie
  else if (hebbenwel == 0)
    return;

  // Kopieer heefttroefniet
  bool heefttroefniet[aantalspelers][aantalkaarten];
  berekenheefttroefniet(opgegooid, slag, heefttroefniet);
  
  int temp = 0;
  for (int i = 0; i < aantalspelers; i++) {
    if (i != huidigespeler) {
      for (int j = 0; j < aantalkaarten; j++) {
        if (heefttroefniet[i][j])
          troefverdeling[temp][j] = 0;
      }
      temp++;
    }
  }

  // Alle troeven die al gespeeld zijn worden op 0 gezet
  for (int i = 0; i < aantalslagen; i++) {
    for (int j = 0; j < aantalspelers; j++) {
      if (istroef(opgegooid[i][j], troefkleur)) {
          alopgegooid[opgegooid[i][j] - 10 * troefkleur] = true;
      }
    }
  }

  // We gebruiken alleen deze extra informatie als de speler niet zelf speelt of iemand
  // verplicht moet. 
  if (speelt != -1) {
    kansspeelt = ((double) kansverdeling[speelt][troefkleur] / (double)totaaltroeven);

    // Kans groter dan 1, maak gelijk aan maximumkans
    if (kansspeelt > 1) 
      kansspeelt = maximumkans;

    restkans = (1 - kansspeelt) / (hebbenwel - 1);
  }
  else {
    // We hebben geen extra informatie, dus de kansen zijn gelijk over de spelers die wel hebben
    kansspeelt = 1 / hebbenwel;
    restkans = kansspeelt;
  }

  // Kopieer kansen uit kansverdeling in troefverdeling
  for (int i = 0; i < aantalspelers - 1; i++) {
    for (int j = 0; j < aantalkaarten - 2; j++) {
      if (troefverdeling[i][j] != 0) {
        if (alopgegooid[j] || kansverdeling[i][troefkleur] == 0) {
          troefverdeling[i][j] = 0;
        }
        else if (troefverdeling[i][j] != 0) {
          if (i == speelt)
            troefverdeling[i][j] = kansspeelt;
          else
            troefverdeling[i][j] = restkans;
        }
      }
    }
  }

  // Troefkansen zijn verdeeld, nu alleen boer en nel nog.
  for (int i = 0; i < aantalspelers - 1; i++) {
    if (kansverdeling[i][troefkleur] == 0) {
      troefverdeling[i][6] = 0;
      troefverdeling[i][7] = 0;
    }
  }

  if (speelt == -1) {
    for (int i = 0; i < aantalspelers - 1; i++) {
      if (alopgegooid[6] || troefverdeling[i][6] == 0)
        troefverdeling[i][6] = 0;
      else
        troefverdeling[i][6] = restkans;
      if (alopgegooid[7] || troefverdeling[i][6] == 0)
        troefverdeling[i][7] = 0;
      else
        troefverdeling[i][7] = restkans;
    }
  }
  else {
    kansspeelt = multiplier * kansspeelt;
    // Kans groter dan 1, maak gelijk aan maximumkans
    if (kansspeelt > 1) 
      kansspeelt = maximumkans;
    // We weten dat hebbenwel > 1, anders was het algorithme al eerder gestopt.
    restkans = (1 - kansspeelt) / (hebbenwel - 1);

    for (int i = 0; i < aantalspelers - 1; i++) {
      if (troefverdeling[i][6] != 0 && !alopgegooid[6]) {
        if (i == speelt)
          troefverdeling[i][6] = kansspeelt;
        else
          troefverdeling[i][6] = restkans;
      }
      if (troefverdeling[i][7] != 0 && !alopgegooid[7]) {
        if (i == speelt)
          troefverdeling[i][7] = kansspeelt;
        else
          troefverdeling[i][7] = restkans;
      }
    }
  }
}

int montecarlokansmove(int kaarten[aantalkaarten], int opgegooid[aantalslagen + 1][aantalkolommen],
                       int slag, int komtuit, int huidigespeler, int niveaurandom, bool output) {
  int mogelijkekaarten[aantalkaarten];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - slag;
  int spelers[aantalspelers] = {niveaurandom, niveaurandom, niveaurandom, niveaurandom};
  int kopie[aantalslagen + 1][aantalkolommen];
  int spelerskaarten[aantalspelers][aantalkaarten];
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];

  geefmogelijkheden(opgegooid[slag], maxkaart, komtuit, huidigespeler, kaarten, troefkleur, mogelijkekaarten, aantalmogelijkheden);

  // Als er maar 1 mogelijkheid is moeten we deze doen.
  if (aantalmogelijkheden == 1) {
    deleteelement(mogelijkekaarten[0], kaarten, maxkaart);
    return mogelijkekaarten[0];
  }

  // Nu doen we aantalrandompotjes potjes voor elke mogelijke kaart
  int beste[2] = {-1, -1}; // beste[0] = beste kaart, beste [1] = aantal punten
  for (int i = 0; i < aantalmogelijkheden; i++) {
    int punten = 0;
    int delingen = 0;
    maxkaart = aantalkaarten - slag;

    for (int j = 0; j < aantalrandompotjes; j++) {

      // Eerst maken we een kopie van de kaarten, leeg voor de andere spelers
      for (int k = 0; k < aantalspelers; k++) {
        if (k == huidigespeler) {
          for (int l = 0; l < aantalkaarten; l++)
            spelerskaarten[k][l] = kaarten[l];
        }
        else {
          for (int l = 0; l < aantalkaarten; l++) {
            spelerskaarten[k][l] = -1;
          }
        }
      }

      // Maak een kopie van de opgegooide kaarten om een random potje mee te spelen
      for (int k = 0; k < aantalslagen + 1; k++) {
        for (int l = 0; l < aantalkolommen; l++) {
          kopie[k][l] = opgegooid[k][l];
        }
      }

      delingen += deelkansverdeling(kopie, slag, komtuit, huidigespeler, spelerskaarten, 1.1, 1.2);

      // Doe de zet in de kopie
      kopie[slag][huidigespeler] = mogelijkekaarten[i];
      deleteelement(mogelijkekaarten[i], spelerskaarten[huidigespeler], maxkaart);
// cout << endl;
//       printkaarten(spelerskaarten);
// cout << endl;
      speel(spelers, kopie, spelerskaarten, slag, huidigespeler + 1, komtuit, false, false);

      punten += kopie[aantalslagen][huidigespeler];
    }

    if (output)
      cout << "Punten voor " << Kaarten(mogelijkekaarten[i]) << " is " << punten
           << ", in gemiddeld " << delingen / aantalrandompotjes << " delingen." << endl;

    if (punten > beste[1]) {
      beste[0] = i;
      beste[1] = punten;
    }
  }

  deleteelement(mogelijkekaarten[beste[0]], kaarten, maxkaart);
  return mogelijkekaarten[beste[0]];
}

int montecarlomove(int kaarten[aantalkaarten], int opgegooid[aantalslagen + 1][aantalkolommen],
                   int slag, int komtuit, int huidigespeler, int niveaurandom, bool output, bool metkans, bool experiment) {
  int mogelijkekaarten[aantalkaarten];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - slag;
  int spelers[aantalspelers] = {niveaurandom, niveaurandom, niveaurandom, niveaurandom};
  int kopie[aantalslagen + 1][aantalkolommen];
  int spelerskaarten[aantalspelers][aantalkaarten];
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];
  float kans;

  geefmogelijkheden(opgegooid[slag], maxkaart, komtuit, huidigespeler, kaarten, troefkleur, mogelijkekaarten, aantalmogelijkheden);

  // Als er maar 1 mogelijkheid is moeten we deze doen.
  if (aantalmogelijkheden == 1) {
    deleteelement(mogelijkekaarten[0], kaarten, maxkaart);
    // if (experiment)
    //   cout << slag << " " << 0 << endl;
    return mogelijkekaarten[0];
  }

  if (!metkans)
    kans = 0;
  else
    kans = 0.5;

  // Nu doen we aantalrandompotjes potjes voor elke mogelijke kaart
  int beste[2] = {-1, -1}; // beste[0] = beste kaart, beste [1] = aantal punten
  for (int i = 0; i < aantalmogelijkheden; i++) {
    int punten = 0;
    int delingen = 0;
    maxkaart = aantalkaarten - slag;

    for (int j = 0; j < aantalrandompotjes; j++) {

      // Eerst maken we een kopie van de kaarten, leeg voor de andere spelers
      for (int k = 0; k < aantalspelers; k++) {
        if (k == huidigespeler) {
          for (int l = 0; l < aantalkaarten; l++)
            spelerskaarten[k][l] = kaarten[l];
        }
        else {
          for (int l = 0; l < aantalkaarten; l++) {
            spelerskaarten[k][l] = -1;
          }
        }
      }

      // Maak een kopie van de opgegooide kaarten om een random potje mee te spelen
      for (int k = 0; k < aantalslagen + 1; k++) {
        for (int l = 0; l < aantalkolommen; l++) {
          kopie[k][l] = opgegooid[k][l];
        }
      }
      
      // Een aantal (int kans) van de keren gebruiken we 'geschatte' informatie, dus 
      // of een speler een kaart niet heeft omdat hij roem bijgelegd heeft.
      if ((double) rand() / (RAND_MAX) > kans)
        delingen += deelrestkaarten(kopie, slag, komtuit, huidigespeler, spelerskaarten, false);
      else
        delingen += deelrestkaarten(kopie, slag, komtuit, huidigespeler, spelerskaarten, true);

      // Doe de zet in de kopie
      kopie[slag][huidigespeler] = mogelijkekaarten[i];
      deleteelement(mogelijkekaarten[i], spelerskaarten[huidigespeler], maxkaart);
      // printkaarten(spelerskaarten);

      speel(spelers, kopie, spelerskaarten, slag, huidigespeler + 1, komtuit, false, false);

      punten += kopie[aantalslagen][huidigespeler];
    }

    if (output)
      cout << Kaarten(mogelijkekaarten[i]) << " has " << punten
           << " points, average of " << delingen / aantalrandompotjes << " reshufflings." << endl;

    // if (experiment)
    //   cout << slag << " " << delingen << endl;

    if (punten > beste[1]) {
      beste[0] = i;
      beste[1] = punten;
    }
  }

  deleteelement(mogelijkekaarten[beste[0]], kaarten, maxkaart);
  return mogelijkekaarten[beste[0]];
}