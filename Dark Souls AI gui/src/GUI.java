import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;

 @SuppressWarnings("serial")
class GuiPane extends JPanel{

    private int width;
    private int height;

    private JPanel MemoryEnemy=new JPanel();
    private JTextArea[] MemoryEnemy_Params=new JTextArea[17];
    
    private JPanel MemoryPlayer=new JPanel();
    private JTextArea[] MemoryPlayer_Params=new JTextArea[17];

    private JPanel Detection=new JPanel();
    private JTextArea[] Detection_Params=new JTextArea[6];
    private JPanel AIState=new JPanel();
    private JTextArea[] AIState_Params=new JTextArea[6];

    private JPanel JoystickInfo=new JPanel();
    private JTextArea[] JoystickInfo_Params=new JTextArea[6];
    
    private JPanel HandlerInfo=new JPanel();
    private JTextArea[] HandlerInfo_Params=new JTextArea[4];
    
    private static Pattern inputDataPattern;

    
    public GuiPane(int height, int width){
        this.width=width;
        this.height=height;
        setLayout(new GridLayout(3,2));
        
        add(MemoryEnemy);
        //for each area, set the title and add the labels which will be changed on update
        MemoryEnemy.setBorder(BorderFactory.createLineBorder(Color.BLACK));
        MemoryEnemy.setLayout(new BoxLayout(MemoryEnemy, BoxLayout.Y_AXIS));
        JLabel Enemy_memory_title = new JLabel("ENEMY MEMORY DATA");
        Enemy_memory_title.setAlignmentX(Component.CENTER_ALIGNMENT);
        MemoryEnemy.add(Enemy_memory_title);
        MemoryEnemy.setMinimumSize(new Dimension(width/2,height/2));
        for(int p=0;p<MemoryEnemy_Params.length;p++){
            MemoryEnemy_Params[p] = new JTextArea(" ");
            MemoryEnemy_Params[p].setEditable(false);
            MemoryEnemy.add(MemoryEnemy_Params[p]);
        }
        
        add(Detection);
        Detection.setBorder(BorderFactory.createLineBorder(Color.BLACK));
        Detection.setLayout(new BoxLayout(Detection, BoxLayout.Y_AXIS));
        JLabel Detection_title = new JLabel("CURRENT DETECTED STATES");
        Detection_title.setAlignmentX(Component.CENTER_ALIGNMENT);
        Detection.add(Detection_title);
        Detection.setMinimumSize(new Dimension(width/2,height/4));
        for(int p=0;p<Detection_Params.length;p++){
            Detection_Params[p] = new JTextArea(" ");
            Detection_Params[p].setEditable(false);
            Detection_Params[p].setLineWrap(true);
            Detection_Params[p].setWrapStyleWord(true);
            Detection_Params[p].setMinimumSize(new Dimension(width/2,height/4));
            Detection.add(Detection_Params[p]);
        }

        add(MemoryPlayer);
        MemoryPlayer.setBorder(BorderFactory.createLineBorder(Color.BLACK));
        MemoryPlayer.setLayout(new BoxLayout(MemoryPlayer, BoxLayout.Y_AXIS));
        JLabel Player_memory_title = new JLabel("PLAYER MEMORY DATA");
        Player_memory_title.setAlignmentX(Component.CENTER_ALIGNMENT);
        MemoryPlayer.add(Player_memory_title);
        MemoryPlayer.setMinimumSize(new Dimension(width/2,height/2));
        for(int p=0;p<MemoryPlayer_Params.length;p++){
            MemoryPlayer_Params[p] = new JTextArea(" ");
            MemoryPlayer_Params[p].setEditable(false);
            MemoryPlayer.add(MemoryPlayer_Params[p]);
        }
        
        add(AIState);
        AIState.setBorder(BorderFactory.createLineBorder(Color.BLACK));
        AIState.setLayout(new BoxLayout(AIState, BoxLayout.Y_AXIS));
        JLabel AI_state_title = new JLabel("CURRENT AI STATES");
        AI_state_title.setAlignmentX(Component.CENTER_ALIGNMENT);
        AIState.add(AI_state_title);
        AIState.setMinimumSize(new Dimension(width/2,3*height/4));
        for(int p=0;p<AIState_Params.length;p++){
            AIState_Params[p] = new JTextArea(" ");
            AIState_Params[p].setEditable(false);
            AIState_Params[p].setLineWrap(true);
            AIState_Params[p].setWrapStyleWord(true);
            AIState_Params[p].setMinimumSize(new Dimension(width/2,3*height/4));
            AIState.add(AIState_Params[p]);
        }
        
        add(JoystickInfo);
        JoystickInfo.setBorder(BorderFactory.createLineBorder(Color.BLACK));
        JoystickInfo.setLayout(new BoxLayout(JoystickInfo, BoxLayout.Y_AXIS));
        JLabel JoystickInfo_title = new JLabel("JOYSTICK INFO");
        JoystickInfo_title.setAlignmentX(Component.CENTER_ALIGNMENT);
        JoystickInfo.add(JoystickInfo_title);
        for(int p=0;p<JoystickInfo_Params.length;p++){
            JoystickInfo_Params[p] = new JTextArea(" ");
            JoystickInfo_Params[p].setEditable(false);
            JoystickInfo_Params[p].setLineWrap(true);
            JoystickInfo_Params[p].setWrapStyleWord(true);
            JoystickInfo.add(JoystickInfo_Params[p]);
        }
        
        add(HandlerInfo);
        HandlerInfo.setBorder(BorderFactory.createLineBorder(Color.BLACK));
        HandlerInfo.setLayout(new BoxLayout(HandlerInfo, BoxLayout.Y_AXIS));
        JLabel HandlerInfo_title = new JLabel("HANDLER INFO");
        HandlerInfo_title.setAlignmentX(Component.CENTER_ALIGNMENT);
        HandlerInfo.add(HandlerInfo_title);
        for(int p=0;p<HandlerInfo_Params.length;p++){
            HandlerInfo_Params[p] = new JTextArea(" ");
            HandlerInfo_Params[p].setEditable(false);
            HandlerInfo_Params[p].setLineWrap(true);
            HandlerInfo_Params[p].setWrapStyleWord(true);
            HandlerInfo.add(HandlerInfo_Params[p]);
        }
        
        //[gui location],[position in location]:[string literal]:[timestamp]
        inputDataPattern = Pattern.compile("(\\d+),(\\d+):(.*):(\\d{2}:\\d{2}:\\d{2}\\.\\d{3})",Pattern.DOTALL);
    }
    
