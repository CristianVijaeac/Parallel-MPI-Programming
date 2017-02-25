

import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {

	static ArrayList<Integer> prime = new ArrayList<Integer>();
	static ArrayList<Integer> fact = new ArrayList<Integer>();
	static ArrayList<Integer> square= new ArrayList<Integer>();
	static ArrayList<Integer> fibb = new ArrayList<Integer>();
	
	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		
		
		EventsQueue q = new EventsQueue(Integer.parseInt(args[0]));
		
		PrintWriter out1 = new PrintWriter("PRIME.out");
		PrintWriter out2 = new PrintWriter("FACT.out");
		PrintWriter out3 = new PrintWriter("SQUARE.out");
		PrintWriter out4 = new PrintWriter("FIB.out");
	
		int no_Events = Integer.parseInt(args[1]);
		int no_Files = args.length - 2;
		int total_Events = no_Events * no_Files;
		
		
		Thread threads[] = new Thread[2 * no_Files];
		for (int i = 0; i < no_Files; i++)
			threads[i] = new Thread(new EventsGenerator(q,args[i+2],no_Events,i));
		for (int i = no_Files; i < 2*no_Files; i++)
			threads[i] = new Thread(new SolveEvents(q,total_Events,i-no_Files));
		for (int i = 0; i < 2 * no_Files; i++)
			threads[i].start();
		
		for (int i = 0; i < 2* no_Files; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		Collections.sort(prime);
		Collections.sort(fact);
		Collections.sort(square);
		Collections.sort(fibb);
		
		//System.out.println("PRIME");
		for (int i = 0 ;i < prime.size();i++){
			//System.out.println(prime.get(i));
			out1.println(prime.get(i));
		}
		
		//System.out.println("FACT");
		for (int i = 0 ;i < fact.size();i++){
			//System.out.println(fact.get(i));
			out2.println(fact.get(i));
		}
		
//		System.out.println("SQUARE");
		for (int i = 0 ;i < square.size();i++){
	//		System.out.println(square.get(i));
			out3.println(square.get(i));
		}
		
		//System.out.println("FIB");
		for (int i = 0 ;i < fibb.size();i++){
			//System.out.println(fibb.get(i));
			out4.println(fibb.get(i));
		}
		
		out1.close();
		out2.close();
		out3.close();
		out4.close();
		
		}
	


}
