public class Reveiver {
    public static void main(String[] args) {

        HuffmanNode root = HuffmanDecoding.buildHuffmanTree("ala ma kota");
        String decoded = HuffmanDecoding.decode("01011011110001111010110111000", root);
        System.out.println("Decoded: " + decoded);
    }
}
