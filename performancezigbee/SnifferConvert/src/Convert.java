import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Reader;
import java.io.Writer;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;


public class Convert {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if (args.length != 2)
		{
			System.out.println("Parâmetros incorretos.");
			System.out.println("Execute segundo o exemplo:");
			System.out.println("\tjava Convert arquivoFonte.arq arquivoDestino.154");
		}
		else
		{
			BufferedReader fileReader = null;
			try {
				fileReader = new BufferedReader(new FileReader(args[0]));
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			}
			
			Writer fileWriter = null;
			try {
				fileWriter = new BufferedWriter(new FileWriter(args[1], false));
			} catch (Exception e) {
				e.printStackTrace();
			}

			try {
				
				fileWriter.write("+INFO:IEEE 802.15.2 Sniffer Log   www.flexipanel.com"+"\r\n");
				
				fileWriter.write("+DATE:Fri Oct 28 18:14:37 2005"+"\r\n");
				fileWriter.write("+CHAN:2405,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2410,80,01,CA,FF" + "\r\n");
				fileWriter.write("+CHAN:2415,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2420,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2425,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2430,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2435,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2440,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2445,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2450,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2455,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2460,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2465,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2470,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2475,80,00,CA,00" + "\r\n");
				fileWriter.write("+CHAN:2480,80,00,CA,00"+"\r\n");
				
				for (String line; (line = fileReader.readLine()) != null; ) 
				{
					if (!line.trim().equals("") && line.indexOf("+PDAI=") > 0)
					{
						String snifferLine = "+FRAM:";
						String split[] = line.split("\\+PDAI=");
						
						long dateTime = Long.parseLong(split[0]);
						Calendar calendar = GregorianCalendar.getInstance();
						calendar.setTimeInMillis(dateTime);

						DecimalFormat format2 = new DecimalFormat("00");
						DecimalFormat format3 = new DecimalFormat("000");
						
						snifferLine += format2.format(calendar.get(Calendar.HOUR_OF_DAY));
						snifferLine += ",";
						snifferLine += format2.format(calendar.get(Calendar.MINUTE));
						snifferLine += ",";
						snifferLine += format2.format(calendar.get(Calendar.SECOND));
						snifferLine += ",";
						snifferLine += format3.format(calendar.get(Calendar.MILLISECOND));

						for (int i = 0; i < split[1].length(); i+=2) 
						{
							snifferLine += ",";
							snifferLine += split[1].substring(i, i+2);
						}
//						System.err.println(snifferLine);
//						System.out.println(split[0]);
//						if (split.length > 1)
//							System.out.println(split[1]);
//						System.out.println(line);
						
						fileWriter.write(snifferLine+"\r\n");
						System.out.println(snifferLine);
					}
				}
				fileWriter
						.write("+NODE:\"General\",FF,FF,FF,FF,??,??,??,??,??,??,??,??,5F"
								+ "\r\n");
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
				try {
					fileReader.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				try {
					fileWriter.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			
		}
	}

}
