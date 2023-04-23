#include <iostream>
#include "PORTInit.h"


unsigned long len = sizeof(ACK);

uint16_t calculate_crc(char data[]) {
    uint16_t crc = 0;
    for (int i = 0; i < 16; i++) {
        crc = crc ^ ((uint16_t) data[i] << 8);
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

unsigned char calculate_checksum(char data[128]) {
    unsigned char checksum = 0;
    for (int i = 0; i < 128; i++) {
        checksum = (checksum + data[i]) % 256;
    }
    return checksum;
}

int main() {
    bool CRCFlag;
    bool isCorrectPacket;
    char buffer;
    char block[128];
    int packetNumber = 1;
    char packet[133];


    system("cls");
    PORT = "COM5";
    cout << "XMODEM - RECEIVER" << endl;
    cout << "Port: " << PORT << endl;

    if (!port_init(PORT)) {
        cout << "Port initialization FAILURE!" << endl;
        return -1;
    }
    cout << "Port initialized!\n" << endl;

    cout << "Choose mode:\n[0]CheckSum\n[1]CRC\n";
    cin >> CRCFlag;

    ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
    if (buffer == NAK) {
        if (CRCFlag) {
            WriteFile(PORTHandle, &C, 1, &len, nullptr);
            cout << "\nConnection established! Mode CRC" << endl;
        } else {
            WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
            cout << "\nConnection established! Mode CS" << endl;
        }
    } else {
        cout << "\nConnection FAILED!" << endl;
        exit(-1);
    }
    ofstream file("../doOdebrania.txt", ios::binary);
    while (true) {
        //Reading packet
        ReadFile(PORTHandle, &packet, 133, &len, nullptr);

        //Reading block and packet number
        int j = 0;
        for (int i = 3; i < 131; i++) {
            block[j] = packet[i];
            j++;
        }
        int packetNumberfromPackt = (int) packet[1];

        //Checking the packet
        if (CRCFlag) {//CRC
            uint16_t crcFromPacket = ((uint16_t) packet[131] << 8) | (uint16_t) packet[132];
            if (calculate_crc(block) == crcFromPacket) {
                isCorrectPacket = true;
            } else {
                isCorrectPacket =false;
            }
        } else {//CS
            if (calculate_checksum(block) == packet[131]) {
                isCorrectPacket=true;
            } else {
                isCorrectPacket =false;
            }
        }

        //Sending ACK or NAC
        if (isCorrectPacket) {
            cout << "Packet " << packetNumberfromPackt << " has been received. ";
            cout << "Sending ACK for packet no. " << packetNumber << "." << endl;
            WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
            for (int i = 0; i < 128; ++i) {
                file << block[i];
            }
        } else {
            cout << "Packet " << packetNumberfromPackt << " has been received but is corrupted. ";
            cout << "Sending NAK for packet no. " << packetNumber << ". ERROR!" << endl;
            WriteFile(PORTHandle, &NAK, 1, &len, nullptr);
        }
        packetNumber++;
        buffer = 0;
        ReadFile(PORTHandle,&buffer,1,&len,nullptr);
        if(buffer == EOT){
            cout <<"\nEnd of transmission! File transferred successfully!"<<endl;
            break;
        }else if(buffer == ACK){
            continue;
        }
    }
    WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
    file.close();
    CloseHandle(PORTHandle);
    return 0;



//if(CRCFlag){
//
//}else{
//    while(true){
//        ReadFile(PORTHandle,&buffer,1,&buffLen,nullptr);
//        if(buffer == EOF) break;
//        ReadFile(PORTHandle,packet,132,&buffLen,nullptr);
//        int j =0;
//        for (int i = 3; i < 131; i++) {
//            block[j] = packet[i];
//            j++;
//        }
//
//        if(packet[131]== calculate_checksum(block)){
//            WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
//        }else{
//            WriteFile(PORTHandle, &NAK, 1, &len, nullptr);
//        }
//        cout << packet;
//
//    }
//}





























//    cout << packet[131];
//    cout << "\nPress: \n\n"
//            "CRC(1)\n"
//            "CheckSum(0)" << endl;
//    cin >> CRCFlag;
//    for (int i = 0; i < 6; ++i) {
//        if (CRCFlag) {
//            WriteFile(PORTHandle, &C, 1, &buffLen, nullptr);
//        } else {
//            WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
//        }
//        ReadFile(PORTHandle, &buff, 1, &buffLen, nullptr);
//        if (buff == SOH) {
//            connection = true;
//            cout << "Polaczono!" << endl;
//            break;
//        }
//    }
//        if (!connection) exit(-1);
//        file.open("doOdebrania.txt", ios::binary);

//        ReadFile(PORTHandle, &blockNumber, 1, &buffLen, nullptr);
//        ReadFile(PORTHandle, &blockComplement, 1, &buffLen, nullptr);
//        ReadFile(PORTHandle, &block, 128, &buffLen, nullptr);
//        cout <<"Block number: "<<(int)blockNumber << "Block complement:  " <<(int)blockComplement<<endl;
//    cout << (int)block[0] <<endl;
//    cout << (int)block[1] <<endl;
//        if (CRCFlag) {
//            char CRCCheckSum[2];
//            ReadFile(PORTHandle, &CRCCheckSum, 2, &buffLen, nullptr);
//            isCorrectPacket = true;
//            if ((char) (255 - blockNumber) != blockComplement) {
//                cout << "Bledny numer pakietu!" << endl;
//                WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
//                isCorrectPacket = false;
//            }
//            if (isCorrectPacket) {
//                for (int i = 0; i < 128; i++) {
//                    if (block[i] != 26)
//                        file << block[i];
//                }
//                cout << "Przeslano pakiet\n";
//                WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
//            }
//            while (true) {
//                ReadFile(PORTHandle, &buff, 1, &buffLen, nullptr);
//                if (buff == EOT || buff == CAN) break;
//                cout << "Odbieranie!";
//                ReadFile(PORTHandle, &blockNumber, 1, &buffLen, nullptr);
//                ReadFile(PORTHandle, &blockComplement, 1, &buffLen, nullptr);
//                ReadFile(PORTHandle, &block, 128, &buffLen, nullptr);
//                ReadFile(PORTHandle, &CRCCheckSum, 2, &buffLen, nullptr);
//                isCorrectPacket = true;
//                if ((char) (255 - blockNumber) != blockComplement) {
//                    cout << "Niewlasciwy numer pakietu!\n";
//                    WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
//                    isCorrectPacket = false;
//                } else {
//                    CRCTmp = 0;
//                    int count = 128;
//                    while (--count >= 0) {
//                        CRCTmp = CRCTmp ^ (int) block[count]++ << 8;
//                        for (int i = 0; i < 8; ++i)
//                            if (CRCTmp & 0x8000) CRCTmp = CRCTmp << 1 ^ 0x1021;
//                            else CRCTmp = CRCTmp << 1;
//                    }
//
//                    if (PoliczCRC_Znaku(CRCTmp, 1) != CRCCheckSum[0] || PoliczCRC_Znaku(CRCTmp, 2) != CRCCheckSum[1]) {
//                        cout << "Bledna suma kontrolna!\n";
//                        WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
//                        isCorrectPacket = false;
//                    }
//                }
//                if (isCorrectPacket) {
//                    for (int i = 0; i < 128; i++) {
//                        if (block[i] != 26)
//                            file << block[i];
//                    }
//
//                    cout << "Przeslanie pakietu zakonczone powodzeniem!\n";
//                    WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
//                }
//            }
//        } else {
//
//            char CheckSum;
//            ReadFile(PORTHandle, &CheckSum, 1, &buffLen, nullptr);
//            isCorrectPacket = true;
//
//
//            if ((char) (255 - blockNumber) != blockComplement) {
//                cout << "Bledny numer pakietu!\n";
//                WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
//                isCorrectPacket = false;
//            }
//            if (isCorrectPacket) {
//                for (int j = 0; j < 128; ++j) {
//                    if (block[j] != 26)
//                        file << block[j];
//                }
//                cout << "Przesłano pakiet!" << endl;
//                WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
//            }
//            // Czytanie pliku do momentu otrzymania signu EOT (end of transmission) albo CAN (canceled)
//            while (true) {
//                ReadFile(PORTHandle, &buff, 1, &buffLen, nullptr);
//
//                if (buff == EOT || buff == CAN) break;
//                cout << "Odbieranie";
//
//                ReadFile(PORTHandle, &blockNumber, 1, &buffLen, nullptr);
//                ReadFile(PORTHandle, &blockComplement, 1, &buffLen, nullptr);
//                ReadFile(PORTHandle, &block, 128, &buffLen, nullptr);
//                ReadFile(PORTHandle, &CheckSum, 1, &buffLen, nullptr);
//
//
//                isCorrectPacket = true;
//                if ((char) (255 - blockNumber) != blockComplement) {
//                    cout << "Niewlasciwy numer pakietu!\n";
//                    WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
//                    isCorrectPacket = false;
//                } else {
//                    char CheckSum1 = (char) 26;
//                    for (int i = 0; i < 128; ++i) {
//                        CheckSum1 += block[i] % 255;
//                    }
//
//                    if (PoliczCRC_Znaku(CheckSum1, 1) != CheckSum) {
//                        cout << "Bledna suma kontrolna!\n";
//                        WriteFile(PORTHandle, &NAK, 1, &buffLen, nullptr);
//                        isCorrectPacket = false;
//                    }
//                }
//                if (isCorrectPacket) {
//                    for (int i = 0; i < 128; i++) {
//                        if (block[i] != 26)
//                            file << block[i];
//                    }
//
//                    cout << "Przeslanie pakietu zakonczone powodzeniem!\n";
//                    WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
//                }
//            }
//            WriteFile(PORTHandle, &ACK, 1, &buffLen, nullptr);
//            file.close();
//            if (buff == CAN) cout << "ERROR - polaczenie zostalo przewane!" << endl;
//            else cout << "Odebrano pliki!" << endl;
//            std::cin.get();
//        }

}