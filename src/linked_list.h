#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

typedef struct LinkedList {
  Node *head;
  Node *tail;
  size_t size;
} LinkedList;

LinkedList *linked_list_create();
void linked_list_destroy(LinkedList *list);
void linked_list_append(LinkedList *list, void *data);

#endif
