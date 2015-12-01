import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;

public class GUI {

    public static void main(String[] args) throws FileNotFoundException, IOException, InterruptedException {
        DatagramSocket serverSocket = new DatagramSocket(4149);
        byte[] receiveData = new byte[500];
        while(true)
           {
              DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
              serverSocket.receive(receivePacket);
              String sentence = new String(receivePacket.getData()).trim();
              System.out.println("RECEIVED: " + sentence);
           }
    }

}
