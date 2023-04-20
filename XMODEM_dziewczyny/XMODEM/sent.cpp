/*  autorzy: Magdalena Jóźwiak & Julita Włodarczyk   */

#include "const.h"

int PoliczCRC(char* wsk, int count) {
    int sumaKontrolnaCRC = 0;
    while (--count >= 0)
    {   /* Dopełnienie znaku ośmioma zerami. */
        sumaKontrolnaCRC = sumaKontrolnaCRC ^ (int)*wsk++ << 8;

        for (int i = 0; i < 8; ++i)
            /*  Jeśli lewy bit jest równy 1 wykonujemy operację XOR generatorem 1021.
                Jeśli jest równy 0, to XORujemy przez 0000. */
            if (sumaKontrolnaCRC & 0x8000) sumaKontrolnaCRC = sumaKontrolnaCRC << 1 ^ 0x1021;
            else sumaKontrolnaCRC = sumaKontrolnaCRC << 1;
    }
    return (sumaKontrolnaCRC & 0xFFFF); //1111 1111 1111 1111
}

int czyParzysty(int x, int y) {
    if (y == 0) return 1;
    if (y == 1) return x;

    int wynik = x;
    for (int i = 2; i <= y; i++) wynik = wynik * x;

    return wynik;
}

/* Przeliczanie sumy CRC na postać binarną. */
char PoliczCRC_Znaku(int n, int nrZnaku)
{
    int x, bin[16];
    for (int z = 0; z < 16; z++) bin[z] = 0;

    for (int i = 0; i < 16; i++) {
        x = n % 2;
        if (x == 1) n = (n - 1) / 2;
        if (x == 0) n = n / 2;
        bin[15 - i] = x;
    }

    //obliczamy poszczegolne znaki sumaKontrolnaCRC (1-szy lub 2-gi)
    x = 0;
    int k;

    if (nrZnaku == 1) k = 7;
    if (nrZnaku == 2) k = 15;

    for (int i = 0; i < 8; i++)
        x = x + czyParzysty(2, i) * bin[k - i];

    return (char)x;//zwraca 1 lub 2 znak (bo 2 znaki to 2 bajty, czyli 16 bitów)
}

int main()
{
    system("cls");
    cout << b_red << f_yellow << "  TELEKOMUNIKACJA - zad. 2  " << s_reset << endl << endl;
    cout << f_green << "NADAJNIK\n" << s_reset;
    ifstream plik;
    char znak;
    int licznikZnakow = 1;
    unsigned long rozmiarZnaku = sizeof(znak);
    int kod;

    bool transmisja = false;
    bool czyPoprawnyPakiet;
    int nrBloku = 1;
    char blok[128]; /* Plik dzielimy na bloki o długości 128 bajtów. */

    /* Otwieranie portu do transmisji. */
    cout << "Wybierz numer portu:" << endl
        << "[1] - COM1\n[2] - COM2\n[3] - COM3\n[4] - COM4\n(ustawienie domyslne: COM1)" << endl;

    switch (getchar()) {
    case '1': { numerPortu = L"COM1"; break; }
    case '2': { numerPortu = L"COM2"; break; }
    case '3': { numerPortu = L"COM3"; break; }
    case '4': { numerPortu = L"COM4"; break; }
    default: { numerPortu = L"COM1"; break; }
    }
    cout << f_green << numerPortu << " - Uruchamianie transferu...\n" << s_reset;
    if (ustawieniaPortu(numerPortu) == false)  return 0;

    cout << "\nOczekiwanie na zgode do transmisji... ";
    for (int i = 0; i < 6; i++) {
        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);

        if (znak == 'C') {
            cout << f_green << "OK.\n" << s_reset;
            kod = 1;
            transmisja = true;
            break;
        }
        else if (znak == NAK) {
            cout << f_yellow << "Brak zgody.\n" << s_reset;
            kod = 2;
            transmisja = true;
            break;
        }
    }
    if (!transmisja) exit(1);

    /* Transmisja pliku. */
    plik.open("wyslana.txt", ios::binary);
    while (!plik.eof())
    {
        /* Pusta tablica do zapisywania danych z jednego bloku. (char)26 - spacja */
        for (int i = 0; i < 128; i++) blok[i] = (char)26;

        int w = 0;

        while (w < 128 && !plik.eof()) {
            blok[w] = plik.get();
            if (plik.eof()) blok[w] = (char)26;
            w++;
        }
        czyPoprawnyPakiet = false;

        while (!czyPoprawnyPakiet)
        {
            cout << "Wysylanie pakietu... ";

            cout << f_cyan << nrBloku << s_reset;
            /* Wysłanie znaku początku nagłówka. */
            WriteFile(uchwytPortu, &SOH, licznikZnakow, &rozmiarZnaku, NULL);
            znak = (char)nrBloku;
            /* Wysłanie numeru bloku (1 bajt). */
            WriteFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
            znak = (char)255 - nrBloku;
            /* Wysłanie dopełnienia (255 - blok). */
            WriteFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);


            for (int i = 0; i < 128; i++) WriteFile(uchwytPortu, &blok[i], licznikZnakow, &rozmiarZnaku, NULL);
            if (kod == 2) //suma kontrolna
            {
                char suma_kontrolna = (char)26;
                for (int i = 0; i < 128; i++)
                    suma_kontrolna += blok[i] % 256;
                WriteFile(uchwytPortu, &suma_kontrolna, licznikZnakow, &rozmiarZnaku, NULL);
                cout << " Suma kontrolna = " << (int)suma_kontrolna << endl;
            }
            else if (kod == 1) //obliczanie CRC i transfer
            {
                tmpCRC = PoliczCRC(blok, 128);
                znak = PoliczCRC_Znaku(tmpCRC, 1);
                WriteFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
                znak = PoliczCRC_Znaku(tmpCRC, 2);
                WriteFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
            }

            while (1) {
                znak = ' ';
                ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);

                if (znak == ACK) {
                    czyPoprawnyPakiet = true;
                    cout << " pakiet przeslany poprawnie.\n";
                    break;
                }

                if (znak == NAK) {
                    cout << f_yellow << "Wysylanie pakietu anulowane (NAK).\n" << s_reset;
                    break;
                }

                if (znak == CAN) {
                    cout << f_red << "Polaczenie przerwane!\n" << s_reset;
                    return 1;
                }
            }
        }
        /* [ZABEZPIECZENIE] Jeśli nr bloku przekroczy 255 zaczynamy inkrementować od nowa. */
        if (nrBloku == 255) nrBloku = 1;
        else nrBloku++;
    }
    plik.close();

    /* Zakończenie transmisji. */
    while (1) {
        znak = EOT;
        WriteFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        if (znak == ACK) break;
    }
    CloseHandle(uchwytPortu);
    cout << f_green << "Transfer pliku zakonczony pomyslnie :)\n" << s_reset;

    return 0;
}
