/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2007 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <windows.h>
#include <string.h>

#ifdef JAMING
#include "../ming.h"
#else
#include "ming.h"
#endif
#include "hspdll.h"
#include "mhsputil.h"

int mhsp_list_init(mhspList *list, int (*dtor)(void *))
{
#define INITIAL_SIZE 16
	if (!(list->head = (mhspNode *)calloc(INITIAL_SIZE, sizeof(mhspNode)))) {
		return -1;
	}
	list->num  = 0;
	list->size = INITIAL_SIZE;
	list->dtor = dtor;
	return 0;
}

int mhsp_list_append(mhspList *list, void *data)
{
	mhspNode *node, *head = list->head;
	int num, size;

	num = list->num;
	size = list->size;

	if (num >= size) {
		mhspNode *tmp;
		int osize = size;

		size *= 2;
		if ( !(tmp = (mhspNode *)realloc(head, sizeof(mhspNode) * size)) ) {
			return -1;
		}
		head = tmp;
		memset(&head[osize], 0, sizeof(mhspNode) * (size - osize));
	}

	if (head[num].occupied) {
		node = head;
		while (node->occupied) {
			node++;
		}
	} else {
		node = &head[num];
	}

	node->occupied = 1;
	node->data = data;
	list->head = head;
	list->num = ++num;

	return 0;
}

int mhsp_list_remove(mhspList *list, void *data)
{
	mhspNode *head = list->head;
	int size = list->size;
	int num = list->num;
	int i, n = 0;

	for (i = 0; i < size; i++) {
		mhspNode *node = &head[i];

		if (n >= num) {
			break;
		}
		if (node->occupied) {
			n++;
			if (node->data == data) {
				node->occupied = 0;
				list->num = --num;
				return 0;
			}
		}
	}

	return -1;
}

int mhsp_list_member(mhspList *list, void *data)
{
	mhspNode *head = list->head;
	int size = list->size;
	int num = list->num;
	int i, n = 0;

	for (i = 0; i < size; i++) {
		mhspNode *node = &head[i];

		if (n >= num) {
			break;
		}
		if (node->occupied) {
			n++;
			if (node->data == data) {
				return 1;
			}
		}
	}

	return 0;
}

void mhsp_list_clear(mhspList *list)
{
	mhspNode *head = list->head;
	int size = list->size;
	int i;

	for (i = 0; i < size; i++) {
		mhspNode *node = &head[i];
		if (node->occupied) {
			list->dtor(node->data);
			node->occupied = 0;
		}
	}
	list->num = 0;
}

void mhsp_list_destroy(mhspList *list)
{
	mhsp_list_clear(list);
	free(list->head);
}
