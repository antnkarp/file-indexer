
#include "file_indexer.h"

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
	memset(file_begin, 0, 4);
	char signature_begin[4] = {-1, -40, -1, -32};
	/* Check the first 4 bytes of a file (magic number)*/
	if (lseek(fd, 0, SEEK_SET) == -1) {
		ERR("lseek");
	}
	if (read(fd, file_begin, 4) == -1) {
		ERR("read");
	}
	if (memcmp(file_begin, signature_begin, 4)) {
		return 0;
	} else {
		return 1;
	}
}

int isPng(int fd) {
	char file_begin[8];
	memset(file_begin, 0, 8);
	char signature_begin[8] = {-119, 80, 78, 71, 13, 10, 26, 10};
	/* Check the first 8 bytes of a file (magic number)*/
	if (lseek(fd, 0, SEEK_SET) == -1) {
		ERR("lseek");
	}
	if (read(fd, file_begin, 8) == -1) {
		ERR("read");
	}
	if (memcmp(file_begin, signature_begin, 4)) {
		return 0;
	} else {
		return 1;
	}
}

int isGzip(int fd) {
	char file_begin[2];
	memset(file_begin, 0, 2);
	char signature_begin[2] = {31, -117};
	/* Check the first 2 bytes of a file (magic number)*/
	if (lseek(fd, 0, SEEK_SET) == -1) {
		ERR("lseek");
	}
	if (read(fd, file_begin, 2) == -1) {
		ERR("read");
	}
	if (memcmp(file_begin, signature_begin, 2)) {
		return 0;
	} else {
		return 1;
	}
}

int isZip(int fd) {
	char file_begin[4];
	memset(file_begin, 0, 4);
	char signature_begin_1[4] = {80, 75, 3, 4};
	char signature_begin_2[4] = {80, 75, 5, 6};
	char signature_begin_3[4] = {80, 75, 7, 8};
	/* There are 3 possibilities for the 4 first bytes of a file
	 * (magic number)*/
	if (lseek(fd, 0, SEEK_SET) == -1) {
		ERR("lseek");
	}
	if (read(fd, file_begin, 4) == -1) {
		ERR("read");
	}
	int match_1 = memcmp(file_begin, signature_begin_1, 4);
	int match_2 = memcmp(file_begin, signature_begin_2, 4);
	int match_3 = memcmp(file_begin, signature_begin_3, 4);

	if (match_1 && match_2 && match_3) {
		return 0;
	} else {
		return 1;
	}
}
