#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// definesc constantele pentru culor
#define COLOR_LIGHT_GREEN 1
#define COLOR_PURPLE 2
#define COLOR_CORAL_PINK 3
#define COLOR_LIGHT_BLUE 4
#define COLOR_LIGHT_PINK 5
#define COLOR_LIGHT_CYAN 6
#define COLOR_LIGHT_PEACH 7
#define COLOR_LIME_GREEN 8
#define COLOR_BRIGHT_YELLOW 9
#define COLOR_ORANGE 10
#define COLOR_GREY 11
#define COLOR_PINK 12
#define COLOR_LIGHT_RED 13

// definesc o structura pentru o celula din tabla pentru a retine numarul din
//  acea celula si daca trebuie colorata sau nu
struct casuta {
  int nr;
  int culoare;
};

struct casuta tabla[6][6];

// -=-=-=-=-=-=- NEW GAME -=-=-=-=-=-=-=

void init_tabla() {
  int i, j;
  for (i = 1; i <= 4; i++) {
    for (j = 1; j <= 4; j++) {
      // initializez fiecare celula ca avand valorile 0
      tabla[i][j].nr = 0;
      tabla[i][j].culoare = 0;
    }
  }
}

// -=-=-=-=-=-=- SAVE GAME -=-=-=-=-=-=-=

// o folosesc pentru a salva starea curenta a jocului intr-un fisier binar
void salvare_joc(const char *fisier, int *scor) {
  FILE *file = fopen(fisier, "wb");
  // verific daca fisierul a fost deschis, iar apoi scriu scorul si tabla in
  // fisier
  if (file != NULL) {
    fwrite(scor, sizeof(int), 1, file);
    fwrite(tabla, sizeof(struct casuta), 6 * 6, file);
    fclose(file);
  } else {
    printf("Eroare la salvarea jocului");
  }
}

// folosesc aceasta functie pentru a salva starea jocului in cazul in care
// doresc sa anulez o mutare
void salvare_inainte_de_mutare(int *scor) {
  salvare_joc("pentru_undo.bin", scor);
}

// -=-=-=-=-=-=- RESUME -=-=-=-=-=-=-=

// am facut aceasta functie pentru a putea incarca starea jocului salvat in
// fisierul binar
void reluare_joc(const char *fisier, int *scor) {
  FILE *file = fopen(fisier, "rb");
  if (file != NULL) {
    // citesc scorul si tabla din fisier
    fread(scor, sizeof(int), 1, file);
    fread(tabla, sizeof(struct casuta), 6 * 6, file);
    fclose(file);
  } else {
    printf("Eroare la incarcarea jocului salvat");
  }
}

// -=-=-=-=-=-=- GAME ENGINE -=-=-=-=-=-=-=

// functia responsabila de mutarea celuleor spre dreapta si calcularea scorului
// daca sunt cuplate doua valori
void mutare_dreapta(int *scor) {
  int linie, coloana;

  for (linie = 1; linie <= 4; linie++) {
    for (coloana = 4; coloana > 1; coloana--) {
      // combin celulele cu aceeași valoare, de la dreapta spre stanga doar
      // daca sunt diferite de 0
      if (tabla[linie][coloana].nr != 0) {
        int j = coloana - 1;
        // cauta prima celula diferita de zero dinspre stanga
        while (j >= 1 && tabla[linie][j].nr == 0) {
          j--;
        }
        if (j >= 1 && tabla[linie][coloana].nr == tabla[linie][j].nr) {
          tabla[linie][coloana].nr += tabla[linie][j].nr;
          *scor += tabla[linie][coloana].nr;
          // resetez valorile pentru celula cuplata din partea stanga
          tabla[linie][j].nr = 0;
          tabla[linie][j].culoare = 0;
        }
      }
    }

    // mut celulele diferite de 0 spre dreapta, verificand ce se afla in
    // fiecare celula in parte
    for (coloana = 4; coloana > 1; coloana--) {
      if (tabla[linie][coloana].nr == 0) {
        int j = coloana - 1;
        while (j >= 1 && tabla[linie][j].nr == 0) {
          j--;
        }
        if (j >= 1) {
          tabla[linie][coloana].nr = tabla[linie][j].nr;
          // resetez valorile pentru celula cuplata din partea stanga
          tabla[linie][j].nr = 0;
          tabla[linie][j].culoare = 0;
        }
      }
    }
  }
}

