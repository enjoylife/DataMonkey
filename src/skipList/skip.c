#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdint.h>
#include "./dbg.h"
#include "./skip.h"

typedef struct skip_node {
    //TODO: add ML and Distributed algorithm's variables
    unsigned int key;
    void *payload;
    struct skip_level {
        struct skip_node *forward;
    } level[]; // C99 construct of variable length array as last member
} skip_node;

typedef struct skip_list {
    //TODO: add graph algorithm's variables
    int current_level; // Ranges from 0 to (SKIP_MAX -1)
    int length;
    struct  skip_node *header;
    free_func_t free_func;
} skip_list;

skip_node *create_skip_node(int level, unsigned int key, void *payload)
{
    skip_node *node = malloc(sizeof(*node) + level * sizeof(struct skip_level));
    //TODO: Switch to a more robust error handling policy
    check_hard(node, "Failed to find memory for a new skip node");
    node->key = key;
    node->payload = payload;
    return node;
}

inline static void default_free (void *ptr)
{
    ;
    //free(ptr);
}

inline static int default_compare(unsigned int a, unsigned int b)
{
    return a <  b;
}

int random_level(void) {
    int level = 1;
    while ((random()&0xFFFF) < (0.25 * 0xFFFF))
        level += 1;
    return (level<SKIP_MAX) ? level : SKIP_MAX;
}

//TODO: Switch to a more robust error handling policy
extern skip_t skip_init(free_func_t free_func)
{
    int i;
    skip_list *new_skip;
    
    new_skip = malloc(sizeof(*new_skip));
    check_hard(new_skip, "Couldn't find memory for a new skip list.");
    
    // data structure callbacks
    new_skip->free_func = (free_func == NULL) ? default_free : free_func;
    
    // init our random generator
    srand(time(NULL));
    
    new_skip->current_level = 1; // following algorithm to the letter
    new_skip->length = 0;
    new_skip->header = create_skip_node(SKIP_MAX, 0, NULL);
    
    for (i = 0; i < SKIP_MAX; i++) {
        new_skip->header->level[i].forward =  NULL;
    }
    return new_skip;
}

extern void skip_destroy(skip_t sl){
    check_hard(sl,"Invalid pointer to skip list");
    skip_node * next, *node;
    node = sl->header->level[0].forward;
    free(sl->header);
    while(node){
        next = node->level[0].forward;
        sl->free_func(node->payload);
        free(node);
        node = next;
    }
    free(sl);
}

//TODO: Switch to a more robust error handling policy
extern int skip_insert(skip_t sl, unsigned int key, void * payload)
{
    int i, rand_level;
    skip_node *x, *update[SKIP_MAX];

    check_hard(sl, "Invalid Skip List");
    check_hard(key, "Invalid Key");
    check_hard(payload," Empty payload");

    x = sl->header;
    for (i = sl->current_level-1; i >= 0; i--) {
        while (x->level[i].forward && // not end NULL
               default_compare(x->level[i].forward->key, key)) {
            x = x->level[i].forward;
         //   log_warn("still in whleLoop");
        }
        update[i] = x;
        //log_warn("still in ForLoop");
    }

    // make sure were not nulled
    x = (x->level[0].forward)? x->level[0].forward: x;
    //TODO: Decide if we should warn, or reject an overwrite,
    log_info(" %u,  %u", x->key, key);
    if (x->key == key) {
        sl->free_func(x->payload);
        x->payload = payload;
        log_success("Changed key");
        return 2;
    } else {
        rand_level = random_level();
        if (rand_level > sl->current_level) {
            for (i = sl->current_level; i < rand_level; i++) {
                update[i] = sl->header;
            }
            sl->current_level = rand_level;
        }
        x = create_skip_node(rand_level,key,payload);
        for (i = 0 ; i < rand_level; i++) {
            x->level[i].forward = update[i]->level[i].forward;
            update[i]->level[i].forward = x;
        }
        log_success("inserted key %u and data", key);
        sl->length++;
        return 1;
    }
}

extern void *skip_search(skip_list *sl, unsigned int key)
{
    
    int i;
    skip_node *x;

    check_hard(sl, "Invalid Skip List");
    check_hard(key, "Invalid Key");

    x = sl->header;
    for (i = sl->current_level-1; i >= 0; i--) {
        while (x->level[i].forward && // not end NULL
               default_compare(x->level[i].forward->key, key)) {
            x = x->level[i].forward;
        }
    }
    // make sure were not nulled
    x = (x->level[0].forward)? x->level[0].forward: x;
    if (x->key == key) {
        log_success("Found key");
        return x->payload;
    } else{
    log_warn("Key search failed");
    return NULL;
    }
}

//TODO: Switch to a more robust error handling policy
extern  int skip_delete(skip_t sl, unsigned int key){
    int i, rand_level;
    skip_node *x, *update[SKIP_MAX];

    check_hard(sl, "Invalid Skip List");
    check_hard(key, "Invalid Key");

    x = sl->header;
    for (i = sl->current_level-1; i >= 0; i--) {
        while (x->level[i].forward && // not end NULL
               default_compare(x->level[i].forward->key, key)) {
            x = x->level[i].forward;
         //   log_warn("still in whleLoop");
        }
        update[i] = x;
        //log_warn("still in ForLoop");
    }

    // make sure were not nulled
    x = (x->level[0].forward)? x->level[0].forward: x;
    if (x->key == key) {
        for(i=0; i < sl->current_level ;i++){
            if(update[i]->level[i].forward == x){
                update[i]->level[i].forward = x->level[i].forward;
            }
        }
        sl->free_func(x->payload);
        log_success("MADEIT");
        free(x);
        while(sl->current_level > 1 &&
                (sl->header->level[sl->current_level-1].forward ==NULL)){
            log_info("Where going down a level");
            sl->current_level--;
        }
        sl->length--;
        return 1;
    }else {
        return 0;
    }
}

extern int skip_current_level(skip_t s)
{
    check_hard(s,"Invalid pointer to skip list");
    return s->current_level;
}

