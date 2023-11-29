#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 900 // ręczne ustalenie max wymiarów obrazu
#define DL_LINII 1024
#define TRUE 1
#define FALSE 0

#define TRYB_PRACY_PROGRAMU 1 /*interaktywna - 1, testy - 0 */

/***************************************
# Autor: Michał Zawada
# Data wykonania: 22.11.2023
#
# Program do przetwarzania obrazów w formacie PGM.
#
# Modyfikacje: utworzenie interfejsu tekstowego, funkcja zapisz, połączenie z GitLab,
# funkcja zapisz, dodanie funkcji przetwarzających, programy i moduły testowe,
# sporządzenie dokumentacji,
#
***************************************/

/* czytaj: otwiera plik, pobiera jego metadane,
oraz kopiuje zawartość obrazu do tablicy dwuwymiarowej*/

int czytaj(FILE *plik_we, int obraz_pgm[][MAX], int *wymx, int *wymy, int *szarosci)
{
    char buf[DL_LINII];
    int znak;
    int koniec = 0;
    int i, j;

    if (plik_we == NULL)
    {
        fprintf(stderr, "Blad: Nie podano uchwytu do pliku\n");
        return (0);
    }

    if (fgets(buf, DL_LINII, plik_we) == NULL)
        koniec = 1;

    if ((buf[0] != 'P') || (buf[1] != '2') || koniec)
    {
        fprintf(stderr, "Blad: To nie jest plik PGM\n");
        return (0);
    }

    do
    {
        if ((znak = fgetc(plik_we)) == '#')
        {
            if (fgets(buf, DL_LINII, plik_we) == NULL)
                koniec = 1;
        }
        else
        {
            ungetc(znak, plik_we);
        }
    } while (znak == '#' && !koniec);

    if (fscanf(plik_we, "%d %d %d", wymx, wymy, szarosci) != 3)
    {
        fprintf(stderr, "Blad: Brak wymiarow obrazu lub liczby stopni szarosci\n");
        return (0);
    }

    if ((*wymx >= MAX) || (*wymy >= MAX))
    {
        fprintf(stderr, "Za duży plik wejsciowy!\n");
        plik_we = NULL;
        return 0;
    }
    /* Pobranie obrazu i zapisanie w tablicy obraz_pgm*/
    for (i = 0; i < *wymy; i++)
    {
        for (j = 0; j < *wymx; j++)
        {
            if (fscanf(plik_we, "%d", &(obraz_pgm[i][j])) != 1)
            {
                fprintf(stderr, "Blad: Niewlasciwe wymiary obrazu\n");
                return (0);
            }
        }
    }

    return *wymx * *wymy;
}

/* Wyswietlenie obrazu o zadanej nazwie za pomoca programu "display"   */
void wyswietl(char *n_pliku)
{
    char polecenie[DL_LINII];

    strcpy(polecenie, "display ");
    strcat(polecenie, n_pliku);
    strcat(polecenie, " &");
    printf("%s\n", polecenie);
    system(polecenie);
}

/*zapisanie pikseli do obrazu wynikowego*/
int zapisz(FILE *p_wynikowy, int tab[][MAX], int wym_x, int wym_y, int max_szarosc)
{
    int i, j;
    fprintf(p_wynikowy, "P2\n%d %d\n%d\n", wym_x, wym_y, max_szarosc);
    for (i = 0; i < wym_y; i++)
    {
        fprintf(p_wynikowy, "\n");
        for (j = 0; j < wym_x; j++)
        {
            fprintf(p_wynikowy, "%d ", tab[i][j]);
        }
    }
}

/* kopiuje tabele wejsciowa do wyjsciowej, nalezy okreslic wymiary tablicy*/

void kopiuj_tabele(int wym_x, int wym_y, int tab_wej[][MAX], int tab_wyj[][MAX])
{
    for (int i = 0; i < wym_y; i++)
    {
        for (int j = 0; j < wym_x; j++)
        {
            tab_wyj[i][j] = tab_wej[i][j];
        }
    }
}
//--------------------------------funkcje konwertujace obraz------------------------------

/*Negatyw Obrazu - wartosci pikseli zostają odwrócone*/
int negatyw(int tab[][MAX], int wym_x, int wym_y, int max_szarosc)
{
    int i, j;
    for (j = 0; j < wym_y; j++)
    {
        for (i = 0; i < wym_x; i++)
        {
            tab[j][i] = max_szarosc - tab[j][i];
        }
    }
    printf("Wykonano negatyw obrazu! Zapisz plik na dysku!\n");
}
/* Progowanie - piksele o szarosci mniejszej niż N% zostaja zamienione na CZARNE
        a piksele o szarosci wiekszej niż próg na BIAŁE */
