#include "klaverjas.h"
#include "spelers.h"

/* Argumenten:
 * - opgegooid: Laatste kaarten die opgegooid zijn, waaraan kleur bekend moet worden
 * - maxkaart: Hoeveel kaarten de speler nog in zijn handen heeft
 * - komtuit: De speler die uitgekomen is, wiens kleur we moeten bekennen
 * - huidigespeler: De huidige speler (in dit geval dus computer). Alleen nodig voor Amsterdams
 * - mijnkaarten: Kaarten die de speler in zijn handen heeft
 * - mogelijkekaarten: hierin worden de mogelijk opgegooide kaarten gereturnt
 * - aantalmogelijkheden: hierin wordt het aantal mogelijke kaarten gereturnt
 */
void geefmogelijkheden(int opgegooid[aantalkolommen], int maxkaart, int komtuit,
                       int huidigespeler, int mijnkaarten[aantalkaarten], int troefkleur,
                       int mogelijkekaarten[aantalkaarten], int & aantalmogelijkheden) {
  
  aantalmogelijkheden = 0;
  // Als wij kleur mogen bepalen is alles mogelijk
  if (opgegooid[komtuit] != -1) {
    int kleur = kleurvankaart(opgegooid[komtuit]);

    // Eerst kijken we of we kleur kunnen bekennen
    if (kleur != troefkleur) {
      for (int i = 0; i < maxkaart; i++) {
        if (kleur == kleurvankaart(mijnkaarten[i])) {
          mogelijkekaarten[aantalmogelijkheden] = mijnkaarten[i];
          aantalmogelijkheden++;
        }
      }
    }

    // Als we geen kleur kunnen bekennen of er troef gevraagd is
    // proberen we over te troeven, als dat niet kan ondertroeven
    if (aantalmogelijkheden == 0) {
      int hoogstopgegooid = -1;
      int ligtaan = komtuit;

      // Check wat de hoogste opgegooide troef was
      for (int i = 0; i < aantalspelers; i++) {
        if (istroef(opgegooid[i], troefkleur)) {
          if (opgegooid[i] > hoogstopgegooid) {
            hoogstopgegooid = opgegooid[i];
            ligtaan = i;
          }
        }
      }

      // Overtroeven, alleen hogere troeven dan hoogstopgegooid kunnen.
      if (rotterdams || ligtaan != maat(huidigespeler)) {
        for (int i = 0; i < maxkaart; i++) {
          if (kleurvankaart(mijnkaarten[i]) == troefkleur) {
            if (mijnkaarten[i] > hoogstopgegooid) {
              mogelijkekaarten[aantalmogelijkheden] = mijnkaarten[i];
              aantalmogelijkheden++;
            }
          }
        }

        // Ondertroeven, alle troeven zijn mogelijk
        if (aantalmogelijkheden == 0) {
          for (int i = 0; i < maxkaart; i++) {
            if (kleurvankaart(mijnkaarten[i]) == troefkleur) {
              mogelijkekaarten[aantalmogelijkheden] = mijnkaarten[i];
              aantalmogelijkheden++;
            }
          }
        }
      }
      else if (kleur == troefkleur) {
        // We spelen Amsterdams, troef is gevraagd en hij ligt aan de maat. Ondertroeven:
        if (aantalmogelijkheden == 0) {
          for (int i = 0; i < maxkaart; i++) {
            if (kleurvankaart(mijnkaarten[i]) == troefkleur) {
              mogelijkekaarten[aantalmogelijkheden] = mijnkaarten[i];
              aantalmogelijkheden++;
            }
          }
        }
      }
    }
  }

  // Als we ook niet in kunnen troeven zijn alle kaarten mogelijk
  // Hetzelfde geldt voor als wij de kleur mogen bepalen of
  // als we Amsterdams spelen en de slag aan de maat ligt
  if (aantalmogelijkheden == 0 || (opgegooid[komtuit == -1] && aantalmogelijkheden == 0)) {
    for (int i = 0; i < maxkaart; i++) {
      mogelijkekaarten[i] = mijnkaarten[i];
      aantalmogelijkheden++;
    }
  }
}

