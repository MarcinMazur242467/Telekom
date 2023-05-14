import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.net.Socket;

public class Sender {
    public static void main(String[] args) throws IOException {
        // szyfrowanie
        String input = "ala ma kota";
        HuffmanNode root = HuffmanEncoding.buildHuffmanTable(input);
        String encoded = HuffmanEncoding.encode(input, root);
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
