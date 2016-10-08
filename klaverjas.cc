#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

const static int aantalspelers = 4;
const static int aantalslagen = 8;
const static int aantalkaarten = 8;
const static int aantalrandompotjes = 1000;
const static bool rotterdams = true;
const static bool metroem = true;

const static bool tienennietweggooien = true;
const static bool semirandom_bijmaat = true;
// aantal kolommen voor opgegooid
const static int aantalkolommen = aantalspelers + 4;
// TODO: troefkleur in class
int troefkleur;
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

ostream& operator<<(ostream& os, const Kleuren kleur) {
  string s;
  switch(kleur) {
    case (Schoppen):
      s = "♠";
      break;
    case (Harten):
      s = "\033[1;31m♥\033[0m";
      break;
    case (Klaver):
      s = "♣";
      break;
    case (Ruiten):
      s = "\033[1;31m♦\033[0m";
      break;
  }

  return os << s << " ";
}

int speel(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalkolommen],
          int spelerskaarten[aantalspelers][aantalkaarten],
          int slag, int huidigespeler, int komtuit, bool output);
int winnaar(int kaarten[aantalspelers], int komtuit);
int waardeerkaarten(int kaarten[], int maxkaart, bool output);
int geefroem(int kaarten[aantalspelers], bool output);
int zoekelement(int element, int input[], int arraysize);
void wisselelement(int element, int input[], int arraysize);
void deleteelement(int element, int input[], int arraysize);

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

void parseargv(int argc, char* argv[], int spelers[aantalspelers], bool &file, string &filename) {
  if (argc == aantalspelers + 1) {
    for (int i = 0; i < aantalspelers; i++) {
      spelers[i] = atoi(argv[i + 1]);
    }
  }
  else if (argc == 3 && argv[1] == string("-f")) {
    file = true;
    filename = argv[2];
  }
  else {
    cout << "Monte Carlo Klaverjas" << endl
         << "---------------------" << endl << endl
         << "Gebruik: "
         << argv[0] << " # # # # " << endl
         << "Met als # een getal als spelertype. Mogelijke spelertypes:" << endl
         << " - 0: Menselijke speler, kaart moet gekozen worden" << endl
         << " - 1: Monte Carlo speler met semirandomspeler potjes" << endl
         << " - 2: Semi-random speler" << endl
         << " - 3: Monte Carlo speler met volledig random potjes" << endl
         << " - 4: Volledig random speler" << endl << endl
         << "Ook kan een bestand ingelezen met -f, bijvoorbeeld: " << endl
         << argv[0] << " -f voorbeeld.kvj" << endl;
    exit(0);
  }
}

