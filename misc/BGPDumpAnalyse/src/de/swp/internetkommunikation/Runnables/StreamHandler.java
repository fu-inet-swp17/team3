package de.swp.internetkommunikation.Runnables;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.BlockingQueue;

public class StreamHandler implements Runnable {

	private Process p;
	private BlockingQueue<String> queue;
	
	public StreamHandler(Process p, BlockingQueue<String> queue) {
		this.p = p;
		this.queue = queue;
	}
	
	@Override
	public void run() {
		BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
		while(!Thread.interrupted()){
			try {
				String tmp = reader.readLine();
				queue.add(tmp);
			} catch (IOException e) {
				e.printStackTrace();
				p.destroy();
				return;
			}
		}
		p.destroy();
	}

}