int progowanie(int tab[][MAX], int wym_x, int wym_y, int max_szarosc, int wart_prog)
{
    int i, j;
    for (i = 0; i < wym_y; i++)
    {
        for (j = 0; j < wym_x; j++)
        {
            if (tab[i][j] <= wart_prog)
            {
                tab[i][j] = 0;
            }
            else
                tab[i][j] = max_szarosc;
        }
    }
    printf("Wykonano progowanie obrazu! Zapisz plik na dysku!\n");
}

void wyswietl_progi(int poziomow_szarosci)
{
    int progi[3] = {0, 0, 0};
    float szar = poziomow_szarosci;
    progi[0] = (szar * 0.25);
    progi[1] = (szar * 0.5);
    progi[2] = (szar * 0.75);
    printf(":Próg 25 procent - %d \n", progi[0]);
    printf(":Próg 50 procent - %d \n", progi[1]);
    printf(":Próg 75 procent - %d \n", progi[2]);
}

/*Konturowanie*/

int konturowanie(int tab[][MAX], int wym_x, int wym_y, int szarosc)
{
    int i, j;
    for (i = 1; i < wym_y; i++) // od 1 w pionie, aby nie czytać poza zakres tablicy
    {
        for (j = 0; j < wym_x - 1; j++)
        {
            int result = abs((tab[i + 1][j] - tab[i][j])) + abs((tab[i][j + 1]) - tab[i][j]);
            tab[i][j] = (result > szarosc ? szarosc : result); /*gdy wartosc obliczen przekroczy maksymalna, daj maksymalna szarosc */
        }
    }
}

/* Rozmywanie Pionowe o promieniu 1*/
int rozmywanie_Pionowe(int tab[][MAX], int wym_x, int wym_y, int szarosc, int promien)
{
    int i, j;
    for (i = 1; i < wym_y - 1; i++)
    {
        for (j = 0; j < wym_x; j++)
        {
            tab[i][j] = (int)((tab[i - promien][j] + tab[i][j] + tab[i + promien][j]) / 3); // rzutuj na int
        }
    }
}

int rozciaganie_histogramu(int tab[][MAX], int wym_x, int wym_y, int szarosc, int min, int max)
{
    int i, j;

    // znajdz wartosc min i max, odeslij jako parametry OUT

    min = szarosc;
    max = 0;

    for (i = 0; i < wym_y; i++)
    {
        for (j = 0; j < wym_x; j++)
        {
            if (tab[i][j] > max)
                max = tab[i][j];
            if (tab[i][j] < min)
                min = tab[i][j];
        }
    }

    // rozciagnij wartosci na całe spektrum

    for (i = 0; i < wym_y; i++)
    {
        for (j = 0; j < wym_x; j++)
        {
            tab[i][j] = (tab[i][j] - min) * (max / (max - min));
        }
    }
}

//----------------------------------------------------------------------------------------
int test_negacji()
{
    int obraz[MAX][MAX], obraz_wynikowy[MAX][MAX];
    int wymx, wymy, odcieni;

    int odczytano_pikseli = 0, zapisano_pikseli = 0;

    FILE *plik_oryginalny, *plik_wynikowy;
    plik_oryginalny = fopen("./Testy/obrazek_1", "r");
    char polecenie[100] = {"touch negacja_result.pgm &"};
    system(polecenie);
    plik_wynikowy = fopen("./Testy/negacja_result.pgm", "w");
    odczytano_pikseli = czytaj(plik_oryginalny, obraz, &wymx, &wymy, &odcieni);
    kopiuj_tabele(wymx, wymy, obraz, obraz_wynikowy);
    negatyw(obraz_wynikowy, wymx, wymy, odcieni);
    zapisano_pikseli = zapisz(plik_wynikowy, obraz_wynikowy, wymx, wymy, odcieni);
}

int test_konturowania()
{
    int obraz[MAX][MAX], obraz_wynikowy[MAX][MAX];
    int wymx, wymy, odcieni;
    int odczytano_pikseli = 0, zapisano_pikseli = 0;
    FILE *plik_oryginalny, *plik_wynikowy;
    plik_oryginalny = fopen("./Testy/kubus.pgm", "r");
    char polecenie[100] = {"touch konturowanie_result.pgm &"};
    system(polecenie);
    plik_wynikowy = fopen("./Testy/konturowanie_result.pgm", "w");
    odczytano_pikseli = czytaj(plik_oryginalny, obraz, &wymx, &wymy, &odcieni);
    kopiuj_tabele(wymx, wymy, obraz, obraz_wynikowy);
    konturowanie(obraz_wynikowy, wymx, wymy, odcieni);
    zapisano_pikseli = zapisz(plik_wynikowy, obraz_wynikowy, wymx, wymy, odcieni);
}

