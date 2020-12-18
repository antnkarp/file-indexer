/*
------------------------------------------------------------------------
  I declare that this piece of work which is the basis for recognition of
  achieving learning outcomes in the OPS2 course was completed on my own.
  Antoni Karpinski 249372
------------------------------------------------------------------------
*/

#include "mole.h"

extern volatile sig_atomic_t last_signal;
		 
void sethandler( void (*f)(int), int sigNo) {
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = f;
	if (-1==sigaction(sigNo, &act, NULL)) {
		ERR("sigaction");
	}
}

void sig_handler(int sig) {
	last_signal = sig;
}

sigset_t getAlarmMask() {
	sigset_t alarmMask;
	if (sigemptyset(&alarmMask) == -1) {
		ERR("sigemptyset");
	}
	if (sigaddset(&alarmMask, SIGALRM) == -1) {
		ERR("sigaddset");
	}
	return alarmMask;
}

void handleSigalrm(threadData *thread_data) {
	enum threadStatus status;
	if (pthread_mutex_lock(thread_data->mxStatus)) {
		ERR("pthread_mutex_lock");
	}
	status = *(thread_data->status);
	if (pthread_mutex_unlock(thread_data->mxStatus)) {
		ERR("pthread_mutex_unlock");
	}
	/* If indexing is in progress, then do nothing. */
	if (status==THREAD_IN_PROGRESS) {
		return;
	}
	if (status==THREAD_PENDING_JOIN) {
		if(pthread_join(thread_data->tid, NULL)) {
			ERR("pthread_join");
		}
	}
	/* Don't set status to THREAD_NOT_EXISTS, because we run the thread
	 * immiediately. */
	runThread(thread_data);
	last_signal = 0;
}
