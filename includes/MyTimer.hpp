/* 
  +-------------------------------+
  |								  |
  |   BatuHan TOPALOĞLU 11/2019   |
  |								  |				
  +-------------------------------+
								   */  
#ifndef MYTIMER_H
#define MYTIMER_H

#include "ITimer.hpp"
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>              
#include <condition_variable> 

struct entry
	{
		entry(int _type,Timepoint _tp,TTimerCallback _cb):
			type(_type),tp(_tp),cb(_cb){}
		
		entry(int _type,Millisecs _period,TTimerCallback _cb):
			type(_type),period(_period),cb(_cb){tp = CLOCK::now();}
		
		entry(int _type,Timepoint _toTp,Millisecs _period,TTimerCallback _cb):
			type(_type),toTp(_toTp),period(_period),cb(_cb){tp = CLOCK::now();}
		
		entry(int _type,TPredicate _pred,Millisecs _period,TTimerCallback _cb):
			type(_type),pred(_pred),period(_period),cb(_cb){tp = CLOCK::now();}
		
		void registerTimer(const TPredicate &pred, const Millisecs & period, const TTimerCallback &cb);

		int type; 
		Timepoint tp;
		Millisecs period;
		TTimerCallback cb;
		Timepoint toTp;
		TPredicate pred;
	};

class MyTimer : public ITimer
{
public:
	MyTimer();
	~MyTimer();
	
	// run the callback once at time point tp.
	void registerTimer(const Timepoint &tp, const TTimerCallback &cb);
	// run the callback periodically forever. The first call will be executed as soon as this callback is registered.
	void registerTimer(const Millisecs & period, const TTimerCallback &cb);
	// Run the callback periodically until time point tp. The first call will be executed as soon as this callback is registered.
	void registerTimer(const Timepoint &tp, const Millisecs & period, const TTimerCallback &cb);
	// Run the callback periodically. Before calling the callback every time, call the predicate first to check if the
	//termination criterion is satisfied. If the predicate returns false, stop calling the callback.
	void registerTimer(const TPredicate &pred, const Millisecs & period, const TTimerCallback &cb);

private:
	int i = 0;
    std::thread myThread;	
    std::mutex mtx;
	std::condition_variable cv;
	bool ready = false;
	std::queue<entry> sq;

	void loop();

	inline bool isDataLoaded(){
    	return ready;
  	}

};

#endif