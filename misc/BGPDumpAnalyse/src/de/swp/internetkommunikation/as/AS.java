package de.swp.internetkommunikation.as;

import java.util.HashMap;

public class AS {
	private String name;
	public void setName(String name) {
		this.name = name;
	}


	private String ip;
	private HashMap<String, AS> links;
	
	
	public AS(String name, String ip) {
		super();
		this.name = name;
		this.ip = ip;
		links = new HashMap<>();
	}

	public boolean setNewLink(AS node){
		if(links.get(node.getIp()) != null){
			return false;
		}else{
			links.put(node.getIp(), node);
			return true;
		}
	}

	public String getName() {
		return name;
	}


	public String getIp() {
		return ip;
	}


	public HashMap<String, AS> getLinks() {
		return links;
	}
	
	public int getLinkSize(){
		return links.size();
	}
	
	@Override
	public String toString() {
		StringBuffer tmp = new StringBuffer("AS name: " + name + ", AS-IP: " + ip + " AS link List: ");
		for (String string : links.keySet()) {
			tmp.append(links.get(string).getIp() + "; ");
		}
		return tmp.toString();
	}
	
}
