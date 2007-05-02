import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;


public class Sniffer {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		
		if (args.length != 3)
		{
			System.out.println("Parâmetros incorretos.");
			System.out.println("Execute segundo o exemplo:");
			System.out.println("\tjava Sniffer 1 19200 teste.txt");
		}
		else
		{
			int portNumber = Integer.parseInt(args[0]);
			int baud = Integer.parseInt(args[1]);
			String fileName = args[2];
			
			SimpleSerial serial = null;
			try {
				serial = new SimpleSerialNative(portNumber, baud, 8, 0, 1);
				
				// If there's an error, throw an exception
				if (!serial.isValid()) {
				    throw (new IOException("Serial port not opened"));
				}
				
				serial.getOutputStream().write("+MSTR=000C\r\n".getBytes());
				serial.getOutputStream().write("+MSTR=5101\r\n".getBytes());
				
				FileWriter fileWriter = new FileWriter(fileName, true);

				fileWriter.write(new Date().getTime()+":Sniffer started\r\n");
				
				for (;true;) {
					char charRead = (char)serial.readByte();
					Date now = new Date();
					if (charRead == '+')
					{
						System.out.print(now.getTime()+"");
						fileWriter.write(now.getTime()+"");
					}
					System.out.print(charRead);
					fileWriter.write(charRead);
					fileWriter.flush();
				}
				
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				if (serial != null)
				{
					serial.close();
				}
			}
		}
	}

}
