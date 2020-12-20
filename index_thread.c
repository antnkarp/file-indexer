
#include "file_indexer.h"

void *thread_work(void *voidPtr) {
	if (pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL)) {
		ERR("pthread_setcanceltype");
	}
	threadData *args = (threadData*) voidPtr;

	/* Free the list to avoid memory leaks. */
	freeList(args->index);
	initList(args->index);

	walk(args->path_d, args->index);

	/* Disable cancellation, so that there are no unfinished writes. */
	if (pthread_setcanceltype(PTHREAD_CANCEL_DISABLE, NULL)) {
		ERR("pthread_setcanceltype");
	}

	saveFile(args->path_f, args->index);

	/* Periodically reindex every t seconds. */
	if(*(args->t) != -1) {
		alarm(0);
		alarm(*(args->t));
	}
	fprintf(stderr, "\nFinished indexing\n");

	if (pthread_mutex_lock(args->mxStatus)) {
		ERR("pthread_mutex_lock");
	}
	/* The operation has been completed. The thread is awaiting join. */
	*(args->status)=THREAD_PENDING_JOIN;

	if (pthread_mutex_unlock(args->mxStatus)) {
		ERR("pthread_mutex_unlock");
	}
	return NULL;
}


void walk(char *dirToOpen, fileInfo_list *index) {
	/* Recursively check every subdirectory and file. */
	DIR *dir;
	struct dirent *entry;
	struct stat s;
	memset(&s, 0, sizeof(struct stat));
	enum fileType cur_type=TYPE_OTHER;
	int fd;
	if ((dir = opendir(dirToOpen)) == NULL) {
		ERR("opendir");
	}
	if ((entry = readdir(dir)) == NULL) {
		ERR("readdir");
	}
	/* opendir() uses malloc for its return pointer. Invoke closedir()
	 * onm cleanup.*/
	pthread_cleanup_push(clean, dir);
	/* Iterate over all entries in the directory. */
	do {
		char path[FILE_PATH_LEN];
		memset(path, 0, FILE_PATH_LEN*sizeof(char));
		int len = snprintf(path, sizeof(path), "%s/%s", dirToOpen, entry->d_name);
		if (len==FILE_PATH_LEN-1) {
			printf("File path too long. Skipping... \n");
			continue;
		}
		if (strlen(entry->d_name)>FILE_NAME_LEN-1) {
			printf("File name too long. Skipping... \n");
			continue;
		}
		int lstat_failed = lstat(path, &s);
		if(!lstat_failed && S_ISDIR(s.st_mode)) {
			/* Case 1: Directory */

			/* Skip "." and ".." directories. */
			if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
				continue;
			}
			cur_type = TYPE_DIR;
			/*Recursive call into a subfolder. */
			walk(path, index);
		} else if (!lstat_failed && S_ISREG(s.st_mode)) {
			/* Case 2: File */
			/* If failed to open then skip this file */
			if ((fd=open(path, O_RDONLY,0777))<0) {
				continue;
			}
			cur_type = getFileType(fd);
			if (close(fd)) {
				ERR("close");
			}
		} else if (lstat_failed)  {
			ERR("lstat");
		}
		if (cur_type==TYPE_OTHER) {
			continue;
		}
		/* Append new node to the list. */
		fileInfo_node* new_node = newNode();
		fileInfo fi;
		memset(&fi, 0, sizeof(fi));
		strcpy(fi.path, path);
		strcpy(fi.name, entry->d_name);
		fi.type = cur_type;
		fi.uid = s.st_uid;
		fi.size = s.st_size;
		new_node->fi = fi;
		pushList(index, new_node);
	} while ((entry = readdir(dir))!=NULL);

	if (closedir(dir) == -1) {
		ERR("closedir");
	}
	pthread_cleanup_pop(0);
}

void clean(void *dir) {
	if (closedir((DIR*)dir) == -1) {
		ERR("closedir");
	}
}

void runThread(threadData *thread_data) {
	/* Set status to THREAD_IN_PROGRESS just before creating a thread
	 * to account for a situation where the scheduler delays creation of the
	 * thread and a duplicate thread might be created.*/
	if (pthread_mutex_lock(thread_data->mxStatus)) {
		ERR("pthread_mutex_lock");
	}
	*(thread_data->status)=THREAD_IN_PROGRESS;
	if (pthread_mutex_unlock(thread_data->mxStatus)) {
		ERR("pthread_mutex_unlock");
	}
	if (pthread_create(&(thread_data->tid), NULL, thread_work, thread_data)) {
		ERR("pthread_create");
	}
}