// functie care verifica daca o mutare este valida sau nu
int verifica_mutare_dreapta() {
  int linie, coloana;

  for (linie = 1; linie <= 4; ++linie) {
    bool exista_numere = false;
    // Verificăm dacă există numere pe coloana curentă in coloana specificata
    for (coloana = 3; coloana >= 1; --coloana) {
      // daca gasesc o celula egala cu 0, continui cautarea
      if (tabla[linie][coloana].nr == 0) {
        continue;
      }

      exista_numere = true;

      // verific dacă există o celulă liberă la dreapta, iar daca exista,
      // inseamna ca exista o mutare valida
      for (int k = coloana + 1; k <= 4; ++k) {
        if (tabla[linie][k].nr == 0) {
          return 1;
        }

        // verific dacă putem combina două numere si daca pot inseamna ca
        // exista o mutare valida
        if (tabla[linie][k].nr == tabla[linie][coloana].nr) {
          return 1;
        }

        break;
      }
    }

    // daca nu am găsit niciun număr pe coloana curentă, trecem la următoarea
    // coloana
    if (!exista_numere) {
      continue;
    }
  }
  // daca am parcurs toata matricea si nu gasesc nicio celul goala sau care
  // poate fi cuplata inseamna ca mutarea nu este valida
  return 0;
}

// functia responsabila de mutarea celuleor spre stanga si calcularea scorului
// daca sunt cuplate doua valori
void mutare_stanga(int *scor) {
  int linie, coloana;

  for (linie = 1; linie <= 4; linie++) {
    for (coloana = 1; coloana < 4; coloana++) {
      // combin celulele cu aceeași valoare, de la stanga spre dreapta doar
      // daca sunt diferite de 0
      if (tabla[linie][coloana].nr != 0) {
        int j = coloana + 1;
        // cauta prima celula diferita de zero dinspre dreapta
        while (j <= 4 && tabla[linie][j].nr == 0) {
          j++;
        }
        if (j <= 4 && tabla[linie][coloana].nr == tabla[linie][j].nr) {
          tabla[linie][coloana].nr += tabla[linie][j].nr;
          *scor += tabla[linie][coloana].nr;
          // resetez valorile pentru celula cuplata din partea dreapta
          tabla[linie][j].nr = 0;
          tabla[linie][j].culoare = 0;
        }
      }
    }

    // mut celulele diferite de 0 spre stanga, verificand ce se afla in
    // fiecare celula in parte
    for (coloana = 1; coloana < 4; coloana++) {
      if (tabla[linie][coloana].nr == 0) {
        int j = coloana + 1;
        while (j <= 4 && tabla[linie][j].nr == 0) {
          j++;
        }
        if (j <= 4) {
          tabla[linie][coloana].nr = tabla[linie][j].nr;
          // resetez valorile pentru celula cuplata din partea dreapta
          tabla[linie][j].nr = 0;
          tabla[linie][j].culoare = 0;
        }
      }
    }
  }
}

// functie care verifica daca o mutare este valida sau nu
int verifica_mutare_stanga() {
  int linie, coloana;
  for (linie = 1; linie <= 4; linie++) {
    bool exista_numere = false;
    // Verificăm dacă există numere pe coloana curentă in linia specificata
    for (coloana = 2; coloana <= 4; coloana++) {
      // daca gasesc o celula egala cu 0, continui cautarea
      if (tabla[linie][coloana].nr == 0) {
        continue;
      }

      exista_numere = true;
      int k;
      // verific dacă există o celulă liberă la stanga, iar daca exista,
      // inseamna ca exista o mutare valida
      for (k = coloana - 1; k >= 1; k--) {
        if (tabla[linie][k].nr == 0) {
          return 1;
        }

        // verific dacă putem combina două numere si daca pot inseamna ca
        // exista o mutare valida
        if (tabla[linie][k].nr == tabla[linie][coloana].nr) {
          return 1;
        }

        break;
      }
    }

    // daca nu am găsit niciun număr pe coloana curentă, trecem la următoarea
    // coloana
    if (!exista_numere) {
      continue;
    }
  }

  // daca am parcurs toata matricea si nu gasesc nicio celul goala sau care
  // poate fi cuplata inseamna ca mutarea nu este valida
  return 0;
}

