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

void sentPacketCS(unsigned char packetNumber, char dataPayload[128]) {
    char packet[133];
    char complement = (unsigned char) 255 - packetNumber;
    for (int i = 3; i < 131; ++i) {
        packet[i] = (char) 26;
    }
    packet[0] = SOH;
    packet[1] = packetNumber;
    packet[2] = complement;
    int j = 0;
    for (int i = 3; i < 131; i++) {
        packet[i] = dataPayload[j];
        j++;
    }
    packet[131] = calculate_checksum(dataPayload);
    WriteFile(PORTHandle, &packet, 132, &len, nullptr);
    cout << "Packet " << (int) packetNumber << " has been sent... ";
}

void sentPacketCRC(unsigned char packetNumber, char dataPayload[128]) {
    char packet[133];
    char complement = (unsigned char) 255 - packetNumber;
    for (int i = 3; i < 131; ++i) {
        packet[i] = (char) 26;
    }
    packet[0] = SOH;
    packet[1] = packetNumber;
    packet[2] = complement;
    int j = 0;
    for (int i = 3; i < 131; i++) {
        packet[i] = dataPayload[j];
        j++;
    }


    uint16_t crc = calculate_crc(dataPayload);
    packet[131] = (uint8_t) ((crc >> 8) & 0xFF);
    packet[132] = (uint8_t) (crc & 0xFF);
    WriteFile(PORTHandle, &packet, 133, &len, nullptr);
    cout << "Packet " << (int) packetNumber << " has been sent...";
}

int main() {
    char block[128];
    char buffer;
    bool CRCFlag;
    int packetCount = 1;

    system("cls");
    PORT = "COM4";
    cout << "XMODEM - SENDER" << endl;
    cout << "Port: " << PORT << endl;

    //Initializing port
    if (!port_init(PORT)) {
        cout << "Port initialization FAILURE!" << endl;
        return -1;
    }
    cout << "Port initialized!\n" << endl;

    //Establishing connection
    WriteFile(PORTHandle, &NAK, 1, &len, nullptr);
    ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
    if (buffer == ACK) {
        cout << "Connection established! Mode: CS" << endl;
        CRCFlag = false;
    } else if (buffer == C) {
        CRCFlag = true;
        cout << "Connection established! Mode: CRC" << endl;
    } else {
        cout << "Connection FAILED!" << endl;
        exit(-1);
    }

    buffer = 0;
    ifstream file("doWyslania.txt", ios::binary);

    while (!file.eof()) {
        //READING BLOCK
        for (int i = 0; i < 128; ++i) {
            block[i] = file.get();
        }
        //Sending packet in CRC or SC
        if (CRCFlag) {
            sentPacketCRC(packetCount, block);
        } else {
            sentPacketCS(packetCount, block);
        }

        //Waiting for ACK or NAK or CAN
        buffer = 0;
        ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
        if (buffer == ACK) {
            cout << "ACK received from packet no. " << packetCount<<endl;
        } else if (buffer == NAK) {
            cout << "NAC received from packet no. " << packetCount << " Retransmition needed!"<<endl;
        } else if(buffer == CAN) return -1;

        packetCount++;

        //EOT
        if(file.eof() == 1 ){
            WriteFile(PORTHandle, &EOT, 1, &len, nullptr);
        }else{
            WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
        }

    }
    cout << "\nEnd of file!" <<endl;
    file.close();
    CloseHandle(PORTHandle);
    return 0;


//    while(!file.eof()){
//
//        if (CRCFlag) {
//
//        } else {
//            sentPacketCS(packetCount, block);
//            for (int i = 0; i < 10; i++) {
//                ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
//                if (buffer == NAK) {
////                    sentPacketCS(packetCount, block);
////                    if (i == 9) {
////                        cout << "Transfer aborted due to NAC limit." << endl;
////                        exit(-1);
////                    }
//                } else if (buffer == ACK) {
//                    break;
//                }
//
//            }
//        }
//        packetCount++;
//    }
//    WriteFile(PORTHandle,&EOT,1,&len,nullptr);






















//    WriteFile(PORTHandle, &SOH, 1, &bufferSize, nullptr);
//    WriteFile(PORTHandle, &blockNumber, 1, &bufferSize, nullptr);
//    unsigned char complement = 255 - blockNumber;
//    WriteFile(PORTHandle, &complement, 1, &bufferSize, nullptr);
//    while (!file.eof()) {
//        for (int i = 0; i < 128; ++i) {
//            block[i] = (char) 26;
//            block[i] = file.get();
//            if (file.eof()) block[i] = (char) 26;
//        }
//        isCorrectPacket = false;
//        while (!isCorrectPacket) {
//            // WRITING SOH to HEADER
//            WriteFile(PORTHandle, &SOH, 1, &bufferSize, nullptr);
//            WriteFile(PORTHandle, &blockNumber, 1, &bufferSize, nullptr);
//            char complement = 255 - blockNumber;
//            WriteFile(PORTHandle, &complement, 1, &bufferSize, nullptr);
//
//            WriteFile(PORTHandle, &block, 128, &bufferSize, nullptr);
//            if (flagCRC) {
//                CRCTmp = 0;
//                int count = 128;
//                while (--count >= 0) {
//                    CRCTmp = CRCTmp ^ (int) block[count]++ << 8;
//                    for (int i = 0; i < 8; ++i)
//                        if (CRCTmp & 0x8000) CRCTmp = CRCTmp << 1 ^ 0x1021;
//                        else CRCTmp = CRCTmp << 1;
//                }
//                cout << "CRC: " << (int) (CRCTmp & 0xFFFF) << endl;
//                CRCTmp = PoliczCRC_Znaku(CRCTmp & 0xFFFF, 1);
//                WriteFile(PORTHandle, &CRCTmp, 1, &bufferSize, nullptr);
//                CRCTmp = PoliczCRC_Znaku(CRCTmp & 0xFFFF, 2);
//                WriteFile(PORTHandle, &CRCTmp, 1, &bufferSize, nullptr);
//            } else {
//                char CheckSum = (char) 26;
//                for (int i = 0; i < 128; ++i) {
//                    CheckSum += block[i] % 255;
//                }
//                cout << "Suma kontrolna: " << (int) CheckSum << endl;
//                WriteFile(PORTHandle, &CheckSum, 1, &bufferSize, nullptr);
//            }
//
//            while (true) {
//                char acknowledgement = ' ';
//                ReadFile(PORTHandle, &acknowledgement, 1, &bufferSize, nullptr);
//                if (acknowledgement == ACK) {
//                    isCorrectPacket = true;
//                    cout << " \nPakiet przeslany poprawnie.\n";
//                    break;
//                }
//
//                if (acknowledgement == NAK) {
//                    cout << "\nWysylanie pakietu anulowane (NAK).\n";
//                    break;
//                }
//
//                if (acknowledgement == CAN) {
//                    cout << "\nPolaczenie przerwane!\n";
//                    exit(1);
//                }
//            }
//        }
//        if (blockNumber == 255) blockNumber = 1;
//        else blockNumber++;
//    }
//    file.close();
//    while(true){
//        WriteFile(PORTHandle,&EOT,1,&bufferSize,nullptr);
//        ReadFile(PORTHandle,&buffer,1,&bufferSize, nullptr);
//        if(buffer == ACK) break;
//    }
    CloseHandle(PORTHandle);
//    cout << "Przeslano plik!" << endl;
}

