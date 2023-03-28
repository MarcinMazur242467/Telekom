import java.io.*;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;

public class Main{
    private static byte[][] H = new byte[][]{
            {(byte) 0xF0}, // {1, 1, 1, 1, 0, 0, 0, 0,
            {(byte) 0x80}, //   1, 0, 0, 0, 0, 0, 0, 0
            {(byte) 0xCC},// {1, 1, 0, 0, 1, 1, 0, 0,
            {(byte) 0x40},//   0, 1, 0, 0, 0, 0, 0, 0
            {(byte) 0xAA},// {1, 0, 1, 0, 1, 0, 1, 0,
            {(byte) 0x20},//   0, 0, 1, 0, 0, 0, 0, 0
            {(byte) 0x56},// {0, 1, 0, 1, 0, 1, 1, 0,
            {(byte) 0x10},//   0, 0, 0, 1, 0, 0, 0, 0
            {(byte) 0xE9},// {1, 1, 1, 0, 1, 0, 0, 1,
            {(byte) 0x08},//   0, 0, 0, 0, 1, 0, 0, 0
            {(byte) 0x95},// {1, 0, 0, 1, 0, 1, 0, 1,
            {(byte) 0x04},//   0, 0, 0, 0, 0, 1, 0, 0
            {(byte) 0x7B},// {0, 1, 1, 1, 1, 0, 1, 1,
            {(byte) 0x02},//   0, 0, 0, 0, 0, 0, 1, 0
            {(byte) 0xE7},// {1, 1, 1, 0, 0, 1, 1, 1,
            {(byte) 0x01}//  0, 0, 0, 0, 0, 0, 0, 1
    };

