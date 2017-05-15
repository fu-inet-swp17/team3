package de.swp.internetkommunikation.dump;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Set;

import de.swp.internetkommunikation.as.AS;

public class DumpAnalyse {
	public static void main(String[] args) throws IOException {
		File file = new File("bgpdump");
		if(!file.exists() && !file.canRead()){
			System.out.println("Keine Datei.");
			return;
		}
		FileReader reader = new FileReader(file);
		BufferedReader buff = new BufferedReader(reader);
		String[] informationList;
		HashMap<String, AS> asList = new HashMap<>();
		while(buff.ready()){
			informationList = buff.readLine().split("[|]");
			if(informationList.length < 9 || informationList[5].isEmpty() || informationList[6].isEmpty() || informationList[8].isEmpty()){
				continue;
			}
			AS tmp = asList.get(informationList[6]);
			if(informationList[6].equals("46.149.194.1")){
				assert Boolean.TRUE;
			}
			if(tmp == null){
				tmp = new AS(informationList[5], informationList[6]);
				asList.put(informationList[6], tmp);
			}
			AS tmp2 = asList.get(informationList[8]);
			if(tmp2 == null){
				tmp2 = new AS("Empty", informationList[8]);
				asList.put(informationList[8], tmp2);
			}
			tmp.setNewLink(tmp2);
			tmp.setName(informationList[5]);
		}
		buff.close();
		Set<String> keySet = asList.keySet();
		for (String string : keySet) {
			System.out.println(asList.get(string).toString());
		}
	}
}
