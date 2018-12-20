#include<conf.h>
#include<kernel.h>
#include<proc.h>
#include<lock.h>

int releaseall(int num,...){
	STATWORD ps;
	disable(ps);

	unsigned long *ptr;
	ptr = (unsigned long *)(&num);
	*ptr++;	
	int i;
	int errorSomewhere = 0;
	for( i = 0 ; i < num ; i++){
		int lockIdx = *ptr++;
		if(release(lockIdx , currpid) == SYSERR){
			errorSomewhere = 1;
		}
	}
	restore(ps);
	if(errorSomewhere == 0)
		return OK;
	else
		return SYSERR;
}


int activeReadersPresent = 0;
int waitingProcsPresent = 0;

void checkWaitingProcs(int lockIdx){
	int i;
	activeReadersPresent = 0;
	waitingProcsPresent = 0;
        for(i = 0; i < NPROC ; i++){
        	if(Locks[lockIdx].waitingProcs[i].lockType == READ && Locks[lockIdx].waitingProcs[i].status == USING)
                	activeReadersPresent = 1;
	
                if(Locks[lockIdx].waitingProcs[i].status == WAITING)
                	waitingProcsPresent = 1;

        }
}

int release(int lockIdx , int pid){
	//If process is neither using it or not in waiting list
	if(Locks[lockIdx].waitingProcs[pid].status == -1){
		return SYSERR;
	}else if(Locks[lockIdx].waitingProcs[pid].status == WAITING){
		//Process was in waiting list
		resetLockProc(lockIdx , pid);
	}else if(Locks[lockIdx].waitingProcs[pid].status == USING){
		if(Locks[lockIdx].lockType == READ){
			//If proc is using lock ar read lock
			resetLockProc(lockIdx , pid);
		 	checkWaitingProcs(lockIdx);	
			if(activeReadersPresent == 0){
				if(waitingProcsPresent == 1){
					reAssignLock(lockIdx);
				}else{
					// No process waiting so just reset the whole lock.				
					resetWholeLock(lockIdx);
					return OK;
				}
			}
		}else if(Locks[lockIdx].lockType == WRITE){
			resetLockProc(lockIdx , pid);
			checkWaitingProcs(lockIdx);
			if(waitingProcsPresent == 1)
				reAssignLock(lockIdx);
			else
				resetWholeLock(lockIdx);
		}
	}
	reAssignPriorities();
	return OK;
}

