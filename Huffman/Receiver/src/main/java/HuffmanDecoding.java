
public class HuffmanDecoding {


    public static String decode(String encoded, HuffmanNode root) {
        StringBuilder decoded = new StringBuilder();
        HuffmanNode current = root;

        for (char bit : encoded.toCharArray()) {
            if (bit == '0') {
                current = current.left;
            } else if (bit == '1') {
                current = current.right;
            }

            if (current.isLeaf()) {
                decoded.append(current.c);
                current = root;
            }
        }

        return decoded.toString();
    }

}