    private static byte[][] transposedH = new byte[][]{
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

    private static byte[] encode(byte message) {
        byte[] result = new byte[2];
        byte temp;
        byte[] temp2 = new byte[]{message};
        System.arraycopy(temp2, 0, result, 0, 1);
        for (int i = 0; i < 16; i=i+2) {
            int counter = 0;
            temp = (byte) (result[0] & H[i][0]);
            for (int j = 0; j < 8; j++) {
                if (((temp >> j) & 1) == 1) {
                    counter++;
                }
            }
            if(counter%2!=0){
                result[1]=(byte)((result[1]|(0x1<<7-(i/2))));
            }
        }
        return result;
    }
    private static byte correct(byte message, byte errorVector){
            for (int i = 0; i < 8; i++) {
                if (errorVector == transposedH[i][0]) {
                    message = (byte) (message ^ (0x1 << 7 - i));
                }
            }
            for (int i = 0; i < 16; i++) {
                for (int j = 0; j < 16; j++) {  // tu u kuby jest j=1
                    byte temp = (byte) (transposedH[i][0] ^ transposedH[j][0]);
                    if (temp == errorVector) {
                        message = (byte) (message ^ (0x1 << 7 - i));
                        message = (byte) (message ^ (0x1 << 7 - j));
                    }
                }
            }
        return message;
    }
    private static byte decode(byte[] encodedMessage) {
        byte errorVector = errorVector(encodedMessage);
        return correct(encodedMessage[0],errorVector);
    }
    public static byte errorVector(byte[] encodedMessage){
        byte result = 0x0;
        for (int i = 0; i < 16; i=i+2) {
            int counter1 = 0;
            int counter2 = 0;
            byte temp1 = (byte)(encodedMessage[0]&H[i][0]);
            for (int j = 0; j < 8; j++) {
                if (((temp1 >> j) & 1) == 1) {
                    counter1++;
                }
            }
            byte temp2 = (byte)(encodedMessage[1]&H[i+1][0]);
            for (int j = 0; j < 8; j++) {
                if (((temp2 >> j) & 1) == 1) {
                    counter2++;
                }
            }
            if ((counter1+counter2) % 2 == 0){
                result =(byte)(result | (0x1 << (7 - (i/2))));
            }
        }
        return result;
    }

    public static void printByteBits(byte b) {
        for (int i = 7; i >= 0; i--) {
            System.out.print((b >> i) & 1);
        }
        System.out.println();
    }
    public static void main(String[] args) throws IOException {
        Scanner scan = new Scanner(System.in);
        System.out.println("Podaj wiadomosc: ");
        String string = scan.nextLine();
        createFile(string,"message.txt");
        byte [] input = read("message.txt");
        StringBuilder builder = new StringBuilder();
        byte[] result;
//        byte[] temp = new byte[input.length*2];
//        int sum=0;
        for (int i = 0; i <input.length ; i++) {
            result = encode(input[i]);
//            test(result);
            for (byte b: result
            ) {
//                temp[sum] = b;
//                sum++;
                builder.append((char)b);
            }
        }
//        System.out.println(sum); // to daje 44 przy 22 stringach, czyli git
//        builder.deleteCharAt(-1); to nie dziala wiadomo, ale pokazuje, ze builder ma dlugosc 44, czyli .getBytes() pierdoli chyba
//        Wydaje mi sie, że jak w tych bitach kontrolnych są "1" na najstarszym bicie to getBytes() traktuje je jako juz kolejny bajt bo to kod znaku jednak jest, ale moge pierdolic glupoty
//        System.out.println(builder.toString().getBytes().length);//a to daje 52 wtf?
//        System.out.println(builder.toString().getBytes(StandardCharsets.US_ASCII).length); // to zwraca git bloodTrail
        //przy niektorych znakach pojawia sie wiecej bajtow niz 1 idk dlaczego (encode czasem daje wiecej bajtow niz x2)
        createFile(builder.toString(),"messageEncoded.txt");
//        System.out.println("Co chcesz teraz zrobic:\n1. Zmienić plik zakodowany i nastepnie go odkodowac.\n2. Odkodować zakodowaną przez ciebie wczesniej wiadomosc.");
//        String choice = scan.nextLine();
//        switch (choice) {
//            case "1":
//tu bedzie kod ktory pozwoli zmienic zawartosc pliku "messageEncoded"
        System.out.println("Wprowadz zmiany w pliku 'messageEncoded' i wpisz cokolwiek jesli juz skonczysz (jak nie chcesz zmieniac to nacisnij cokolwiek bez zmiany");
        scan.nextLine();
        StringBuilder builder3 = new StringBuilder();
        byte[] encodeResultt = read("messageEncoded.txt");
//                test(temp);
//                System.out.println(encodeResult.length);
        for (int i = 0; i < encodeResultt.length; i+= 2) {
//                    byte[] temp2 = new byte[]{temp[i], temp[i + 1]};
            byte[] temp2 = new byte[]{encodeResultt[i], encodeResultt[i + 1]};
            byte decodeResult = decode(temp2);
            builder3.append((char) decodeResult);
        }
        createFile(builder3.toString(), "messageDecoded.txt");
//                break;
//            case "2":
//                StringBuilder builder2 = new StringBuilder();
//                byte[] encodeResult = builder.toString().getBytes(StandardCharsets.US_ASCII);
////                test(temp);
////                System.out.println(encodeResult.length);
//                for (int i = 0; i < temp.length; i+= 2) {
////                    byte[] temp2 = new byte[]{temp[i], temp[i + 1]};
//                    byte[] temp2 = new byte[]{encodeResult[i], encodeResult[i + 1]};
//                    byte decodeResult = decode(temp2);
//                    builder2.append((char) decodeResult);
//                }
//                    createFile(builder2.toString(), "messageDecoded.txt");
//                    break;
//            default:
//                throw new Error("Wrong input");
//        }


//        byte[] input = new byte[]{'a'};
//        input=encode(input[0]);
//        test(input);
//        byte[] tab = new byte[]{96,127};
//        test(tab);
//        printByteBits(errorVector(tab));
//        printByteBits(decode(tab));
    }
}