/* De structuur van een .kvj bestand is als volgt:
 * - eerste regel is de spelers[] array en bevat het type spelers (mens, random, MC)
 * - regels 2 t/m 5 bevatten de kaarten van de spelers
 * - regel 6 bevat de troefkleur, speelt en komtuit. -1 voor nog te bepalen
 * - regel 7 bevat slag
 * - regels 8 t/m (8 + slag - 1) bevat opgegooid[][]
*/
bool leesbestand(string filename, int spelers[aantalspelers], int spelerskaarten[aantalspelers][aantalkaarten], 
                 int &troef, int opgegooid[aantalspelers + 1][aantalkolommen], int &slag, int &komtuit) {
  ifstream bestand(filename);
  string regel;
  int regels = 0;
  int opgegooidregels = 0;
  int speelt = -1;

  if (bestand.is_open()) {
    while (getline(bestand, regel)) {
      istringstream iss(regel);

      if (regels == 0) {
        // spelers[] initieren

        for (int i = 0; i < aantalspelers; i++) {
          string substring;
          int speler;

          iss >> substring;
          
          try {
            speler = stoi(substring);
          }
          catch (exception const & e) {
            cout << "Error in regel " << regels << ":" << i << " van bestand " << filename << endl
                 << "Character " << substring << " kon niet omgezet worden naar int" << endl;

            return false;
          }

          spelers[i] = speler;
        }
      }
      else if (regels == 5) {
        // troefkleur bepalen, speelt en komtuit
        if (regel.length() != 1 && regel.length() != 5) {
          cout << "Error in regel " << regels << " van bestand " << filename << ", te veel karakters: " 
               << regel.length() << endl;
          return false;
        }

        string substring;
        iss >> substring;

        try {
          troef = stoi(substring);
          iss >> substring;
          speelt = stoi(substring);
          iss >> substring;
          komtuit = stoi(substring);
        }
        catch (exception const & e) {
          cout << "Error in bestand " << filename << ": troefkleur geen int" << endl;
          return false;
        }
      }
      else if (regels > 0 && regels < 5) {
        // spelerskaarten initieren
        for (int i = 0; i < aantalkaarten; i++) {
          string substring;
          int kaart;

          iss >> substring;

          try {
            kaart = stoi(substring);
          }
          catch (exception const & e) {
            cout << "Error in regel " << regels << ":" << i << " van bestand " << filename << endl
                 << "Character " << substring << " kon niet omgezet worden naar int" << endl;
            return false;
          }

          spelerskaarten[regels - 1][i] = kaart;
        }
      }
      else if (regels == 6) {
        // slag initieren
        try {
          slag = stoi(regel);
        }
        catch (exception const & e) {
          cout << "Error in regel " << regels << " van bestand " << filename << endl
               << "Character " << regel << " kon niet omgezet worden naar int" << endl;
          return false;
        }
      }
      else {
        // opgegooid initieren
        if (regels == 7) {
          // Komtuit in de eerste regel van opgegooid zetten
          opgegooid[0][aantalspelers] = komtuit;
        }
        if (opgegooidregels > aantalslagen) {
          cout << "Error in regel " << regels << " van bestand " << filename << endl
               << "Teveel regels voor opgegooid" << endl;
          return false;
        }
        
        for (int i = 0; i < slag; i ++) {
          string substring;
          iss >> substring;
          
          try {
            opgegooid[regels - 7][i] = stoi(substring);
          }
          catch (exception const & e) {
            cout << "Error in regel " << regels << ":" << i << " van bestand " << filename << endl
                 << "Character " << substring << " kon niet omgezet worden naar int" << endl;
          }
        }

        // Na de regel kunnen we winnaar & punten bepalen
        int slagwinnaar = winnaar(opgegooid[regels - 7], komtuit);
        opgegooid[regels - 6][aantalspelers] = slagwinnaar;
        opgegooid[regels - 7][aantalspelers + 1] = slagwinnaar;
        opgegooid[regels - 7][aantalspelers + 2] = waardeerkaarten(opgegooid[regels - 7], aantalspelers, false);
        opgegooid[regels - 7][aantalspelers + 3] = geefroem(opgegooid[regels - 7], false);
        opgegooid[aantalslagen][aantalspelers] = speelt;

        opgegooidregels++;
      }

      regels++;
    }
  }
  else
    return false;

  // Delete opgegooide kaarten uit spelerskaarten
  // int aantalgedelete = 0;
  for (int i = 0; i < slag; i++) {
    for (int j = 0; j < aantalspelers; j++) {
      if (opgegooid[i][j] != -1) {
        int index = zoekelement(opgegooid[i][j], spelerskaarten[j], aantalkaarten - i);

        spelerskaarten[j][index] = -1;
        wisselelement(index, spelerskaarten[j], aantalkaarten - i - 1);
      }
    }
  }

  return true;
}