// functia responsabila de mutarea celuleor spre jos si calcularea scorului
// daca sunt cuplate doua valori
void mutare_jos_spre_dracu_sa_ma_ia(int *scor) {
  int linie, coloana;

  for (coloana = 1; coloana <= 4; coloana++) {
    for (linie = 4; linie > 1; linie--) {
      // combin celulele cu aceeași valoare, de sus in jos doar daca sunt
      // diferite de 0
      if (tabla[linie][coloana].nr != 0) {
        int i = linie - 1;
        // cauta prima celula diferita de zero dinspre sus
        while (i >= 1 && tabla[i][coloana].nr == 0) {
          i--;
        }
        if (i >= 1 && tabla[linie][coloana].nr == tabla[i][coloana].nr) {
          tabla[linie][coloana].nr += tabla[i][coloana].nr;
          *scor += tabla[linie][coloana].nr;
          // resetez valorile pentru celula cuplata din partea de sus
          tabla[i][coloana].nr = 0;
          tabla[i][coloana].culoare = 0;
        }
      }
    }

    // mut celulele diferite de 0 spre jos, verificand ce se afla in
    // fiecare celula in parte
    for (linie = 4; linie > 1; linie--) {
      if (tabla[linie][coloana].nr == 0) {
        int i = linie - 1;
        while (i >= 1 && tabla[i][coloana].nr == 0) {
          i--;
        }
        if (i >= 1) {
          tabla[linie][coloana].nr = tabla[i][coloana].nr;
          // resetez valorile pentru celula cuplata din partea de sus
          tabla[i][coloana].nr = 0;
          tabla[i][coloana].culoare = 0;
        }
      }
    }
  }
}

// functie care verifica daca o mutare este valida sau nu
int verifica_mutare_jos() {
  int linie, coloana;

  for (coloana = 1; coloana <= 4; coloana++) {
    bool exista_numere = false;

    // Verificăm dacă există numere pe linia curentă in coloana specificata
    for (linie = 3; linie >= 1; linie--) {
      // daca gasesc o celula egala cu 0, continui cautarea in jos
      if (tabla[linie][coloana].nr == 0) {
        continue;
      }

      exista_numere = true;
      int k;
      // verific dacă există o celulă liberă sub celula curenta, iar daca
      // exista, inseamna ca exista o mutare valida
      for (k = linie + 1; k <= 4; k++) {
        if (tabla[k][coloana].nr == 0) {
          return 1;
        }

        // verific dacă putem combina două numere si daca pot inseamna ca
        // exista o mutare valida
        if (tabla[k][coloana].nr == tabla[linie][coloana].nr) {
          return 1;
        }

        break;
      }
    }

    // daca nu am găsit niciun număr pe linia curentă, trecem la următoarea
    // linie
    if (!exista_numere) {
      continue;
    }
  }

  // daca am parcurs toata matricea si nu gasesc nicio celul goala sau care
  // poate fi cuplata inseamna ca mutarea nu este valida
  return 0;
}

// functia responsabila de mutarea celuleor spre jos si calcularea scorului
// daca sunt cuplate doua valori
void mutare_sus_spre_doamne_doamne(int *scor) {
  int linie, coloana;

  for (coloana = 1; coloana <= 4; coloana++) {
    for (linie = 1; linie < 4; linie++) {
      // combin celulele cu aceeași valoare, de jos in sus doar daca sunt
      // diferite de 0
      if (tabla[linie][coloana].nr != 0) {
        int i = linie + 1;
        // cauta prima celula diferita de zero dinspre jos
        while (i <= 4 && tabla[i][coloana].nr == 0) {
          i++;
        }
        if (i <= 4 && tabla[linie][coloana].nr == tabla[i][coloana].nr) {
          tabla[linie][coloana].nr += tabla[i][coloana].nr;
          // resetez valorile pentru celula cuplata din partea de jos
          *scor += tabla[linie][coloana].nr;
          tabla[i][coloana].nr = 0;
          tabla[i][coloana].culoare = 0;
        }
      }
    }

    // mut celulele diferite de 0 spre sus, verificand ce se afla in
    // fiecare celula in parte
    for (linie = 1; linie < 4; linie++) {
      if (tabla[linie][coloana].nr == 0) {
        int i = linie + 1;
        while (i <= 4 && tabla[i][coloana].nr == 0) {
          i++;
        }
        if (i <= 4) {
          tabla[linie][coloana].nr = tabla[i][coloana].nr;
          // resetez valorile pentru celula cuplata din partea de jos
          tabla[i][coloana].nr = 0;
          tabla[i][coloana].culoare = 0;
        }
      }
    }
  }
}

