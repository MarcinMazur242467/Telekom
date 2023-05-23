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

        ofstream file("../doOdebrania.txt", ios::binary);
        while (true) {
            //Reading packet
            ReadFile(PORTHandle, &packet, 133, &len, nullptr);
    //        cout<<packet<<endl;
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
                uint16_t left = packet[131]<<8;
                uint16_t right = 0xff;
                right = right&packet[132];

                uint16_t crcFromPacket = left|right;

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
                if(counter == 10) {cout<<"PACKET TRANSMITION FAILED";break;}
                counter++;
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
        cout <<"\nEnd of transmission!";
        file.close();
        CloseHandle(PORTHandle);
        return 0;
    }