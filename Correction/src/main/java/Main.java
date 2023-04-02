import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;

public class Main {

//    	      1 1 1 1 0 0 0 0 1 0 0 0 0 0 0 0
//            1 1 0 0 1 1 0 0 0 1 0 0 0 0 0 0
//            1 0 1 0 1 0 1 0 0 0 1 0 0 0 0 0
//            0 1 0 1 0 1 1 0 0 0 0 1 0 0 0 0
//            1 1 1 0 1 0 0 1 0 0 0 0 1 0 0 0
//            1 0 0 1 0 1 0 1 0 0 0 0 0 1 0 0
//            0 1 1 1 1 0 1 1 0 0 0 0 0 0 1 0
//            1 1 1 0 0 1 1 1 0 0 0 0 0 0 0 1

//            1 1 1 0 1 1 0 1
//            1 1 0 1 1 0 1 1
//            1 0 1 0 1 0 1 1
//            1 0 0 1 0 1 1 0
//            0 1 1 0 1 0 1 0
//            0 1 0 1 0 1 0 1
//            0 0 1 1 0 1 1 1
//            0 0 0 0 1 1 1 1
//            1 0 0 0 0 0 0 0
//            0 1 0 0 0 0 0 0
//            0 0 1 0 0 0 0 0
//            0 0 0 1 0 0 0 0
//            0 0 0 0 1 0 0 0
//            0 0 0 0 0 1 0 0
//            0 0 0 0 0 0 1 0
//            0 0 0 0 0 0 0 1

    public static byte[] encodingMatrix = {
            (byte) (0xF0),
            (byte) (0xCC),
            (byte) (0xAA),
            (byte) (0x56),
            (byte) (0xE9),
            (byte) (0x95),
            (byte) (0x7B),
            (byte) (0xE7)};
    public static short[] H = {
            (short) 61568,
            (short) 52288,
            (short) 43552,
            (short) 22032,
            (short) 59656,
            (short) 38148,
            (short) 31490,
            (short) 59137};
    private static final byte[][] transposedH = new byte[][]{
            {(byte) 0xED},
            {(byte) 0xDB},
            {(byte) 0xAB},
            {(byte) 0x96},
            {(byte) 0x6A},
            {(byte) 0x55},
            {(byte) 0x33},
            {(byte) 0xF0},

            {(byte) 0x80},
            {(byte) 0x40},
            {(byte) 0x20},
            {(byte) 0x10},
            {(byte) 0x8},
            {(byte) 0x4},
            {(byte) 0x2},
            {(byte) 0x1},
    };

    //PLIKI

    private static byte[] read(String fileName) throws FileNotFoundException {
        Scanner scanner = new Scanner(new File(fileName));
        String text = scanner.nextLine();
        scanner.close();
        return text.getBytes(StandardCharsets.US_ASCII);
    }

    private static void createFile(String message, String fileName) throws IOException {
        File file = new File(fileName);
        file.createNewFile();
        FileWriter writer = new FileWriter(file);
        writer.write(message);
        writer.close();
    }


    //LOGIKA

    private static short encode(byte message) {
        short result = (short) ((short) (message << 8) | (short) (255));
        byte temp;
        byte temp1 = 0;
        for (int i = 0; i < 8; i++) {
            temp = (byte) (message & encodingMatrix[i]);
            if (countOnes(temp) % 2 != 0) {
                temp1 = (byte) ((temp1 | (0x1 << 7 - i)));
            }
        }
        short temp2 = -1;
        temp2 = (short) (temp2<<8);
        temp2 = ((short) (temp2|temp1));
        result = (short) (result & temp2);
        printBinary(result);
        return result;
    }

