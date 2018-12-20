#include<conf.h>
#include<kernel.h>
#include<proc.h>
#include<lock.h>

void resetWholeLock(int idx){
	//kprintf("Called\n");
        Locks[idx].status = OPEN;
        Locks[idx].lockType = -1;
        int j;
        for(j = 0 ; j < NPROC ; j++){
                Locks[idx].waitingProcs[j].prio = 0;
                Locks[idx].waitingProcs[j].waitTime = -1;
                Locks[idx].waitingProcs[j].lockType = -1;
                Locks[idx].waitingProcs[j].status = -1;
		proctab[j].locksUsing[idx] = 0;
		proctab[j].locksWaiting[idx] = 0;
		proctab[j].locksDeleted[idx] = 0;
		proctab[j].inhprio = 0;
		proctab[j].pprio = proctab[j].origPrio;
        }
	
}

void resetLockProc(int lockIdx , int pid){
        Locks[lockIdx].waitingProcs[pid].prio = 0;
        Locks[lockIdx].waitingProcs[pid].waitTime = -1;
        Locks[lockIdx].waitingProcs[pid].lockType = -1;
        Locks[lockIdx].waitingProcs[pid].status = -1;
	proctab[pid].locksUsing[lockIdx] = 0;
	proctab[pid].locksWaiting[lockIdx] = 0;
	proctab[pid].inhprio = 0;
	proctab[pid].pprio = proctab[pid].origPrio;
	proctab[pid].locksDeleted[lockIdx] = 0;
}

void reAssignLock(int lockIdx){
        //kprintf("Re Assigned Called\n");
        //kprintf("Status: %d " , Locks[lockIdx].status);
        //kprintf("lockType: %d \n" , Locks[lockIdx].lockType);
        /*int j;
        for(j = 0 ; j < NPROC ; j++){
                kprintf("\t%d\n" , j);
                kprintf("\tprio: %d\n" ,Locks[lockIdx].waitingProcs[j].prio);
                kprintf("\tlockType: %d\n" , Locks[lockIdx].waitingProcs[j].lockType);
                kprintf("\tStatus: %d\n" , Locks[lockIdx].waitingProcs[j].status);
        }*/


        int pid;
        int i;
        int maxPrio = -1;

        for(i = 0 ; i < NPROC ; i++){
                if(Locks[lockIdx].waitingProcs[i].prio > maxPrio){
                        maxPrio = Locks[lockIdx].waitingProcs[i].prio;
                        pid = i;
                }else if(Locks[lockIdx].waitingProcs[i].prio == maxPrio){
			int diff = Locks[lockIdx].waitingProcs[i].waitTime - Locks[lockIdx].waitingProcs[pid].waitTime;	
			if(diff < 0 )
				diff *= -1;

			if(diff <= 1){
				if(Locks[lockIdx].waitingProcs[i].lockType == WRITE || Locks[lockIdx].waitingProcs[pid].lockType == WRITE){
					// Choose one anyone writer or the only one.
					if(Locks[lockIdx].waitingProcs[pid].lockType != WRITE){
						maxPrio = Locks[lockIdx].waitingProcs[i].prio;
                        			pid = i;
					}
				}
			}else{
				if(Locks[lockIdx].waitingProcs[i].waitTime > Locks[lockIdx].waitingProcs[pid].waitTime){
					maxPrio = Locks[lockIdx].waitingProcs[i].prio;
                        		pid = i;
				}
			}
		}
        }
        if(Locks[lockIdx].waitingProcs[pid].lockType == WRITE){ 
        	Locks[lockIdx].status = LOCKED;
        	Locks[lockIdx].lockType = Locks[lockIdx].waitingProcs[pid].lockType;
       	 	Locks[lockIdx].waitingProcs[pid].waitTime = -1;
        	Locks[lockIdx].waitingProcs[pid].status = USING;
		proctab[pid].locksUsing[lockIdx] = 1;
		proctab[pid].locksWaiting[lockIdx] = 0;
        	proctab[pid].pstate = PRREADY;
        	ready(pid , RESCHNO);
	}else{
		int maxWritePrio = findMaxWritePrio(lockIdx);
		//kprintf("Simultaneous Release: \n");
		for(i = 0 ; i < NPROC ; i++){
			if(Locks[lockIdx].waitingProcs[i].status == WAITING && Locks[lockIdx].waitingProcs[i].lockType == READ && Locks[lockIdx].waitingProcs[i].prio >= maxWritePrio){
				Locks[lockIdx].status = LOCKED;
                		Locks[lockIdx].lockType = Locks[lockIdx].waitingProcs[i].lockType;
                		Locks[lockIdx].waitingProcs[i].waitTime = -1;
                		Locks[lockIdx].waitingProcs[i].status = USING;
                		proctab[i].locksUsing[lockIdx] = 1;
                		proctab[i].locksWaiting[lockIdx] = 0;
               			proctab[i].pstate = PRREADY;
				//kprintf("%d\n" , i);
                		ready(i , RESCHNO);
			}
		}
	}
}

