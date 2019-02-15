#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

/* Initialize the linked list to an empty linked list; */
void linked_list_init(linked_list_t *list)
{
    list->head = NULL;
    list->tail = NULL;
}

/* Push a filepath to the back of the list; Can add duplicates; */
int linked_list_push_back(linked_list_t *list, char *filepath)
{
    if (list->head == NULL) {
        list->head = (list_node_t*) malloc(sizeof(list_node_t));
        strcpy(list->head->filepath, filepath);
        list->tail = list->head;
    } else {
        list->tail->next = (list_node_t*) malloc(sizeof(list_node_t));
        list->tail = list->tail->next;
        strcpy(list->tail->filepath, filepath);
    }

    return 0;
}

/* Remove an filepath from the list; Removes duplicates; */
int linked_list_remove(linked_list_t *list, char *filepath)
{
    list_node_t *p, *temp;

    p = list->head;
    while (p->next != NULL) {
        if (strcmp(p->next->filepath, filepath) == 0) {
            temp = p->next;
            p->next = temp->next;
            free(temp);
        } else {
            p = p->next;
        }
    }

    if (strcmp(list->head->filepath, filepath) == 0) {
        temp = list->head;
        list->head = temp->next;
        free(temp);
    }

    return 0;
}

/* Check if a filepath exists in the list; */
int linked_list_lookup(linked_list_t *list, char *filepath)
{
    list_node_t *p;

    p = list->head;
    while (p != NULL) {
        if (strcmp(p->filepath, filepath) == 0) {
            return 1;
        }
        p = p->next;
    }

    return 0;
}

void recursive_clear(list_node_t *node)
{
    if (node == NULL) {
        return;
    }

    if (node->next == NULL) {
        return;
    }

    recursive_clear(node->next);
    free(node->next);
}

/* Clear a linked list and free the used memory; */
void linked_list_destroy(linked_list_t *list)
{
    recursive_clear(list->head);
    free(list->head);
    list->head = NULL;
    list->tail = NULL;
}