int test_rozmywania_pionowego()
{

    int promien = 3;

    int obraz[MAX][MAX], obraz_wynikowy[MAX][MAX];
    int wymx, wymy, odcieni;
    int odczytano_pikseli = 0, zapisano_pikseli = 0;
    FILE *plik_oryginalny, *plik_wynikowy;
    plik_oryginalny = fopen("./Testy/kubus.pgm", "r");
    char polecenie[100] = {"touch rozmywanie_result.pgm &"};
    system(polecenie);
    plik_wynikowy = fopen("./Testy/rozmywanie_result.pgm", "w");
    odczytano_pikseli = czytaj(plik_oryginalny, obraz, &wymx, &wymy, &odcieni);
    kopiuj_tabele(wymx, wymy, obraz, obraz_wynikowy);
    rozmywanie_Pionowe(obraz_wynikowy, wymx, wymy, odcieni, promien);
    zapisano_pikseli = zapisz(plik_wynikowy, obraz_wynikowy, wymx, wymy, odcieni);
}

int test_rozciagania_histogramu()
{
    int min, max;

    int obraz[MAX][MAX], obraz_wynikowy[MAX][MAX];
    int wymx, wymy, odcieni;
    int odczytano_pikseli = 0, zapisano_pikseli = 0;
    FILE *plik_oryginalny, *plik_wynikowy;
    plik_oryginalny = fopen("./Testy/kubus.pgm", "r");
    char polecenie[100] = {"touch rozciaganie_histogramu_result.pgm &"};
    system(polecenie);
    plik_wynikowy = fopen("./Testy/rozciaganie_histogramu_result.pgm", "w");
    odczytano_pikseli = czytaj(plik_oryginalny, obraz, &wymx, &wymy, &odcieni);
    kopiuj_tabele(wymx, wymy, obraz, obraz_wynikowy);
    rozciaganie_histogramu(obraz_wynikowy, wymx, wymy, odcieni, min, max);
    zapisano_pikseli = zapisz(plik_wynikowy, obraz_wynikowy, wymx, wymy, odcieni);
}