// functie care verifica daca o mutare este valida sau nu
int verifica_mutare_sus() {
  int linie, coloana;

  for (coloana = 1; coloana <= 4; coloana++) {
    bool exista_numere = false;

    // Verificăm dacă există numere pe linia curentă in coloana specificata
    for (linie = 2; linie <= 4; linie++) {
      // daca gasesc o celula egala cu 0, continui cautarea in jos
      if (tabla[linie][coloana].nr == 0) {
        continue;
      }

      exista_numere = true;
      int k;
      // verific dacă există o celulă liberă deasupra celulei curenta, iar daca
      // exista, inseamna ca exista o mutare valida
      for (k = linie - 1; k >= 1; k--) {
        if (tabla[k][coloana].nr == 0) {
          return 1;
        }

        // verific dacă putem combina două numere si daca pot inseamna ca
        // exista o mutare valida
        if (tabla[k][coloana].nr == tabla[linie][coloana].nr) {
          return 1;
        }

        break;
      }
    }

    // daca nu am găsit niciun număr pe linia curentă, trecem la următoarea
    // linie
    if (!exista_numere) {
      continue;
    }
  }

  // daca am parcurs toata matricea si nu gasesc nicio celul goala sau care
  // poate fi cuplata inseamna ca mutarea nu este valida
  return 0;
}

// functia responsabila de mutarea automata si determina mutarea favorabila
int mutare_automata() {
  int scor = 0;
  int dreapta = 0, stanga = 0, sus = 0, jos = 0;

  // salvez si reiau jocul pentru a putea evalua care miscare creste mai mult
  // scorul
  salvare_joc("joc_salvat.bin", &scor);
  reluare_joc("joc_salvat.bin", &scor);

  mutare_stanga(&stanga);
  reluare_joc("joc_salvat.bin", &scor);
  mutare_dreapta(&dreapta);
  reluare_joc("joc_salvat.bin", &scor);
  mutare_sus_spre_doamne_doamne(&sus);
  reluare_joc("joc_salvat.bin", &scor);
  mutare_jos_spre_dracu_sa_ma_ia(&jos);
  reluare_joc("joc_salvat.bin", &scor);

  int directie_optima = 0;
  int max_scor = 0;

  // determin directia optima pe baza scorului maxim obtinut
  if (stanga > max_scor) {
    max_scor = stanga;
    directie_optima = 2;
  }

  if (dreapta > max_scor) {
    max_scor = dreapta;
    directie_optima = 1;
  }

  if (sus > max_scor) {
    max_scor = sus;
    directie_optima = 4;
  }

  if (jos > max_scor) {
    max_scor = jos;
    directie_optima = 3;
  }

  return directie_optima;
}

// functia responsabila pentru calcularea numarului de celule goale de pe
// fiecare linie si coloana
int casute_libere() {
  int linie, coloana;
  int celule_goale_stanga = 0;
  int celule_goale_dreapta = 0;
  int celule_goale_jos = 0;
  int celule_goale_sus = 0;

  // parcurg tabla si numar cate celule goale sunt pe fiecare linie si coloana
  for (linie = 1; linie <= 4; linie++) {
    for (coloana = 4; coloana >= 1; coloana--) {
      if (tabla[linie][coloana].nr == 0) {
        celule_goale_dreapta++;
      }
    }
  }

  for (coloana = 1; coloana <= 4; coloana++) {
    for (linie = 1; linie <= 4; linie++) {
      if (tabla[linie][coloana].nr == 0) {
        celule_goale_sus++;
      }
    }
  }

  for (coloana = 1; coloana <= 4; coloana++) {
    for (linie = 4; linie >= 1; linie--) {
      if (tabla[linie][coloana].nr == 0) {
        celule_goale_jos++;
      }
    }
  }

  int directie_optima = 0;
  // compar numarul de celule goale de pe fiecare linie si coloana si stabilesc
  // directia optima
  if (celule_goale_stanga >= celule_goale_dreapta &&
      celule_goale_stanga >= celule_goale_jos &&
      celule_goale_stanga >= celule_goale_sus) {
    directie_optima = 2;
  } else if (celule_goale_dreapta >= celule_goale_stanga &&
             celule_goale_dreapta >= celule_goale_jos &&
             celule_goale_dreapta >= celule_goale_sus) {
    directie_optima = 1;
  } else if (celule_goale_jos >= celule_goale_stanga &&
             celule_goale_jos >= celule_goale_dreapta &&
             celule_goale_jos >= celule_goale_sus) {
    directie_optima = 3;
  } else {
    directie_optima = 4;
  }

  return directie_optima;
}

void afisare();

