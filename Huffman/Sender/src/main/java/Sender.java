import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.net.Socket;

public class Sender {
    public static void main(String[] args) throws IOException {

        String filePath = "D:\\JavaProjects\\Telekom\\Huffman\\Sender\\doSzyfrowania.txt";
        String fileContent;

        try {
            // Odczytaj zawartość pliku i zapisz jako ciąg znaków
            fileContent = HuffmanEncoding.readFileToString(filePath);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        // szyfrowanie
        HuffmanNode root = HuffmanEncoding.buildHuffmanTable(fileContent);
        String encoded = HuffmanEncoding.encode(fileContent, root);
        System.out.println("Encoded: " + encoded);


        // wysylanie
        Socket socket = new Socket("127.0.0.1", 6666);
        ObjectOutputStream objectOutputStream = new ObjectOutputStream(socket.getOutputStream());
        objectOutputStream.writeObject(root);
        OutputStream outputStream = socket.getOutputStream();
        byte[] data = encoded.getBytes();
        outputStream.write(data);

        // zamykanie strumieni
        objectOutputStream.close();
        outputStream.close();
        socket.close();
    }
}
