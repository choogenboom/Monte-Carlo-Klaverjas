#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>

using namespace std;

const static int aantalspelers = 4;
const static int aantalslagen = 8;
const static int aantalkaarten = 8;
const static int aantalrandompotjes = 10000;
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

int speel(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalspelers + 3], 
          int spelerskaarten[aantalspelers][aantalkaarten],
          int slag, int huidigespeler, int komtuit, bool output);

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

void printkaarten(int spelerskaarten[aantalspelers][aantalkaarten]) {
  for (int i = 0; i < aantalspelers; i++) {
    cout << i << ": ";
    
    for (int j = 0; j < aantalkaarten; j++)
      cout << Kaarten(spelerskaarten[i][j]) << " ";
    cout << endl;
  }
}

int winnaar(int kaarten[aantalspelers], int komtuit) {
  int kleur = kleurvankaart(kaarten[komtuit]);  // De te bekennen kleur
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

void printspel(int opgegooid[aantalslagen + 1][aantalspelers + 3]) {
  cout << endl << "Opgegooide kaarten:" << endl;

  for (int i = 0; i < aantalslagen; i++) {
    for (int j = 0; j < aantalspelers; j++) {
      cout << Kaarten(opgegooid[i][j]) << " ";
    }
    cout << " speler " << opgegooid[i][aantalspelers + 1] << " met " << opgegooid[i][aantalspelers + 2] << " punten. " 
         << opgegooid[i][aantalspelers] << " kwam uit." << endl;
  }

  if (opgegooid[aantalslagen][0] != -1) {
    for (int i = 0; i < aantalspelers; i++)
      cout << "Speler " << i << ":" << opgegooid[aantalslagen][i] << endl;
  }
  cout << "---------------------------------------------" << endl;
}

void printarray(int input[], int maxkaart) {
  cout << "Kaarten: ";
  for (int i = 0; i < maxkaart; i++)
    cout << input[i] << " ";

  cout << endl;
}

int zoekelement(int element, int input[], int arraysize) {
  for (int i = 0; i < arraysize; i++)
    if (input[i] == element)
      return i;

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

void deelkaarten(int spelerskaarten[aantalspelers][aantalkaarten]) {
  int allekaarten[aantalkaarten * aantalspelers];
  // Initieer alle kaarten
  for (int i = 0; i < aantalkaarten * aantalspelers ; i++)
    allekaarten[i] = (10 * floor(i / aantalkaarten) + i % aantalkaarten);

  // Pak een willekeurige kaart uit allekaarten, haal deze uit de array en 
  // geef deze aan een speler.
  int maxkaart = aantalkaarten * aantalspelers;
  for (int i = 0; i < aantalspelers - 1; i++) {
    for (int j = 0; j < aantalkaarten; j++) {
      int randomkaart = rand() % (maxkaart - 1);
      spelerskaarten[i][j] = allekaarten[randomkaart];
      wisselelement(randomkaart, allekaarten, maxkaart - 1);
      maxkaart--;
    }
  }

  for (int j = 0; j < aantalkaarten; j++)
    spelerskaarten[3][j] = allekaarten[j];
}

void berekenheeftniet(int opgegooid[aantalslagen + 1][aantalspelers + 3], 
                      int slag, int komtuit, bool heeftniet[4][aantalspelers]) {
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
          if (!istroef(kaart))
            heeftniet[troefkleur][j] = true;
        }
      }
    }
  }

  // cout << "heeftniet: " << endl;
  // for (int i = 0; i < 4; i++) {
  //   for (int j = 0; j < aantalspelers; j++) {
  //     cout << heeftniet[i][j] << " ";
  //   }
  //   cout << endl;
  // }
}

bool checkdeling(int spelerskaarten[aantalspelers][aantalslagen], bool heeftniet[4][aantalspelers], int maxkaart) {
  for (int i = 0; i < aantalspelers; i++) {
    for (int j = 0; j < maxkaart; j++) {
      if (spelerskaarten[i][j] != -1 && heeftniet[kleurvankaart(spelerskaarten[i][j])][i]) {
        return false;
      }
    }
  }

  return true;
}