// functia efectiva care dtermina cea mai buna mutare, tinand cont atat de
// marirea scorului cat si de numarul de celule goale
void muta_singur(int *scor) {
  // determin cea mai buna mutare folosind metoda pentru marirea scorului
  int cea_mai_buna_mutare = mutare_automata();

  // verifica daca mutarea determinata este intre 1 si 4
  if (cea_mai_buna_mutare == 1 || cea_mai_buna_mutare == 2 ||
      cea_mai_buna_mutare == 3 || cea_mai_buna_mutare == 4) {
    // efectueaza mutarea determinata
    switch (cea_mai_buna_mutare) {
      case 1:
        mutare_dreapta(scor);
        afisare(*scor);
        break;
      case 2:
        mutare_stanga(scor);
        afisare(*scor);
        break;
      case 3:
        mutare_jos_spre_dracu_sa_ma_ia(scor);
        afisare(*scor);
        break;
      case 4:
        mutare_sus_spre_doamne_doamne(scor);
        afisare(*scor);
        break;
      default:
        printw("Nu se poate realiza o mutare valida\n");
        break;
    }
  } else {
    // daca mutarea pe baza scorului nu este valida, determin cea mai buna
    // mutare pe baza numarului de casute libere
    int cea_mai_buna_mutare = casute_libere();
    if (cea_mai_buna_mutare == 1 || cea_mai_buna_mutare == 2 ||
        cea_mai_buna_mutare == 3 || cea_mai_buna_mutare == 4) {
      // efectueaza mutarea determinata
      switch (cea_mai_buna_mutare) {
        case 1:
          mutare_dreapta(scor);
          afisare(*scor);
          break;
        case 2:
          mutare_stanga(scor);
          afisare(*scor);
          break;
        case 3:
          mutare_jos_spre_dracu_sa_ma_ia(scor);
          afisare(*scor);
          break;
        case 4:
          mutare_sus_spre_doamne_doamne(scor);
          afisare(*scor);
          break;
        default:
          printw("Nu se poate realiza o mutare valida\n");
          break;
      }
    } else {
      // daca niciuna dintre mutarile anterioare nu sunt valide, atunci se va
      // alege o mutare aleatorie
      cea_mai_buna_mutare = rand() % 4 + 1;
      switch (cea_mai_buna_mutare) {
        case 1:
          mutare_dreapta(scor);
          afisare(*scor);
          break;
        case 2:
          mutare_stanga(scor);
          afisare(*scor);
          break;
        case 3:
          mutare_jos_spre_dracu_sa_ma_ia(scor);
          afisare(*scor);
          break;
        case 4:
          mutare_sus_spre_doamne_doamne(scor);
          afisare(*scor);
          break;
        default:
          printw("Nu se poate realiza o mutare valida\n");
          break;
      }
    }
  }
}

// -=-=-=-=-=-=- afisare -=-=-=-=-=-=-=

// functia initializeza culorile pentru design-ul jocului din terminal
void init_colors() {
  // verific daca terminalul suporta culori si inchid aplicatia daca nu
  if (!has_colors()) {
    endwin();
    printf("Terminalul nu suporta culori\n");
    exit(1);
  }

  // incep folosirea culorilor
  start_color();

  // initializez culorile folosind codurile RGB, adaptate pentru NCURSES
  // cu formula (x * 1000) / 255
  init_color(COLOR_LIGHT_GREEN, 600, 1000, 200);
  init_color(COLOR_PURPLE, 811.76, 623.52, 1000);
  init_color(COLOR_CORAL_PINK, 974, 513.72, 474.5);
  init_color(COLOR_LIGHT_BLUE, 529.41, 807.84, 980.39);
  init_color(COLOR_LIGHT_PINK, 1000, 400, 1000);
  init_color(COLOR_LIGHT_CYAN, 800, 1000, 1000);
  init_color(COLOR_LIGHT_PEACH, 1000, 854.9, 725.49);
  init_color(COLOR_LIME_GREEN, 337.25, 682.35, 207.84);
  init_color(COLOR_BRIGHT_YELLOW, 858.82, 792.15, 411.76);
  init_color(COLOR_ORANGE, 1000, 509.8, 3.92);
  init_color(COLOR_GREY, 725.49, 713.72, 725.49);
  init_color(COLOR_PINK, 1000, 411.76, 705.88);
  init_color(COLOR_LIGHT_RED, 1000, 278.43, 298.03);

  // initializez perechile de culori, numarul reprezinta numarul perechii,
  // prima culoare, culoarea scrisului, iar a doua culoare, culoarea fundalului
  init_pair(1, COLOR_BLACK, COLOR_LIGHT_GREEN);
  init_pair(2, COLOR_BLACK, COLOR_PURPLE);
  init_pair(3, COLOR_BLACK, COLOR_CORAL_PINK);
  init_pair(4, COLOR_BLACK, COLOR_LIGHT_BLUE);
  init_pair(5, COLOR_BLACK, COLOR_LIGHT_PINK);
  init_pair(6, COLOR_BLACK, COLOR_LIGHT_CYAN);
  init_pair(7, COLOR_BLACK, COLOR_LIGHT_PEACH);
  init_pair(8, COLOR_BLACK, COLOR_LIME_GREEN);
  init_pair(9, COLOR_BLACK, COLOR_BRIGHT_YELLOW);
  init_pair(10, COLOR_BLACK, COLOR_ORANGE);
  init_pair(11, COLOR_BLACK, COLOR_GREY);
  init_pair(12, COLOR_BLACK, COLOR_PINK);
  init_pair(13, COLOR_BLACK, COLOR_LIGHT_RED);
}

