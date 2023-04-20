#include <fstream>
#include <iostream>
#include "const.h"

using namespace std;

ofstream plik;

char blokDanych[128];
char znak;
unsigned long rozmiarZnaku = sizeof(znak);
int licznikZnakow = 1;      //potrzebne przy czytaniu i pisaniu
bool transmisja = false;
bool poprawnyPakiet;
int nrBloku;
char dopelnienieDo255;
char sumaKontrolnaCRC[2];      //odebrane sumaKontrolnaCRC

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

int sumaKontrolna(byte blocks[128]) {
    int suma = 0;
    for (int i = 0; i < 128; i++)
    {
        suma += blocks[i] % 255;
    }

    return suma;
}
int czyParzysty(int x, int y) {
    if (y == 0) return 1;
    if (y == 1) return x;

    int wynik = x;
    for (int i = 2; i <= y; i++) wynik = wynik * x;

    return wynik;
}


char PoliczCRC_Znaku(int n, int nrZnaku) //przeliczanie CRC na postac binarna
{
    int x, binarna[16];

    for (int z = 0; z < 16; z++) binarna[z] = 0;

    for (int i = 0; i < 16; i++)
    {
        x = n % 2;
        if (x == 1) n = (n - 1) / 2;
        if (x == 0) n = n / 2;
        binarna[15 - i] = x;
    }

    //obliczamy poszczegolne znaki sumaKontrolnaCRC (1-szy lub 2-gi)
    x = 0;
    int k;

    if (nrZnaku == 1) k = 7;
    if (nrZnaku == 2) k = 15;

    for (int i = 0; i < 8; i++)
        x = x + czyParzysty(2, i) * binarna[k - i];

    return (char)x;//zwraca 1 lub 2 znak (bo 2 znaki to 2 bajty, czyli 16 bitów)
}

int main()
{
    system("cls");
    cout << "XMODEM - Receiver" << endl;
    numerPortu = L"COM2";
    cout << "Port: COM 2" << endl;

    if (ustawieniaPortu(numerPortu) == false)  return 0;

    cout << "Zgoda na transfer pliku" << endl
        << "[1] - tak (ACK)" << endl
        << "[2] - nie (NAK)" << endl;

    switch (getchar()) {
    case '1': {
        znak = C;
        break;
    }
    case '2': {
        znak = NAK;
        break;
    }
    default: {
        cout << "Ustawienie domyslne: ACK.\n";
        znak = C;
    }
    }

    for (int i = 0; i < 6; i++) {
        //HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED
        WriteFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        cout << "Oczekiwanie na komunikat SOH...\n";
        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        cout << (int)znak << endl;
        if (znak == SOH) {
            transmisja = true;
            break;
        }
    }

    if (!transmisja) {
        exit(1);
    }
    plik.open("odebrana.txt", ios::binary);
    cout << "Trwa odbieranie pliku, prosze czekac...\n";

    ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
    nrBloku = (int)znak;

    ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
    dopelnienieDo255 = znak;

    for (int i = 0; i < 128; i++) {
        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        blokDanych[i] = znak;
    }

    ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
    sumaKontrolnaCRC[0] = znak;
    ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
    sumaKontrolnaCRC[1] = znak;
    poprawnyPakiet = true;


    if ((char)(256 - nrBloku) != dopelnienieDo255) {
        cout << "Niepoprawny numer pakietu.\n";
        WriteFile(uchwytPortu, &NAK, licznikZnakow, &rozmiarZnaku, NULL);
        poprawnyPakiet = false;
    }
    else {
        tmpCRC = PoliczCRC(blokDanych, 128); // sprawdzanie czy sumy kontrole sa poprawne

        if (PoliczCRC_Znaku(tmpCRC, 1) != sumaKontrolnaCRC[0] || PoliczCRC_Znaku(tmpCRC, 2) != sumaKontrolnaCRC[1]) {
            WriteFile(uchwytPortu, &NAK, licznikZnakow, &rozmiarZnaku, NULL); //NAK
            poprawnyPakiet = false;
        }
    }

    if (poprawnyPakiet) {
        for (int i = 0; i < 128; i++) {
            if (blokDanych[i] != 26)
                plik << blokDanych[i];
        }
        cout << "Odbieranie pakietu." << endl;
        cout << " Pakiet odebrany poprawnie.\n";
        WriteFile(uchwytPortu, &ACK, licznikZnakow, &rozmiarZnaku, NULL);
    }

    while (1) {
        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        if (znak == EOT || znak == CAN) break;
        cout << "Odbieranie pakietu...";

        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        nrBloku = (int)znak;

        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        dopelnienieDo255 = znak;

        for (int i = 0; i < 128; i++) {
            ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
            blokDanych[i] = znak;
        }


        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        sumaKontrolnaCRC[0] = znak;
        ReadFile(uchwytPortu, &znak, licznikZnakow, &rozmiarZnaku, NULL);
        sumaKontrolnaCRC[1] = znak;
        poprawnyPakiet = true;

        if ((char)(255 - nrBloku) != dopelnienieDo255) {
            cout << "Niepoprawny numer pakietu.\n";
            WriteFile(uchwytPortu, &NAK, licznikZnakow, &rozmiarZnaku, NULL);
            poprawnyPakiet = false;

        }else {
            tmpCRC = PoliczCRC(blokDanych, 128);

            if (PoliczCRC_Znaku(tmpCRC, 1) != sumaKontrolnaCRC[0] || PoliczCRC_Znaku(tmpCRC, 2) != sumaKontrolnaCRC[1]) {
                cout << "Nieprawidlowa suma kontrolna.\n";
                WriteFile(uchwytPortu, &NAK, licznikZnakow, &rozmiarZnaku, NULL);
                poprawnyPakiet = false;
            }
        }
        if (poprawnyPakiet) {
            for (int i = 0; i < 128; i++) {
                if (blokDanych[i] != 26)
                    plik << blokDanych[i];
            }

            cout << " pakiet odebrany poprawnie.\n";
            WriteFile(uchwytPortu, &ACK, licznikZnakow, &rozmiarZnaku, NULL);
        }
    }
    WriteFile(uchwytPortu, &ACK, licznikZnakow, &rozmiarZnaku, NULL);

    plik.close();
    CloseHandle(uchwytPortu);

    return 0;
}
