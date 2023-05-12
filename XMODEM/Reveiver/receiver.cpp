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
    bool theSamePacket;


    system("cls");
    PORT = "COM3";
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
        if(!theSamePacket){
            int j = 0;
            for (int i = 3; i < 131; i++) {
                block[j] = packet[i];
                j++;
            }
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
            if ((unsigned char)calculate_checksum(block) == (unsigned char)packet[131]) {
                isCorrectPacket=true;
            } else {
                isCorrectPacket =false;
            }
        }

        //Sending ACK or NAC
        if (isCorrectPacket) {
            WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
            cout << "Packet " << packetNumberfromPackt << " has been received. ";
            cout << "Sending ACK for packet no. " << packetNumber << "." << endl;
            for (int i = 0; i < 128; ++i) {
                file << block[i];
            }
        } else {
            WriteFile(PORTHandle, &NAK, 1, &len, nullptr);
            cout << "Packet " << packetNumberfromPackt << " has been received but is corrupted. ";
            cout << "Sending NAK for packet no. " << packetNumber << ". ERROR!" << endl;
            theSamePacket = true;
            continue;
        }
        theSamePacket=false;
        packetNumber++;


        buffer = 0;
        ReadFile(PORTHandle,&buffer,1,&len,nullptr);
        if(buffer == EOT){
            WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
            break;
        }else if(buffer == ACK){
            continue;
        }
    }
    cout <<"\nEnd of transmission! File transferred successfully!"<<endl;
    file.close();
    CloseHandle(PORTHandle);
    return 0;
}