    public static byte errorVector(short encodedMessage) {
        byte result = 0x0;
        for (int i = 0; i < 8; i++) {
            byte temp = 0;
            // TU MUSIMY PIONOWO ZLICZAC JEDYNKI Z MATRIXA JAK BIT encodedMessage jest ustawiony na jedynke z tego co wyczytalem na necie
            if (countOnes(temp) % 2 != 0) {
                result |= 0x1 << (7 - (byte) i);
            }

        }
        return result;
    }
    private static byte correct(byte message, byte errorVector) {
        for (int i = 0; i < 8; i++) {
            if (errorVector == transposedH[i][0]) {
                message = (byte) (message ^ (0x1 << 7 - i));
            }
        }
        for (int i = 0; i < 16; i++) {
            for (int j = i; j < 16; j++) {  // tu u kuby jest j=1
                byte temp = (byte) (transposedH[i][0] ^ transposedH[j][0]);
                if (temp == errorVector) {
                    message = (byte) (message ^ (0x1 << 7 - i));
                    message = (byte) (message ^ (0x1 << 7 - j));
                }
            }
        }
        return message;
    }

    private static byte decode(short encodedMessage) {
        byte errorVector = errorVector(encodedMessage);
        byte message = 0;
        message = (byte) (message | encodedMessage >> 8);

        return correct(message, errorVector);
    }

    public static void main(String[] args) throws IOException {
//        //BLAD 1-bitowy
//        byte message = (byte) 0x40;// @ w asci
//        short encodedMessage = encode(message);
//        printBinary(encodedMessage);
//        short changedMessage = (byte) 0x1 << 9;
//        encodedMessage = (short) (encodedMessage | changedMessage);
//        printBinary(encodedMessage);
//        System.out.println((char) decode(encodedMessage));
//        //BLAD 2-bitowy NIE DZIALA
//        byte message = (byte) 0x40;// @ w asci
//        short encodedMessage = encode(message);
//        printBinary(encodedMessage);
//        short changedMessage = (byte) 0x9 << 10;
//        encodedMessage = (short) (encodedMessage | changedMessage);
//        printBinary(encodedMessage);
//        System.out.println((char) decode(encodedMessage));




        Scanner scan = new Scanner(System.in);
        System.out.println("Podaj wiadomosc: ");
        String string = scan.nextLine();
        createFile(string,"message.txt");
        byte [] input = read("message.txt");
        StringBuilder builder = new StringBuilder();
        short encoded;
        for (byte b : input) {
            encoded = encode(b);
            byte message = (byte) (encoded>>8);
            byte parityBits = (byte) (encoded);

            builder.append((char) message);
            builder.append((char) parityBits);
        }
        System.out.println(builder);

        createFile(builder.toString(),"messageEncoded.txt");

        System.out.println("Wprowadz zmiany w pliku 'messageEncoded' i wpisz cokolwiek jesli juz skonczysz (jak nie chcesz zmieniac to nacisnij cokolwiek bez zmiany");
        scan.nextLine();
        StringBuilder builder3 = new StringBuilder();

        byte[] encodeResultt = read("messageEncoded.txt");

        for (int i = 0; i < encodeResultt.length; i+= 2) {
            short toBeDecoded = 0;
            byte message = encodeResultt[i];
            byte parityBits = encodeResultt[i + 1];
            toBeDecoded = (short) ((short) (toBeDecoded|message)<<8);
            toBeDecoded = (short)(toBeDecoded|parityBits);
            byte decodeResult = decode(toBeDecoded);
            builder3.append((char) decodeResult);
        }
        System.out.println(builder3);
        createFile(builder3.toString(), "messageDecoded.txt");
    }


    public static int countOnes(short arr) {
        int suma = 0;
        for (int i = 0; i < 16; i++) {
            suma += (arr >> i) & 1;
        }
        return suma;
    }

    public static void printByteBits(byte b) {
        for (int i = 7; i >= 0; i--) {
            System.out.print((b >> i) & 1);
        }
        System.out.println();
    }

    public static void printBinary(short s) {
        for (int i = 15; i >= 0; i--) {
            System.out.print((s >> i) & 1);
        }
        System.out.println();
    }
    public static void test(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        int bits = 0;
        for (byte b : bytes) {
            int unsignedByte = b & 0xFF;
            String binaryString = Integer.toBinaryString(unsignedByte);
            // Pad the binary string with leading zeros if necessary
            while (binaryString.length() < 8) {
                binaryString = "0" + binaryString;
            }
            bits++;
            sb.append(binaryString + " ");
        }
        String binaryOutput = sb.toString();
        System.out.println(binaryOutput);
    }

}