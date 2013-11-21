#ifndef __ll_h__
#define __ll_h__

#include <Arduino.h>

/* Linked List (of ints) */
typedef struct _ll_node {
	int value;
	struct _ll_node *next;
} ll_node;

typedef struct {
	ll_node *head;
	ll_node *tail;
	int length;
} linked_list;

linked_list *ll_new();
int ll_length(linked_list *list);
void ll_prepend(linked_list *list, int x);
void ll_append(linked_list *list, int x);
int ll_pop_front(linked_list *list);
int ll_pop_back(linked_list *list);
int ll_lookup(linked_list *list, int index);
void ll_free(linked_list *list);

#endif