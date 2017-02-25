

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
public class EventsQueue {

	ArrayBlockingQueue<Event> gen;
	
	public EventsQueue(){
		gen = new ArrayBlockingQueue<Event>(10);
	}
	
	public EventsQueue(int dim){
		this.gen = new ArrayBlockingQueue<Event>(dim);
	}
	
	public void put(Event e) {
		
		//System.out.println("BAGAM "+ e.name+"\n");
	
			try {
				gen.put(e);
			} catch (InterruptedException e1) {
			
				e1.printStackTrace();
			}
		
		
	}
	
	public Event get (){
		Event x = null;
		
		try {
			x = gen.take();
		//	System.out.println("SCOATEM "+x.name+"\n");
		} catch (InterruptedException e) {
			
			e.printStackTrace();
		}
		
		
		return x;
	}
	
	
}
