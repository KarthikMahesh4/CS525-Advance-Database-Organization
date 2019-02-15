#ifndef LINKED_LIST
#define LINKED_LIST

/* The node of a linked list, that stores a filepath; */
typedef struct list_node_t {
    char filepath[4096];
    struct list_node_t *next;
} list_node_t;

/* A linked list that stores filepaths; */
typedef struct linked_list_t {
    struct list_node_t *head;
    struct list_node_t *tail;
} linked_list_t;

/* Initialize the linked list to an empty linked list; */
void linked_list_init(linked_list_t *list);

/* Push a filepath to the back of the list; Can add duplicates; */
int linked_list_push_back(linked_list_t *list, char *filepath);

/* Remove an filepath from the list; Removes duplicates; */
int linked_list_remove(linked_list_t *list, char *filepath);

/* Check if a filepath exists in the list; */
int linked_list_lookup(linked_list_t *list, char *filepath);

/* Clear a linked list and free the used memory; */
void linked_list_destroy(linked_list_t *list);

#endif