void deelrestkaarten(int opgegooid[aantalslagen + 1][aantalspelers + 3], int slag, int komtuit, int huidigespeler, 
                     int spelerskaarten[aantalspelers][aantalkaarten]) {
  int allekaarten[aantalkaarten * aantalspelers];
  int maxkaart = aantalkaarten * aantalspelers;
  int aantalgedelete = 0;
  bool heeftniet[4][aantalspelers];
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
  berekenheeftniet(opgegooid, slag, komtuit, heeftniet);

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
    maxkaart = maxorig;

    for (int i = 1; i <= aantalspelers - 2; i++) {
      for (int j = aantalgedeeld[(huidigespeler + i) % 4]; j < aantalkaarten - slag - 1; j++) {
          int randomkaart = rand() % (maxkaart);
          spelerskaarten[(huidigespeler + i) % 4][j] = allekaarten[randomkaart];
          wisselelement(randomkaart, allekaarten, maxkaart - 1);
          maxkaart--;
      }
    }

    // Deel speler links van huidigespeler (+3 %4) de rest van de kaarten
    for (int i = aantalgedeeld[(huidigespeler + 3) % 4]; i < aantalkaarten - slag - 1; i++) {
        int randomkaart = rand() % (aantalkaarten - slag - i);

        spelerskaarten[(huidigespeler + 3) % 4][i] = allekaarten[randomkaart];
        wisselelement(randomkaart, allekaarten, maxkaart - 1);
        maxkaart--;
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

    goededeling = checkdeling(spelerskaarten, heeftniet, aantalkaarten - slag);
    aantaldelingen++;
  }

  // cout << aantaldelingen << " delingen nodig gehad." << endl;
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

int waardeerkaarten(int kaarten[aantalspelers], bool output) {
  int punten = 0;
  int roem = 0;

  for (int i = 0; i < aantalspelers; i++) {
    punten += waardeerkaart(kaarten[i]);
  }

  if (output)
    cout << punten << " punten";
  if (metroem) {
    roem = checkroem(kaarten);
    if (output)
      cout << " en " << roem << " roem." << endl;
  }
  else if (output)
    cout << "." << endl;

  return punten + roem;
}

int totaalwinnaar(int kaarten[aantalslagen][aantalspelers + 3]) {
  int nultwee = 0;
  int eendrie = 0;

  for (int i = 0; i < aantalkaarten; i++) {
    if (kaarten[i][aantalspelers + 1] == 0 || kaarten[i][aantalspelers + 1] == 2)
      nultwee += kaarten[i][aantalspelers + 2];
    else
      eendrie += kaarten[i][aantalspelers + 2];
  }

  kaarten[aantalslagen][0] = nultwee;
  kaarten[aantalslagen][1] = eendrie;
  kaarten[aantalslagen][2] = nultwee;
  kaarten[aantalslagen][3] = eendrie;

  if (nultwee > eendrie)
    return 0;
  else
    return 1;
}

/* Argumenten:
 * - opgegooid: Laatste kaarten die opgegooid zijn, waaraan kleur bekend moet worden
 * - maxkaart: Hoeveel kaarten de speler nog in zijn handen heeft
 * - komtuit: De speler die uitgekomen is, wiens kleur we moeten bekennen
 * - huidigespeler: De huidige speler (in dit geval dus computer). Alleen nodig voor Amsterdams
 * - mijnkaarten: Kaarten die de speler in zijn handen heeft  
 * - mogelijkekaarten: hierin worden de mogelijk opgegooide kaarten gereturnt
 * - aantalmogelijkheden: hierin wordt het aantal mogelijke kaarten gereturnt
 */
void geefmogelijkheden(int opgegooid[aantalspelers + 3], int maxkaart, int komtuit, 
                       int huidigespeler, int mijnkaarten[aantalkaarten], 
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
        if (istroef(opgegooid[i])) {
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

int randommove(int kaarten[aantalkaarten], int opgegooid[aantalspelers + 3], 
               int slag, int komtuit, int huidigespeler, bool output) {
  int mogelijkekaarten[aantalkaarten];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - slag;

  geefmogelijkheden(opgegooid, maxkaart, komtuit, huidigespeler, kaarten, mogelijkekaarten, aantalmogelijkheden);
  
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

int montecarlomove(int kaarten[aantalkaarten], int opgegooid[aantalslagen + 1][aantalspelers + 3], 
                   int slag, int komtuit, int huidigespeler) {
  int mogelijkekaarten[aantalkaarten];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - slag;
  int spelers[aantalspelers] = {2, 2, 2, 2};
  int kopie[aantalslagen + 1][aantalspelers + 3];
  int spelerskaarten[aantalspelers][aantalkaarten];

  geefmogelijkheden(opgegooid[slag], maxkaart, komtuit, huidigespeler, kaarten, mogelijkekaarten, aantalmogelijkheden);

  // Als er maar 1 mogelijkheid is moeten we deze doen.
  if (aantalmogelijkheden == 1) {
    deleteelement(mogelijkekaarten[0], kaarten, maxkaart);
    return mogelijkekaarten[0];
  }

  // Nu doen we aantalrandompotjes potjes voor elke mogelijke kaart
  int beste[2] = {-1, -1}; // beste[0] = beste kaart, beste [1] = aantal punten
  for (int i = 0; i < aantalmogelijkheden; i++) {
    int punten = 0;
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
        for (int l = 0; l < aantalspelers + 3; l++) {
          kopie[k][l] = opgegooid[k][l];
        }
      }

      // Doe de zet in de kopie
      deelrestkaarten(kopie, slag, komtuit, huidigespeler, spelerskaarten);
      kopie[slag][huidigespeler] = mogelijkekaarten[i];
      deleteelement(mogelijkekaarten[i], spelerskaarten[huidigespeler], maxkaart);
      // printkaarten(spelerskaarten);

      speel(spelers, kopie, spelerskaarten, slag, huidigespeler + 1, komtuit, false);

      punten += kopie[aantalslagen][huidigespeler];
    }
    cout << "Punten voor " << Kaarten(mogelijkekaarten[i]) << " is " << punten << endl;

    if (punten > beste[1]) {
      beste[0] = i;
      beste[1] = punten;
    }
  }

  deleteelement(mogelijkekaarten[beste[0]], kaarten, maxkaart);
  return mogelijkekaarten[beste[0]];
}

int usermove(int kaarten[aantalkaarten], int slag) {
  int input;
  int maxkaart = aantalkaarten - slag;

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

int speel(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalspelers + 3],
          int spelerskaarten[aantalspelers][aantalkaarten], int slag, int huidigespeler, int komtuit, bool output) {

  while (slag < aantalslagen) {
    for (int i = 0; i < aantalspelers; i++) {
      // Als we halverwege de slag invallen kan een speler al aan de beurt zijn geweest
      if (opgegooid[slag][huidigespeler] == -1) {
        int waarde = -1;

        if (output)
          cout << "Speler " << huidigespeler << " aan de beurt, " << komtuit << " komt uit. " << endl;

        if (spelers[huidigespeler] == 0)
          while (waarde == -1)
            waarde = usermove(spelerskaarten[huidigespeler], slag);
        else if (spelers[huidigespeler] == 1) {
          waarde = montecarlomove(spelerskaarten[huidigespeler], opgegooid, slag, komtuit, huidigespeler);
          if (output) {
            cout << "Monte Carlo heeft " << Kaarten(waarde) << " opgegooid." << endl << endl;
          }
        }
        else {
          waarde = randommove(spelerskaarten[huidigespeler], opgegooid[slag], slag, komtuit, huidigespeler, output);
          if (output)
            cout << "Random heeft " << Kaarten(waarde) << " opgegooid." << endl << endl;
        }
        
        opgegooid[slag][huidigespeler] = waarde;
        huidigespeler = (huidigespeler + 1) % aantalspelers;
      }
    }
    if (output)
      cout << "Winnaar: " << winnaar(opgegooid[slag], komtuit) << endl;

    // Achter de spelers komt een nummer wie uitkwam, wie won en met hoeveel punten
    komtuit = winnaar(opgegooid[slag], komtuit);
    opgegooid[slag][aantalspelers + 1] = komtuit;
    opgegooid[slag][aantalspelers + 2] = waardeerkaarten(opgegooid[slag], output);

    if (slag < aantalslagen - 1)
      opgegooid[slag + 1][aantalspelers] = komtuit;
    
    // 10 punten extra voor de laatste slag
    if (slag == aantalslagen - 1)
      opgegooid[slag][aantalspelers + 2] = opgegooid[slag][aantalspelers + 2] + 10;

    if (output) {
      printkaarten(spelerskaarten);
      printspel(opgegooid);
    }

    huidigespeler = komtuit;
    slag++;
  }

    // Printen wie uiteindelijk met hoeveel punten gewonnen heeft
  if (totaalwinnaar(opgegooid) == 0) {
    if (output)
      cout << "0 en 2 hebben gewonnen met " << opgegooid[aantalslagen][0] << " punten!" 
           << " 1 en 3 hadden er " << opgegooid[aantalslagen][1] << endl;
    return 0;
  }
  else {
    if (output)
      cout << "1 en 3 hebben gewonnen met " << opgegooid[aantalslagen][1] << " punten!" 
           << " 0 en 2 hadden er " << opgegooid[aantalslagen][0] << endl;
    return 1;
  }
}

int main(int argc, char* argv[]) {
  // int zuid[aantalkaarten], west[aantalkaarten], noord[aantalkaarten], oost[aantalkaarten];
  int spelerskaarten[aantalspelers][aantalkaarten];
  int spelers[aantalspelers];                            // Samenstelling van de spelers (mens/computer)
  int opgegooid[aantalslagen + 1][aantalspelers + 3];
  int komtuit = 0;

  parseargv(argc, argv, spelers);
  srand(time(NULL));

  deelkaarten(spelerskaarten);
  printkaarten(spelerskaarten);

  for (int i = 0; i < aantalslagen + 1; i++)
    for (int j = 0; j < aantalspelers + 3; j++)
      opgegooid[i][j] = -1;

  opgegooid[0][aantalspelers] = komtuit;

  speel(spelers, opgegooid, spelerskaarten, 0, 0, komtuit, true);
  printspel(opgegooid);
  return 0;
}