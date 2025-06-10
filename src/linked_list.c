#include "linked_list.h"
#include <stddef.h>
#include <stdlib.h>

LinkedList *linked_list_create() {
  LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  return list;
}

void linked_list_destroy(LinkedList *list) {
  Node *current = list->head;
  while (current) {
    Node *next = current->next;
    free(current);
    current = next;
  }
  free(list);
}

void linked_list_append(LinkedList *list, void *data) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  new_node->data = data;
  new_node->next = NULL;

  if (list->tail) {
    list->tail->next = new_node;
  } else {
    list->head = new_node;
  }
  list->tail = new_node;
  list->size++;
}
