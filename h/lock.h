#define NOOFLOCKS 50

typedef struct waitingProc{
	int prio;
	int waitTime;
	int lockType;
	int status;
}wProc;

struct lock{
	int deleted;
	int status;
	int lockType;
	wProc waitingProcs[100];
};

extern struct lock Locks[NOOFLOCKS];

#define OPEN 101
#define LOCKED 102
#define LDELETED 103

#define READ 104
#define WRITE 105

#define WAITING 106
#define USING 107
