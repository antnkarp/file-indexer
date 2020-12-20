
#include "file_indexer.h"

void menuCount(fileInfo_list *index) {
	int n_dir=0, n_jpeg=0, n_png=0, n_gzip=0, n_zip=0;
	fileInfo_node *tmp = index->head;
	while (tmp != NULL) {
		switch(tmp->fi.type) {
			case TYPE_DIR:
				n_dir++;
				break;
			case TYPE_JPEG:
				n_jpeg++;
				break;
			case TYPE_PNG:
				n_png++;
				break;
			case TYPE_GZIP:
				n_gzip++;
				break;
			case TYPE_ZIP:
				n_zip++;
				break;
			default:
				break;
		}
		tmp = tmp->next;
	}
	printf("Number of directories: %d\n", n_dir);
	printf("Number of JPEG files: %d\n", n_jpeg);
	printf("Number of PNG files: %d\n", n_png);
	printf("Number of GZIP files: %d\n", n_gzip);
	printf("Number of ZIP files: %d\n", n_zip);

}

int largerthanCondition(fileInfo_node *node, int x) {
	return (node->fi.size > (off_t)x);
}

int namepartCondition(fileInfo_node *node, char *str) {
	return (strstr(node->fi.name, str) != 0);
}

int ownerCondition(fileInfo_node *node, int uid) {
	return (node->fi.uid == (uid_t)uid);
}

int modeCondition(fileInfo_node *node, int x, int uid, char *str, enum selectMode mode) {
	switch(mode) {
		case MODE_LARGERTHAN:
			return largerthanCondition(node, x);
		case MODE_NAMEPART:
			return namepartCondition(node, str);
		case MODE_OWNER:
			return ownerCondition(node, uid);
		default:
			return -1;
	}
}

void menuSelectRecord(fileInfo_list *index, int x, int uid, char *str, enum selectMode mode) {
	/* Function for "largerthan", "namepart" and "owner" commands. */
	fileInfo_node *tmp = index->head;
	char buf[BUF_RECORD_SIZE];
	memset(buf, 0, BUF_RECORD_SIZE);
	int count=0;
	int offset=0;
	int printToStdin=-1;
	FILE* pager_in;
	char* pager;
	while(tmp != NULL) {
		/* Variable |mode| specifies which command was entered.*/
		if (modeCondition(tmp, x, uid, str, mode)) {
			count++;
			if (count<=3) {
				offset = printNodeBuf(tmp, buf, offset);
			} else if (count==4) {
				pager = getenv("PAGER");
				if (!pager) {
					printf("%s", buf);
					printToStdin=1;
				} else {
					pager_in = popen(pager, "w");
					if (pager_in == NULL) {
						ERR("popen");
					}
					fprintf(pager_in, "%s", buf);
					printToStdin=0;
				}
			}
			if (count>=4) {
				if (printToStdin) {
					printNode(tmp, stdin);
				} else {
					printNode(tmp, pager_in);
				}
			}
		}
		tmp = tmp->next;
	}
	if (printToStdin==-1) {
		printf("%s", buf);
	}
	if (!printToStdin) {
		if (pclose(pager_in)==-1){
			if (errno!=EPIPE) {
				ERR("pclose");
			}
		}
	}
}

void menuExit(threadData* thread_data) {
	enum threadStatus status;
	if (pthread_mutex_lock(thread_data->mxStatus)) {
		ERR("pthread_mutex_lock");
	}
	status = *(thread_data->status);
	if (pthread_mutex_unlock(thread_data->mxStatus)) {
		ERR("pthread_mutex_unlock");
	}
	/* If status is THREAD_IN_PROGRESS, it will eventually turn to
	 * THREAD_PENDING_JOIN.*/
	if (status!=THREAD_NOT_EXISTS) {
		if(pthread_join(thread_data->tid, NULL)) {
			ERR("pthread_join");
		}
	}
	if (pthread_mutex_lock(thread_data->mxStatus)) {
		ERR("pthread_mutex_lock");
	}
	*(thread_data->status) = THREAD_NOT_EXISTS;
	if (pthread_mutex_unlock(thread_data->mxStatus)) {
		ERR("pthread_mutex_unlock");
	}
	freeList(thread_data->index);

}

void menuForceExit(threadData* thread_data) {
	enum threadStatus status;
	if (pthread_mutex_lock(thread_data->mxStatus)) {
		ERR("pthread_mutex_lock");
	}
	status = *(thread_data->status);
	if (pthread_mutex_unlock(thread_data->mxStatus)) {
		ERR("pthread_mutex_unlock");
	}
	/* If the thread does not exists, then exit immidiately.*/
	if (status==THREAD_NOT_EXISTS) {
		freeList(thread_data->index);
		return;
	} /* If the indexing is in action, then cancel the thread*/
	else if (status==THREAD_IN_PROGRESS) {
		if(pthread_cancel(thread_data->tid)) {
			ERR("pthread_cancel");
		}
	}
	/* Even if the thread was cancelled, we need to join.*/
	if (pthread_join(thread_data->tid, NULL)) {
		ERR("pthread_join");
	}
	if (pthread_mutex_lock(thread_data->mxStatus)) {
		ERR("pthread_mutex_lock");
	}
	*(thread_data->status) = THREAD_NOT_EXISTS;
	if (pthread_mutex_unlock(thread_data->mxStatus)) {
		ERR("pthread_mutex_unlock");
	}
	freeList(thread_data->index);
}

void menuIndex(threadData* thread_data) {
	enum threadStatus status;
	if (pthread_mutex_lock(thread_data->mxStatus)) {
		ERR("pthread_mutex_lock");
	}
	status = *(thread_data->status);
	if (pthread_mutex_unlock(thread_data->mxStatus)) {
		ERR("pthread_mutex_unlock");
	}
	switch (status) {
		case THREAD_IN_PROGRESS:
			printf("Indexing in action.\n");
			break;
		case THREAD_PENDING_JOIN:
			if(pthread_join(thread_data->tid, NULL)) {
				ERR("pthread_join");
			}
			runThread(thread_data);
			break;
		case THREAD_NOT_EXISTS:
			runThread(thread_data);
			break;
	}
}