// Doet een random zet, maar gooit geen azen, 10en en roem bij de tegenstander bij
int semiramdommove(int kaarten[aantalkaarten], int opgegooid[aantalkolommen],
                   int slag, int komtuit, int huidigespeler, int troefkleur, bool output) {
  int mogelijkekaarten[aantalkaarten];
  int slechtekaarten[aantalkaarten];       // Domme kaarten, gooien roem bij tegenstander of punten weg
  int beterekaarten[aantalkaarten];        // Slimmere kaarten, leggen roem bij maat
  int kopiekaarten[aantalspelers];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - slag;
  int slagwinnaar = -1;
  int aantalslechte = 0;
  int aantalbetere = 0;
  int roemvoorkaart = checkroem(opgegooid, troefkleur);

  geefmogelijkheden(opgegooid, maxkaart, komtuit, huidigespeler, kaarten, troefkleur, mogelijkekaarten, aantalmogelijkheden);

  // Als er maar 1 mogelijkheid is moeten we deze doen.
  if (aantalmogelijkheden == 1) {
    deleteelement(mogelijkekaarten[0], kaarten, maxkaart);
    return mogelijkekaarten[0];
  }

  // We maken een kopie van de opgegooide kaarten om de kaarten in 'op te gooien'
  for (int i = 0; i < aantalspelers; i++)
    kopiekaarten[i] = opgegooid[i];

  for (int i = 0; i < aantalmogelijkheden; i++) {
    int kleur = kleurvankaart(mogelijkekaarten[i]);
    int kaart = mogelijkekaarten[i] - 10 * kleur;

    kopiekaarten[huidigespeler] = mogelijkekaarten[i];
    slagwinnaar = winnaar(kopiekaarten, komtuit, troefkleur);

    if (!(slagwinnaar == huidigespeler || slagwinnaar == maat(huidigespeler))) {
      // Slag ligt NIET aan ons, geen roem of hoge weggooien.
      if ((huidigespeler == (komtuit + 3) % 4) && checkroem(kopiekaarten, troefkleur) > roemvoorkaart) {
        // We zijn de laatste speler, dus geen roem bijleggen
        if (output)
          cout << "Niet " << Kaarten(mogelijkekaarten[i]) << " in de roem leggen (4e)..." << endl;
        slechtekaarten[aantalslechte] = mogelijkekaarten[i];
        aantalslechte++;
      }
      else if ((huidigespeler == (komtuit + 2) % 4) && checkroem(kopiekaarten, troefkleur) > roemvoorkaart) {
        // We zijn de een na laatste speler, dus ook geen roem bijleggen
        if (output)
          cout << "Niet " << Kaarten(mogelijkekaarten[i]) << " in de roem leggen (3e)..." << endl;
        slechtekaarten[aantalslechte] = mogelijkekaarten[i];
        aantalslechte++;
      }
      else if (kleur != kleurvankaart(opgegooid[komtuit]) && kleur != troefkleur) {
        if (kaart == 5 || (kaart == 4 && tienennietweggooien)) {
          if (output)
            cout << "Aas of 10 " << Kaarten(mogelijkekaarten[i]) << " niet weggooien..." << endl;
          slechtekaarten[aantalslechte] = mogelijkekaarten[i];
          aantalslechte++;
        }
      }
    }
    else if (semirandom_bijmaat) {
      // Slag ligt aan ons, roem bijleggen als het kan
      if ((huidigespeler == (komtuit + 3) % 4) && checkroem(kopiekaarten, troefkleur) > roemvoorkaart) {
        if (output)
          cout << "Wel " << Kaarten(mogelijkekaarten[i]) << " in de roem leggen..." << endl;
        beterekaarten[aantalbetere] = mogelijkekaarten[i];
        aantalbetere++;
      }
    }
  }

  if (aantalbetere == 0 && aantalslechte > 0 && (aantalmogelijkheden - aantalslechte > 0)) {
    if (output) {
        cout << aantalslechte << " slechte kaarten verwijderen: ";
        for (int i = 0; i < aantalslechte; i++)
          cout << Kaarten(slechtekaarten[i]);
        cout << endl;
    }

    // Verwijder de slechte kaarten uit mogelijkekaarten
    for (int i = 0; i < aantalslechte; i++) {
      for (int j = 0; j < aantalmogelijkheden - i; j++) {
        if (mogelijkekaarten[j] == slechtekaarten[i]) {
          // deleteelement(slechtekaarten[i], mogelijkekaarten, aantalmogelijkheden - i);
          mogelijkekaarten[j] = -1;
          wisselelement(j, mogelijkekaarten, aantalmogelijkheden - 1);
          aantalmogelijkheden--;
          j--;
        }
      }
    }

    // Nieuwe mogelijkekaarten printen
    if (output) {
      cout << "Wel mogelijke kaarten (" << aantalmogelijkheden << "): ";
      for (int i = 0; i < aantalmogelijkheden; i++)
        cout << Kaarten(mogelijkekaarten[i]);
      cout << endl;
    }

    int randomkaart = rand() % aantalmogelijkheden;
    deleteelement(mogelijkekaarten[randomkaart], kaarten, maxkaart);

    return mogelijkekaarten[randomkaart];
  }
  else if (aantalbetere > 0) {
    // Betere (in de roem-leggende) mogelijkheden printen
    if (output) {
      cout << "Mogelijke betere kaarten (" << aantalbetere << "): ";
      for (int i = 0; i < aantalbetere; i++)
        cout << Kaarten(beterekaarten[i]);
      cout << endl;
    }

    int randomkaart = rand() % aantalbetere;
    deleteelement(beterekaarten[randomkaart], kaarten, maxkaart);

    return beterekaarten[randomkaart];
  }
  else {
    if (output) {
      cout << aantalmogelijkheden << " normale mogelijkheden: ";
      for (int i = 0; i < aantalmogelijkheden; i++)
        cout << Kaarten(mogelijkekaarten[i]);
      cout << endl;
    }

    int randomkaart = rand() % aantalmogelijkheden;
    deleteelement(mogelijkekaarten[randomkaart], kaarten, maxkaart);

    return mogelijkekaarten[randomkaart];
  }
}

