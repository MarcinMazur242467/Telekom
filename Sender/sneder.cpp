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

char calculate_checksum(char data[128]) {
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
    ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
    if (buffer == NAK) {
        CRCFlag = false;
        cout << "\nConnection established! Mode CS" << endl;
    } else if (buffer == C) {
        CRCFlag = true;
        cout << "\nConnection established! Mode CRC" << endl;
    } else {
        cout << "\nConnection FAILED!" << endl;
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
            cout << "ACK received from packet no. " << packetCount << endl;
        } else if (buffer == NAK) {
            cout << "NAC received from packet no. " << packetCount << " Retransmition needed!" << endl;
            for (int i = 0; i < 5; ++i) {
                if (CRCFlag) {
                    sentPacketCRC(packetCount, block);
                } else {
                    sentPacketCS(packetCount, block);
                }
                buffer = 0;
                ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
                if(buffer == ACK){
                    cout << "ACK received from packet no. " << packetCount << endl;
                    break;
                }else if(buffer == NAK){
                    cout << "NAC received from packet no. " << packetCount << " Retransmition needed!" << endl;
                }
            }
        } else if (buffer == CAN) return -1;

        packetCount++;

        //EOT
        if (file.eof() == 1) {
            WriteFile(PORTHandle, &EOT, 1, &len, nullptr);
        } else {
            WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
        }

    }
    cout << "\nEnd of file!" << endl;
    file.close();
    CloseHandle(PORTHandle);
    return 0;
}

