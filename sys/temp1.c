#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <sem.h>
#include <stdio.h>


void semaphore1(int s1){
        kprintf("A\n");
        int x = wait(s1);
        kprintf("A: in CS\n");
        sleep(1);
        kprintf("A: moving outside CS.\n");
        signal(s1);
}

void semaphore2(int s2){
        int i=0;
        while(i++ < 50)
                kprintf("B");
        kprintf("B\n");
        //int x = wait(s2);
        //kprintf("B: in CS\n");
        sleep(1);
        i=0;
        while(i++<50)
                kprintf("B");        
        kprintf("B\n");
        //signal(s2);
}

void semaphore3(int s3){
        kprintf("C\n");
        int x = wait(s3);
        kprintf("C: in CS\n");
        sleep(1);
        kprintf("C: moving outside CS.\n");
        signal(s3);
}

void lock1(int l1){
        kprintf("A\n");
        int x = lock(l1,WRITE,20);
        kprintf("A: Lock acquired.\n");
        sleep(1);
        kprintf("A: Releasing lock.\n");
        releaseall(1,l1);
}

void lock2(int l2){
        int i=0;
        while(i++<50)
                kprintf("B");
        kprintf("B\n");
        sleep(3);
        i=0;
        while(i++<50)
                kprintf("B");
        kprintf("B\n");
}

void lock3(int l3){
        kprintf("C\n");
        int x = lock(l3,WRITE,20);
        kprintf("C: Lock acquired\n");
        sleep(1);
        kprintf("C: Releasing lock\n");
        releaseall(1,l3);
}

int main(){

        kprintf("Testing using semaphore.\n");
        int lock_sem = screate(1);
        int procSem1 = create(semaphore1,2000,25,"A",1,lock_sem);
        int procSem2 = create(semaphore2,2000,30,"B",1,lock_sem);
        int procSem3 = create(semaphore3,2000,35,"C",1,lock_sem);

        kprintf("Starting A.\n");
        resume(procSem1);
        kprintf("Starting B.\n");
        resume(procSem2);
        sleep(1);
        kprintf("Starting C.\n");
        resume(procSem3);
        sleep(10);

        kprintf("\nTesting using reader/writer locks with priority inversion.\n");

        int lockIdx = lcreate();
        int proc1 = create(lock1,2000,25,"A",1,lockIdx);
        int proc2 = create(lock2,2000,30,"B",1,lockIdx);
        int proc3 = create(lock3,2000,35,"C",1,lockIdx);
        kprintf("Starting A.\n");
        resume(proc1);
        kprintf("Starting B.\n");
        resume(proc2);
        sleep(1);
        kprintf("Starting C.\n");
        resume(proc3);

        sleep(10);
        shutdown();
        return 0;
}