// afiseaza starea curenta a jocului in terminal alaturi de scor, data, timp
// si lista comenzilor valide
void afisare(int scor) {
  // initializez modul ncurses
  initscr();
  // initializez culorile folosinf functia definita anterior
  init_colors();
  // face ca tastatura sa functioneze in modul "chracter-break"
  cbreak();
  // dezactiveaza afisarea caracterelor tastate
  noecho();

  time_t timpulCurent;
  struct tm *info_timp;
  char timpBuf[80];

  int i, j;

  // preia datele despre timp si locatie
  time(&timpulCurent);
  info_timp = localtime(&timpulCurent);
  strftime(timpBuf, sizeof(timpBuf), "%d-%m-%Y %H:%M:%S", info_timp);

  // afiseaza tabla de joc, colorand fiecare numar in parte diferit, pentru a
  // facilita urmarirea mutarilor
  for (i = 1; i <= 4; i++) {
    printw("----");
  }
  printw("\n");
  for (i = 1; i <= 4; i++) {
    printw("|");
    for (j = 1; j <= 4; j++) {
      if (tabla[i][j].nr != 0) {
        if (tabla[i][j].nr == 2) {
          attron(COLOR_PAIR(1));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(1));
        } else if (tabla[i][j].nr == 4) {
          attron(COLOR_PAIR(2));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(2));
        } else if (tabla[i][j].nr == 8) {
          attron(COLOR_PAIR(3));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(3));
        } else if (tabla[i][j].nr == 16) {
          attron(COLOR_PAIR(4));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(4));
        } else if (tabla[i][j].nr == 32) {
          attron(COLOR_PAIR(5));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(5));
        } else if (tabla[i][j].nr == 64) {
          attron(COLOR_PAIR(6));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(6));
        } else if (tabla[i][j].nr == 128) {
          attron(COLOR_PAIR(7));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(7));
        } else if (tabla[i][j].nr == 256) {
          attron(COLOR_PAIR(8));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(8));
        } else if (tabla[i][j].nr == 512) {
          attron(COLOR_PAIR(9));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(9));
        } else if (tabla[i][j].nr == 1024) {
          attron(COLOR_PAIR(10));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(10));
        } else if (tabla[i][j].nr == 2048) {
          attron(COLOR_PAIR(11));
          printw(" %d ", tabla[i][j].nr);
          attroff(COLOR_PAIR(11));
        } else {
          printw(" %d ", tabla[i][j].nr);
        }
      } else {
        printw("   ");
      }
      printw("|");
    }
    printw("\n");
    if (i <= 3) {
      printw(" ");
      for (j = 1; j <= 4; j++) {
        printw("----");
      }
      printw("\n");
    }
  }
  printw(" ");
  for (i = 1; i <= 4; i++) {
    printw("----");
  }
  printw("\n");

  // afisez scorul si data si ora curenta
  printw("Scor %d", scor);
  printw("\nData si ora curenta: %s", timpBuf);

  printw("\n");
  printw("\n");

  // afisarea listei de comenzi valide
  attron(A_BOLD | COLOR_PAIR(12));
  printw("===LISTA DE COMENZI VALIDE===\n");
  attroff(A_BOLD | COLOR_PAIR(12));
  attron(COLOR_PAIR(4));
  printw("1. Tasta 'W' sau sageata sus = mutare in sus\n");
  printw("2. Tasta 'S' sau sageata jos = mutare in jos\n");
  printw("3. Tasta 'D' sau sageata dreapta = mutare la dreapta\n");
  printw("4. Tasta 'A' sau sageata stanga = mutare la stanga\n");
  printw("5. Tasta 'Q' = iesire joc si eventuala salvare\n");
  printw("6. Tasta 'Z' = anulare ultima mutare\n");
  attroff(COLOR_PAIR(4));

  printw("\n");
  printw("\n");

  refresh();

  endwin();
}

