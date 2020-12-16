/*
------------------------------------------------------------------------
  I declare that this piece of work which is the basis for recognition of
  achieving learning outcomes in the OPS2 course was completed on my own.
  Antoni Karpinski 249372
------------------------------------------------------------------------
*/

#include "mole.h"

void *thread_work(void *voidPtr) {
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	threadData *args = voidPtr;
	
	pthread_mutex_lock(args->mxInProgress);
	*(args->inProgress)=1;
	pthread_mutex_unlock(args->mxInProgress);
	
	/* Free list to avoid memory leaks. */
	freeList(args->index);
	initList(args->index);
	
	walk(args->path_d, args->index);
	pthread_setcanceltype(PTHREAD_CANCEL_DISABLE, NULL);
	
	saveFile(args->path_f, args->index);
	
	pthread_mutex_lock(args->mxInProgress);
	*(args->inProgress)=0;
	pthread_mutex_unlock(args->mxInProgress);
	
	/* Periodically reindex every t seconds. */
	if(*(args->t) != -1) {
		alarm(0);
		alarm(*(args->t));
	}
	fprintf(stderr, "\nFinished indexing\n");
	return NULL;
}
	 

void walk(char *dirToOpen, fileInfo_list *index) {
	/* Recursively check every subdirectory and file. */
	DIR *dir;
	struct dirent *entry;
	struct stat s;
	if (!(dir = opendir(dirToOpen))) {
		return;
	}
	if (!(entry = readdir(dir))) {
		return;
	}
	enum fileType cur_type;
	/* Iterate over all entries in the directory. */
	do {
		char path[FILE_PATH_LEN];
		int len = snprintf(path, sizeof(path)-1, "%s/%s", dirToOpen, entry->d_name);
		path[len] = '\0';
		if(!lstat(path, &s) && S_ISDIR(s.st_mode)) {	
			/* Skip "." and ".." directories. */
			if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
				continue;
			}
			cur_type = TYPE_DIR;
			/*Recursive call into a subfolder. */
			walk(path, index);
		} else {
			int fd;
			if ((fd=open(path, O_RDONLY,0777))<0) {
				continue;
			}
			cur_type = getFileType(fd);
			if (close(fd)) {
				continue;
			}
			if (cur_type==TYPE_OTHER) {
				continue;
			}
		}
		fileInfo_node* new_node = newNode();
		char absolute_path[FILE_PATH_LEN];
		realpath(path, absolute_path);
		strncpy(new_node->fi.path, absolute_path, FILE_PATH_LEN);
		strncpy(new_node->fi.name, entry->d_name, FILE_NAME_LEN);
		new_node->fi.name[FILE_NAME_LEN - 1] = '\0';
		new_node->fi.path[FILE_PATH_LEN - 1]= '\0';
		if (strlen(new_node->fi.name) == FILE_NAME_LEN-1) {
			printf("File name too long. Skipping... \n");
		} else if (strlen(new_node->fi.path) == FILE_PATH_LEN-1) {
			printf("File path too long. Skipping... \n");
		}
		new_node->fi.type = cur_type;
		new_node->fi.uid = s.st_uid;
		new_node->fi.size = s.st_size;
		pushList(index, new_node);	
	} while ((entry = readdir(dir))!=0);
	closedir(dir); 	
}

void runThread(threadData *thread_data) {
	pthread_attr_t threadAttr;
	if (pthread_attr_init(&threadAttr)) {
		ERR("pthread_attr_init");
	}
	if (pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED)) {
		ERR("pthread_setdetachstate");
	}
	if (pthread_create(&(thread_data->tid), &threadAttr, thread_work, thread_data)) {
		ERR("pthread_create");
	}
	if (pthread_attr_destroy(&threadAttr)) {
		ERR("pthread_attr_destroy");
	}
}
