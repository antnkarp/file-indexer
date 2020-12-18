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
	if (lseek(fd,0,SEEK_SET) == -1) {
		ERR("lseek");
	}
	if (write(fd,&size,sizeof(int)) == -1) {
		ERR("write");
	}
	fileInfo_node *ptr = index->head;
	/* Write all fileInfo structures to the file. */
	while (ptr != NULL) {
		if(write(fd, &(ptr->fi), sizeof(fileInfo)) == -1){
			ERR("write");
		}
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
		if (errno==ENOENT) {
			//File doesn't exist.
			return 0;
		} else {
			ERR("access");
		}
	}
	if ((fd=open(path_f, O_RDONLY,0777))<0) {
		ERR("open");
	}
	if (lseek(fd, 0, SEEK_SET) == -1) {
		ERR("lseek");
	}
	if (read(fd, &size, sizeof(int)) == -1) {
		ERR("read");
	}
	
	fileInfo fi_arr[FILE_READ_CHUNK_SIZE];
	memset(fi_arr, 0, FILE_READ_CHUNK_SIZE*sizeof(fileInfo));
	initList(index);
	int more_data=1;
	int bytes_read;
	fileInfo_node* node;
	int n;
	/* Read node in chunks and append it to the list. */
	while(more_data) {
		if ((bytes_read= read(fd, fi_arr, sizeof(fileInfo)*FILE_READ_CHUNK_SIZE)) == -1) {
			ERR("read");
		}
		n = bytes_read / sizeof(fileInfo);
		if (n<FILE_READ_CHUNK_SIZE) {
			more_data = 0;
		}
		for(int i=0; i<n; i++) {
			node = newNode();
			node->fi = fi_arr[i];
			pushList(index, node);
		}	
	}
	if (close(fd)) {
		ERR("close");
	}
	return 1;
}
