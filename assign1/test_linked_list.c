#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

#define NUM_FILES 10

int main(int argc, char **argv)
{
    int i;
    char filepath[NUM_FILES][4096];
    linked_list_t *list;

    printf("Testing the linked list implementation by adding 10 files, "
            "and then removing 5 files:\n");

    for (i = 0; i < NUM_FILES; i++) {
        sprintf(filepath[i], "/file/path%d\0", i);
    }

    list = (linked_list_t*) malloc(sizeof(linked_list_t));
    linked_list_init(list);

    if (list->head != NULL || list->tail != NULL) {
        printf("Initializing the linked list failed!\n");
        return 0;
    }

    for (i = 0; i < NUM_FILES / 2; i++) {
        linked_list_push_back(list, filepath[i]);
    }

    for (i = NUM_FILES - 1; i >= NUM_FILES / 2; i--) {
        linked_list_push_back(list, filepath[i]);
    }

    for (i = 0; i < NUM_FILES; i++) {
        if (!linked_list_lookup(list, filepath[i])) {
            printf("%s was not added! Test failed!\n",
                    filepath[i]);
            return 0;
        }
    }

    for (i = 0; i < NUM_FILES; i += 2) {
        linked_list_remove(list, filepath[i]);
    }

    for (i = 0; i < NUM_FILES; i++) {
        if (i % 2 == 0) {
            if (linked_list_lookup(list, filepath[i])) {
                printf("%s was not removed! Test failed!\n",
                        filepath[i]);
                return 0;
            }
        } else {
            if (!linked_list_lookup(list, filepath[i])) {
                printf("%s was removed! Test failed!\n",
                        filepath[i]);
                return 0;
            }
        }
    }


    linked_list_destroy(list);
    if (list->head != NULL || list->tail != NULL) {
        printf("Destroying the linked list failed!\n");
        return 0;
    }

    printf("All tests passed!\n");
    
    return 0;
}