int findMaxWritePrio(int lockIdx){
	int max = -1;
        int i;
        for(i = 0 ; i < NPROC ; i++){
                int temp;
                if(proctab[i].inhprio == 0){
                        proctab[i].inhprio = proctab[i].pprio;
                }
                temp = proctab[i].inhprio;

                if(Locks[lockIdx].waitingProcs[i].status == WAITING && temp > max && Locks[lockIdx].waitingProcs[i].lockType == WRITE)
                        max = temp;
        }
	return max;
}

int findMaxPrio(int lockIdx){
        int max = -1;
        int i;
        for(i = 0 ; i < NPROC ; i++){
		int temp;
		if(proctab[i].inhprio == 0){
			proctab[i].inhprio = proctab[i].pprio;
		}
		temp = proctab[i].inhprio;
		
                if(Locks[lockIdx].waitingProcs[i].status == WAITING && temp > max)
                        max = temp;

	 	/*if(i == 43){
                        kprintf("%d %d %d %d\n" ,  Locks[lockIdx].waitingProcs[i].prio ,  proctab[i].pprio , temp , max);
                }*/

        }
	return max;
}

void reAssignPriorities(){
        int i , j;
  	/*kprintf("Status: %d\n" , Locks[0].waitingProcs[46].status);
        kprintf("Prio: %d\n" , Locks[0].waitingProcs[46].prio);
	kprintf("Status: %d\n" , Locks[0].waitingProcs[48].status);
        kprintf("Prio: %d\n" , Locks[0].waitingProcs[48].prio);
	*/
        for(i = 0 ; i < NPROC ;i++){
		int max = -1;
		for(j = 0 ; j < NOOFLOCKS ; j++){
			if(proctab[i].locksUsing[j] == 1){
				//kprintf("Lock Found: %d for %d\n" , j , i);
				//Get Max of all the waitingList prios
				int temp = findMaxPrio(j);
				//if(i == 41)
					//kprintf("Here: %d\n" , temp); 
				if(temp > max)
					max = temp;	
			}
		}
		//kprintf("id: %d , max: %d\n" , i , max);
		if(max > 0){
			proctab[i].inhprio = max;
			proctab[i].pprio = max;
			for(j = 0 ; j < NOOFLOCKS ; j++){
				Locks[j].waitingProcs[i].prio = max;
			}
		}else if(max = -1){
			proctab[i].inhprio = proctab[i].origPrio;
                        proctab[i].pprio = proctab[i].origPrio;
		}
        }
	
	/*kprintf("Status: %d\n" , Locks[0].waitingProcs[46].status);
        kprintf("Prio: %d\n" , Locks[0].waitingProcs[46].prio);
        kprintf("Status: %d\n" , Locks[0].waitingProcs[48].status);
        kprintf("Prio: %d\n" , Locks[0].waitingProcs[48].prio);
	*/
}


void lockKillUtil(int pid){
	int i;
	for(i = 0 ; i < NOOFLOCKS ; i++){
		if(proctab[pid].locksUsing[i] == 1 || proctab[pid].locksWaiting[i] == 1){
			release(i,pid);
		}
	}
	
}
