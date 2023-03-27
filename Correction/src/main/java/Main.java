import java.io.*;
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
    private static byte[] read(String fileName) throws FileNotFoundException {
        Scanner scanner = new Scanner(new File(fileName));
        String text = scanner.nextLine();
        scanner.close();
        return text.getBytes();
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
                result[1]=(byte)((result[1]|(0x1<<7-i)));
            }
        }
        return result;
    }
    private static byte[] correct(byte[] encodedMessage, byte[] errorVector){
        byte[] result = new byte[2];
        return result;
    }
    private static byte decode(byte[] encodedMessage) {
        /*byte[] vector;
        encodedMessage = correct(encodedMessage,vector);*/
        return encodedMessage[0];
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
            if ((counter1+counter2) % 2 != 0){
                result =(byte)(result | (0x1 << (7 - i)));
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
//        Scanner scan = new Scanner(System.in);
//        System.out.println("Podaj wiadomosc: ");
//        String string = scan.nextLine();
//        createFile(string,"message.txt");
//        byte [] input = read("message.txt");
//        StringBuilder builder = new StringBuilder();
//        byte[] result;
//        for (int i = 0; i <input.length ; i++) {
//            result = encode(input[i]);
//            for (byte b: result
//                 ) {
//                builder.append((char)b);
//            }
//        }
//        System.out.println(builder);
//        createFile(builder.toString(),"messageEncoded.txt");
        byte[] input = new byte[]{'a'};
        input=encode(input[0]);
        test(input);
        byte[] tab = new byte[]{96,42};
        test(tab);
        printByteBits(errorVector(tab));
    }
}
