import java.util.HashMap;
import java.util.Map;
import java.util.PriorityQueue;

public class HuffmanEncoding {
    public static Map<Character, String> buildHuffmanTable(String input) {
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

        HuffmanNode root = priorityQueue.poll();
        Map<Character, String> huffmanTable = new HashMap<>();
        buildHuffmanTableRecursive(root, "", huffmanTable);

        return huffmanTable;
    }

    private static void buildHuffmanTableRecursive(HuffmanNode node, String code, Map<Character, String> huffmanTable) {
        if (node.isLeaf()) {
            huffmanTable.put(node.c, code);
            return;
        }

        buildHuffmanTableRecursive(node.left, code + "0", huffmanTable);
        buildHuffmanTableRecursive(node.right, code + "1", huffmanTable);
    }

    public static Map<Character, Integer> buildFrequencyTable(String input) {
        Map<Character, Integer> frequencyTable = new HashMap<>();

        for (char c : input.toCharArray()) {
            frequencyTable.put(c, frequencyTable.getOrDefault(c, 0) + 1);
        }

        return frequencyTable;
    }

    public static String encode(String input) {
        Map<Character, String> huffmanTable = buildHuffmanTable(input);
        StringBuilder encoded = new StringBuilder();

        for (char c : input.toCharArray()) {
            encoded.append(huffmanTable.get(c));
        }

        return encoded.toString();
    }
}