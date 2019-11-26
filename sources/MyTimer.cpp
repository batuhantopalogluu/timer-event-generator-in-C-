/* 
  +-------------------------------+
  |								  |
  |   BatuHan TOPALOĞLU 11/2019   |
  |								  |				
  +-------------------------------+
								   */  
#include "MyTimer.hpp"

void MyTimer::registerTimer(const Timepoint &_tp, const TTimerCallback &_cb){

	entry e(0,_tp,_cb);							/* gelen parametreler 'entry' tipinde bir obje oluşturulur.
												   Burada 0 hangi tipte bir registerTimer fonksiyonu için 
												   entry oluşturulduğunu anlamak için kullanılır.*/

	std::unique_lock<std::mutex> lck(mtx);      /* mutex kilitlenir, queue'ya yeni eleman eklenir, condition variable 
												nofity edilir ve spurious wakeup kontrolü için bir değişken setlenir.*/
	sq.push(e);
	
	ready = true;	
	cv.notify_one();

}

// run the callback periodically forever. The first call will be executed as soon as this callback is registered.
void MyTimer::registerTimer(const Millisecs & _period, const TTimerCallback &_cb){

	entry e(1,_period,_cb);

	std::unique_lock<std::mutex> lck(mtx);
	
	sq.push(e);
	
	ready = true;	
	cv.notify_one();
}

// Run the callback periodically until time point tp. The first call will be executed as soon as this callback is
//registered.
void MyTimer::registerTimer(const Timepoint &_toTp, const Millisecs & _period, const TTimerCallback &_cb){

	entry e(2,_toTp,_period,_cb);

	std::unique_lock<std::mutex> lck(mtx);
	
	sq.push(e);
	
	ready = true;	
	cv.notify_one();
}

// Run the callback periodically. Before calling the callback every time, call the predicate first to check if the
//termination criterion is satisfied. If the predicate returns false, stop calling the callback.
void MyTimer::registerTimer(const TPredicate &_pred, const Millisecs &_period, const TTimerCallback &_cb){

	entry e(3,_pred,_period,_cb);

	std::unique_lock<std::mutex> lck(mtx);
	
	sq.push(e);
	
	ready = true;	
	cv.notify_one();
}

MyTimer::MyTimer(){

	myThread = std::thread(&MyTimer::loop,this);      /* Queue da gezerek gerekli kontrolleri ve callback fonksiyonlarının
														çalıştırılması işlemini yapan thread oluşturulur.*/	
}

MyTimer::~MyTimer(){

	myThread.join();

}	
	
