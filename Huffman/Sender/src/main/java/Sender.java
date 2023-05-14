public class Sender {
    public static void main(String[] args) {
        String input = "ala ma kota";
        String encoded = HuffmanEncoding.encode(input);
        System.out.println("Encoded: " + encoded);
    }
}
