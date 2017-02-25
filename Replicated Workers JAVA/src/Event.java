

import java.util.ArrayList;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.ExecutorService;

public class Event {

	String name;
	int N ;
	ExecutorService tpe;
	
	public Event(){
		name = "";
		N = 0;
	}
	
	public Event(String name , int N){
		this.name = name;
		this.N = N;
	}

	
	
	
}
