/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2007 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#ifndef _MINGHSP_MHSPUTIL_H_
#define _MINGHSP_MHSPUTIL_H_

typedef struct _mhspNode {
	int occupied;
	void *data;
} mhspNode;

typedef struct _mhspList {
	mhspNode *head;
	int num;
	int size;
	int (*dtor)(void *);
} mhspList;

int mhsp_list_init(mhspList *list, int (*dtor)(void *));
int mhsp_list_append(mhspList *list, void *object);
int mhsp_list_remove(mhspList *list, void *object);
int mhsp_list_member(mhspList *list, void *object);
void mhsp_list_clear(mhspList *list);
void mhsp_list_destroy(mhspList *list);

#endif
