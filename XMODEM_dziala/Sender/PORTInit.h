#ifndef PORTInit_H
#define  PORTInit_H

#include <fstream>
#include <string.h>
#include <windows.h>
using namespace std;

const char SOH = 0x1;   // Start Of Heading
const char EOT = 0x4;   // End Of Transmission
const char ACK = 0x6;   // Acknowledge
const char NAK = 0xF;   // Negative Acknowledge
const char CAN = (char)18; // Cancel
const char C = 0x43;

/* INFORMACJE O PORCIE */
HANDLE          PORTHandle;            // identyfikator portu
LPCTSTR         PORT;             // przechowuje nazwę portu
DCB             PORTConfiguration;   // struktura kontroli portu szeregowego
COMSTAT         PORTStat;            // dodatkowa informacja o zasobach portu
DWORD           PORTError;                   // reprezentuje typ ewentualnego błędu
COMMTIMEOUTS    PORTTime;
USHORT          CRCTmp;

/* NADANIE PORTOWI PARAMETROW + OTWORZENIE GO */
bool port_init(LPCTSTR PORT) {

    PORTHandle = CreateFile(PORT, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (PORTHandle != INVALID_HANDLE_VALUE)
    {
        PORTConfiguration.DCBlength = sizeof(PORTConfiguration);
        GetCommState(PORTHandle, &PORTConfiguration);
        PORTConfiguration.BaudRate = CBR_9600;     /* predkosc transmisji */
        PORTConfiguration.Parity = NOPARITY;     /* bez bitu parzystosci */
        PORTConfiguration.StopBits = ONESTOPBIT; /* ustawienie bitu stopu (jeden bit) */
        PORTConfiguration.ByteSize = 8;          /* liczba wysylanych bitów */

        PORTConfiguration.fParity = TRUE;
        PORTConfiguration.fDtrControl = DTR_CONTROL_DISABLE; /* Kontrola linii DTR: sygnal nieaktywny */
        PORTConfiguration.fRtsControl = RTS_CONTROL_DISABLE; /* Kontrola linii RTS: sygnal nieaktywny */
        PORTConfiguration.fOutxCtsFlow = FALSE;
        PORTConfiguration.fOutxDsrFlow = FALSE;
        PORTConfiguration.fDsrSensitivity = FALSE;
        PORTConfiguration.fAbortOnError = FALSE;
        PORTConfiguration.fOutX = FALSE;
        PORTConfiguration.fInX = FALSE;
        PORTConfiguration.fErrorChar = FALSE;
        PORTConfiguration.fNull = FALSE;

        PORTTime.ReadIntervalTimeout = 10000;
        PORTTime.ReadTotalTimeoutMultiplier = 10000;
        PORTTime.ReadTotalTimeoutConstant = 10000;
        PORTTime.WriteTotalTimeoutMultiplier = 100;
        PORTTime.WriteTotalTimeoutConstant = 100;

        SetCommState(PORTHandle, &PORTConfiguration);
        SetCommTimeouts(PORTHandle, &PORTTime);
        ClearCommError(PORTHandle, &PORTError, &PORTStat);
        return true;
    }
    else { return false; }
}
#endif  PORTInit_H