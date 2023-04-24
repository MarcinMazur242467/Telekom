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

bool checkPacket(bool CRCFlag) {
    char packet[133];
    char block[128];

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
            return true;
        } else {
            return false;
        }
    } else {//CS
        if (calculate_checksum(block) == packet[131]) {
            return true;
        } else {
            return false;
        }
    }
}

int main() {
    bool CRCFlag;
    bool isCorrectPacket;
    char buffer;
    char block[128];
    bool flaga;
    int packetNumber = 1;


    system("cls");
    PORT = "COM5";
    cout << "XMODEM - RECEIVER" << endl;
    cout << "Port: " << PORT << endl;

    //Initializing port
    if (!port_init(PORT)) {
        cout << "Port initialization FAILURE!" << endl;
        return -1;
    }
    cout << "Port initialized!\n" << endl;

    cout << "Choose mode:\n[0]CheckSum\n[1]CRC\n";
    cin >> CRCFlag;

    //Establishing connection
    if (CRCFlag) {
        WriteFile(PORTHandle, &C, 1, &len, nullptr);
        cout << "Connection established! Mode: CRC" << endl;
    } else {
        WriteFile(PORTHandle, &NAK, 1, &len, nullptr);
        cout << "Connection established! Mode: CS" << endl;

    }


    ofstream file("../doOdebrania.txt", ios::binary);

    while (true) {
        char packet[133];
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
                isCorrectPacket = false;
            }
        } else {//CS
            if (calculate_checksum(block) == packet[131]) {
                isCorrectPacket = true;
            } else {
                isCorrectPacket = false;
            }
        }
        cout << packet << endl;
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
            for (int i = 0; i < 5; ++i) {
                if (checkPacket(CRCFlag)) {
                    cout << "Packet " << packetNumberfromPackt << " has been received. ";
                    cout << "Sending ACK for packet no. " << packetNumber << "." << endl;
                    WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
                    for (int i = 0; i < 128; ++i) {
                        file << block[i];
                    }
                    break;
                } else {
                    cout << "Packet " << packetNumberfromPackt << " has been received but is corrupted. ";
                    cout << "Sending NAK for packet no. " << packetNumber << ". ERROR!" << endl;
                    WriteFile(PORTHandle, &NAK, 1, &len, nullptr);
                }
            }
        }
        packetNumber++;
//        cout << "Waiting for something????"<<endl;
        for (int i = 0; i < 2; ++i) {
            buffer = 0;
            ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
            if (buffer == EOT) {
                cout << "\nEnd of transmission! File transferred successfully!" << endl;
                flaga = true;
                break;
            } else if (buffer == ACK) {
                continue;
            }
        }
        if(flaga) break;
    }
    WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
    file.close();
    CloseHandle(PORTHandle);
    return 0;
}