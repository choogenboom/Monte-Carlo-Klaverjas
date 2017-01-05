#include "klaverjas.h"
#include "spelers.h"
#include "montecarlo.h"
#include "speelpas.h"
#include "competitie.h"

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

void parseargv(int argc, char* argv[], int spelers[aantalspelers], int &komtuit, 
               bool &experiment, bool &competitite, bool &file, string &filename, int &seed) {
  if (argc == aantalspelers + 1) {
    for (int i = 0; i < aantalspelers; i++) {
      spelers[i] = atoi(argv[i + 1]);
    }
  }
  if (argc < 2) {
    cout << "Monte Carlo Klaverjas" << endl
         << "---------------------" << endl << endl
         << "Gebruik: "
         << argv[0] << " # # # # " << endl
         << "Met als # een getal als spelertype. Mogelijke spelertypes:" << endl
         << " - 0: Menselijke speler, kaart moet gekozen worden" << endl
         << " - 1: Monte Carlo speler met semirandomspeler potjes en kansen" << endl
         << " - 2: Monte Carlo speler met semirandom potjes maar zonder kansen" << endl
         << " - 3: Monte Carlo speler met volledig random potjes" << endl
         << " - 4: Semi-random speler" << endl
         << " - 5: Monte Carlo speler met kansverdeling" << endl
         << " - 6: Volledig random speler" << endl << endl
         << "Ook kan een bestand ingelezen met -f, bijvoorbeeld: " << endl
         << argv[0] << " -f voorbeeld.kvj" << endl
         << "Met -e wordt de output geminimaliseerd voor experimenten." << endl
         << "Als -im wordt opgegeven wordt alleen INT_MAX geoutput en afgesloten." << endl;
    exit(0);
  }
  else if (argc == 3 && argv[1] == string("-f")) {
    file = true;
    filename = argv[2];
  }
  else if (argv[1] == string("-e")) {
    if (argc >= aantalspelers + 3) {
      experiment = true;
      seed = atoi(argv[6]);
    }
    else {
      // Random seed 
      experiment = true;
      seed = time(NULL);
    }

    for (int i = 0; i < aantalspelers; i++) {
      spelers[i] = atoi(argv[i + 2]);
    }

    if (argc > aantalspelers + 3) {
      komtuit = atoi(argv[7]);
    }
  }
  else if (argv[1] == string("-c")) {
    // Competitie
    competitite = true;

    if (argc == 7) {
      // laatste arg is komtuit
      for (int i = 0; i < aantalspelers; i++)
        spelers[i] = atoi(argv[i + 2]);

      komtuit = atoi(argv[6]);
    }
    if (argc == 6) {
      // Argumenten zijn alleen de spelers
      for (int i = 0; i < aantalspelers; i++)
        spelers[i] = atoi(argv[i + 2]);
    }
  }
  else if (argv[1] == string("-im")) {
    cout << UINT_MAX << endl;
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
  ifstream bestand(filename.c_str());
  string regel;
  int regels = 0;
  int opgegooidregels = 0;
  int speelt = -1;
  int troefkleur = -1;

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
            speler = atoi(substring.c_str());
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
          troef = atoi(substring.c_str());
          troefkleur = troef;
          iss >> substring;
          speelt = atoi(substring.c_str());
          iss >> substring;
          komtuit = atoi(substring.c_str());
        }
        catch (exception const & e) {
          cout << "Error in bestand " << filename << ": troefkleur geen int" << endl;
          return false;
        }

        opgegooid[aantalslagen][aantalspelers] = speelt;
        opgegooid[aantalslagen][aantalspelers + 1] = troefkleur;
        opgegooid[0][aantalspelers] = komtuit;
      }
      else if (regels > 0 && regels < 5) {
        // spelerskaarten initieren
        for (int i = 0; i < aantalkaarten; i++) {
          string substring;
          int kaart;

          iss >> substring;

          try {
            kaart = atoi(substring.c_str());
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
          slag = atoi(regel.c_str());
        }
        catch (exception const & e) {
          cout << "Error in regel " << regels << " van bestand " << filename << endl
               << "Character " << regel << " kon niet omgezet worden naar int" << endl;
          return false;
        }
      }
      else {
        // opgegooid initieren
        if (opgegooidregels > aantalslagen) {
          cout << "Error in regel " << regels << " van bestand " << filename << endl
               << "Teveel regels voor opgegooid" << endl;
          return false;
        }
        
        string substring;
        iss >> substring;
        
        try {
          opgegooid[opgegooidregels][0] = atoi(substring.c_str());
          iss >> substring;
          opgegooid[opgegooidregels][1] = atoi(substring.c_str());
          iss >> substring;
          opgegooid[opgegooidregels][2] = atoi(substring.c_str());
          iss >> substring;
          opgegooid[opgegooidregels][3] = atoi(substring.c_str());
        }
        catch (exception const & e) {
          cout << "Error in regel " << regels << ":"  << " van bestand " << filename << endl
               << "Character " << substring << " kon niet omgezet worden naar int" << endl;
        }

        // Na de regel kunnen we winnaar & punten bepalen
        int slagwinnaar = winnaar(opgegooid[regels - 7], komtuit, troefkleur);
        // TODO: if-statement testen
        if (opgegooidregels < 7)
          opgegooid[opgegooidregels + 1][aantalspelers] = slagwinnaar;
        opgegooid[opgegooidregels][aantalspelers + 1] = slagwinnaar;
        opgegooid[opgegooidregels][aantalspelers + 2] = waardeerkaarten(opgegooid[regels - 7], aantalspelers, troefkleur, false);
        opgegooid[opgegooidregels][aantalspelers + 3] = geefroem(opgegooid[regels - 7], troefkleur, false);

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

string naarstring(int input) {
  stringstream ss;
  ss << input;

  return ss.str();
}

int kleurvankaart(int kaart) {
  if (kaart == -1)
    return -1;
  else
    return (kaart - (kaart % 10)) / 10;
}

bool istroef(int kaart, int opgegooid[aantalslagen + 1][aantalkolommen]) {
  return kleurvankaart(kaart) == opgegooid[aantalslagen][aantalspelers + 1];
}

bool istroef(int kaart, int troefkleur) {
  return kleurvankaart(kaart) == troefkleur;
}

bool gedekt(int kaart, int kaarten[]) {
  int kleur = kleurvankaart(kaart);

  for (int i = 0; i < aantalkaarten; i++) {
    if (kleurvankaart(kaarten[i]) == kleur && kaart != kaarten[i])
      return true;
  }

  return false;
}

// Obv 7 8 V H 10 A 9 B
int waardeerkaart(int kaart, int troefkleur) {
  int kleur = kleurvankaart(kaart);
  kaart = kaart % 10;

  if ((kleur != troefkleur) || kaart < 6) {
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
void insertionsort(int input[], int lengte) {
  for (int i = 1; i < lengte; i++) {
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

int winnaar(int kaarten[aantalspelers], int komtuit, int troefkleur) {
  int kleur = kleurvankaart(kaarten[komtuit]);  // De te bekennen kleur
  int hoogste[3] = {-1, -1, 0};               // Hoogste aantal punten, speler en of ingetroefd is

  for (int i = 0; i < aantalspelers; i++) {
    if (kleurvankaart(kaarten[i]) == kleur) {
      int kaartpunten = waardeerkaart(kaarten[i], troefkleur);

      // Als het aantal punten van de huidige kaart hoger is en de hoogste geen troef
      if (kaartpunten > hoogste[1] && !hoogste[2]) {
        hoogste[0] = i;
        hoogste[1] = kaartpunten;
        hoogste[2] = istroef(kaarten[i], troefkleur);
      }
      // Hoogste is troef,
      else if (hoogste[2] && istroef(kaarten[i], troefkleur)) {
        if (kaartpunten > hoogste[1]) {
          hoogste[0] = i;
          hoogste[1] = kaartpunten;
          hoogste[2] = 1;
        }
      }

    }
    // ingetroefd
    else if (istroef(kaarten[i], troefkleur)) {
      int kaartpunten = waardeerkaart(kaarten[i], troefkleur);

      // Hoogste kaart tot nu toe ook troef?
      if (istroef(kaarten[hoogste[0]], troefkleur)) {
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
  cout << endl << "Cards played:" << endl;

  for (int i = 0; i < aantalslagen; i++) {
    for (int j = 0; j < aantalspelers; j++) {
      cout << Kaarten(opgegooid[i][j]) << " ";
    }
    cout << " player " << opgegooid[i][aantalspelers + 1] << " won with " << opgegooid[i][aantalspelers + 2] << " points and "
         << opgegooid[i][aantalspelers + 3] << " roem. "
         << opgegooid[i][aantalspelers] << " led." << endl;
  }

  if (opgegooid[aantalslagen][0] != -1) {
    for (int i = 0; i < aantalspelers; i++)
      cout << "Player " << i << ":" << opgegooid[aantalslagen][i] << endl;
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
                      int slag, bool heeftniet[4][aantalspelers]) 
{
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];

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
          if (!istroef(kaart, troefkleur))
            heeftniet[troefkleur][j] = true;
        }
      }
    }
  }
}

/* Overzicht voor welke speler welke troefkaarten niet mogen hebben in de volgende format:
 * 
 *   |  7 8 V H 10 A 9 B
 * --|-------------
 * 0 |  x 
 * 1 |  y
 * 2 |  x
 * 3 |  q
 *
 * Waarin x, y, z en booleans zijn.
*/
void berekenheefttroefniet(int opgegooid[aantalslagen + 1][aantalkolommen],
                           int slag, bool heeftniet[aantalspelers][aantalkaarten]) {
  int hoogstetroef = -1;
    for (int i = 0; i < aantalspelers; i++) {
      for (int j = 0; j < aantalkaarten; j++) {
        heeftniet[i][j] = false;
      }
    }

  for (int i = 0; i <= slag; i++) {
    int komtuit = opgegooid[i][aantalspelers];
    for (int j = 0; j < aantalspelers; j++) {
      hoogstetroef = -1;
      int kaart = opgegooid[i][j];

      if (istroef(kaart, opgegooid)) {
        int k = komtuit;

        if (istroef(opgegooid[i][k], opgegooid))
          hoogstetroef = opgegooid[i][k];

        // Zoek de hoogst eerder opgegooide troef
        while (k != j) {
          if (istroef(opgegooid[i][k], opgegooid)) {
            if (opgegooid[i][k] > hoogstetroef)
              hoogstetroef = opgegooid[i][k];
          }
          k = (k + 1) % aantalspelers;
        }

        if (kaart < hoogstetroef) {
          int zonderkleur = hoogstetroef - (10 * kleurvankaart(hoogstetroef));
          for (int n = zonderkleur + 1; n < aantalkaarten; n++) {
            heeftniet[j][n] = true;
          }
        }
      }
    }
  }
}

bool checkdeling(int spelerskaarten[aantalspelers][aantalslagen], bool heeftniet[4][aantalspelers], 
                 bool heefttroefniet[aantalspelers][aantalkaarten], int maxkaart, int troefkleur) {
  for (int i = 0; i < aantalspelers; i++) {
    for (int j = 0; j < maxkaart; j++) {
      if (spelerskaarten[i][j] != -1) {
        if (heeftniet[kleurvankaart(spelerskaarten[i][j])][i])
          return false;
        if (istroef(spelerskaarten[i][j], troefkleur) && heefttroefniet[i][spelerskaarten[i][j] - 10 * troefkleur])
          return false;
      }
    }
  }

  return true;
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
bool checkstuk(int kaarten[aantalspelers], int troefkleur) {
  int troefvrouw = 10 * troefkleur + 5;
  int troefheer = troefvrouw + 1;

  for (int i = 0; i < aantalspelers - 1; i++) {
    if (kaarten[i] == troefvrouw)
      if (kaarten[i + 1] == troefheer)
        return true;
  }

  return false;
}

int checkroem(int originelekaarten[aantalspelers], int troefkleur) {
  int roem = 0;
  int kaarten[aantalspelers];

  // Als het 4 dezelfde kaarten zijn is het 100 roem
  if (checkvierdezelfde(originelekaarten))
    return 100;

  for (int i = 0; i < aantalspelers; i++) {
    kaarten[i] = roemvolgorde(originelekaarten[i]);
  }

  insertionsort(kaarten, aantalspelers);

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
  if (checkstuk(kaarten, troefkleur)) {
    roem += 20;
  }

  return roem;
}

int geefroem(int kaarten[aantalspelers], int troefkleur, bool output) {
  int roem = checkroem(kaarten, troefkleur);

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

int waardeerkaarten(int kaarten[], int maxkaart, int troefkleur, bool output) {
  int punten = 0;

  for (int i = 0; i < maxkaart; i++) {
    punten += waardeerkaart(kaarten[i], troefkleur);
  }

  if (output)
    cout << punten << " points." << endl;

  return punten;
}

// De boolean percentage is om in plaats van het aantal punten een percentage van de gewonnen
// punten te outputten.
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

void speelslag(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalkolommen],
  int spelerskaarten[aantalspelers][aantalkaarten], int slag, int &huidigespeler, 
  int komtuit, bool output, bool experiment, bool competitite) {
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];

  for (int i = 0; i < aantalspelers; i++) {
    if (competitite)
      schrijfbestanden(spelers, opgegooid, spelerskaarten, slag, "");

    // Als we halverwege de slag invallen kan een speler al aan de beurt zijn geweest
    if (opgegooid[slag][huidigespeler] == -1) {
      int waarde = -1;
      if (output)
        cout << "Player " << huidigespeler << " must play a card, " << komtuit << " has the elder hand. " << endl;

      if (spelers[huidigespeler] == 0) {
        while (waarde == -1)
          waarde = usermove(spelerskaarten[huidigespeler], slag);
      }
      else if (spelers[huidigespeler] == 1) {
        waarde = montecarlomove(spelerskaarten[huidigespeler], opgegooid, slag, komtuit, huidigespeler, 4, output, true, kans, experiment);
        if (output)
          cout << "Monte Carlo played " << Kaarten(waarde) << endl << endl;
      }
      else if (spelers[huidigespeler] == 2) {
        waarde = montecarlomove(spelerskaarten[huidigespeler], opgegooid, slag, komtuit, huidigespeler, 4, output, false, 0, experiment);
        if (output)
          cout << "Monte Carlo (deterministic) played " << Kaarten(waarde) << endl << endl;
      }
      else if (spelers[huidigespeler] == 3) {
        waarde = montecarlomove(spelerskaarten[huidigespeler], opgegooid, slag, komtuit, huidigespeler, 100, output, false, 0, experiment);
        if (output)
          cout << "Monte Carlo (fully random) played " << Kaarten(waarde) << endl << endl;
      }
      else if (spelers[huidigespeler] == 4) {
        waarde = semiramdommove(spelerskaarten[huidigespeler], opgegooid[slag], slag, komtuit, huidigespeler, troefkleur, output);
        if (output)
          cout << "Semirandom played " << Kaarten(waarde) << endl << endl;
      }
      else if (spelers[huidigespeler] == 5) {
        waarde = montecarlokansmove(spelerskaarten[huidigespeler], opgegooid, slag, komtuit, huidigespeler, 4, output);
        if (output)
          cout << "Monte Carlo (probability distribution) played " << Kaarten(waarde) << endl << endl;
      }
      else {
        waarde = randommove(spelerskaarten[huidigespeler], opgegooid[slag], slag, komtuit, huidigespeler, troefkleur, output);
        if (output)
          cout << "Random played " << Kaarten(waarde)  << endl << endl;
      }

      opgegooid[slag][huidigespeler] = waarde;
      huidigespeler = (huidigespeler + 1) % aantalspelers;
    }
  }
}

int speel(int spelers[aantalspelers], int opgegooid[aantalslagen + 1][aantalkolommen],
          int spelerskaarten[aantalspelers][aantalkaarten], int slag, int huidigespeler, 
          int komtuit, bool output, bool experiment, bool competitite) {
  
  int troefkleur = opgegooid[aantalslagen][aantalspelers + 1];
  
  while (slag < aantalslagen) {
    speelslag(spelers, opgegooid, spelerskaarten, slag, huidigespeler, komtuit, output, experiment, competitite);

    if (output)
      cout << winnaar(opgegooid[slag], komtuit, troefkleur) << " wins the trick" << endl;

    // Achter de spelers komt een nummer wie uitkwam, wie won en met hoeveel punten
    komtuit = winnaar(opgegooid[slag], komtuit, troefkleur);
    opgegooid[slag][aantalspelers + 1] = komtuit;
    opgegooid[slag][aantalspelers + 2] = waardeerkaarten(opgegooid[slag], aantalspelers, troefkleur, output);
    opgegooid[slag][aantalspelers + 3] = geefroem(opgegooid[slag], troefkleur, output);

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
      cout << "0 and 2 have won with " << opgegooid[aantalslagen][0] << " points!"
           << " 1 and 3 had " << opgegooid[aantalslagen][1] << endl;
    return 0;
  }
  else {
    if (output)
      cout << "1 and 3 have won with " << opgegooid[aantalslagen][1] << " punten!"
           << " 0 an 2 had " << opgegooid[aantalslagen][0] << endl;
    return 1;
  }
}

void bepaaltroef(int spelerskaarten[aantalspelers][aantalkaarten], int spelers[aantalspelers],
                 int opgegooid[aantalslagen + 1][aantalkolommen], int komtuit, bool output, bool competitite) {
  int speelt = -1;
  int i = 0;
  int troefkleur = rand() % 4;

  // Deze waarde in opgegooid wordt gebruikt om aan te geven als een speler verplicht moet.
  opgegooid[aantalslagen][aantalspelers + 2] = 0;
  while (speelt == -1) {
    int nieuwekleur = rand() % 4;

    if (i < 2) {
      while (nieuwekleur == troefkleur)
        nieuwekleur = rand() % 4;

      troefkleur = nieuwekleur;
      speelt = speelpasrondje(spelerskaarten, spelers, troefkleur, komtuit, competitite);
    }
    else {
      if (output)
        cout << "Verplicht!" << endl;
      speelt = komtuit;
      troefkleur = nieuwekleur;
      opgegooid[aantalslagen][aantalspelers + 2] = 1;
    }

    i++;
  }

  if (output)
    cout << speelt << " playes on " << Kleuren(troefkleur) << endl;

  opgegooid[aantalslagen][aantalspelers] = speelt;
  opgegooid[aantalslagen][aantalspelers + 1] = troefkleur;
}

int main(int argc, char* argv[]) {
  /*               Samenstelling van opgegooid-array:
   *  zuid   west   noord   oost  |  komtuit   gewonnen   punten   roem
   *  ...                         |
   *  ...                         |
   * -------------------------------------------------------------------
   *  z+n %  w+o %   z+n    w+o   |   speelt   troefkleur verplicht
  */
  int opgegooid[aantalslagen + 1][aantalkolommen];

  /*                Verdeling van de spelersvormen:
   * - 0: Menselijke speler, kaart moet gekozen worden
   * - 1: Monte Carlo speler met semirandomspeler potjes en kansen
   * - 2: Monte Carlo speler met semirandomspeler potjes en zonder kansen
   * - 3: Monte Carlo speler met volledig random potjes, zonder kans
   * - 4: Semi-random speler
   * - 5: Monte Carlo speler met kansverdeling
   * - 6: Volledig random speler
  */
  int spelers[aantalspelers];

  int spelerskaarten[aantalspelers][aantalkaarten];

  int komtuit = 0;
  // Deze variabelen zijn alleen voor als er gegevens van een bestand worden ingelezen
  bool output = true;
  bool file = false;
  bool experiment = false;
  bool competitite = false;
  int troef;
  int slag = 0;
  int huidigespeler = 0;
  int seed = time(NULL);
  string filename = "";

  parseargv(argc, argv, spelers, komtuit, experiment, competitite, file, filename, seed);
  srand(seed);

  opgegooid[0][aantalspelers] = -1;

  for (int i = 0; i < aantalslagen + 1; i++)
    for (int j = 0; j < aantalkolommen; j++)
      opgegooid[i][j] = -1;

  if (!file) {
    if (experiment)
      output = false;

    if (competitite) {
      speelcompetitie(spelers, komtuit);
      exit(0);
    }

    opgegooid[0][aantalspelers] = komtuit;

    deelkaarten(spelerskaarten);

    if (!experiment)
      printkaarten(spelerskaarten);

    huidigespeler = opgegooid[slag][aantalspelers];
    bepaaltroef(spelerskaarten, spelers, opgegooid, komtuit, output, false);
  }
  else {
    if (!leesbestand(filename, spelers, spelerskaarten, troef, opgegooid, slag, komtuit))
      return 0;

    if (opgegooid[0][aantalspelers] != komtuit) {
      for (int i = 0; i < aantalslagen + 1; i++) {
        for (int j = 0; j < aantalkolommen; j++) {
          opgegooid[i][j] = -1;      
        }
      }
    }

    if (troef != -1)
      opgegooid[aantalslagen][aantalspelers + 1] = troef;
    else
      bepaaltroef(spelerskaarten, spelers, opgegooid, komtuit, output, false);

    huidigespeler = opgegooid[slag][aantalspelers];
    komtuit = opgegooid[slag][aantalspelers];
    printkaarten(spelerskaarten);
    printspel(opgegooid);
  }

  speel(spelers, opgegooid, spelerskaarten, slag, huidigespeler, komtuit, output, experiment, false);

  if (!experiment)
    printspel(opgegooid);
  else {
    cout << opgegooid[aantalslagen][0] << " " << opgegooid[aantalslagen][1] 
         << " " << opgegooid[aantalslagen][aantalspelers] << endl;
  }

  return 0;
}