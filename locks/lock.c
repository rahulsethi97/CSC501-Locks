#include<lock.h>
#include<kernel.h>
#include<proc.h>

unsigned long ctr1000;

int lock(int lockIdx , int type , int prio){
	//kprintf("%d\n" , currpid);
	STATWORD ps;
	disable(ps);
	
	if(proctab[currpid].locksDeleted[lockIdx] == 1){
		//kprintf("Here4\n");
		restore(ps);
		return SYSERR;
	}else if(type == READ){
		if(Locks[lockIdx].status == OPEN){
			//kprintf("Called %d %d\n" ,currpid ,  lockIdx);
			Locks[lockIdx].status = LOCKED;
			Locks[lockIdx].lockType = type;
			//Locks[lockIdx].waitingProcs[currpid].prio = prio;
			Locks[lockIdx].waitingProcs[currpid].lockType = type;
			Locks[lockIdx].waitingProcs[currpid].status = USING;
			proctab[currpid].locksUsing[lockIdx] = 1;
		}else{
			
			if(Locks[lockIdx].lockType == WRITE){
				addToWaitList(currpid , prio , lockIdx , type);
                                //kprintf("Here\n");
				reAssignPriorities();
                                freeze(currpid);
                                restore(ps);
                                return proctab[currpid].pwaitret;	
			}	
			int max = -1;
			int i;
			for(i = 0 ; i < NPROC ; i++){
				//kprintf("Lock: %d , PID: %d , PRIO: %d\n" ,lockIdx , i , Locks[lockIdx].waitingProcs[i].prio);
				int temp;
				if(Locks[lockIdx].waitingProcs[i].prio == 0)
					temp = proctab[i].origPrio;
				else
					temp = Locks[lockIdx].waitingProcs[i].prio;

				if(temp > max && Locks[lockIdx].waitingProcs[i].lockType == WRITE)
					max = temp;
			}
			//kprintf("Prio: %d , Max: %d\n" ,prio, max);
			if(prio >= max){
				Locks[lockIdx].waitingProcs[currpid].status = USING;
				//Locks[lockIdx].waitingProcs[currpid].prio = prio;
				Locks[lockIdx].waitingProcs[currpid].lockType = type;
				proctab[currpid].locksUsing[lockIdx] = 1;
			}else{
				addToWaitList(currpid , prio , lockIdx , type);			     
				reAssignPriorities();
				freeze(currpid);
				restore(ps);
				return proctab[currpid].pwaitret;
			}
		}
	}else if(type == WRITE){
		//kprintf("Here2\n");
		if(Locks[lockIdx].status == OPEN){
			//kprintf("Here\n");
                        Locks[lockIdx].status = LOCKED;
                        Locks[lockIdx].lockType = type;
                        //Locks[lockIdx].waitingProcs[currpid].prio = prio;
                        Locks[lockIdx].waitingProcs[currpid].lockType = type;
                        Locks[lockIdx].waitingProcs[currpid].status = USING;
			proctab[currpid].locksUsing[lockIdx] = 1;
                }else{
			//kprintf("%d %d %d\n" , currpid , prio , lockIdx);
			addToWaitList(currpid , prio , lockIdx , type);
			reAssignPriorities();
			freeze(currpid);
			restore(ps);
			return proctab[currpid].pwaitret;
		}
	}
	reAssignPriorities();
	restore(ps);
	return OK;
}

void freeze(int pid){
	proctab[pid].pstate = PRWAIT;
        proctab[pid].pwaitret = OK;
        resched();
}

void addToWaitList(int pid , int prio , int lockIdx , int type){
	//kprintf("LOCK: %d , PID: %d , PRIO: %d , TYPE: %d\n" ,lockIdx , pid , prio , type);
	Locks[lockIdx].waitingProcs[pid].prio = prio;
	Locks[lockIdx].waitingProcs[pid].lockType = type;
	Locks[lockIdx].waitingProcs[pid].status = WAITING;
	Locks[lockIdx].waitingProcs[currpid].waitTime = ctr1000;
	proctab[pid].locksWaiting[lockIdx] = 1;
	//kprintf("%d\n" ,  Locks[lockIdx].waitingProcs[pid].prio);
}

