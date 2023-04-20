#ifndef CONST_H
#define CONST_H

#include <fstream>
#include <string.h>
#include <windows.h>
using namespace std;

/* KODY STERUJĄCE */
const char SOH = 0x1;   // Start Of Heading
const char EOT = 0x4;   // End Of Transmission
const char ACK = 0x6;   // Acknowledge              Zgoda na przesylanie danych
const char NAK = 0xF;   // Negative Acknowledge     Brak zgody na przesłanie danych
const char CAN = (char)18; // Cancel                  Flaga do przerwania połączenia (24?) 
const char C = 0x43;
/* INFORMACJE O PORCIE */
HANDLE          uchwytPortu;            // identyfikator portu
LPCTSTR         numerPortu;             // przechowuje nazwę portu
DCB             ustawieniaSterowania;   // struktura kontroli portu szeregowego
COMSTAT         zasobyPortu;            // dodatkowa informacja o zasobach portu
DWORD           blad;                   // reprezentuje typ ewentualnego błędu
COMMTIMEOUTS    ustawieniaCzasu;
USHORT tmpCRC;

/* NADANIE PORTOWI PARAMETROW + OTWORZENIE GO */
bool ustawieniaPortu(LPCTSTR numerPortu) {

    uchwytPortu = CreateFile(numerPortu, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (uchwytPortu != INVALID_HANDLE_VALUE)
    {
        ustawieniaSterowania.DCBlength = sizeof(ustawieniaSterowania);
        GetCommState(uchwytPortu, &ustawieniaSterowania);
        ustawieniaSterowania.BaudRate = CBR_9600;     /* predkosc transmisji */
        ustawieniaSterowania.Parity = NOPARITY;     /* bez bitu parzystosci */
        ustawieniaSterowania.StopBits = ONESTOPBIT; /* ustawienie bitu stopu (jeden bit) */
        ustawieniaSterowania.ByteSize = 8;          /* liczba wysylanych bitów */

        ustawieniaSterowania.fParity = TRUE;
        ustawieniaSterowania.fDtrControl = DTR_CONTROL_DISABLE; /* Kontrola linii DTR: sygnal nieaktywny */
        ustawieniaSterowania.fRtsControl = RTS_CONTROL_DISABLE; /* Kontrola linii RTS: sygnal nieaktywny */
        ustawieniaSterowania.fOutxCtsFlow = FALSE;
        ustawieniaSterowania.fOutxDsrFlow = FALSE;
        ustawieniaSterowania.fDsrSensitivity = FALSE;
        ustawieniaSterowania.fAbortOnError = FALSE;
        ustawieniaSterowania.fOutX = FALSE;
        ustawieniaSterowania.fInX = FALSE;
        ustawieniaSterowania.fErrorChar = FALSE;
        ustawieniaSterowania.fNull = FALSE;

        ustawieniaCzasu.ReadIntervalTimeout = 10000;
        ustawieniaCzasu.ReadTotalTimeoutMultiplier = 10000;
        ustawieniaCzasu.ReadTotalTimeoutConstant = 10000;
        ustawieniaCzasu.WriteTotalTimeoutMultiplier = 100;
        ustawieniaCzasu.WriteTotalTimeoutConstant = 100;

        SetCommState(uchwytPortu, &ustawieniaSterowania);
        SetCommTimeouts(uchwytPortu, &ustawieniaCzasu);
        ClearCommError(uchwytPortu, &blad, &zasobyPortu);
        return true;
    }
    else { return false; }
}

#endif