int randommove(int kaarten[aantalkaarten], int opgegooid[aantalkolommen],
               int slag, int komtuit, int huidigespeler, int troefkleur, bool output) {
  int mogelijkekaarten[aantalkaarten];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - slag;

  geefmogelijkheden(opgegooid, maxkaart, komtuit, huidigespeler, kaarten, troefkleur, mogelijkekaarten, aantalmogelijkheden);

  if (output) {
    cout << aantalmogelijkheden << " mogelijkheden: ";
      for (int i = 0; i < aantalmogelijkheden; i++)
       cout << Kaarten(mogelijkekaarten[i]);
     cout << endl;
   }

  int randomkaart = rand() % aantalmogelijkheden;
  deleteelement(mogelijkekaarten[randomkaart], kaarten, maxkaart);

  return mogelijkekaarten[randomkaart];
}

int usermove(int kaarten[aantalkaarten], int slag) {
  int input;
  int maxkaart = aantalkaarten - slag;

  // We sorteren de kaarten van menselijke spelers voor het overzicht 
  insertionsort(kaarten, maxkaart);

  cout << endl << "Kies een kaart om op te gooien: " << endl;
  for (int i = 0; i < maxkaart; i++)
    cout << i << ": " << Kaarten(kaarten[i]) << endl;

  cout << endl << "Voer getal in: ";
  cin >> input;
  cout << Kaarten(kaarten[input]) << endl;

  if (input >= 0 && input < maxkaart) {
    int keuze = kaarten[input];
    kaarten[input] = -1;
    wisselelement(input, kaarten, maxkaart - 1);

    return keuze;
  }
  else {
    cout << "Geen getal tussen de 1 en " << maxkaart << " ingevoerd. Maak een nieuwe keuze." << endl;
    return -1;
  }
}
