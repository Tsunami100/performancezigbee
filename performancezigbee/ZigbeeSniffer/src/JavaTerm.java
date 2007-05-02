import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;


/*

Created on January 2, 2000
Ben Resner
benres@media.mit.edu

Simple terminal program.  Good example for the SimpleSerial class.
There's a fair amount of code that's irrelevant to the serial port.
For a first-pass understanding, search for 'm_SerialPort'

For an even simpler example, see the main() rotine of SimpleSerial.java
*/


public class JavaTerm implements ItemListener, ActionListener {    
    
    Frame                   m_Frame;                                            // main window frame
    TextArea                m_TextInput;                                        // bottom half of window where text gets read in
    TextArea                m_TextOutput;                                       // top half of window where text gets sent out
    CheckboxMenuItem        m_PortMenuItem[] = new CheckboxMenuItem[5];         // com port menu items
    MenuItem                m_ClearMenuItem = new MenuItem("Clear Buffers");    // 'clear buffers' menu item
    CheckboxMenuItem        m_ModeMenuItem[] = new CheckboxMenuItem[2];
    SimpleSerial            m_SerialPort = null;                                // Serial port
    int                     m_PortIndex;                                        // Which comm port to use (1-based value -- there is no Comm0)
    boolean                 m_IsNative = true;                                  // Use local code, or use JavaComm from Sun
    static final String     m_PrefsFileName = new String("JavaTerm.pref");      // name of preferences file
    
    public static void main(String[] argh) {       
        // Create instance of JavaTerm().  No reason why we shouldn't be able to create multiple instances.
        // The only wrinkle would be making the various prefs files unique.
        new JavaTerm();
    }

    // Init the serial port and associated Input/Output streams
    private void initSerialPort() throws IOException {    
        
        // If serial port was previously opened, close it now
        // Most applications open serial port, and never need to close it again.
        if (m_SerialPort != null) {
            m_SerialPort.close();     
            m_SerialPort = null;
        }
        
        // New instance of the serial port.
        if (m_IsNative) {
            m_SerialPort = new SimpleSerialNative(m_PortIndex);
        }
        else {
            m_SerialPort = new SimpleSerialJava(m_PortIndex);
        }

        // If there's an error, throw an exception
        if (!m_SerialPort.isValid()) {
            throw (new IOException("Serial port not opened"));
        }
    }        
        
