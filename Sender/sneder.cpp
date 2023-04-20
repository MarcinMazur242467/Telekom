#include <iostream>
#include <mapidefs.h>
#include "PORTInit.h"
#include <Cmath>

char PoliczCRC_Znaku(int n, int nrZnaku) {
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
        x = x + pow(2, i) * bin[k - i];

    return (char) x;//zwraca 1 lub 2 znak (bo 2 znaki to 2 bajty, czyli 16 bitów)
}

int main() {
    system("cls");
    cout << "XMODEM - SENDER" << endl;
    bool isCorrectPacket;
    char block[128];
    PORT = "COM1";
    char buffer;
    bool flagCRC;
    bool conection = false;
    char blockNumber = 1;
    unsigned long bufferSize = sizeof(buffer);

    if (!port_init(PORT)) return -1;

    for (int i = 0; i < 6; ++i) {
        ReadFile(PORTHandle, &buffer, 1, &bufferSize, nullptr);
        if (buffer == NAK) {
            cout << "Odmowa polaczenia!" << endl;
            Sleep(10000);
            flagCRC = false;
            conection = true;
            break;
        } else if (buffer == C) {
            cout << "CRC!";
            Sleep(10000);
            flagCRC = true;
            conection = true;
            break;
        }
    }
    if (!conection) exit(1);
    fstream file;
    file.open("doWyslania.txt", ios::binary);

    while (!file.eof()) {
        for (int i = 0; i < 128; ++i) {
            block[i] = (char) 26;
            block[i] = file.get();
            if (file.eof()) block[i] = (char) 26;
        }
        isCorrectPacket = false;
        while (!isCorrectPacket) {
            // WRITING SOH to HEADER
            WriteFile(PORTHandle, &SOH, 1, &bufferSize, nullptr);
            WriteFile(PORTHandle, &blockNumber, 1, &bufferSize, nullptr);
            char complement = 255 - blockNumber;
            WriteFile(PORTHandle, &complement, 1, &bufferSize, nullptr);

            WriteFile(PORTHandle, &block, 128, &bufferSize, nullptr);
            if (flagCRC) {
                CRCTmp = 0;
                int count = 128;
                while (--count >= 0) {
                    CRCTmp = CRCTmp ^ (int) block[count]++ << 8;
                    for (int i = 0; i < 8; ++i)
                        if (CRCTmp & 0x8000) CRCTmp = CRCTmp << 1 ^ 0x1021;
                        else CRCTmp = CRCTmp << 1;
                }
                cout << "CRC: " << (int) (CRCTmp & 0xFFFF) << endl;
                CRCTmp = PoliczCRC_Znaku(CRCTmp & 0xFFFF, 1);
                WriteFile(PORTHandle, &CRCTmp, 1, &bufferSize, nullptr);
                CRCTmp = PoliczCRC_Znaku(CRCTmp & 0xFFFF, 2);
                WriteFile(PORTHandle, &CRCTmp, 1, &bufferSize, nullptr);
            } else {
                char CheckSum = (char) 26;
                for (int i = 0; i < 128; ++i) {
                    CheckSum += block[i] % 255;
                }
                cout << "Suma kontrolna: " << (int) CheckSum << endl;
                WriteFile(PORTHandle, &CheckSum, 1, &bufferSize, nullptr);
            }

            while (true) {
                char acknowledgement = ' ';
                ReadFile(PORTHandle, &acknowledgement, 1, &bufferSize, nullptr);
                if (acknowledgement == ACK) {
                    isCorrectPacket = true;
                    cout << " \nPakiet przeslany poprawnie.\n";
                    break;
                }

                if (acknowledgement == NAK) {
                    cout << "\nWysylanie pakietu anulowane (NAK).\n";
                    break;
                }

                if (acknowledgement == CAN) {
                    cout << "\nPolaczenie przerwane!\n";
                    exit(1);
                }
            }
        }
        if (blockNumber == 255) blockNumber = 1;
        else blockNumber++;
    }
    file.close();
    while(true){
        WriteFile(PORTHandle,&EOT,1,&bufferSize,nullptr);
        ReadFile(PORTHandle,&buffer,1,&bufferSize, nullptr);
        if(buffer == ACK) break;
    }
    CloseHandle(PORTHandle);
    cout << "Przeslano plik!"<<endl;
}