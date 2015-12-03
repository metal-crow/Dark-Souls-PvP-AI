import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

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
    private JTextArea[] MemoryEnemy_Params=new JTextArea[11];
    
    private JPanel MemoryPlayer=new JPanel();
    private JTextArea[] MemoryPlayer_Params=new JTextArea[11];

    private JPanel Detection=new JPanel();
    private JTextArea[] Detection_Params=new JTextArea[4];
    private JPanel AIState=new JPanel();
    private JTextArea[] AIState_Params=new JTextArea[2];

    public GuiPane(int height, int width){
        this.width=width;
        this.height=height;
        setLayout(new GridBagLayout());
        GridBagConstraints c = new GridBagConstraints();
        c.fill = GridBagConstraints.BOTH;
        c.weightx = 1;
        c.weighty = 1;
        
        c.gridheight = 2;
        c.gridx = 0;
        c.gridy = 0;
        add(MemoryEnemy, c);
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
            MemoryEnemy_Params[p].setMinimumSize(new Dimension(width/2,height/2));
            MemoryEnemy.add(MemoryEnemy_Params[p]);
        }

        c.gridy = 2;
        add(MemoryPlayer, c);
        MemoryPlayer.setBorder(BorderFactory.createLineBorder(Color.BLACK));
        MemoryPlayer.setLayout(new BoxLayout(MemoryPlayer, BoxLayout.Y_AXIS));
        JLabel Player_memory_title = new JLabel("PLAYER MEMORY DATA");
        Player_memory_title.setAlignmentX(Component.CENTER_ALIGNMENT);
        MemoryPlayer.add(Player_memory_title);
        MemoryPlayer.setMinimumSize(new Dimension(width/2,height/2));
        for(int p=0;p<MemoryPlayer_Params.length;p++){
            MemoryPlayer_Params[p] = new JTextArea(" ");
            MemoryPlayer_Params[p].setEditable(false);
            MemoryPlayer_Params[p].setMinimumSize(new Dimension(width/2,height/2));
            MemoryPlayer.add(MemoryPlayer_Params[p]);
        }
        
        c.gridx=1;
        c.gridy=0;
        c.gridheight=1;
        add(Detection,c);
        Detection.setBorder(BorderFactory.createLineBorder(Color.BLACK));
        Detection.setLayout(new BoxLayout(Detection, BoxLayout.Y_AXIS));
        JLabel Detection_title = new JLabel("CURRENT DETECTED STATES");
        Detection_title.setAlignmentX(Component.CENTER_ALIGNMENT);
        Detection.add(Detection_title);
        Detection.setMinimumSize(new Dimension(width/2,height/4));
        for(int p=0;p<Detection_Params.length;p++){
            Detection_Params[p] = new JTextArea(" ");
            Detection_Params[p].setEditable(false);
            Detection_Params[p].setMinimumSize(new Dimension(width/2,height/4));
            Detection.add(Detection_Params[p]);
        }
        
        c.gridx=1;
        c.gridy=1;
        c.gridheight=3;
        add(AIState,c);
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
    }
    
    public Dimension getPreferredSize() {
        return new Dimension(width,height);
    }
    
    public void HandleGUIUpdate(String updateData){
        int locationPosition_Seperator = updateData.indexOf(',');
        int positionData_Seperator = updateData.indexOf(':');
        int location = Integer.parseInt(updateData.substring(0,locationPosition_Seperator));
        int position = Integer.parseInt(updateData.substring(locationPosition_Seperator+1,positionData_Seperator));
        String data = updateData.substring(positionData_Seperator+1);
        
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
        }
    }
 }
    
public class GUI{
        
    static GuiPane innerGui;
    
    public static void main(String[] args) throws FileNotFoundException, IOException, InterruptedException {
        //get screen dimensions
        createAndShowGUI(500,500);
        
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
          }
        }
        //serverSocket.close();
    }
    
    private static void createAndShowGUI(int height, int width) {
        JFrame f = new JFrame("Dark Souls AI GUI");
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        f.setSize(height,width);
        innerGui=new GuiPane(height,width);
        f.add(innerGui);
        f.pack();
        f.setVisible(true);
    }

}
