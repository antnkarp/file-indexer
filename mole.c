/*
------------------------------------------------------------------------
  I declare that this piece of work which is the basis for recognition of
  achieving learning outcomes in the OPS2 course was completed on my own.
  Antoni Karpinski 249372
------------------------------------------------------------------------
*/

#include "mole.h"

volatile sig_atomic_t last_signal = 0;

void readArguments(int argc, char **argv, char *path_d, char *path_f, int *t) {
	char c;
	int set_d=0, set_f=0;
	char relative_path_d[FILE_PATH_LEN];
	char relative_path_f[FILE_PATH_LEN];
	while ((c = getopt(argc, argv, "d:f:t:"))!=-1) {
		switch(c) {
			case 'd':
				strcpy(relative_path_d, optarg);
				set_d=1;
				break;
			case 'f':
				strcpy(relative_path_f, optarg);
				set_f=1;
				break;
			case 't':
				*t = atoi(optarg);
				break;
		}
	}
	if (!set_d) {
		char *mole_dir = getenv("MOLE_DIR");
		if (!mole_dir) {
			ERR("No directory set");
		}
		strcpy(relative_path_d, mole_dir);
	}
	if (!set_f) {
		char *mole_index_path = getenv("MOLE_INDEX_PATH");
		if (!mole_index_path) {
			char *home_str = getenv("HOME");
			strcpy(relative_path_f, home_str);
			char *suffix = "/.mole-index";
			strcat(relative_path_f, suffix); 
		} else {
			strcpy(relative_path_f, mole_index_path);
		}
	}
	/* Store aboslute paths in path_d, path_f. */ 
	if (realpath(relative_path_d, path_d) == NULL) {
		ERR("realpath");
	}
	if (realpath(relative_path_f, path_f) == NULL) {
		strcpy(path_f, relative_path_f);
	}
	if(*t!=-1 && (*t<30 || *t>7200)) {
		ERR("t>=30 and t<=7200");
	}
}

void getCommands(threadData* thread_data) {
	char line[LINE_LEN];
	char *part1, *part2;
	fd_set rfds;
	struct timespec tv;
	sigset_t alarmMask = getAlarmMask();
	tv.tv_sec=1;
	tv.tv_nsec=0;
	for(;;) {
		FD_ZERO(&rfds);
		FD_SET(0,&rfds);
		/* Use pselect with timeout to synchronously handle SIGALRM.
		 * Block SIGALRM while waiting for input. */
		if (pselect(1, &rfds, NULL, NULL, &tv, &alarmMask) == -1) {
			ERR("pselect");
		}
		if (last_signal == SIGALRM) {
			handleSigalrm(thread_data);
		}
		if(FD_ISSET(0, &rfds)) {	
			if(fgets(line,LINE_LEN,stdin) == NULL) {
				if (ferror(stdin)) {
					ERR("Input error (stdin)");
				}
			}
			line[strlen(line)-1]='\0';
			/* Blank line. */
			if (line[0] == '\0') {
				continue;
			}
			part1 = strtok(line, " ");
			part2 = line+strlen(part1)+1;
			(void)part2;
			if (!strcmp(part1, "exit")) {
				menuExit(thread_data);
				break;
			} else if (!strcmp(part1, "exit!")) {
				menuForceExit(thread_data);
				break;
			} else if (!strcmp(part1, "index")) {
				menuIndex(thread_data);
			} else if (!strcmp(part1, "count")) {
				menuCount(thread_data->index);
			} else if (!strcmp(part1, "largerthan")) {
				menuSelectRecord(thread_data->index, atoi(part2), 0, NULL, MODE_LARGERTHAN);
			} else if (!strcmp(part1, "namepart")) {
				menuSelectRecord(thread_data->index, 0, 0, part2, MODE_NAMEPART);
			} else if (!strcmp(part1, "owner")) {
				menuSelectRecord(thread_data->index, 0, atoi(part2), NULL, MODE_OWNER);
			} else {
				printf("Not recognized command\n");
			}
		}
	}		
}

					
int main(int argc, char **argv) {
	
	char path_d[FILE_PATH_LEN], path_f[FILE_PATH_LEN];
	int t=-1;
	readArguments(argc, argv, path_d, path_f, &t);
	sethandler(sig_handler,SIGALRM);
	/* Ignore SIGPIPE to prevent termination after quitting $PAGER */
	sethandler(SIG_IGN, SIGPIPE);
	
	threadData thread_data;
	fileInfo_list index;
	initList(&index);
	enum threadStatus status = THREAD_NOT_EXISTS;
	pthread_mutex_t mxStatus = PTHREAD_MUTEX_INITIALIZER;
	
	thread_data.status = &status;
	thread_data.mxStatus = &mxStatus;
	thread_data.index = &index;
	thread_data.path_d = path_d;
	thread_data.path_f = path_f;
	thread_data.t = &t;
	
	if (!loadFile(path_f, &index)) {
		runThread(&thread_data);
	} else if (t != -1) {
		struct stat attr;
		/* If the file is older than t seconds then start indexing*/
		int stat_result = stat(path_f, &attr);
		if (!stat_result) {
			time_t t_file = attr.st_mtime;
			time_t t_now = time(0);
			int diff = (int)difftime(t_now, t_file);
			if (diff>=t) {
				runThread(&thread_data);
			}
		}
		if (stat_result) {
			ERR("stat");
		}
	}
	
	getCommands(&thread_data);
	return EXIT_SUCCESS;
}