void MyTimer::loop(){

	while(true){ 									  /* program sonsuza kadar çalışabileceği için ana döngünün bir sonlanma koşulu yok.*/
    	
    	std::unique_lock<std::mutex> lck(mtx);
    	cv.wait(lck, std::bind(&MyTimer::isDataLoaded, this));
    	ready = false;
    	
    	for(;;){

    		int eType = sq.front().type;

    		if(eType == 0){							  /* MyTimer::registerTimer(const Timepoint &_tp, const TTimerCallback &_cb)*/
    			
    			std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(CLOCK::now() - sq.front().tp);

	    		if(time_span.count() >= 0 ){ 		  /*istenile timepoint'e gelinin gelmediği kontrol edilir.*/
	    			
	    			sq.front().cb();                  /*callback fonksiyonu çalıştırılır.*/
 			    		
	    			if(time_span.count()*1000 > 5.0){ /*deadline'nın kaçması durumu kontrol edilir.*/

	    				std::cout << "! - Deadline ERROR : " <<  time_span.count()*1000 << "milliseconds late" << std::endl;
	    			}
	    			sq.pop(); 						  /*işlemi gerçekleştirilen entry queue'dan çıkarılır.*/
	    		}
	    		else{								  /* kontrol edilen entry'nin zamanı gelmediyse queue'nun başından
    				    							  alınıp sonra tekrar kontrol edilmek üzere en sonuna konulur */ 
	    			entry temp = sq.front();
	    			sq.pop();
	    			sq.push(temp);
	    		}
	    	}
	    	else if(eType == 1){ 					  /* MyTimer::registerTimer(const Millisecs & _period, const TTimerCallback &_cb)*/
	    		
	    		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(CLOCK::now() - sq.front().tp);

	    		if(time_span.count() >= 0 ){
	    			
	    			Timepoint tp1 = CLOCK::now();
	    	
	    			sq.front().cb();
	    
	  				Timepoint tp2 = CLOCK::now();
	  
	    			if(time_span.count()*1000 > 5.0){

	    				std::cout << "! - Deadline ERROR : " <<  time_span.count()*1000 << "milliseconds late" << std::endl;
	    			}
	    			
	    			auto a = std::chrono::duration_cast<std::chrono::milliseconds>(tp2- tp1 );

					sq.front().tp = CLOCK::now()+sq.front().period - a; 			
	    		}

    			entry temp = sq.front(); 
    			sq.pop();	
    			sq.push(temp);
	    		
	    	}
	    	else if(eType == 2){ /* MyTimer::registerTimer(const Timepoint &_toTp, const Millisecs & _period, const TTimerCallback &_cb)*/
				
				std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(CLOCK::now() - sq.front().tp);
				std::chrono::duration<double> time_span2 = std::chrono::duration_cast<std::chrono::duration<double>>(CLOCK::now() - sq.front().toTp);

	    		if(time_span.count() >= 0 && time_span2.count() <= 0){
	    			
	    			Timepoint tp1 = CLOCK::now();
	    	
	    			sq.front().cb();
	    
	  				Timepoint tp2 = CLOCK::now();
	  
	    			if(time_span.count()*1000 > 5.0){

	    				std::cout << "! - Deadline ERROR : " <<  time_span.count()*1000 << "milliseconds late" << std::endl;
	    			}
	    			
	    			auto a = std::chrono::duration_cast<std::chrono::milliseconds>(tp2- tp1 );
	    		
					sq.front().tp = CLOCK::now()+sq.front().period - a; 			
	    		}
	    		if(time_span2.count() > 0){
	    			sq.pop();
	    		}
	    		else{
	    			entry temp = sq.front();
	    			sq.pop();
	    			sq.push(temp);

	    		}    										   			
	    	}
	    	else if(eType == 3 ){ /* MyTimer::registerTimer(const TPredicate &_pred, const Millisecs &_period, const TTimerCallback &_cb)*/

	    		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(CLOCK::now() - sq.front().tp);
				
	    	 	if(time_span.count() >= 0){             /* periyotu tutturmak için istenilen zamanın geçip geçmediği kontrol edilir.*/ 
	    											    /* predicate fonksiyonu 'true' dönerse  callback fonsksiyonu çalıştırılır,  */
	    			if(bool predCon = sq.front().pred()){    /* aksi durumda queue'dan sırılır*/	    			       

	    				Timepoint tp1 = CLOCK::now();   /* callback fonksiyonun çalışma zamanı periyodtan çıkarılarak daha doğru aralıklarda */ 
	    												/*   çalışması sağlanmaya çalışılır.*/	    	
		    			sq.front().cb();				
		    
		  				Timepoint tp2 = CLOCK::now();
		  
		    			if(time_span.count()*1000 > 5.0){

		    				std::cout << "! - Deadline ERROR : " <<  time_span.count()*1000 << "milliseconds late" << std::endl;
		    			}
		    			
		    			auto a = std::chrono::duration_cast<std::chrono::milliseconds>(tp2- tp1 );
	    		
						sq.front().tp = CLOCK::now()+sq.front().period - a; 			
	    			}
	    			else{
	    				sq.pop();
	    			}
	    		}
	    	
	    		entry temp = sq.front();
	    		sq.pop();
	    		sq.push(temp);
	    		   					
	    	}

    		if(sq.empty()){						/*eğer queue da bekleyen bir iş yoksa thread yeni bir entry gelene kadar beklemeye geçer.*/
    	   		
	    		break;
	    	}

    	}
	}
}