//-----------------------------------------------------------------------------------------
int main()
{

    int obraz[MAX][MAX], obraz_wynikowy[MAX][MAX];
    int wymx, wymy, odcieni;

    int min, max; // do rozciagania histogramu

    int odczytano_pikseli = 0, zapisano_pikseli = 0;
    FILE *plik_oryginalny, *plik_wynikowy;
    char nazwa_oryginalu[100], nazwa_wynikowego[100];

    int zakoncz_program = FALSE;
    int praca_rodzaj = TRYB_PRACY_PROGRAMU;

    /*zmienne sterujące*/
    char opcja[2], opcja_funkcje_przetwarzania[2];
    int go_menu_glowne;

    /*zmienne programow konwertujących*/
    int prog; /*prog w operacji progowania*/

    printf("-------------------------------------------\n");
    printf("Program do obróbki zdjęć w formacie PGM.   \n");
    printf("-------------------------------------------\n");

    if (!praca_rodzaj)
    {
        /* Test przetwarzania obrazów */

        test_negacji();
        test_konturowania();
        test_rozmywania_pionowego();
        test_rozciagania_histogramu();
    }

    do
    {
        if (!praca_rodzaj)
            break;

        if (odczytano_pikseli)
        {
            printf("-------------------------------------------\n");
            printf("Wczytany do programu obraz: %s \n", nazwa_oryginalu);
            printf("-------------------------------------------\n");
        }
        else
        {
            printf("-------------------------------------------\n");
            printf("Wczytaj obraz do programu!\n");
            printf("-------------------------------------------\n");
        }
        printf("1 - Wczytaj obraz do programu\n");
        printf("2 - Edytuj obraz\n");
        printf("3 - Zapisz obraz na dysku\n");
        printf("4 - Wyświetl obraz\n");
        printf("5 - Zakończ działanie\n");
        printf("Podaj numer czynności do wykonania!\n ");
        scanf("%s", opcja);

        switch (opcja[0])
        {

        case '1': /*Wczytaj obraz do programu*/
                  /* Wczytanie zawartosci wskazanego pliku do pamieci */
            printf(":T> Podaj nazwe pliku:\n");
            scanf("%s", nazwa_oryginalu);
            plik_oryginalny = fopen(nazwa_oryginalu, "r");
            if (plik_oryginalny != NULL)
            {
                printf(":T> Wczytano plik %s: \n", nazwa_oryginalu);
                odczytano_pikseli = czytaj(plik_oryginalny, obraz, &wymx, &wymy, &odcieni);
                fclose(plik_oryginalny);
            }
            else
            {
                fprintf(stderr, "Nieprawidłowy uchwyt do pliku!\n");
            }
            break;

        case '2': /*Edytuj obraz*/
            if (odczytano_pikseli == 0)
            {
                printf(":T> Blad: Najpierw wczytaj obraz!\n");
                continue;
            }
            printf("-------------------------------------------\n");
            printf(":T> Wszelkie operacje wykonywane są na KOPI a nie na orginale.\n");
            printf(":T> Zapisz na końcu efekty swojej pracy lub odrzuć zmainy!\n");
            printf("-------------------------------------------\n");

            kopiuj_tabele(wymx, wymy, obraz, obraz_wynikowy);
            printf(":T> Utworzono lokalną kopię pliku do modyfikacji!");

            do
            {
                go_menu_glowne = FALSE;
                printf("Dostepne opcje przetworzenia obrazu\n");

                printf("-----Operacje punktowe-----\n");
                printf("-> 1 Negatyw\n");
                printf("-> 2 Progowanie\n");
                printf("-> 3 Korekcja gamma\n");
                printf("-> 4 Zmiana poziomow\n");
                printf("-----Operacje lokalne------\n");
                printf("-> 5 Konturowanie\n");
                printf("-> 6 Rozmywanie\n");
                printf("-----Operacje globalbe-----\n");
                printf("-> 7 Zmiana poziomow\n");
                printf("-> 8 Rozciaganie histogramu\n");
                printf("-------------------------------------------\n");
                printf("*9 Powrót do menu głównego\n");

                scanf("%s", opcja_funkcje_przetwarzania);
                switch (opcja_funkcje_przetwarzania[0])
                {
                case '1': /*Negatyw*/

                    negatyw(obraz_wynikowy, wymx, wymy, odcieni);
                    go_menu_glowne = TRUE;
                    break;
                case '2': /*Progowanie*/
                    wyswietl_progi(odcieni);

                    printf("Podaj wartość progu: ");
                    scanf("%d", &prog);
                    progowanie(obraz_wynikowy, wymx, wymy, odcieni, prog);
                    go_menu_glowne = TRUE;
                    break;

                case '8': /*Rozciaganie histogramu*/

                    rozciaganie_histogramu(obraz_wynikowy, wymx, wymy, odcieni, min, max);
                    go_menu_glowne = TRUE;
                    break;

                case '5': /*konturowanie*/

                    konturowanie(obraz_wynikowy, wymx, wymy, odcieni);
                    go_menu_glowne = TRUE;
                    break;

                case '6': /*rozmywanie pionowe*/

                    rozmywanie_Pionowe(obraz_wynikowy, wymx, wymy, odcieni, 2);
                    go_menu_glowne = TRUE;
                    break;

                case '9': /*idz do menu glownego*/
                    go_menu_glowne = TRUE;
                    break;
                }

            } while (go_menu_glowne == FALSE);

            break;

        case '3': /*Zapisz obraz na dysku*/

            printf("Podaj nazwe pliku z rozszerzeniem PGM w katalogu roboczym: ");
            scanf("%s,", nazwa_wynikowego);

            char polecenie[100];
            strcpy(polecenie, "touch ");
            strcat(polecenie, nazwa_wynikowego);
            strcat(polecenie, " &");
            printf("%s\n", polecenie);
            system(polecenie);

            plik_wynikowy = fopen(nazwa_wynikowego, "w");
            if (plik_wynikowy == NULL)
            {
                printf(":T> Błąd: nie udalo sie utworzyć plik wynikowego na dysku!");
            }
            else
            {
                char tekst[] = "Helllo World!";
                zapisano_pikseli = zapisz(plik_wynikowy, obraz_wynikowy, wymx, wymy, odcieni);
            }
            fclose(plik_wynikowy);
            printf("Plik %s został zapisany\n", nazwa_wynikowego);
            break;
        case '4':
            /* Wyswietlenie poprawnie wczytanego obrazu zewnetrznym programem */
            if (odczytano_pikseli != 0)
                wyswietl(nazwa_oryginalu);
            break;

        case '5': /*Zakończ działanie*/
            zakoncz_program = TRUE;
            printf("Program kończy pracę!\n!");
            break;

        default:
            printf(":T> Nie rozpoznana opcja programu!\n");
            break;
        }

    } while (zakoncz_program == FALSE); /*menu główne programu*/



    return odczytano_pikseli;
}
