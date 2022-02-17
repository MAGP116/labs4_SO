#include <scheduler.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

#define MAXDEEP 20

#define min(a,b) (a < b)? a: b
void updateDeep(int callingthread);
int dequeue();

int threadsdeep[MAXTHREAD];

QUEUE ready;
QUEUE readys[MAXDEEP];
QUEUE waitinginevent[MAXTHREAD];

int nThreads = 0;

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;
	
	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		nThreads++;
		threads[callingthread].status=READY;
		threadsdeep[callingthread] = 0;
		_enqueue(&readys[0],callingthread);
	}
	
	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;
		nThreads--;
	}
	
	if(event==UNBLOCKTHREAD)
	{
		threads[callingthread].status=READY;
		updateDeep(callingthread);
		_enqueue(&readys[threadsdeep[callingthread]],callingthread);
	}
	
	if(event==TIMER)
	{
		threads[callingthread].status=READY;
		updateDeep(callingthread);
		_enqueue(&readys[threadsdeep[callingthread]],callingthread);
		changethread=1;
	}

	
	if(changethread)
	{
		old=currthread;
		next=dequeue();
		
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}

int dequeue(){
	for(int i = 0; i < MAXDEEP; i++){
		if(!_emptyq(&readys[i])){
			return _dequeue(&readys[i]);
		}
	}
	return -1;
}

void updateDeep(int callingthread){
	if(nThreads > 1)
		threadsdeep[callingthread] = min(threadsdeep[callingthread] + 1, MAXDEEP - 1);
	else
		threadsdeep[callingthread] = 0;
	
}





