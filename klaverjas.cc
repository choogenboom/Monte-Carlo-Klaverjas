#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>

using namespace std;

const static int aantalspelers = 4;
const static int aantalhandjes = 8;
const static int aantalkaarten = 8;
const static bool rotterdams = true;
const static bool metroem = true;
int troefkleur = 2;

enum Kaarten {
  S7 = 0,  S8, SV, SH, S10, SA, S9, SB, 
  H7 = 10, H8, HV, HH, H10, HA, H9, HB,
  K7 = 20, K8, KV, KH, K10, KA, K9, KB,
  R7 = 30, R8, RV, RH, R10, RA, R9, RB,
  X = -1
};

ostream& operator<<(ostream& os, const Kaarten kaart) {
  string s;
  switch(kaart) {
    case (S7): 
      s = "♠7 ";
      break;
    case (S8):
      s = "♠8 ";
      break;
    case (S9):
      s = "♠9 ";
      break;
    case (SB):
      s = "♠B ";
      break;
    case (SV):
      s = "♠V ";
      break;
    case (SH):
      s = "♠H ";
      break;
    case (S10):
      s = "♠10";
      break;
    case (SA):
      s = "♠A ";
      break;
    case (H7):
      s = "\033[1;31m♥\033[0m7 ";
      break;
    case (H8):
      s = "\033[1;31m♥\033[0m8 ";
      break;
    case (H9):
      s = "\033[1;31m♥\033[0m9 ";
      break;
    case (HB):
      s = "\033[1;31m♥\033[0mB ";
      break;
    case (HV):
      s = "\033[1;31m♥\033[0mV ";
      break;
    case (HH):
      s = "\033[1;31m♥\033[0mH ";
      break;
    case (H10):
      s = "\033[1;31m♥\033[0m10";
      break;
    case (HA):
      s = "\033[1;31m♥\033[0mA ";
      break;
    case (K7):
      s = "♣7 ";
      break;
    case (K8):
      s = "♣8 ";
      break;
    case (K9):
      s = "♣9 ";
      break;
    case (KB):
      s = "♣B ";
      break;
    case (KV):
      s = "♣V ";
      break;
    case (KH):
      s = "♣H ";
      break;
    case (K10):
      s = "♣10";
      break;
    case (KA):
      s = "♣A ";
      break;
    case (R7):
      s = "\033[1;31m♦\033[0m7 ";
      break;
    case (R8):
      s = "\033[1;31m♦\033[0m8 ";
      break;
    case (R9):
      s = "\033[1;31m♦\033[0m9 ";
      break;
    case (RB):
      s = "\033[1;31m♦\033[0mB ";
      break;
    case (RV):
      s = "\033[1;31m♦\033[0mV ";
      break;
    case (RH):
      s = "\033[1;31m♦\033[0mH ";
      break;
    case (R10):
      s = "\033[1;31m♦\033[0m10";
      break;
    case (RA):
      s = "\033[1;31m♦\033[0mA ";
      break;
    case (X):
      s = "\033[1;31mX\033[0m  ";
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

// Obv 7 8 V H 10 A 9 B
int waardeerkaart(int kaart) {
  int kleur = kleurvankaart(kaart);
  
  kaart = kaart % 10;
  
  if (kleur != troefkleur || kaart < 6) {
    if (kaart < 2 || kaart == 6)
      return 0;
    else if (kaart <= 3)
      return kaart + 1;
    else if (kaart <= 5)
      return kaart + 6;
    else
      return 2;
  }
  else {
    if (kaart == 6) // Nel
      return 14;
    else if (kaart == 7) // Boer
      return 20;
  }

  return 0;
}

// Zet het nummer van een kaart om in 7 8 9 10 B V H A volgorde
int roemvolgorde(int kaart) {
  int kleur = kleurvankaart(kaart);
  kaart = kaart % 10;

  // 7 of 8
  if (kaart < 2)
    return 10 * kleur + kaart;
  // V of H
  else if (kaart < 4)
    return 10 * kleur + kaart + 3;
  // 10
  else if (kaart == 4)
    return 10 * kleur +  kaart - 1;
  // A
  else if (kaart == 5)
    return 10 * kleur + kaart + 2;
  // 9
  else if (kaart == 6)
    return 10 * kleur + kaart - 4;
  // B
  else
    return 10 * kleur + kaart - 3;
}

int maat(int speler) {
  return (speler + 2) % 4;
}

// Partitionering voor QuickSort
int partitie(int A[aantalspelers], int p, int r) {
  int x = A[p];
  int i = p - 1;
  int j = r + 1;

  while (i < j) {
    j = j - 1;
    
    while (A[j] > x)
      j = j - 1;

    i = i + 1;

    while (A[i] < x)
      i = i + 1;

    if (i < j) {
      // Wissel A[i] en A[j]
      int temp = A[i];
      A[i] = A[j];
      A[j] = temp;
    }
  }

  return j;
}

// QuickSort algoritme voor Roemdetectie
void quicksort(int A[aantalspelers], int p, int r) {
  if (p < r) {
    int q = partitie(A, p, r);
    quicksort(A, p, q);
    quicksort(A, q + 1, r);
  }
}

void printkaarten(int zuid[aantalkaarten], int west[aantalkaarten], int noord[aantalkaarten], int oost[aantalkaarten]) {
  cout << "0: ";
  for (int i = 0; i < aantalkaarten; i++)
    cout << Kaarten(zuid[i]) << " ";
  cout << endl;

  cout << "1: ";
  for (int i = 0; i < aantalkaarten; i++)
    cout << Kaarten(west[i]) << " ";
  cout << endl;

  cout << "2: ";
  for (int i = 0; i < aantalkaarten; i++)
    cout << Kaarten(noord[i]) << " ";
  cout << endl;

  cout << "3: ";
  for (int i = 0; i < aantalkaarten; i++)
    cout <<  Kaarten(oost[i]) << " ";
  cout << endl;
}

int winnaar(int kaarten[aantalspelers], int beurt) {
  int kleur = kleurvankaart(kaarten[beurt]);  // De te bekennen kleur
  int hoogste[3] = {-1, -1, 0};               // Hoogste aantal punten, speler en of ingetroefd is

  for (int i = 0; i < aantalspelers; i++) {
    if (kleurvankaart(kaarten[i]) == kleur) {
      int kaartpunten = waardeerkaart(kaarten[i]);

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
  }

  return hoogste[0];
}

void printspel(int opgegooid[aantalhandjes][aantalspelers + 2]) {
  cout << endl << "Opgegooide kaarten:" << endl;

  for (int i = 0; i < aantalhandjes; i++) {
    for (int j = 0; j < aantalspelers; j++) {
      cout << Kaarten(opgegooid[i][j]) << " ";
    }
    cout << " speler " << opgegooid[i][aantalspelers] << " met " << opgegooid[i][aantalspelers + 1] << " punten." << endl;
  }
  cout << "---------------------------------------------" << endl;
}

int zoekelement(int element, int input[], int arraysize) {
  for (int i = 0; i < arraysize; i++) {
    if (input[i] == element)
      return i;
    else
      cout << ".";
  }

  return -1;
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

void deelrestkaarten(int zuid[aantalkaarten], int west[aantalkaarten], int noord[aantalkaarten], int oost[aantalkaarten]) {
  int allekaarten[aantalkaarten * aantalspelers];
  int maxkaart = aantalkaarten * aantalspelers;
  int *huidig;                                      // Pointer naar de huidige speler, wiens kaarten bekend zijn
  // Initieer alle kaarten
  for (int i = 0; i < aantalkaarten * aantalspelers ; i++)
    allekaarten[i] = (10 * floor(i / aantalkaarten) + i % aantalkaarten);

  if (zuid != NULL)
    huidig = zuid;
  else if (west != NULL)
    huidig = west;
  else if (noord != NULL)
    huidig = noord;
  else
    huidig = oost;

cout << "huidig: ";
for (int i = 0; i < aantalkaarten; i++)
  cout << Kaarten(huidig[i]);
cout << endl;

  for (int i = 0; i < aantalkaarten; i++) {
    int index = (aantalkaarten * floor(huidig[i] / 10) + huidig[i] % 10);
    cout << "maxkaart: " << maxkaart << endl;
    if (allekaarten[index] != huidig[i]) {
      index = zoekelement(huidig[i], allekaarten, maxkaart);
      cout << "index != huidig[i]. Gevonden: " << allekaarten[index] << "=" << huidig[i] << endl;
    }

    allekaarten[index] = -1;
    wisselelement(index, allekaarten, maxkaart - i);
  }

  maxkaart -= aantalkaarten;
cout << "allekaarten (maxkaart=" << maxkaart << "): ";
for (int i = 0; i < maxkaart; i++)
  cout << Kaarten(allekaarten[i]);
cout << endl;

}

// Preconditie: kaarten moet een gesorteerde array zijn
bool checkstuk(int kaarten[aantalspelers]) {
  int troefvrouw = 10 * troefkleur + 5;
  int troefheer = troefvrouw + 1;

  for (int i = 0; i < aantalspelers - 1; i++) {
    if (kaarten[i] == troefvrouw)
      if (kaarten[i + 1] == troefheer)
        return true;
  }
  
  return false;
}

int checkroem(int originelekaarten[aantalspelers]) {
  int roem = 0;
  int kaarten[aantalspelers];

  for (int i = 0; i < aantalspelers; i++) {
    kaarten[i] = roemvolgorde(originelekaarten[i]);
  }

  quicksort(kaarten, 0, aantalspelers);

  for (int i = 0; i < 2; i++) {
    if (kaarten[i + 1] == kaarten[i] + 1 && kaarten[i + 2] == kaarten[i + 1] + 1) {
      roem = 20;
      if (i == 0 && kaarten[aantalspelers - 1] == kaarten[aantalspelers - 2] + 1) {
        roem = 50;
        break;
      }
    }
  }

  // Stuk:
  if (checkstuk(kaarten)) {
    roem += 20;
  }

  return roem;
}

int waardeerkaarten(int kaarten[aantalspelers]) {
  int punten = 0;
  int roem = 0;

  for (int i = 0; i < aantalspelers; i++) {
    punten += waardeerkaart(kaarten[i]);
  }

  cout << punten << " punten";
  if (metroem) {
    roem = checkroem(kaarten);
    cout << " en " << roem << " roem." << endl;
  }
  else
    cout << "." << endl;

  return punten + roem;
}

int totaalwinnaar(int kaarten[aantalhandjes][aantalspelers + 2], int &zuid, int &west, int &noord, int &oost) {
  int nultwee = 0;
  int eendrie = 0;

  for (int i = 0; i < aantalkaarten; i++) {
    if (kaarten[i][aantalspelers] == 0 || kaarten[i][aantalspelers] == 2)
      nultwee += kaarten[i][aantalspelers + 1];
    else
      eendrie += kaarten[i][aantalspelers + 1];
  }

  zuid = nultwee;
  west = eendrie;
  noord = nultwee;
  oost = eendrie;

  if (nultwee > eendrie)
    return 0;
  else
    return 1;
}

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
      int hoogstopgegooid = -1;
      int ligtaan = beurt;

      // Check wat de hoogste opgegooide troef was
      for (int i = 0; i < aantalspelers; i++) {
        if (istroef(opgegooidekaarten[i])) {
          if (opgegooidekaarten[i] > hoogstopgegooid) {
            hoogstopgegooid = opgegooidekaarten[i];
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

int montecarlomove(int kaarten[aantalkaarten], int opgegooid[aantalspelers + 2], int handje, int beut, int huidigespeler) {

}

int randommove(int kaarten[aantalkaarten], int opgegooid[aantalspelers + 2], int handje, int beurt, int huidigespeler) {
  int mogelijkekaarten[aantalkaarten];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - handje;

  geefmogelijkheden(opgegooid, maxkaart, beurt, huidigespeler, kaarten, mogelijkekaarten, aantalmogelijkheden);

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
  int opgegooid[aantalhandjes][aantalspelers + 2];

  parseargv(argc, argv, spelers);
  srand(time(NULL));

  deelkaarten(zuid, west, noord, oost);
  printkaarten(zuid, west, noord, oost);

  deelrestkaarten(zuid, NULL, NULL, NULL);

  for (int i = 0; i < aantalhandjes; i++)
    for (int j = 0; j < aantalspelers + 2; j++)
      opgegooid[i][j] = -1;

  while (handje < aantalhandjes) {
    for (int i = 0; i < aantalspelers; i++) {
      int waarde = -1;

      cout << "Speler " << huidigespeler << " aan de beurt, " << beurt << " komt uit. " << endl;

      if (spelers[huidigespeler] == 0)
        while (waarde == -1)
          waarde = usermove(spelerskaarten[huidigespeler], handje);
      else if (spelers[huidigespeler] == 1) {
        waarde = montecarlomove(spelerskaarten[huidigespeler], opgegooid[handje], handje, beurt, huidigespeler);
        cout << "Monte Carlo heeft " << Kaarten(waarde) << " opgegooid." << endl << endl;
      }
      else {
        waarde = randommove(spelerskaarten[huidigespeler], opgegooid[handje], handje, beurt, huidigespeler);
        cout << "Random heeft " << Kaarten(waarde) << " opgegooid." << endl << endl;
      }
      
      opgegooid[handje][huidigespeler] = waarde;
      huidigespeler = (huidigespeler + 1) % aantalspelers;
    }
    cout << "Winnaar: " << winnaar(opgegooid[handje], beurt) << endl;

    beurt = winnaar(opgegooid[handje], beurt);
    opgegooid[handje][aantalspelers] = beurt;
    opgegooid[handje][aantalspelers + 1] = waardeerkaarten(opgegooid[handje]);
    
    if (handje == aantalhandjes - 1)
      opgegooid[handje][aantalspelers + 1] = opgegooid[handje][aantalspelers + 1] + 10;

    printspel(opgegooid);
    huidigespeler = beurt;
    handje++;
  }

  // Printen wie uiteindelijk met hoeveel punten gewonnen heeft
  int z = 0, w = 0, n = 0, o = 0;

  if (totaalwinnaar(opgegooid, z, w, n, o) == 0)
    cout << "0 en 2 hebben gewonnen met " << z << " punten!" << " 1 en 3 hadden er " << w << endl;
  else
    cout << "1 en 3 hebben gewonnen met " << w << " punten!" << " 0 en 2 hadden er " << z << endl;

  return 0;
}