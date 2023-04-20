#include <iostream>
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
    PORT = "COM2";
    cout << "XMODEM - Receiver";
    cout << "Port: " << PORT << endl;
    fstream file;
    if (!port_init(PORT)) return -1;
    bool CRCFlag;
    bool isCorrectPacket;
    cout << "Z CRC(1), bez CRC 0" << endl;
    cin >> CRCFlag;
    unsigned long buffLen = sizeof(C);
    bool connection;
    char buff;
    for (int i = 0; i < 6; ++i) {
        if (CRCFlag) {
            WriteFile(PORTHandle, &C, 1, &buffLen, nullptr);
        } else {
            WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
        }
        ReadFile(PORTHandle, &buff, 1, &buffLen, nullptr);
        if (buff == SOH) {
            connection = true;
            cout << "Polaczono!" << endl;
            break;
        }
    }
        if (!connection) exit(-1);
        file.open("doOdebrania.txt", ios::binary);
        char block[128];
        char blockNumber;
        char blockComplement;
        ReadFile(PORTHandle, &blockNumber, 1, &buffLen, nullptr);
        ReadFile(PORTHandle, &blockComplement, 1, &buffLen, nullptr);
        ReadFile(PORTHandle, &block, 128, &buffLen, nullptr);
        if (CRCFlag) {
            char CRCCheckSum[2];
            ReadFile(PORTHandle, &CRCCheckSum, 2, &buffLen, nullptr);
            isCorrectPacket = true;
            if ((char) (255 - blockNumber) != blockComplement) {
                cout << "Bledny numer pakietu!" << endl;
                WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
                isCorrectPacket = false;
            }
            if (isCorrectPacket) {
                for (int i = 0; i < 128; i++) {
                    if (block[i] != 26)
                        file << block[i];
                }
                cout << "Przeslano pakiet\n";
                WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
            }
            while (true) {
                ReadFile(PORTHandle, &buff, 1, &buffLen, nullptr);
                if (buff == EOT || buff == CAN) break;
                cout << "Odbieranie!";
                ReadFile(PORTHandle, &blockNumber, 1, &buffLen, nullptr);
                ReadFile(PORTHandle, &blockComplement, 1, &buffLen, nullptr);
                ReadFile(PORTHandle, &block, 128, &buffLen, nullptr);
                ReadFile(PORTHandle, &CRCCheckSum, 2, &buffLen, nullptr);
                isCorrectPacket = true;
                if ((char) (255 - blockNumber) != blockComplement) {
                    cout << "Niewlasciwy numer pakietu!\n";
                    WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
                    isCorrectPacket = false;
                } else {
                    CRCTmp = 0;
                    int count = 128;
                    while (--count >= 0) {
                        CRCTmp = CRCTmp ^ (int) block[count]++ << 8;
                        for (int i = 0; i < 8; ++i)
                            if (CRCTmp & 0x8000) CRCTmp = CRCTmp << 1 ^ 0x1021;
                            else CRCTmp = CRCTmp << 1;
                    }

                    if (PoliczCRC_Znaku(CRCTmp, 1) != CRCCheckSum[0] || PoliczCRC_Znaku(CRCTmp, 2) != CRCCheckSum[1]) {
                        cout << "Bledna suma kontrolna!\n";
                        WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
                        isCorrectPacket = false;
                    }
                }
                if (isCorrectPacket) {
                    for (int i = 0; i < 128; i++) {
                        if (block[i] != 26)
                            file << block[i];
                    }

                    cout << "Przeslanie pakietu zakonczone powodzeniem!\n";
                    WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
                }
            }
        } else {

            char CheckSum;
            ReadFile(PORTHandle, &CheckSum, 1, &buffLen, nullptr);
            isCorrectPacket = true;


            if ((char) (255 - blockNumber) != blockComplement) {
                cout << "Bledny numer pakietu!\n";
                WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
                isCorrectPacket = false;
            }
            if (isCorrectPacket) {
                for (int j = 0; j < 128; ++j) {
                    if (block[j] != 26)
                        file << block[j];
                }
                cout << "Przesłano pakiet!" << endl;
                WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
            }
            // Czytanie pliku do momentu otrzymania signu EOT (end of transmission) albo CAN (canceled)
            while (true) {
                ReadFile(PORTHandle, &buff, 1, &buffLen, nullptr);

                if (buff == EOT || buff == CAN) break;
                cout << "Odbieranie";

                ReadFile(PORTHandle, &blockNumber, 1, &buffLen, nullptr);
                ReadFile(PORTHandle, &blockComplement, 1, &buffLen, nullptr);
                ReadFile(PORTHandle, &block, 128, &buffLen, nullptr);
                ReadFile(PORTHandle, &CheckSum, 1, &buffLen, nullptr);


                isCorrectPacket = true;
                if ((char) (255 - blockNumber) != blockComplement) {
                    cout << "Niewlasciwy numer pakietu!\n";
                    WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
                    isCorrectPacket = false;
                } else {
                    char CheckSum1 = (char) 26;
                    for (int i = 0; i < 128; ++i) {
                        CheckSum1 += block[i] % 255;
                    }

                    if (PoliczCRC_Znaku(CheckSum1, 1) != CheckSum) {
                        cout << "Bledna suma kontrolna!\n";
                        WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
                        isCorrectPacket = false;
                    }
                }
                if (isCorrectPacket) {
                    for (int i = 0; i < 128; i++) {
                        if (block[i] != 26)
                            file << block[i];
                    }

                    cout << "Przeslanie pakietu zakonczone powodzeniem!\n";
                    WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
                }
            }
            WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
            file.close();
            if (buff == CAN) cout << "ERROR - polaczenie zostalo przewane!" << endl;
            else cout << "Odebrano pliki!" << endl;
            std::cin.get();
        }
}