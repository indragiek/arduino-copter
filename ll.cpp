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

/* ll_new
 * 
 * Makes a new empty linked list.
 */
 linked_list *ll_new()
 {
 	linked_list *list = (linked_list *) malloc(sizeof(linked_list));
 	list->head = NULL;
 	list->tail = NULL;
 	list->length = 0;
 	return list;
 }

/* ll_length
 * 
 * Returns the number of elements in a linked list.
 */
 int ll_length(linked_list *list)
 {
 	return list->length;
 }

/* ll_prepend
 * 
 * Adds an element on to the front of the linked list.
 */
 void ll_prepend(linked_list *list, int x)
 {
 	ll_node *node = (ll_node *) malloc(sizeof(ll_node));
 	node->value = x;
 	node->next = list->head;
 	list->head = node;
 	
 	list->length++;

 	if (list->tail == NULL) {
 		list->tail = node;
 	}
 }

/* ll_append
 *
 * Adds an element on to the back of the linked list.
 */
 void ll_append(linked_list *list, int x)
 {
 	ll_node *node = (ll_node *) malloc(sizeof(ll_node));
 	node->value = x;
 	node->next = NULL;

 	if (list->tail == NULL) {
 		list->head = node;
 		list->tail = node;
 	} else {
 		list->tail->next = node;
 		list->tail = node;
 	}
 	
 	list->length++;
 }

/* ll_pop_front
 *
 * Removes the element from the front of the list and returns it.
 * NOTE: DO NOT CALL THIS ON AN EMPTY LIST.
 */
 int ll_pop_front(linked_list *list)
 {
 	int rv = list->head->value;

 	ll_node *tmp = list->head;
 	list->head = list->head->next;
 	free(tmp);

 	if (list->head == NULL) {
 		list->tail = NULL;
 	}

 	list->length--;

 	return rv;
 }

/* Exercise 7 */
 int ll_pop_back(linked_list *list)
 {
 }

/* ll_lookup
 * 
 * Returns the element at the index position in the linked list.
 * NOTE: YOU MUST BE SURE index < ll_length(list).
 */
 int ll_lookup(linked_list *list, int index)
 {
 	ll_node *n = list->head;

 	for (int i=0; i<index; i++) {
 		n = n->next;
 	}
 	
 	return n->value;
 }

/* ll_free
 *
 * Cleans up all of the memory for the linked list.
 */
 void ll_free(linked_list *list)
 {
 	while (list->tail != NULL) {
 		ll_pop_front(list);
 	}

 	free(list);
 }
