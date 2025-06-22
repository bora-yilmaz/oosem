#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_CAPACITY 16

// ==== Array-Backed Node ====
typedef struct Node {
    int* data;
    int size;
    int capacity;
} Node;

// ==== List of Array-Backed Nodes ====
typedef struct ListNode {
    Node** sublist;
    int size;
    int capacity;
} ListNode;

// ==== Core Memory ====
static Node* create_node(int value) {
    Node* n = malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(1); }
    n->capacity = INITIAL_CAPACITY;
    n->size = 1;
    n->data = malloc(sizeof(int) * n->capacity);
    if (!n->data) { perror("malloc"); exit(1); }
    n->data[0] = value;
    return n;
}

static ListNode* create_listnode(Node* sublist) {
    ListNode* ln = malloc(sizeof(ListNode));
    if (!ln) { perror("malloc"); exit(1); }
    ln->capacity = INITIAL_CAPACITY;
    ln->size = 1;
    ln->sublist = malloc(sizeof(Node*) * ln->capacity);
    if (!ln->sublist) { perror("malloc"); exit(1); }
    ln->sublist[0] = sublist;
    return ln;
}

// ==== Core Operations ====
static int getindex(Node* n, int index) {
    if (index < 0 || index >= n->size) {
        fprintf(stderr, "getindex: out of bounds\n");
        exit(1);
    }
    return n->data[index];
}

static Node* mgetindex(ListNode* l, int index) {
    if (index < 0 || index >= l->size) {
        fprintf(stderr, "mgetindex: out of bounds\n");
        exit(1);
    }
    return l->sublist[index];
}

static void setindex(Node* n, int index, int value) {
    if (index < 0 || index >= n->size) {
        fprintf(stderr, "setindex: out of bounds\n");
        exit(1);
    }
    n->data[index] = value;
}

static void push(Node** nref, int value) {
    Node* n = *nref;
    if (!n) {
        *nref = create_node(value);
        return;
    }
    if (n->size >= n->capacity) {
        n->capacity *= 2;
        n->data = realloc(n->data, sizeof(int) * n->capacity);
        if (!n->data) { perror("realloc"); exit(1); }
    }
    n->data[n->size++] = value;
}

static void pop(Node** nref) {
    Node* n = *nref;
    if (!n || n->size == 0) return;
    n->size--;
}

static void remove_index(Node** nref, int index) {
    Node* n = *nref;
    if (!n || index < 0 || index >= n->size) {
        fprintf(stderr, "remove_index: out of bounds\n");
        exit(1);
    }
    memmove(&n->data[index], &n->data[index + 1], sizeof(int) * (n->size - index - 1));
    n->size--;
}

static int len(Node* n) {
    return n ? n->size : 0;
}

static Node* slicebyindex(Node* n, int start, int end) {
    if (!n || start < 0 || end > n->size || start > end) return NULL;
    Node* out = malloc(sizeof(Node));
    out->size = end - start;
    out->capacity = out->size;
    out->data = malloc(sizeof(int) * out->capacity);
    memcpy(out->data, n->data + start, sizeof(int) * out->size);
    return out;
}

static ListNode* mslicebyindex(ListNode* l, int start, int end) {
    if (!l || start < 0 || end > l->size || start > end) return NULL;
    ListNode* out = malloc(sizeof(ListNode));
    out->size = end - start;
    out->capacity = out->size;
    out->sublist = malloc(sizeof(Node*) * out->capacity);
    memcpy(out->sublist, l->sublist + start, sizeof(Node*) * out->size);
    return out;
}

// ==== Splitting ====
static ListNode* splitbyvalue(Node* n, int delim) {
    ListNode* out = malloc(sizeof(ListNode));
    out->capacity = INITIAL_CAPACITY;
    out->size = 0;
    out->sublist = malloc(sizeof(Node*) * out->capacity);

    Node* part = malloc(sizeof(Node));
    part->capacity = INITIAL_CAPACITY;
    part->size = 0;
    part->data = malloc(sizeof(int) * part->capacity);

    for (int i = 0; i < n->size; i++) {
        if (n->data[i] == delim) {
            if (out->size == out->capacity) {
                out->capacity *= 2;
                out->sublist = realloc(out->sublist, sizeof(Node*) * out->capacity);
            }
            out->sublist[out->size++] = part;
            part = malloc(sizeof(Node));
            part->capacity = INITIAL_CAPACITY;
            part->size = 0;
            part->data = malloc(sizeof(int) * part->capacity);
        } else {
            if (part->size == part->capacity) {
                part->capacity *= 2;
                part->data = realloc(part->data, sizeof(int) * part->capacity);
            }
            part->data[part->size++] = n->data[i];
        }
    }

    if (out->size == out->capacity) {
        out->capacity *= 2;
        out->sublist = realloc(out->sublist, sizeof(Node*) * out->capacity);
    }
    out->sublist[out->size++] = part;

    return out;
}

static ListNode* splitbylen(Node* n, int chunk_size) {
    ListNode* out = malloc(sizeof(ListNode));
    out->capacity = INITIAL_CAPACITY;
    out->size = 0;
    out->sublist = malloc(sizeof(Node*) * out->capacity);

    for (int i = 0; i < n->size; i += chunk_size) {
        Node* chunk = malloc(sizeof(Node));
        chunk->size = (i + chunk_size <= n->size) ? chunk_size : (n->size - i);
        chunk->capacity = chunk->size;
        chunk->data = malloc(sizeof(int) * chunk->size);
        memcpy(chunk->data, &n->data[i], sizeof(int) * chunk->size);

        if (out->size == out->capacity) {
            out->capacity *= 2;
            out->sublist = realloc(out->sublist, sizeof(Node*) * out->capacity);
        }
        out->sublist[out->size++] = chunk;
    }

    return out;
}

// ==== Freeing ====
static void free_list(Node* n) {
    if (!n) return;
    free(n->data);
    free(n);
}

static void free_listlist(ListNode* l) {
    if (!l) return;
    for (int i = 0; i < l->size; i++) {
        free_list(l->sublist[i]);
    }
    free(l->sublist);
    free(l);
}

#endif // LINKEDLIST_H
