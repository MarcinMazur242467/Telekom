public class HuffmanNode implements Comparable<HuffmanNode> {
    int freq;
    char c;
    HuffmanNode left;
    HuffmanNode right;

    public HuffmanNode(int freq, char c, HuffmanNode left, HuffmanNode right) {
        this.freq = freq;
        this.c = c;
        this.left = left;
        this.right = right;
    }

    public boolean isLeaf() {
        return left == null && right == null;
    }

    @Override
    public int compareTo(HuffmanNode otherNode) {
        return this.freq - otherNode.freq;
    }

}
