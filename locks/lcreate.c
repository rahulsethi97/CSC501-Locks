#include<lock.h>
#include<kernel.h>
#include<proc.h>

int lcreate(){
	STATWORD ps;
	disable(ps);
	int idx = 0;
	for(idx = 0 ; idx < NOOFLOCKS ; idx++){
		if(Locks[idx].status == OPEN){
			restore(ps);
			int i;
			for(i = 0 ; i < NPROC ; i++){
				proctab[i].locksDeleted[idx] = 0;
			}
			return idx;
		}
	}
	restore(ps);
	return SYSERR;
}