    // Constructor        
    JavaTerm() {      
        int                     ii;        
        Menu                    menu;
        MenuBar                 menuBar;        
        
        // Load in the port number from the last time we ran
        try {
            // try opening the file JavaTerm.pref
            DataInputStream prefs = new DataInputStream(new FileInputStream(m_PrefsFileName));
            // Read in serial port index
            m_PortIndex = prefs.readInt();
            m_IsNative = prefs.readBoolean();
            // check to make sure the value we read is valid
            if (m_PortIndex < 0 || m_PortIndex >= m_PortMenuItem.length) {
                throw new IOException(m_PrefsFileName + " is corrupt");
            }
            // init the serial port
            initSerialPort();
        }
        // We'll get here if the file doesn't exist because it's the first time running this program,
        // it can't be found / opened, or the requested serial port can't be opened
        catch(IOException e) {
            System.out.println("preferences file 'JavaTerm.pref' not found / didn't open or there was a problem opening serial port.  Searching for serial port");
            
            // Open the first available serial port.
            // This code can be adapted to provide a list of available serial ports.
            find_open_serial_port:
            for (ii = 0; ii < m_PortMenuItem.length; ii++) {
                try {
                    m_PortIndex = ii + 1;             // This is the serial port we want to open
                    initSerialPort();                       // Try opening this serial port.  Throws exception if there's a problem
                    System.out.println("Opening serial port Comm" + m_PortIndex);
                    break find_open_serial_port;            // If we haven't thrown an exception, we're done
                }
                catch (IOException ee) {                     // wind up here if initSerialPort() above has a problem
                    if (ii == m_PortMenuItem.length - 1) {  
                        System.out.println("Couldn't open any serial ports");
                        System.exit(0);                     // can't open any serial ports.
                    }
                }
            }                        
        }
        
        // new our window frame
        m_Frame = new Frame("Javaterm");
            
        // set frame parameters
        m_Frame.setSize(400, 600);
        m_Frame.setLocation(100, 100);
        m_Frame.setVisible(true);                        

        // add the menu.  
        menuBar = new MenuBar();
        menu = new Menu("Port");
        for (ii = 0; ii < m_PortMenuItem.length; ii++) {
            menu.add(m_PortMenuItem[ii] = new CheckboxMenuItem("Com " + (ii + 1), false));
            m_PortMenuItem[ii].addItemListener(this);
        }
        m_PortMenuItem[m_PortIndex - 1].setState(true);
        menuBar.add(menu);
            
        menu = new Menu("Clear");
        menu.add(m_ClearMenuItem);
        m_ClearMenuItem.addActionListener(this);          
        menuBar.add(menu);
        
        menu = new Menu("Mode");
        m_ModeMenuItem[0] = new CheckboxMenuItem("Native");
        m_ModeMenuItem[0].addItemListener(this);
        m_ModeMenuItem[0].setState(m_IsNative);
        menu.add(m_ModeMenuItem[0]);
        m_ModeMenuItem[1] = new CheckboxMenuItem("JavaComm");
        m_ModeMenuItem[1].addItemListener(this);
        m_ModeMenuItem[1].setState(!m_IsNative);
        menu.add(m_ModeMenuItem[1]);
        menuBar.add(menu);
            
        m_Frame.setMenuBar(menuBar);            

        // create where the text areas goe
        m_TextInput = new TextArea();
        m_TextInput.setEditable(false);
        m_Frame.add(m_TextInput);                       
            
        m_TextOutput = new TextArea();
        m_Frame.add(m_TextOutput);
            
        // position the text areas
        updateWindowComponents();
                                    
        // When a key is pressed, send that keystroke to the serial port
        m_TextOutput.addKeyListener(new KeyAdapter() {
            public void keyTyped(KeyEvent  e) {
                byte    tempByte = (byte)e.getKeyChar();                  
                m_SerialPort.writeByte(tempByte);     // <-- Here's where the data gets written
                                                            // DONT use writeChar().  It's a two byte char
                                                            // and has screwey results.
                        
                // HACKISH:  Need to add newline to carriage returns for proper operation
                // You may not need this.
                if (tempByte == '\r') {
                    m_SerialPort.writeByte((byte)'\n');
                }                        
            }
        } );   
            
        // Make sure the focus stays on the output window.  If input pane gets focus, set focus back to output pane
        m_TextInput.addFocusListener(new FocusAdapter() {
            public void focusGained(FocusEvent e) {
                m_TextOutput.requestFocus();
            }
        });
            
        // If user clicks close box, exit this program
        m_Frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                // Write the current serial port to disk
                writePrefs();
                System.exit(0);
            }
        } );
              
        // If user resizes the window, need to reposition the text areas.            
        m_Frame.addComponentListener(new ComponentAdapter() {
            public void componentResized(ComponentEvent e) {
                updateWindowComponents();
            }
        });
                        
        // Infinite loop.  WindowListener above will break us out of loop with call to System.exit(0);
        for (;true;) {                
            // Get any pending characters from serial port.
            // Returns empty string if there's nothing to read.
            // This is in contrast to readByte() which patiently waits for data.
            
            if (m_SerialPort != null) {            
                String      inputString = m_SerialPort.readString();                                       
                m_TextInput.append(inputString);
            }

            try {                
                Thread.sleep(50);
            }
            catch (InterruptedException e) {
            }
        }            
    }
    
    // When user changes ComPort from menu, this code gets hit.
    public void itemStateChanged(ItemEvent e) {
        int         ii;
        int         prevSerialPort = m_PortIndex;
        boolean     prevIsNative = m_IsNative;
        boolean     reinitPort = false;
        
        // find which com port was requested
        find_menu_com_port:
        for (ii = 0; ii < m_PortMenuItem.length; ii++) {
            if (e.getSource() == m_PortMenuItem[ii]) {
                m_PortIndex = ii + 1;
                reinitPort = true;
                break find_menu_com_port;
            }
        }                        
        
        // Maybe it's the mode that's changed.
        if (e.getSource() == m_ModeMenuItem[0]) {
            m_IsNative = true;
        }
        if (e.getSource() == m_ModeMenuItem[1]) {
            m_IsNative = false;
        }
        
        if (m_IsNative != prevIsNative) {
            reinitPort = true;
        }
        
        if (reinitPort) {                
            try {                    
                initSerialPort();                
            }
            catch (IOException ex) {
                System.out.println("Requsted serial port couldn't be opened -- reverting");
                m_PortIndex = prevSerialPort;
                m_IsNative = prevIsNative;
                try {
                    initSerialPort();
                }
                catch (IOException ex2) {
                    System.out.println("Couldn't restore previous serial port.  You're hosed");
                    System.exit(0);
                }
            }
        }
        
        // Make sure menu checkmarks match current com port.
        for (ii = 0; ii < m_PortMenuItem.length; ii++) {
            m_PortMenuItem[ii].setState( (m_PortIndex == ii + 1) ? true : false);
        }
                        
        m_ModeMenuItem[0].setState(m_IsNative);
        m_ModeMenuItem[1].setState(!m_IsNative);
                
        writePrefs();
    }
    
    private void writePrefs() {
        try {
            DataOutputStream prefs = new DataOutputStream(new FileOutputStream(m_PrefsFileName));
            prefs.writeInt(m_PortIndex);
            prefs.writeBoolean(m_IsNative);
        } catch (IOException ee) {}                            
    }
    
    // If user selects "Clear Buffers" we wind up here.
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == m_ClearMenuItem) {
            m_TextInput.setText("");
            m_TextOutput.setText("");
        }
    }
    
    // Updates input and output text areas.
    private void updateWindowComponents()
    {            
        Insets      frameInsets = m_Frame.getInsets();
        Dimension   dim = m_Frame.getSize();
        
        dim.height -= (frameInsets.bottom + frameInsets.top);
        dim.width -= (frameInsets.right + frameInsets.left);
        
        m_TextInput.setSize(dim.width, dim.height / 2);
        m_TextInput.setLocation(frameInsets.left, frameInsets.top + dim.height / 2);
                
        m_TextOutput.setSize(dim.width, dim.height / 2);
        m_TextOutput.setLocation(frameInsets.left, frameInsets.top);                        
        
        m_TextOutput.requestFocus();
    }    
};