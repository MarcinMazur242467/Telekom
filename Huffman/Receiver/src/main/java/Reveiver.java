import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class Reveiver {
    public static void main(String[] args) throws IOException, ClassNotFoundException {

        ServerSocket serverSocket = new ServerSocket(6666);
        Socket socket = serverSocket.accept();
        System.out.println("Polączono");
        ObjectInputStream objectInputStream = new ObjectInputStream(socket.getInputStream());

        // Deserializacja obiektu z strumienia wejściowego (HuffmanNode)
        HuffmanNode root = (HuffmanNode) objectInputStream.readObject();

        // Odbieranie wiadomosci z socketa
        InputStream inputStream = socket.getInputStream();
        byte[] buffer = new byte[1024];
        int bytesRead = inputStream.read(buffer);
        String message = new String(buffer, 0, bytesRead);
        System.out.println("Odebrano: " + message);

        // Zamknięcie strumienia i socketu
        objectInputStream.close();
        inputStream.close();
        socket.close();
        serverSocket.close();

        // decode
        String decoded = HuffmanDecoding.decode(message, root);
        System.out.println("Decoded: " + decoded);

        try {
            FileWriter writer = new FileWriter(".\\poDeszyfrowaniu.txt");

            // Tworzenie obiektu BufferedWriter dla wydajnego zapisu
            BufferedWriter bufferedWriter = new BufferedWriter(writer);

            // Zapisywanie zawartości do pliku
            bufferedWriter.write(decoded);

            // Zamykanie BufferedWriter
            bufferedWriter.close();
        } catch (Exception e){
            throw new IOException(e);
        }

    }
}
