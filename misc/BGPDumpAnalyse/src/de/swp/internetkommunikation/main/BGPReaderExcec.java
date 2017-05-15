package de.swp.internetkommunikation.main;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import de.swp.internetkommunikation.Runnables.StreamHandler;
import de.swp.internetkommunikation.as.AS;

public class BGPReaderExcec {
	public static void main(String[] args) throws IOException {
		
		File file = new File("Links");
		if(!file.exists()){
			if(!file.createNewFile()){
				System.out.println("Konnte logfile nicht erstellen.");
				return;
			}
		}
		PrintWriter writer = new PrintWriter(file);
		
		BlockingQueue<String> queue = new LinkedBlockingQueue<>();
		LinkedList<Thread> processes = new LinkedList<>();
		for (int i = 0; i < args.length; i++) {
			String processStarterString = "bgpreader -w 1494337423 -c " + args[i] + " -t updates";
			System.out.println("Started: " + processStarterString);
			Thread tmp = new Thread(new StreamHandler(Runtime.getRuntime().exec(processStarterString),queue));
			tmp.start();
			processes.add(tmp);
		}
		
		String tempRead = null;
		String[] informationList;
		HashMap<String, AS> asList = new HashMap<>();
		if(processes.size() > 0){
			try {
				while((tempRead = queue.poll(40, TimeUnit.SECONDS)) != null){
					informationList = tempRead.split("[|]");
					if(informationList.length < 9 || informationList[5].isEmpty() || informationList[6].isEmpty() || informationList[8].isEmpty()){
						continue;
					}
					AS tmp = asList.get(informationList[6]);
					if(tmp == null){
						tmp = new AS(informationList[5], informationList[6]);
						asList.put(informationList[6], tmp);
					}
					AS tmp2 = asList.get(informationList[8]);
					if(tmp2 == null){
						tmp2 = new AS("Empty", informationList[8]);
						asList.put(informationList[8], tmp2);
					}
					if(tmp.setNewLink(tmp2)){
						writer.println("Found new link.");
						writer.println(tmp.toString());
						writer.flush();
					}
					tmp.setName(informationList[5]);
					
					if(queue.size() > 1000000)
						System.out.println("Warning Queue leuft voll.");
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			for (Thread process : processes) {
				process.interrupt();;
			}
			writer.close();
		}
	}
}
