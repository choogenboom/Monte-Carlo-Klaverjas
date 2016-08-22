#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>

using namespace std;

const static int aantalspelers = 4;
const static int aantalhandjes = 8;
const static int aantalkaarten = 8;
const static bool rotterdams = true;
int opgegooid[aantalhandjes][aantalspelers + 1];
int troefkleur = 2;

enum Kaarten {
  S7 = 0,  S8, S9, SB, SV, SH, S10, SA, 
  H7 = 10, H8, H9, HB, HV, HH, H10, HA,
  K7 = 20, K8, K9, KB, KV, KH, K10, KA,
  R7 = 30, R8, R9, RB, RV, RH, R10, RA,
  X = -1
};

ostream& operator<<(ostream& os, const Kaarten kaart) {
  string s;
  switch(kaart) {
    case (S7): 
      s = "♠7";
      break;
    case (S8):
      s = "♠8";
      break;
    case (S9):
      s = "♠9";
      break;
    case (SB):
      s = "♠B";
      break;
    case (SV):
      s = "♠V";
      break;
    case (SH):
      s = "♠H";
      break;
    case (S10):
      s = "♠10";
      break;
    case (SA):
      s = "♠A";
      break;
    case (H7):
      s = "\033[1;31m♥\033[0m7";
      break;
    case (H8):
      s = "\033[1;31m♥\033[0m8";
      break;
    case (H9):
      s = "\033[1;31m♥\033[0m9";
      break;
    case (HB):
      s = "\033[1;31m♥\033[0mB";
      break;
    case (HV):
      s = "\033[1;31m♥\033[0mV";
      break;
    case (HH):
      s = "\033[1;31m♥\033[0mH";
      break;
    case (H10):
      s = "\033[1;31m♥\033[0m10";
      break;
    case (HA):
      s = "\033[1;31m♥\033[0mA";
      break;
    case (K7):
      s = "♣7";
      break;
    case (K8):
      s = "♣8";
      break;
    case (K9):
      s = "♣9";
      break;
    case (KB):
      s = "♣B";
      break;
    case (KV):
      s = "♣V";
      break;
    case (KH):
      s = "♣H";
      break;
    case (K10):
      s = "♣10";
      break;
    case (KA):
      s = "♣A";
      break;
    case (R7):
      s = "\033[1;31m♦\033[0m7";
      break;
    case (R8):
      s = "\033[1;31m♦\033[0m8";
      break;
    case (R9):
      s = "\033[1;31m♦\033[0m9";
      break;
    case (RB):
      s = "\033[1;31m♦\033[0mB";
      break;
    case (RV):
      s = "\033[1;31m♦\033[0mV";
      break;
    case (RH):
      s = "\033[1;31m♦\033[0mH";
      break;
    case (R10):
      s = "\033[1;31m♦\033[0m10";
      break;
    case (RA):
      s = "\033[1;31m♦\033[0mA";
      break;
    case (X):
      s = "\033[1;31mX\033[0m";
        break;
  }

  return os << s << " ";
}

void parseargv(int argc, char* argv[], int spelers[aantalspelers]) {
  if (argc != aantalspelers + 1) {
    cout << "Verkeerd aantal parameters meegegeven." << endl;
    exit(0);
  }
  else {
    for (int i = 0; i < aantalspelers; i++) {
      spelers[i] = atoi(argv[i + 1]);
    }
  }
}

int kleurvankaart(int kaart) {
  return (kaart - (kaart % 10)) / 10;
}

bool istroef(int kaart) {
  return kleurvankaart(kaart) == troefkleur;
}

int waardeerkaart(int kaart) {
  int kleur = kleurvankaart(kaart);
  
  kaart = kaart % 10;
  
  if (kleur != troefkleur) {
    if (kaart > 2 && kaart <= 5)
      return kaart - 1;
    else if (kaart > 5)
      return kaart + 4;
  }
  else {
    if (kaart == 2) // Nel
      return 14;
    else if (kaart == 3) // Boer
      return 20;
    else if (kaart > 3 && kaart <= 5)
      return kaart - 1;
    else if (kaart > 5)
      return kaart + 4;
    }

  return 0;
}

int maat(int speler) {
  return (speler + 2) % 4;
}

void printkaarten(int zuid[aantalkaarten], int west[aantalkaarten], int noord[aantalkaarten], int oost[aantalkaarten]) {
  cout << "Zuid: ";
  for (int i = 0; i < aantalkaarten; i++)
    cout << Kaarten(zuid[i]) << " ";
  cout << endl;

  cout << "West: ";
  for (int i = 0; i < aantalkaarten; i++)
    cout << Kaarten(west[i]) << " ";
  cout << endl;

  cout << "Noord: ";
  for (int i = 0; i < aantalkaarten; i++)
    cout << Kaarten(noord[i]) << " ";
  cout << endl;

  cout << "Oost: ";
  for (int i = 0; i < aantalkaarten; i++)
    cout <<  Kaarten(oost[i]) << " ";
  cout << endl;
}

