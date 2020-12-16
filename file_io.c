/*
------------------------------------------------------------------------
  I declare that this piece of work which is the basis for recognition of
  achieving learning outcomes in the OPS2 course was completed on my own.
  Antoni Karpinski 249372
------------------------------------------------------------------------
*/

#include "mole.h"

void saveFile(char *path_f, fileInfo_list *index) {
	int fd;
	int size = index->size;
	if ((fd=open(path_f, O_WRONLY | O_CREAT | O_TRUNC, 0777))<0) {
		ERR("open");
	}
	lseek(fd,0,SEEK_SET);
	write(fd,&size,sizeof(int));
	fileInfo_node *ptr = index->head;
	while (ptr != NULL) {
		write(fd, &(ptr->fi), sizeof(fileInfo));
		ptr=ptr->next;
	}
	if (close(fd)) {
		ERR("close");
	}
}

int loadFile(char *path_f, fileInfo_list *index) {
	int fd;
	int size;
	if (access(path_f, F_OK)==-1) {
		//File doesn't exist.
		return 0;
	}
	if ((fd=open(path_f, O_RDONLY,0777))<0) {
		ERR("open");
	}
	lseek(fd, 0, SEEK_SET);
	read(fd, &size, sizeof(int));
	/* Allocate array to load data from file. After building a
	 * linked list, free this array */
	fileInfo* arr = (fileInfo*) malloc(size*sizeof(fileInfo));
	read(fd, arr, size*sizeof(fileInfo));
	initList(index);
	for(int i=0; i<size; i++) {
		fileInfo_node* node = newNode();
		node->fi = arr[i];
		pushList(index, node);	
	}
	if (close(fd)) {
		ERR("close");
	}
	free(arr);
	return 1;
}