int kleurvankaart(int kaart) {
  if (kaart == -1)
    return -1;
  else
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
  // Als de kaart nog niet opgegooid is maken we hem -2 zodat er geen roem mee valt
  if (kaart == -1)
    return -2;

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

int wiespeelt(int opgegooid[aantalslagen + 1][aantalkolommen]) {
  return opgegooid[aantalslagen][aantalspelers];
}

// We gebruiken insertion sort omdat dit goed presteert (en stabiel is)
// op rijen van kleine invoer. Onze invoer is normaalgesproken 4.
void insertionsort(int input[aantalspelers]) {
  for (int i = 1; i < aantalspelers; i++) {
    int hulp = input[i];
    int j = i - 1;

    while (j >= 0 && hulp < input[j]) {
      input[j + 1] = input[j];
      j--;
    }

    input[j + 1] = hulp;
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

void printspel(int opgegooid[aantalslagen + 1][aantalkolommen]) {
  cout << endl << "Opgegooide kaarten:" << endl;

  for (int i = 0; i < aantalslagen; i++) {
    for (int j = 0; j < aantalspelers; j++) {
      cout << Kaarten(opgegooid[i][j]) << " ";
    }
    cout << " speler " << opgegooid[i][aantalspelers + 1] << " met " << opgegooid[i][aantalspelers + 2] << " punten en "
         << opgegooid[i][aantalspelers + 3] << " roem. "
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

void berekenheeftniet(int opgegooid[aantalslagen + 1][aantalkolommen],
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

int deelrestkaarten(int opgegooid[aantalslagen + 1][aantalkolommen], int slag, int komtuit, int huidigespeler,
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

  return aantaldelingen;
}

bool checkvierdezelfde(int kaarten[aantalspelers]) {
  for (int i = 0; i < aantalspelers - 1; i++) {
    int kaart = kaarten[i] - 10 * kleurvankaart(kaarten[i]);

    if (kaart != kaarten[i + 1] - 10 * kleurvankaart(kaarten[i + 1]) || kaarten[i] == -1)
      return false;
  }

  return true;
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

  // Als het 4 dezelfde kaarten zijn is het 100 roem
  if (checkvierdezelfde(originelekaarten))
    return 100;

  for (int i = 0; i < aantalspelers; i++) {
    kaarten[i] = roemvolgorde(originelekaarten[i]);
  }

  insertionsort(kaarten);

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

int geefroem(int kaarten[aantalspelers], bool output) {
  int roem = checkroem(kaarten);

  if (roem != 0 && output)
    cout << roem << " roem." << endl;

  return roem;
}

int teamroem(int opgegooid[aantalslagen + 1][aantalkolommen], int speler) {
  int roem = 0;

  for (int i = 0; i < aantalslagen; i++) {
    if (opgegooid[i][aantalspelers + 1] == speler || opgegooid[i][aantalspelers + 1] == maat(speler)) {
      roem += opgegooid[i][aantalspelers + 3];
    }
  }

  return roem;
}

int waardeerkaarten(int kaarten[], int maxkaart, bool output) {
  int punten = 0;

  for (int i = 0; i < maxkaart; i++) {
    punten += waardeerkaart(kaarten[i]);
  }

  if (output)
    cout << punten << " punten." << endl;

  return punten;
}

int totaalwinnaar(int kaarten[aantalslagen + 1][aantalkolommen], bool percentage) {
  int nultwee = 0;
  int eendrie = 0;
  float nultweeperc = 0.0;
  float eendrieperc = 0.0;
  int nultweeroem = teamroem(kaarten, 0);
  int eendrieroem = teamroem(kaarten, 1);
  int slagen = 0; // We tellen alleen het aantal slagen voor 0+2 voor een pit

  for (int i = 0; i < aantalkaarten; i++) {
    if (kaarten[i][aantalspelers + 1] == 0 || kaarten[i][aantalspelers + 1] == 2) {
      nultwee += kaarten[i][aantalspelers + 2];
      slagen++;
    }
    else
      eendrie += kaarten[i][aantalspelers + 2];
  }

  if (wiespeelt(kaarten) == 0 || wiespeelt(kaarten) == 2) {
    // 0 en 2 spelen
    if ((nultwee + nultweeroem) < (eendrie + eendrieroem)) {
      // en gaan nat
      nultwee = 0;
      eendrie = 162 + nultweeroem + eendrieroem;
    }
    else {
      nultwee += nultweeroem;
      eendrie += eendrieroem;

      // Pit?
      if (slagen == aantalslagen)
        nultwee += 100;
    }
  }
  else {
    // 1 en 3 spelen
    if ((eendrie + eendrieroem) < nultwee + nultweeroem) {
      // en gaan nat
      eendrie = 0;
      nultwee = 162 + nultweeroem + eendrieroem;
    }
    else {
      nultwee += nultweeroem;
      eendrie += eendrieroem;

      // Pit?
      if (slagen == 0)
        eendrie += 100;
    }
  }

  if (percentage) {
    nultweeperc = (float) nultwee / (float)(nultwee + eendrie);
    eendrieperc = (float) eendrie / (float)(nultwee + eendrie);
    nultwee = nultweeperc * 100;
    eendrie = eendrieperc * 100;
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
void geefmogelijkheden(int opgegooid[aantalkolommen], int maxkaart, int komtuit,
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
                   int slag, int komtuit, int huidigespeler, bool output) {
  int mogelijkekaarten[aantalkaarten];
  int slechtekaarten[aantalkaarten];       // Domme kaarten, gooien roem bij tegenstander of punten weg
  int beterekaarten[aantalkaarten];        // Slimmere kaarten, leggen roem bij maat
  int kopiekaarten[aantalspelers];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - slag;
  int slagwinnaar = -1;
  int aantalslechte = 0;
  int aantalbetere = 0;
  int roemvoorkaart = checkroem(opgegooid);

  geefmogelijkheden(opgegooid, maxkaart, komtuit, huidigespeler, kaarten, mogelijkekaarten, aantalmogelijkheden);

cout << "Mogelijke kaarten: ";
for (int i = 0; i < aantalmogelijkheden; i++) {
  cout << Kaarten(mogelijkekaarten[i]);
}
cout << endl;

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
    slagwinnaar = winnaar(kopiekaarten, komtuit);

    if (!(slagwinnaar == huidigespeler || slagwinnaar == maat(huidigespeler))) {
      // Slag ligt NIET aan ons, geen roem of hoge weggooien.
      if ((huidigespeler == (komtuit + 3) % 4) && checkroem(kopiekaarten) > roemvoorkaart) {
        // We zijn de laatste speler, dus geen roem bijleggen
        if (output)
          cout << "Niet " << Kaarten(mogelijkekaarten[i]) << " in de roem leggen (4e)..." << endl;
        slechtekaarten[aantalslechte] = mogelijkekaarten[i];
        aantalslechte++;
      }
      else if ((huidigespeler == (komtuit + 2) % 4) && checkroem(kopiekaarten) > roemvoorkaart) {
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
      if ((huidigespeler == (komtuit + 3) % 4) && checkroem(kopiekaarten) > roemvoorkaart) {
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

int montecarlomove(int kaarten[aantalkaarten], int opgegooid[aantalslagen + 1][aantalkolommen],
                   int slag, int komtuit, int huidigespeler, int niveaurandom) {
  int mogelijkekaarten[aantalkaarten];
  int aantalmogelijkheden = 0;
  int maxkaart = aantalkaarten - slag;
  int spelers[aantalspelers] = {niveaurandom, niveaurandom, niveaurandom, niveaurandom};
  int kopie[aantalslagen + 1][aantalkolommen];
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

      // Doe de zet in de kopie
      delingen += deelrestkaarten(kopie, slag, komtuit, huidigespeler, spelerskaarten);
      kopie[slag][huidigespeler] = mogelijkekaarten[i];
      deleteelement(mogelijkekaarten[i], spelerskaarten[huidigespeler], maxkaart);
      // printkaarten(spelerskaarten);

      speel(spelers, kopie, spelerskaarten, slag, huidigespeler + 1, komtuit, false);

      punten += kopie[aantalslagen][huidigespeler];
    }

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

int speel(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalkolommen],
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
          waarde = montecarlomove(spelerskaarten[huidigespeler], opgegooid, slag, komtuit, huidigespeler, 2);
          if (output) {
            cout << "Monte Carlo heeft " << Kaarten(waarde) << " opgegooid." << endl << endl;
          }
        }
        else if (spelers[huidigespeler] == 2) {
          waarde = semiramdommove(spelerskaarten[huidigespeler], opgegooid[slag], slag, komtuit, huidigespeler, output);
          if (output)
            cout << "Semirandom heeft " << Kaarten(waarde) << " opgegooid." << endl << endl;
        }
        else if (spelers[huidigespeler] == 3) {
          waarde = montecarlomove(spelerskaarten[huidigespeler], opgegooid, slag, komtuit, huidigespeler, 4);
          if (output) {
            cout << "Monte Carlo heeft (volledig random) " << Kaarten(waarde) << " opgegooid." << endl << endl;
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
    opgegooid[slag][aantalspelers + 2] = waardeerkaarten(opgegooid[slag], aantalspelers, output);
    opgegooid[slag][aantalspelers + 3] = geefroem(opgegooid[slag], output);

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
  if (totaalwinnaar(opgegooid, false) == 0) {
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

// Speelt op basis van 1000 random potjes met deze hand
bool montecarlospeelt(int kaarten[aantalkaarten], int komtuit) {
  int niveaurandom = 2;
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

    // De rest van de kaarten worden willekeurig verdeeld
    deelrestkaarten(opgegooid, 0, komtuit, 0, spelerskaarten);
    opgegooid[aantalslagen][aantalspelers] = 0;

    speel(spelers, opgegooid, spelerskaarten, 0, komtuit, komtuit, false);
    wij += opgegooid[aantalslagen][0];
    zij += opgegooid[aantalslagen][1];
  }

  cout << "Punten: " << wij << " op " << Kleuren(troefkleur) << endl;
  return (wij > zij);
}

bool gedekt(int kaart, int kaarten[]) {
  int kleur = kleurvankaart(kaart);

  for (int i = 0; i < aantalkaarten; i++) {
    if (kleurvankaart(kaarten[i]) == kleur && kaart != kaarten[i])
      return true;
  }

  return false;
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
bool puntenspeelt(int kaarten[aantalkaarten]) {
  int punten = 0;

  for (int i = 0; i < aantalkaarten; i++) {
    int kaart = kaarten[i] - (10 * kleurvankaart(kaarten[i]));

    if (kaart == 5)
      punten += 3;
    else if (kaart == 4 && gedekt(kaarten[i], kaarten))
      punten += 2;
    else if (kaart == 3)
      punten += 1;

    if (istroef(kaarten[i])) {
      if (kaart == 7)
        punten += 5;
      else if (kaart == 6)
        punten += 4;
      else
        punten += 1;
    }
  }
  cout << punten << endl;
  return (punten > 10);
}

// Speelt op basis van aantal troeven
bool troefspeelt(int kaarten[aantalkaarten]) {
  int troeven = 0;
  int azen = 0;
  bool boer = false;
  bool nel = false;

  for (int i = 0; i < aantalkaarten; i++) {
    int kaart = kaarten[i] - (10 * kleurvankaart(kaarten[i]));
    if (istroef(kaarten[i])) {
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
    if (troeven > 2)
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

int speelpasrondje(int spelerskaarten[aantalspelers][aantalkaarten], int spelers[aantalspelers], int kleur, int komtuit) {
  int speelt = -1;

  for (int i = 0; i < aantalspelers; i++) {
    int maghetzeggen = (komtuit + i) % aantalspelers;

    if (spelers[maghetzeggen] == 0) {
      cout << maghetzeggen << ": " << endl;

      if (userspeelt(spelerskaarten[maghetzeggen], kleur))
        speelt = maghetzeggen;
    }
    else if (spelers[maghetzeggen] == 1) {
      if (montecarlospeelt(spelerskaarten[maghetzeggen], komtuit))
        speelt = maghetzeggen;
    }
    else {
      // if (troefspeelt(spelerskaarten[maghetzeggen]))
      if (puntenspeelt(spelerskaarten[maghetzeggen]))
        speelt = maghetzeggen;
    }

    if (speelt != -1)
      break;
  }

  return speelt;
}

void bepaaltroef(int spelerskaarten[aantalspelers][aantalkaarten], int spelers[aantalspelers],
                 int opgegooid[aantalslagen + 1][aantalkolommen], int komtuit) {
  int speelt = -1;
  int i = 0;
  troefkleur = rand() % 4;

  while (speelt == -1) {
    int nieuwekleur = rand() % 4;

    if (i < 2) {
      while (nieuwekleur == troefkleur)
        nieuwekleur = rand() % 4;

      troefkleur = nieuwekleur;
      speelt = speelpasrondje(spelerskaarten, spelers, troefkleur, komtuit);
    }
    else {
      cout << "Verplicht!" << endl;
      speelt = komtuit;
      troefkleur = nieuwekleur;
    }

    i++;
  }

  cout << speelt << " speelt op " << Kleuren(troefkleur) << endl;

  opgegooid[aantalslagen][aantalspelers] = speelt;
  opgegooid[aantalslagen][aantalspelers + 1] = troefkleur;
  // troefkleur = kleur;
}

int main(int argc, char* argv[]) {
  /*               Samenstelling van opgegooid-array:
   *  zuid   west   noord   oost  |  komtuit   gewonnen   punten   roem
   *  ...                         |
   *  ...                         |
   * -------------------------------------------------------------------
   *  z+n %  w+o %   z+n    w+o   |   speelt   troefkleur
  */
  int opgegooid[aantalslagen + 1][aantalkolommen];

  /*                Verdeling van de spelersvormen:
   * - 0: Menselijke speler, kaart moet gekozen worden
   * - 1: Monte Carlo speler met semirandomspeler potjes
   * - 2: Semi-random speler
   * - 3: Monte Carlo speler met volledig random potjes
   * - 4: Volledig random speler
  */
  int spelers[aantalspelers];

  int spelerskaarten[aantalspelers][aantalkaarten];

  int komtuit = 0;
  // Deze variabelen zijn alleen voor als er gegevens van een bestand worden ingelezen
  // int slag = 0;
  bool file = false;
  int troef;
  int slag = 0;
  int huidigespeler = 0;
  string filename = "";

  parseargv(argc, argv, spelers, file, filename);
  srand(time(NULL));

  opgegooid[0][aantalspelers] = -1;

  for (int i = 0; i < aantalslagen + 1; i++)
    for (int j = 0; j < aantalkolommen; j++)
      opgegooid[i][j] = -1;

  if (!file) {

    opgegooid[0][aantalspelers] = komtuit;

    deelkaarten(spelerskaarten);
    printkaarten(spelerskaarten);
    bepaaltroef(spelerskaarten, spelers, opgegooid, komtuit);
  }
  else {
    if (!leesbestand(filename, spelers, spelerskaarten, troef, opgegooid, slag, komtuit))
      return 0;

    if (troef != -1)
      troefkleur = troef;
    else
      bepaaltroef(spelerskaarten, spelers, opgegooid, komtuit);

    if (opgegooid[0][aantalspelers] != komtuit) {
      for (int i = 0; i < aantalslagen + 1; i++) {
        for (int j = 0; j < aantalkolommen; j++) {
          opgegooid[i][j] = -1;      
        }
      }
      
      opgegooid[0][aantalspelers] = komtuit;
    }

    huidigespeler = opgegooid[slag][aantalspelers];
    printkaarten(spelerskaarten);
  }

  speel(spelers, opgegooid, spelerskaarten, slag, huidigespeler, komtuit, true);
  printspel(opgegooid);
  return 0;
}