int winnaar(int kaarten[aantalspelers], int beurt) {
  int kleur = kleurvankaart(kaarten[beurt]);  // De te bekennen kleur
  int hoogste[3] = {-1, -1, 0};               // Hoogste aantal punten, speler en of ingetroefd is
  // punten = 0;

  for (int i = 0; i < aantalspelers; i++) {
    if (kleurvankaart(kaarten[i]) == kleur) {
      int kaartpunten = waardeerkaart(kaarten[i]);
      // punten += kaartpunten;

      // Als het aantal punten van de huidige kaart hoger is en de hoogste geen troef
      if (kaartpunten > hoogste[1] && !hoogste[2]) {
        hoogste[0] = i;
        hoogste[1] = kaartpunten;
        hoogste[2] = istroef(kaarten[i]);
      }
      // Hoogste is troef,
      else if (hoogste[2] && istroef(kaarten[i])) {
        if (kaartpunten > hoogste[1]) {
          hoogste[0] = i;
          hoogste[1] = kaartpunten;
          hoogste[2] = 1;
        }
      }

    }
    // ingetroefd
    else if (istroef(kaarten[i])) {
      int kaartpunten = waardeerkaart(kaarten[i]);
      // punten += kaartpunten;

      // Hoogste kaart tot nu toe ook troef?
      if (istroef(kaarten[hoogste[0]])) {
        if (kaartpunten > hoogste[1]) {
          hoogste[0] = i;
          hoogste[1] = kaartpunten;
          hoogste[2] = 1;
        }
      }
      // Ingetroefd maar hoogste tot nu toe geen troef
      else {
          hoogste[0] = i;
          hoogste[1] = kaartpunten;
          hoogste[2] = 1;
      }
    }
    // Geen kleur bekend, geen punten
    else {
      // 
    }
  }

  return hoogste[0];
}

void printspel() {
  cout << endl << "Opgegooide kaarten:" << endl;

  for (int i = 0; i < aantalhandjes; i++) {
    for (int j = 0; j < aantalspelers; j++) {
      cout << Kaarten(opgegooid[i][j]) << " ";
    }
    cout << " " << opgegooid[i][aantalspelers] << endl;
  }
  cout << "---------------------------------------------" << endl;
}

void wisselelement(int element, int input[], int arraysize) {
  if (element <= arraysize) {
    int temp = input[element];
    input[element] = input[arraysize];
    input[arraysize] = temp;
  }
}

void deleteelement(int element, int input[], int arraysize) {
  for (int i = 0; i < arraysize; i++) {
    if (input[i] == element) {
      input[i] = -1;
      wisselelement(i, input, arraysize - 1);
      break;
    }
  }
}  

void deelkaarten(int zuid[aantalkaarten], int west[aantalkaarten], int noord[aantalkaarten], int oost[aantalkaarten]) {
  int allekaarten[aantalkaarten * aantalspelers];
  // Initieer alle kaarten
  for (int i = 0; i < aantalkaarten * aantalspelers ; i++)
    allekaarten[i] = (10 * floor(i / aantalkaarten) + i % aantalkaarten);

  // Pak een willekeurige kaart uit allekaarten, haal deze uit de array en 
  // geef deze aan een speler.
  int maxkaart = aantalkaarten * aantalspelers;
  for (int j = 0; j < aantalkaarten; j++) {
    int randomkaart = rand() % (maxkaart - 1) + 1;
    zuid[j] = allekaarten[randomkaart];
    wisselelement(randomkaart, allekaarten, maxkaart - 1);
    maxkaart--;
  }

  for (int j = 0; j < aantalkaarten; j++) {
    int randomkaart = rand() % (maxkaart - 1) + 1;
    west[j] = allekaarten[randomkaart];
    wisselelement(randomkaart, allekaarten, maxkaart - 1);
    maxkaart--;
  }
  
  for (int j = 0; j < aantalkaarten; j++) {
    int randomkaart = rand() % (maxkaart - 1) + 1;
    noord[j] = allekaarten[randomkaart];
    wisselelement(randomkaart, allekaarten, maxkaart - 1);
    maxkaart--;
  }

  for (int j = 0; j < aantalkaarten; j++)
    oost[j] = allekaarten[j];
}

// int checkroem(int kaarten[aantalspelers]) {

// }

/* Argumenten:
 * - opgegooidekaarten: Laatste kaarten die opgegooid zijn, waaraan kleur bekend moet worden
 * - maxkaart: Hoeveel kaarten de speler nog in zijn handen heeft
 * - beurt: De speler die uitgekomen is, wiens kleur we moeten bekennen
 * - huidigespeler: De huidige speler (in dit geval dus computer). Alleen nodig voor Amsterdams
 * - mijnkaarten: Kaarten die de speler in zijn handen heeft  
 * - mogelijkekaarten: hierin worden de mogelijk opgegooide kaarten gereturnt
 * - aantalmogelijkheden: hierin wordt het aantal mogelijke kaarten gereturnt
 */