void numar_aleatoriu() {
  srand(time(NULL));

  // creez un vector pentru a reține toate pozițiile libere, 16 fiind numarul
  // maxim de pozitii libere intr-o matrice
  int pozitii_libere[16][2];
  int numar_pozitii_libere = 0;

  // identific pozițiile libere din matrice și le adăugăm în vector
  for (int i = 1; i <= 4; i++) {
    for (int j = 1; j <= 4; j++) {
      if (tabla[i][j].nr == 0) {
        pozitii_libere[numar_pozitii_libere][0] = i;
        pozitii_libere[numar_pozitii_libere][1] = j;
        numar_pozitii_libere++;
      }
    }
  }

  // aleg o poziție aleatorie dintre pozițiile libere, daca exista cel putin o
  // pozitie libera
  if (numar_pozitii_libere > 0) {
    // genereaza o pozitite random
    int pozitie_aleatorie = rand() % numar_pozitii_libere;
    // linia aleasa aleatoriu
    int l = pozitii_libere[pozitie_aleatorie][0];
    // coloana aleasa aleatoriu
    int c = pozitii_libere[pozitie_aleatorie][1];

    // aleg aleatoriu între 2 și 4 și adaugă în tabla, in functie de paritatea
    // numarului generat
    if (rand() % 2 == 0) {
      tabla[l][c].nr = 2;
    } else {
      tabla[l][c].nr = 4;
    }
  }
}

// functie pentru a stabili daca jocul este pierdut si returneaza 1 daca este
// pierdut si 0 daca mai exista mutari
int sfarsit_joc() {
  int i, j;

  // verific daca existe celule goale in tabla
  for (i = 1; i <= 4; i++) {
    for (j = 1; j <= 4; j++) {
      if (tabla[i][j].nr == 0) {
        return 0;
      }
    }
  }

  // verific daca am celule vecine care au aceeasi valoare
  for (i = 1; i <= 4; i++) {
    for (j = 1; j <= 4; j++) {
      // verificarea pe linii
      if ((j + 1 <= 4 && tabla[i][j].nr == tabla[i][j + 1].nr) ||
          (j - 1 >= 1 && tabla[i][j].nr == tabla[i][j - 1].nr)) {
        return 0;
      }

      // verificarea pe coloane
      if ((i + 1 <= 4 && tabla[i][j].nr == tabla[i + 1][j].nr) ||
          (i - 1 >= 1 && tabla[i][j].nr == tabla[i - 1][j].nr)) {
        return 0;
      }
    }
  }

  // daca nu au fost gasite nici celule goale nici celule vecine egale, atunci
  // jocul este pierdut
  return 1;
}

// -=-=-=-=-=-=- MENU -=-=-=-=-=-=-=

void meniu();

// functia responsabila de gestionarea actiunilor conform intrarilor din
// tastatura
void actiune(int *scor) {
  // setez un timeout in urma expirarii acestuia, incepe mutarea automata
  timeout(10000);

  // primeste comanda din tastatura
  int cmd = getch();
  // variabila necesara pentru a tine evidenta mutarilor
  bool mutat = false;

  // verifiarea primirii unei mutari valide
  if (cmd != ERR) {
    switch (cmd) {
      // pentru fiecare tasta apasata se apeleaza functia corespunzatoare
      // salvez inainte de a muta pentru a ma putea intoarce la mutarea
      // anterioara daca este necesar
      case KEY_LEFT:
      case 'a':
        if (verifica_mutare_stanga()) {
          salvare_inainte_de_mutare(scor);
          mutare_stanga(scor);
          mutat = TRUE;
          break;
        }
        break;
      case KEY_RIGHT:
      case 'd':
        if (verifica_mutare_dreapta()) {
          salvare_inainte_de_mutare(scor);
          mutare_dreapta(scor);
          mutat = TRUE;
          break;
        }
        break;
      case KEY_DOWN:
      case 's':
        if (verifica_mutare_jos()) {
          salvare_inainte_de_mutare(scor);
          mutare_jos_spre_dracu_sa_ma_ia(scor);
          mutat = TRUE;
          break;
        }
        break;
      case KEY_UP:
      case 'w':
        if (verifica_mutare_sus()) {
          salvare_inainte_de_mutare(scor);
          mutare_sus_spre_doamne_doamne(scor);
          mutat = TRUE;
          break;
        }
        break;
      // pentru a ma intoarce la miscarea anterioara
      case 'z':
        reluare_joc("pentru_undo.bin", scor);
        break;
      case 'q':
        // pentru a iesi din joc si pentru a salva starea jocului daca asta isi
        // doreste jucatorul
        printw("Vrei sa salvezi jocul ? (y/n)");
        refresh();
        int intrebare = getch();
        if (intrebare == 'y') {
          salvare_joc("joc_salvat.bin", scor);
          clear();
          printw("Joc salvat cu succes. Apasati orice tasta pentru a iesi.");
          refresh();
          getch();
          endwin();
          while (1) {
            meniu();
          }
          break;
        } else {
          endwin();
          while (1) {
            meniu();
          }
          break;
        }
      default:
        // pentru apasarea unei taste necunoscute, asteapta o tasta valida
        // pentru a relua jocul
        printw("Nu stiu ce ai apasat. Apasa o tasta valida.\n");
        getch();
        afisare(*scor);
        refresh();
        break;
    }
    // generarea unui numar random daca s-a produs o mutare
    if (mutat) {
      numar_aleatoriu();
    }
    clear();
    afisare(*scor);
    refresh();
  } else {
    // daca nu s-a primit nicio comanda de la jucator timp de 10s, atunci jocul
    // se va misca automat
    muta_singur(scor);
    numar_aleatoriu();
    clear();
    afisare(*scor);
    refresh();
  }
}

