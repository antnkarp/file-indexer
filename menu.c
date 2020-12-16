/*
------------------------------------------------------------------------
  I declare that this piece of work which is the basis for recognition of
  achieving learning outcomes in the OPS2 course was completed on my own.
  Antoni Karpinski 249372
------------------------------------------------------------------------
*/

#include "mole.h"

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
	return (node->fi.size > x);
}

int namepartCondition(fileInfo_node *node, char *str) {
	return (strstr(node->fi.name, str) != 0);
}

int ownerCondition(fileInfo_node *node, int uid) {
	return (node->fi.uid == uid);
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
		pclose(pager_in);
	}
}

