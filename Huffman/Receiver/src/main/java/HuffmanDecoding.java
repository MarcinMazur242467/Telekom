import java.util.HashMap;
import java.util.Map;
import java.util.PriorityQueue;

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

    public static Map<Character, Integer> buildFrequencyTable(String input) {
        Map<Character, Integer> frequencyTable = new HashMap<>();

        for (char c : input.toCharArray()) {
            frequencyTable.put(c, frequencyTable.getOrDefault(c, 0) + 1);
        }

        return frequencyTable;
    }

    public static HuffmanNode buildHuffmanTree(String input) {
        Map<Character, Integer> frequencyTable = buildFrequencyTable(input);
        PriorityQueue<HuffmanNode> priorityQueue = new PriorityQueue<>();

        for (Map.Entry<Character, Integer> entry : frequencyTable.entrySet()) {
            priorityQueue.offer(new HuffmanNode(entry.getValue(), entry.getKey(), null, null));
        }

        while (priorityQueue.size() > 1) {
            HuffmanNode left = priorityQueue.poll();
            HuffmanNode right = priorityQueue.poll();

            HuffmanNode parent = new HuffmanNode(left.freq + right.freq, '\0', left, right);
            priorityQueue.offer(parent);
        }

        return priorityQueue.poll();
    }

}
