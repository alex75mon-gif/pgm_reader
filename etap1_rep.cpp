#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Wymiary dopasowane do wybranego obrazu testowego (np. 512x512)
#define MAX_SZEROKOSC 512
#define MAX_WYSOKOSC 512
#define MAX_NAZWA 128

// Definica struktury obrazu PGM
typedef struct {
    char nazwa[MAX_NAZWA];                      // Nazwa identyfikacyjna obrazu
    char magic[3];                              // Magiczny numer (np. "P2")
    int szerokosc;                              // Szerokość obrazu w pikselach
    int wysokosc;                               // Wysokość obrazu w pikselach
    int max_val;                                // Maksymalna wartość jasności (np. 255)
    int piksele[MAX_WYSOKOSC][MAX_SZEROKOSC];   // Statyczna tablica pikseli
} ObrazPGM;

// Funkcja pomijająca białe znaki oraz komentarze (linie zaczynające się od '#')
void pomin_komentarze(FILE *plik) {
    int ch;
    while ((ch = fgetc(plik)) != EOF) {
        if (isspace(ch)) {
            continue;
        }
        if (ch == '#') {
            // Pomijaj wszystkie znaki do końca linii
            while ((ch = fgetc(plik)) != '\n' && ch != EOF);
        } else {
            // Zwróć znak do strumienia, jeśli nie jest komentarzem ani białym znakiem
            ungetc(ch, plik);
            break;
        }
    }
}

// Funkcja wczytująca obraz z pliku tekstowego do struktury
int wczytaj_obraz_pgm(const char *sciezka_pliku, ObrazPGM *obraz, const char *nazwa_identyfikacyjna) {
    FILE *plik = fopen(sciezka_pliku, "r");
    if (!plik) {
        printf("Błąd: Nie można otworzyć pliku %s\n", sciezka_pliku);
        return 0;
    }

    // Zapisananie nazwy identyfikacyjnej w strukturze
    strncpy(obraz->nazwa, nazwa_identyfikacyjna, MAX_NAZWA - 1);
    obraz->nazwa[MAX_NAZWA - 1] = '\0';

    // 1. Odczyt magicznego numeru (np. P2)
    pomin_komentarze(plik);
    if (fscanf(plik, "%2s", obraz->magic) != 1) {
        fclose(plik);
        return 0;
    }

    // Sprawdzenie czy plik jest formatu tekstowego P2
    if (strcmp(obraz->magic, "P2") != 0) {
        printf("Błąd: Oczekiwano formatu P2 (zastano %s)\n", obraz->magic);
        fclose(plik);
        return 0;
    }

    // 2. Odczyt wymiarów (szerokość, wysokość)
    pomin_komentarze(plik);
    fscanf(plik, "%d", &obraz->szerokosc);
    pomin_komentarze(plik);
    fscanf(plik, "%d", &obraz->wysokosc);

    // Weryfikacja czy wymiary mieści się w statycznej tablicy
    if (obraz->szerokosc > MAX_SZEROKOSC || obraz->wysokosc > MAX_WYSOKOSC) {
        printf("Błąd: Obraz o wymiarach %dx%d przekracza rozmiar statycznej tablicy (%dx%d).\n",
               obraz->szerokosc, obraz->wysokosc, MAX_SZEROKOSC, MAX_WYSOKOSC);
        fclose(plik);
        return 0;
    }

    // 3. Odczyt maksymalnej wartości szarości
    pomin_komentarze(plik);
    fscanf(plik, "%d", &obraz->max_val);

    // 4. Odczyt pikseli
    for (int i = 0; i < obraz->wysokosc; i++) {
        for (int j = 0; j < obraz->szerokosc; j++) {
            pomin_komentarze(plik);
            if (fscanf(plik, "%d", &obraz->piksele[i][j]) != 1) {
                printf("Błąd podczas odczytu piksela [%d][%d]\n", i, j);
                fclose(plik);
                return 0;
            }
        }
    }

    fclose(plik);
    return 1;
}

int main(void) {
    // Deklaracja zmiennej strukturalnej na obraz
    ObrazPGM moj_obraz;

    // Próba wczytania obrazu z pliku tekstowego
    if (wczytaj_obraz_pgm("test.pgm", &moj_obraz, "Obraz_Testowy_1")) {
        printf("Obraz wczytany pomyślnie!\n");
        printf("Identyfikator: %s\n", moj_obraz.nazwa);
        printf("Format: %s\n", moj_obraz.magic);
        printf("Wymiary: %dx%d\n", moj_obraz.szerokosc, moj_obraz.wysokosc);
        printf("Max szarość: %d\n", moj_obraz.max_val);
        printf("Wartość piksela [0][0]: %d\n", moj_obraz.piksele[0][0]);
    } else {
        printf("Nie udało się wczytać obrazu.\n");
    }

    return 0;
}