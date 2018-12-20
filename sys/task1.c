#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <sem.h>
#include <stdio.h>

void invPrioFunct1(int lockIdx){
        kprintf("A\n");
        int x = lock(lockIdx,WRITE,20);
        kprintf("A: Lock acquired.\n");
        sleep(1);
        kprintf("A: Releasing lock.\n");
        releaseall(1,lockIdx);
}

void invPrioFunct2(int lockIdx){
        kprintf("B\n");
        int x = lock(lockIdx,WRITE,20);
        kprintf("B: Lock acquired\n");
        sleep(1);
        kprintf("B: Releasing lock\n");
        releaseall(1,lockIdx);
}


void semFunc1(int lockIdx){
        kprintf("A\n");
        int x = wait(lockIdx);
        kprintf("A in critical section\n");
        sleep(1);
        kprintf("A out of critical section.\n");
        signal(lockIdx);
}

void semFunc2(int lockIdx){
        kprintf("B\n");
        int x = wait(lockIdx);
        kprintf("B in critical section\n");
        sleep(1);
        kprintf("B out of critical section\n");
        signal(lockIdx);
}


int main(){

        kprintf("Semaphore Test.\n");
        int lock_sem = screate(1);
        int procSem1 = create(semFunc1,2000,10,"A",1,lock_sem);
        int procSem2 = create(semFunc2,2000,15,"B",1,lock_sem);

        kprintf("Starting A.\n");
        resume(procSem1);
        kprintf("Starting B.\n");
        resume(procSem2);
        sleep(5);

        kprintf("\nPriority Inversion Test.\n");

        int lockIdx = lcreate();
        int proc1 = create(invPrioFunct1,2000,10,"A",1,lockIdx);
        int proc2 = create(invPrioFunct2,2000,15,"B",1,lockIdx);
        kprintf("Starting A.\n");
        resume(proc1);
	sleep(2);
        kprintf("Starting B.\n");
        resume(proc2);

        sleep(5);
        shutdown();
        return 0;
}
