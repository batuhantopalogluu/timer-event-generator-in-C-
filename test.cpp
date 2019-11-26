/* 
  +-------------------------------+
  |								  |
  |   BatuHan TOPALOĞLU 11/2019   |
  |								  |				
  +-------------------------------+
								   */ 
#include "MyTimer.hpp"

int main(int argc, char const *argv[]){

	auto period = Millisecs(10);
	
	std::cout << "---- TEST FILE FOR MyTimer -----" << std::endl;

	MyTimer myTimer;
	
	/*timepoint test */
	myTimer.registerTimer(CLOCK::now()+Millisecs(5000),[](){std::cout<< "timepoint timer seconds :- 5 " << std::endl;});
	myTimer.registerTimer(CLOCK::now()+Millisecs(4000),[](){std::cout<< "timepoint timer seconds :- 4 " << std::endl;});
	myTimer.registerTimer(CLOCK::now()+Millisecs(3000),[](){std::cout<< "timepoint timer seconds :- 3 " << std::endl;});
	myTimer.registerTimer(CLOCK::now()+Millisecs(2000),[](){std::cout<< "timepoint timer seconds :- 2 " << std::endl;});
	myTimer.registerTimer(CLOCK::now()+Millisecs(1000),[](){std::cout<< "timepoint timer seconds :- 1 " << std::endl;});
	

	/*periodics forevertimer test*/
	myTimer.registerTimer(Millisecs(8000),[](){std::cout<< "--> FOREVER <--         6 seconds period" << std::endl;});
	
	/*periodics until timepoint */	
	myTimer.registerTimer(CLOCK::now()+Millisecs(15000),Millisecs(3000),[](){
		std::cout<< "++> until TIMEPOINT <++ 3 seconds period "<< std::endl;
	});              
	
	/*periodics with predicate*/
	myTimer.registerTimer([](){return true;},Millisecs(4000),[](){
		std::cout<< "//> PREDICATE TRUE <//  4 seconds period" << std::endl;
	});

	// bu callback hiç çalışmayacak.
	myTimer.registerTimer([](){return false;},Millisecs(10000),[](){
		std::cout<< "//> PERIODICS with PREDICATE FALSE <//  10 seconds period" << std::endl;
	});

	return 0;
}