    public Dimension getPreferredSize() {
        return new Dimension(width,height);
    }
    
    public void HandleGUIUpdate(String updateData){
        Matcher m = inputDataPattern.matcher(updateData);
        if (m.find()) {
            int location = Integer.parseInt(m.group(1));
            int position = Integer.parseInt(m.group(2));
            String data = m.group(3);
            String timestamp = m.group(4);
            
            switch(location){
                case 0:
                    MemoryEnemy_Params[position].setText(data);
                    break;
                case 1:
                    MemoryPlayer_Params[position].setText(data);
                    break;
                case 2:
                    Detection_Params[position].setText(data);
                    break;
                case 3:
                    AIState_Params[position].setText(data);
                    break;
                case 4:
                    JoystickInfo_Params[position].setText(data);
                    break;
                case 5:
                    HandlerInfo_Params[position].setText(data);
                    break;
            }
            
            //update latest timestamp
            HandlerInfo_Params[3].setText(timestamp);
        }
    }
 }
    
public class GUI{
        
    static GuiPane innerGui;
    
    public static void main(String[] args) throws FileNotFoundException, IOException, InterruptedException {
        //get screen dimensions
        createAndShowGUI(450,700);
        
        DatagramSocket serverSocket = new DatagramSocket(4149);
        byte[] receiveData = new byte[500];
        
        while(true){
          DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
          serverSocket.receive(receivePacket);
          String updateData = new String(receivePacket.getData()).trim();
          
          try{
              innerGui.HandleGUIUpdate(updateData);
          }catch(Exception e){
              e.printStackTrace();
              System.err.println("Invalid data: \""+updateData+"\"");
          }
        }
        //serverSocket.close();
    }
    
    private static void createAndShowGUI(int width, int height) {
        JFrame f = new JFrame("Dark Souls AI GUI");
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        f.setSize(width, height);
        innerGui=new GuiPane(height,width);
        f.add(innerGui);
        f.pack();
        f.setVisible(true);
    }

}
