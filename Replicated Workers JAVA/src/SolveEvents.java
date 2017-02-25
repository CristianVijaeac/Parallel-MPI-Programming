import java.util.ArrayList;
import java.util.concurrent.ExecutorService;

public class SolveEvents implements Runnable{

	EventsQueue q;
	int id;
	int total_Events;
	static int i = 0 ;
	
	public SolveEvents(){
		q = null;
		total_Events= 0;
		id = 0;
	}
	
	public SolveEvents(EventsQueue q,int total_Events,int id){
		
		this.q = q;
		this.total_Events = total_Events;
		this.id = id;
	
	}
	
	public void PRIME(int N){

		int x ;
		
		for (int i = N ; i >=2 ; i--){
			for (x = 2 ; x<i ; x++){
				if (i % x == 0){
					break;
				}
			}
			if (x == i){
				Main.prime.add(i);
				break;
			}
		}
	}
	
	public void FACT(int N){
		
		int init = 1;
		int res = 1;
		int count = 1;
		
		while((init+1) * res <= N){
			init +=1;
			res =init * res;
			count ++;
		}
		
		Main.fact.add(count);
	}

	public void FIB(int N){
		
		int res1 = 0;
		int res2 = 1;
		int poz = 1;
		
		while (res1 + res2 <= N){
			int aux = res2;
			res2 = res1+res2;
			res1 = aux;
			poz++;
		}
		
		Main.fibb.add(poz);
	}
	
	public void SQUARE(int N){

		int init = 0;
		
		
		while (init * init <= N){
		
		init+=1;
	
		}
	//	System.out.println(init-1);
		Main.square.add(init-1);
	}
	
	int getNextI() {
		int value;
		synchronized (SolveEvents.class) {
			value = i;
			i++;
		}
		return value;
	}
	
	@Override
	public void run() {
		// TODO Auto-generated method stub
		while (true) {
			
			int i = getNextI();
			if (i >= this.total_Events)
				break;
			Event e = q.get();
		//	System.out.println( e.name + "Rulat de "+this.id);
			
			if (e.name.equals("PRIME")){
				PRIME(e.N);
			}
			if (e.name.equals("FACT")){
				FACT(e.N);
			}
			if (e.name.equals("FIB")){
				FIB(e.N);
			}
			if (e.name.equals("SQUARE")){
				SQUARE(e.N);
			}
			
		}
	}

}
