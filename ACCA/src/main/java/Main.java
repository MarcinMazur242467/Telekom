import javax.sound.sampled.*;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.Scanner;

public class Main {
    public static boolean isRecording = true;

    public static void main(String[] args) {
        int sampleRate = 44100;  // Przykładowa częstotliwość próbkowania (44100 Hz)
        int bitDepth = 16;       // Przykładowa głębia bitowa (16 bitów)

        // Przetwarzanie A/C (analog-to-digital)
        AudioFormat format = new AudioFormat(sampleRate, bitDepth, 1, true, false);
        DataLine.Info info = new DataLine.Info(TargetDataLine.class, format);
        Scanner scanner = new Scanner(System.in);
        System.out.println("Naciśnij ENTER jeśli chcesz rozpocząć nagrywanie. " +
                "Aby zakonczyć nagrywanie musisz także nacisnąc ENTER podczas nagrywania.");
        scanner.nextLine();
        try {
            TargetDataLine line = (TargetDataLine) AudioSystem.getLine(info);
            line.open(format);
            line.start();

            byte[] buffer = new byte[sampleRate * bitDepth / 8];
            System.out.println("Nagrywanie rozpoczęte...");

            // Tworzenie threada do nasluchu inputu uzytkownika
            Thread inputThread = new Thread(() -> {
                try {
                    System.in.read();
                    isRecording = false;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });
            inputThread.start();

            // Odczytaj dane z mikrofonu i zapisz do bufora
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            while (isRecording) {
                int bytesRead = line.read(buffer, 0, buffer.length);
                output.write(buffer, 0, bytesRead);
            }

            System.out.println("Nagrywanie zakończone.");

            System.out.print("Naciśnij ENTER jeśli chcesz odtworzyć swoje nagranie.");
            scanner.nextLine();

            // Zapisz bufor do pliku WAV
            ByteArrayInputStream input = new ByteArrayInputStream(output.toByteArray());
            AudioInputStream audioInputStream = new AudioInputStream(input, format, output.toByteArray().length);
            AudioSystem.write(audioInputStream, AudioFileFormat.Type.WAVE, new File("record.wav"));

        } catch (LineUnavailableException | IOException e) {
            e.printStackTrace();
        }

        // Przetwarzanie C/A (digital-to-analog)
        AudioInputStream audioInputStream;
        try {
            audioInputStream = AudioSystem.getAudioInputStream(new File("record.wav"));
            AudioFormat audioFormat = audioInputStream.getFormat();
            SourceDataLine line = AudioSystem.getSourceDataLine(audioFormat);
            line.open(audioFormat);
            line.start();

            byte[] buffer = new byte[4096];
            int bytesRead = 0;

            System.out.println("Odtwarzanie rozpoczęte...");

            // Odczytaj dane z pliku WAV i zapisz do bufora
            while ((bytesRead = audioInputStream.read(buffer)) != -1) {
                // Przetwarzanie strumienia audio (możesz dodać swoje operacje na danych)
                line.write(buffer, 0, bytesRead);
            }

            System.out.println("Odtwarzanie zakończone.");

            line.drain();
            line.close();
            audioInputStream.close();

        } catch (UnsupportedAudioFileException | IOException | LineUnavailableException e) {
            e.printStackTrace();
        }
    }

}