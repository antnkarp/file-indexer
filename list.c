/*
------------------------------------------------------------------------
  I declare that this piece of work which is the basis for recognition of
  achieving learning outcomes in the OPS2 course was completed on my own.
  Antoni Karpinski 249372
------------------------------------------------------------------------
*/

#include "mole.h"

void initList(fileInfo_list *list) {
	list->head=NULL;
	list->tail=NULL;
	list->size=0;
}
fileInfo_node* newNode() {
	 fileInfo_node* node = (fileInfo_node*) malloc(sizeof(fileInfo_node));
	 node->next=NULL;
	 return node;
}

void pushList(fileInfo_list *list, fileInfo_node *node) {
	if (list->size==0) {
		list->head=node;
		list->tail=node;
		list->size=1;
	} else {
		list->tail->next = node;
		list->tail = list->tail->next;
		(list->size)++;
	}
}
int printNodeBuf(fileInfo_node *node, char *buf, int init_off) {
	int sum_off=init_off;
	sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "name: %s\n", node->fi.name);
	sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "size: %ld\n", node->fi.size);
	sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "uid: %d\n", node->fi.uid);
	switch(node->fi.type) {
		case TYPE_DIR:
			sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "type: Directory\n");
			break;
		case TYPE_GZIP:
			sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "type: GZIP\n");
			break;
		case TYPE_ZIP:
			sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "type: ZIP\n");
			break;
		case TYPE_JPEG:
			sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "type: JPEG\n");
			break;
		case TYPE_PNG:
			sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "type: PNG\n");
			break;
		default:
			break;
	}
	sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "path: %s\n", node->fi.path);
	sum_off+=snprintf(buf+sum_off, BUF_RECORD_SIZE, "\n\n");
	return sum_off;
}

void printNode(fileInfo_node *node, FILE* fp) {
	fprintf(fp, "name: %s\n", node->fi.name);
	fprintf(fp, "size: %ld\n", node->fi.size);
	fprintf(fp, "uid: %d\n", node->fi.uid);
	
	switch(node->fi.type) {
		case TYPE_DIR:
			fprintf(fp, "type: Directory\n");
			break;
		case TYPE_GZIP:
			fprintf(fp, "type: GZIP\n");
			break;
		case TYPE_ZIP:
			fprintf(fp, "type: ZIP\n");
			break;
		case TYPE_JPEG:
			fprintf(fp, "type: JPEG\n");
			break;
		case TYPE_PNG:
			fprintf(fp, "type: PNG\n");
			break;
		default:
			break;
	}
	fprintf(fp, "path: %s\n", node->fi.path);
	fprintf(fp, "\n\n");
}

void freeList(fileInfo_list *list) {
	fileInfo_node *tmp = list->head;
	fileInfo_node *next;
	while (tmp!=NULL) {
		next = tmp->next;
		free(tmp);
		tmp = next;
	}
	list->head=NULL;
	list->tail=NULL;
}


void printList(fileInfo_list *list) {
	fileInfo_node *tmp = list->head;
	while (tmp != NULL) {
		printNode(tmp, stdin);
		tmp = tmp->next;
	}
} 
