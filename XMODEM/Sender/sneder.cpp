#include <iostream>
#include "PORTInit.h"

void printBits(unsigned short num) {
    for (int i = 15; i >= 0; i--) {
        unsigned short mask = 1 << i;
        unsigned short bit = (num & mask) >> i;
        std::cout << static_cast<int>(bit);
    }
    std::cout << std::endl;
}
unsigned long len = sizeof(ACK);

uint16_t CRC(char *data, int length) {
    unsigned short crc, i;
    crc = 0;
    while (--length >= 0) {
        crc = crc ^ (int) *data++ << 8;
        for (i = 0; i < 8; ++i)
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
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
    char packet[132];
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
    packet[132] = 0;
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


    uint16_t crc = CRC(dataPayload,128);
    packet[131]=0;
    packet[132]=0;
    packet[131] = (char) ((crc >> 8) & 0xFF);
    packet[132] = (char) (crc & 0xFF);
    WriteFile(PORTHandle, &packet, 133, &len, nullptr);
    cout << "Packet " << (int) packetNumber << " has been sent...";
}




int main() {
    char block[128];
    char buffer;
    bool CRCFlag;
    int packetCount = 1;
    int counter=0;
    bool theSamePacket= false;

    system("cls");
    PORT = "COM1";
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


    while (true) {

        //READING BLOCK
        if(!theSamePacket){
            for (int i = 0; i < 128; ++i) {
                block[i] = file.get();
            }
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

        if (buffer == ACK ) {
            cout << "ACK received from packet no. " << packetCount<<endl;
        } else if (buffer == NAK or buffer == C) {
            cout << "NAC received from packet no. " << packetCount << " Retransmition needed!"<<endl;
            theSamePacket = true;
            if(counter == 10) {cout<<"PACKET TRANSMITION FAILED";break;}
            counter++;
            continue;
        } else if(buffer == CAN) return -1;
        theSamePacket = false;
        packetCount++;

        //EOT
        if(file.eof() == 1 ){
            while(true){
                WriteFile(PORTHandle, &EOT, 1, &len, nullptr);
                buffer=0;
                ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
                if(buffer == ACK){
                    break;
                }
            }
            break;
        }

    }
    cout << "\nEnd of file!" <<endl;
    file.close();
    CloseHandle(PORTHandle);
    return 0;
}

