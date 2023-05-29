#include <iostream>
#include "PORTInit.h"

unsigned long len = sizeof(ACK);

int CRC(char *data, int length) {
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

int main() {

    bool CRCFlag;
    bool isCorrectPacket;
    char buffer;
    char dataPayload[128];
    bool theSamePacket;
    int counter=0;


    system("cls");
    PORT = "COM2";
    cout << "XMODEM - RECEIVER" << endl;
    cout << "Port: " << PORT << endl;

    if (!port_init(PORT)) {
        cout << "Port initialization FAILURE!" << endl;
        return -1;
    }
    cout << "Port initialized!\n" << endl;

    cout << "Choose mode:\n[0]CheckSum\n[1]CRC\n";
    cin >> CRCFlag;

    //Establishing connection
    if(CRCFlag){
        WriteFile(PORTHandle, &C, 1, &len, nullptr);
        cout << "\nConnection established! Mode CRC" << endl;
    }else{
        WriteFile(PORTHandle, &NAK, 1, &len, nullptr);
        cout << "\nConnection established! Mode CS" << endl;
    }
    ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
    ofstream file("../doOdebrania.txt", ios::binary);
    while (true) {

        //Reading packet
        ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
        uint8_t packetNumber = buffer;
        ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
        uint8_t complement = buffer;

        //Reading block and packet number
        if(!theSamePacket){
            for (int i = 0; i < 128; i++) {
                ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
                dataPayload[i] = buffer;
            }
        }

        if(255-packetNumber != complement) isCorrectPacket =false;
        else isCorrectPacket = true;

        //Checking the packet
        if (CRCFlag) {//CRC
            ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
            unsigned char left = buffer;

            ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
            unsigned char right =buffer;
            unsigned short mergedCRC = 0;
            mergedCRC = (mergedCRC | left) << 8;
            mergedCRC = (mergedCRC | right);

            if (CRC(dataPayload,128) == mergedCRC) {
                isCorrectPacket = true;
            } else {
                isCorrectPacket =false;
            }
        } else {//CS
            ReadFile(PORTHandle, &buffer, 1, &len, nullptr);
            if ((unsigned char)calculate_checksum(dataPayload) == (unsigned char)buffer) {
                isCorrectPacket=true;
            } else {
                isCorrectPacket =false;
            }
        }

        //Sending ACK or NAC
        if (isCorrectPacket) {
            WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
            cout << "Packet " << (int)packetNumber << " has been received. ";
            cout << "Sending ACK for packet no. " << (int)packetNumber << "." << endl;
            for (int i = 0; i < 128; ++i) {
                file << dataPayload[i];
            }
        } else {
            WriteFile(PORTHandle, &NAK, 1, &len, nullptr);
            cout << "Packet " << (int)packetNumber << " has been received but is corrupted. ";
            cout << "Sending NAK for packet no. " << (int)packetNumber << ". ERROR!" << endl;
            theSamePacket = true;
            if(counter == 10) {cout<<"PACKET TRANSMITION FAILED";break;}
            counter++;
            continue;
        }
        theSamePacket=false;


        buffer = 0;
        ReadFile(PORTHandle,&buffer,1,&len,nullptr);
        if(buffer == EOT){
            WriteFile(PORTHandle, &ACK, 1, &len, nullptr);
            break;
        }
    }
    cout <<"\nEnd of transmission!";
    file.close();
    CloseHandle(PORTHandle);
    return 0;
}