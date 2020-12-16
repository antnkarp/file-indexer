/*
------------------------------------------------------------------------
  I declare that this piece of work which is the basis for recognition of
  achieving learning outcomes in the OPS2 course was completed on my own.
  Antoni Karpinski 249372
------------------------------------------------------------------------
*/

#include "mole.h"

enum fileType getFileType(int fd) {
	enum fileType type;
	if (isJpeg(fd)) {
		type=TYPE_JPEG;
	} else if (isPng(fd)) {
		type=TYPE_PNG;
	} else if (isGzip(fd)) {
		type=TYPE_GZIP;
	} else if (isZip(fd)) {
		type=TYPE_ZIP;
	} else {	
		type=TYPE_OTHER;
	}
	return type;
}

int isJpeg(int fd) {
	char file_begin[4];
	char signature_begin_1[4] = {-1, -40, -1, -32};
	/* Check the first 4 bytes of a file (magic number)*/
	lseek(fd, 0, SEEK_SET);
	read(fd, file_begin, 4);
	for(int i=0; i<4; i++) {
		if (signature_begin_1[i] != file_begin[i]) {
			return 0;
		}
	}
	return 1;
}

int isPng(int fd) {
	char file_begin[8];
	char signature_begin[8] = {-119, 80, 78, 71, 13, 10, 26, 10};
	/* Check the first 8 bytes of a file (magic number)*/
	lseek(fd, 0, SEEK_SET);
	read(fd, file_begin, 8);
	
	for(int i=0; i<8; i++) {
		if (file_begin[i]!=signature_begin[i]) {
			return 0;
		}
	}
	return 1;
}

int isGzip(int fd) {
	char file_begin[2];
	char signature_begin[2] = {31, -117};
	/* Check the first 2 bytes of a file (magic number)*/
	lseek(fd, 0, SEEK_SET);
	read(fd, file_begin, 2);
	for(int i=0; i<2; i++) {
		if (file_begin[i]!=signature_begin[i]) {
			return 0;
		}
	}
	return 1;
}

int isZip(int fd) {
	char file_begin[4];
	char signature_begin_1[4] = {80, 75, 3, 4};
	char signature_begin_2[4] = {80, 75, 5, 6};
	char signature_begin_3[4] = {80, 75, 7, 8};
	/* There are 3 possibilities for the 4 first bytes of a file
	 * (magic number)*/
	int match_1=1, match_2=1, match_3=1;
	lseek(fd, 0, SEEK_SET);
	read(fd, file_begin, 4);
	for(int i=0; i<4; i++) {
		if (file_begin[i]!=signature_begin_1[i]) {
			match_1=0;
		}
		if (file_begin[i]!=signature_begin_2[i]) {
			match_2=0;
		}
		if (file_begin[i]!=signature_begin_3[i]) {
			match_3=0;
		}
	}
	if (match_1||match_2||match_3) {
		return 1;
	} return 0;
}

