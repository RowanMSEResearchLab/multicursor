#include <iostream>  
#include <boost/thread.hpp>         

using namespace std;

class Runnable {
public:
    Runnable ( int s, int e) : start(s), end(e) { }
    
    int collatz ( int value ) {
    	// Collatz conjecture: 
    	// See https://en.wikipedia.org/wiki/Collatz_conjecture
    	
    	int i, count = 0;
    	for (i = value; i > 1; count++) {
    	    if (i % 2 == 0) 
    	    	i = i /2;
    	    
    	    else 
    	    	i = 3 * i + 1;
    	    
    	  
    	}
    	
    	return count;
    }
    void operator() ( ) // The run function
    {  

    	int max = 0, temp;
    	for (int i = start; i <= end; i++) {
    	    temp = collatz ( i );
    	    if (temp > max) {
    	    	max = temp;
    	    	cout << boost::this_thread::get_id()
    	    	<< " : " << i << " : " << max << endl;
    	    }
    	    
    	}
    	
    }    
    int start, end;
};



int main(int argc, char* argv[])  
{  

    boost::thread_group group;
    
    group.create_thread ( Runnable (60000000, 70000000));
    group.create_thread ( Runnable (20000000, 30000000));

    
    cout << "main: waiting for threads to complete" << endl;  
    group.join_all();
    
    
    cout << "main: done" << endl;          
    return 0;  
}
