

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class EventsGenerator implements Runnable{
	
	EventsQueue events;
	String filename;
	int no_Events;
	int id;
	
	public EventsGenerator(){
		this.events = null;
		this.filename = " ";
		this.no_Events = 0;
		this.id = 0;
	}
	
	public EventsGenerator(EventsQueue events,String filename,int no_Events,int id){
		this.events = events;
		this.filename = filename;
		this.no_Events = no_Events;
		this.id = id;
	}

	@Override
	public void run() {
		
		int time = 0;
		String name ;
		int N = 0;
		
		try {
			Scanner sc = new Scanner(new File(filename));
			
			for (int i = 0 ; i < no_Events ; i++){
				sc.useDelimiter("\\s*[,\n]\\s*");
				time = sc.nextInt();
				name = sc.next();		
				N = sc.nextInt();
				Thread.sleep(time);
			//	System.out.println(time + " " + name + " " + N);
				sc.nextLine();
				events.put(new Event(name,N));
			}
			
			
		
			sc.close();	
	
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		
		
		
	}

}
