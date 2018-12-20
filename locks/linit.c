#include<lock.h>
#include<kernel.h>
#include<proc.h>	

struct lock Locks[NOOFLOCKS];

void linit(){
	int idx = 0;
	int j;
	//kprintf("\nLinit Called\n");
	for(idx = 0 ; idx < NOOFLOCKS ; idx++){
		Locks[idx].status = OPEN;
		Locks[idx].lockType = -1;
		for(j = 0 ; j < NPROC ; j++){
			Locks[idx].waitingProcs[j].prio = 0;
			Locks[idx].waitingProcs[j].waitTime = -1;
			Locks[idx].waitingProcs[j].lockType = -1;
			Locks[idx].waitingProcs[j].status = -1;
		}
	}

	for(j = 0 ; j < NPROC ; j++){
		for(idx = 0 ; idx < NOOFLOCKS ; idx++){
			proctab[j].locksUsing[idx] = 0;
			proctab[j].locksWaiting[idx] = 0;
			proctab[j].inhprio = 0;
			proctab[j].locksDeleted[idx] = 0;
		}
	}
}

void display(){
	int idx = 0;
        int j;
        //kprintf("\nLinit Called\n");
        //for(idx = 0 ; idx < NOOFLOCKS ; idx++){
                for(j = 0 ; j < NPROC ; j++){
			kprintf("%d\n%d " , j, Locks[idx].status);
			kprintf("%d " , Locks[idx].lockType);
                        kprintf("%d " , Locks[idx].waitingProcs[j].prio);
                        kprintf("%d " , Locks[idx].waitingProcs[j].waitTime);
                        kprintf("%d " , Locks[idx].waitingProcs[j].lockType);
                        kprintf("%d\n" , Locks[idx].waitingProcs[j].status);
                }
        //}

        for(j = 0 ; j < NPROC ; j++){
               // for(idx = 0 ; idx < NOOFLOCKS ; idx++){
                        kprintf("%d\n%d " , j , proctab[j].locksUsing[0]);
                        kprintf("%d " , proctab[j].locksWaiting[0]);
                        kprintf("%d\n" , proctab[j].inhprio);
                //}
        }
}

int waitTest(){
	STATWORD ps;
	disable(ps);
	proctab[currpid].pstate = PRWAIT;
	//proctab[currpid].pwaitret = OK;
	resched();
	restore(ps);
	return proctab[currpid].pwaitret;
}