// afisarea meniului si umblarea prin acesta
void meniu() {
  int alegere = 1;
  // variabila pentru a mentine meniul afisat
  int continua = 1;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  start_color();

  // ciclu pentru afisarea meniu
  while (continua) {
    clear();
    // afiseaza titlului jocului cu o culoare specifica
    attron(COLOR_PAIR(2));
    printw("======Jocul 2048======\n");
    attroff(COLOR_PAIR(2));

    // afiseaza optiunilor meniului si evidentierea optiunii selectate
    if (alegere == 1) {
      attron(COLOR_PAIR(3));
      printw("-->1.New Game\n");
      attroff(COLOR_PAIR(3));
      printw(" 2.Resume\n");
      printw(" 3.Quit\n");
    } else if (alegere == 2) {
      printw(" 1.New Game\n");
      attron(COLOR_PAIR(3));
      printw("-->2.Resume\n");
      attroff(COLOR_PAIR(3));
      printw(" 3.Quit\n");
    } else if (alegere == 3) {
      printw(" 1.New Game\n");
      printw(" 2.Resume\n");
      attron(COLOR_PAIR(3));
      printw("-->3.Quit\n");
      attroff(COLOR_PAIR(3));
    }

    int ch = getch();

    // proceseaza comanda jucatorului pentru a schimba selectia sau pentru a
    // alege o optiune
    switch (ch) {
      case KEY_UP:
      case 'w':
        if (alegere > 1) alegere--;
        break;
      case KEY_DOWN:
      case 's':
        if (alegere < 3) alegere++;
        break;
      case 10:
        // executa actiunea corespunzatoare unei selectii
        if (alegere == 1) {
          int scor = 0;
          // afisez mesajul pentru crearea jocului timp de 2s
          printw("Se creeaza un joc nou. Succes!\n");
          refresh();
          napms(2000);

          // generez noua tabla alaturi de cele doua numere random
          init_tabla();
          numar_aleatoriu();
          numar_aleatoriu();

          clear();
          afisare(scor);

          // cat timp jocul nu este pierdut jucatorul poate folosi actiunile
          // valide
          while (!sfarsit_joc()) {
            refresh();
            // printw("caca\n");
            actiune(&scor);
          }

          if (sfarsit_joc()) {
            attron(A_BOLD | COLOR_PAIR(13));
            printw("Ai pierdut, boss! Apasa ceva ca sa te intorci la meniu.\n");
            attroff(A_BOLD | COLOR_PAIR(13));
          }

          getch();
        } else if (alegere == 2) {
          int scor = 0;
          printw("Se reia jocul.");
          refresh();
          napms(1500);

          // reiau jocul
          reluare_joc("joc_salvat.bin", &scor);
          clear();
          afisare(scor);
          refresh();

          // cat timp jocul nu este pierdut jucatorul poate folosi actiunile
          // valide
          while (!sfarsit_joc()) {
            refresh();
            // printw("caca\n");
            actiune(&scor);
          }

          if (sfarsit_joc()) {
            printw("Ai pierdut, boss! Apasa ceva ca sa te intorci la meniu.\n");
          }

          getch();
          break;
        } else if (alegere == 3) {
          // daca jucatorul alege functia "QUIT", atunci rularea prigramului
          // se incheie
          endwin();
          exit(0);
        }
        // variabila continua devine 0, pentru ca meniul sa dispara atunci cand
        // se incepe sau se reia un joc
        continua = 0;
        break;
      default:
        break;
    }
  }
}

int main() {
  // pentru a accepta intrari fara asteptare
  nodelay(stdscr, TRUE);
  // initializez ecranul si culorile
  init_tabla();
  initscr();
  init_colors();

  cbreak();
  noecho();

  // permite citirea tastelor speciale, adica a sagetilor
  keypad(stdscr, TRUE);

  // fac un ciclu pentru a mentine meniul jocului
  while (1) {
    meniu();
  }

  return 0;
}