void geefmogelijkheden(int opgegooidekaarten[aantalspelers], int maxkaart, int beurt, 
                       int huidigespeler, int mijnkaarten[aantalkaarten], 
                       int mogelijkekaarten[aantalkaarten], int & aantalmogelijkheden) {
  aantalmogelijkheden = 0;
  // Als wij kleur mogen bepalen is alles mogelijk
  if (opgegooidekaarten[beurt] != -1) {
    int kleur = kleurvankaart(opgegooidekaarten[beurt]);

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
      int hoogstopgegooid = beurt;
      // Check wat de hoogste opgegooide troef was
      for (int i = 0; i < aantalspelers; i++) {
        if (istroef(opgegooidekaarten[i])) {
          if (waardeerkaart(opgegooidekaarten[i]) > waardeerkaart(opgegooidekaarten[hoogstopgegooid])) {
            hoogstopgegooid = i;
            
          }
        }
      }

      // Overtroeven
      if (rotterdams || hoogstopgegooid != maat(huidigespeler)) {
        for (int i = 0; i < maxkaart; i++) {
          if (kleurvankaart(mijnkaarten[i]) == troefkleur) {
            if (waardeerkaart(mijnkaarten[i]) > waardeerkaart(opgegooidekaarten[hoogstopgegooid])) {
              mogelijkekaarten[aantalmogelijkheden] = mijnkaarten[i];
              aantalmogelijkheden++;
            }
          }
        }

        // Ondertroeven
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
  if (aantalmogelijkheden == 0 || (opgegooidekaarten[beurt == -1] && aantalmogelijkheden == 0)) {
    for (int i = 0; i < maxkaart; i++) {
      mogelijkekaarten[i] = mijnkaarten[i];
      aantalmogelijkheden++;
    }
  }
}


int randommove(int kaarten[aantalkaarten], int handje, int beurt, int huidigespeler) {
  int mogelijkekaarten[aantalkaarten];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - handje;

  geefmogelijkheden(opgegooid[handje], maxkaart, beurt, huidigespeler, kaarten, mogelijkekaarten, aantalmogelijkheden);

  cout << aantalmogelijkheden << " mogelijkheden: ";
  for (int i = 0; i < aantalmogelijkheden; i++)
    cout << Kaarten(mogelijkekaarten[i]);
  cout << endl;
  
  int randomkaart = rand() % aantalmogelijkheden;
  deleteelement(mogelijkekaarten[randomkaart], kaarten, maxkaart);

  return mogelijkekaarten[randomkaart];
}

int usermove(int kaarten[aantalkaarten], int handje) {
  int input;
  int maxkaart = aantalkaarten - handje;

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

int main(int argc, char* argv[]) {
  int huidigespeler = 0, handje = 0, beurt = 0;
  int zuid[aantalkaarten], west[aantalkaarten], noord[aantalkaarten], oost[aantalkaarten];
  int spelers[aantalspelers];                            // Samenstelling van de spelers (mens/computer)
  int* spelerskaarten[4] = {zuid, west, noord, oost};    // Array met pointers naar de kaarten van spelers

  parseargv(argc, argv, spelers);
  srand(time(NULL));

  deelkaarten(zuid, west, noord, oost);
  printkaarten(zuid, west, noord, oost);

  for (int i = 0; i < aantalhandjes; i++)
    for (int j = 0; j < aantalspelers; j++)
      opgegooid[i][j] = -1;

  while (handje < aantalhandjes) {
    for (int i = 0; i < aantalspelers; i++) {
      int waarde = -1;

      cout << "Speler " << huidigespeler << " aan de beurt, " << beurt << " komt uit. " << endl;

      if (spelers[huidigespeler] == 0)
        while (waarde == -1)
          waarde = usermove(spelerskaarten[huidigespeler], handje);
      else {
        waarde = randommove(spelerskaarten[huidigespeler], handje, beurt, huidigespeler);
        cout << "Computer heeft " << Kaarten(waarde) << " opgegooid." << endl << endl;
      }
      
      opgegooid[handje][huidigespeler] = waarde;
      huidigespeler = (huidigespeler + 1) % aantalspelers;
    }
    cout << "Winnaar: " << winnaar(opgegooid[handje], beurt) << endl;
    beurt = winnaar(opgegooid[handje], beurt);
    opgegooid[handje][aantalspelers] = beurt;
    printspel();
    printkaarten(zuid, west, noord, oost);
    huidigespeler = beurt;
    handje++;
  }

  return 0;
}