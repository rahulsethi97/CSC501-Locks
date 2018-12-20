#include<lock.h>
#include<kernel.h>
#include<proc.h>

int ldelete(int idx){
	Locks[idx].deleted = 1;
        Locks[idx].status = OPEN;
        Locks[idx].lockType = -1;
        int j;
        for(j = 0 ; j < NPROC ; j++){
                Locks[idx].waitingProcs[j].prio = 0;
                Locks[idx].waitingProcs[j].waitTime = -1;
                Locks[idx].waitingProcs[j].lockType = -1;
                Locks[idx].waitingProcs[j].status = -1;
		if(proctab[j].locksUsing[idx] == 1 || proctab[j].locksWaiting[idx] == 1)
			proctab[j].locksDeleted[idx] = 1;
                proctab[j].locksUsing[idx] = 0;
                proctab[j].locksWaiting[idx] = 0;
                proctab[j].inhprio = 0;
                proctab[j].pprio = proctab[j].origPrio;
        }
	return OK;
}


