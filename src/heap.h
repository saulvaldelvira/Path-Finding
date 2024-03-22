/*
 * Heap structure to support the A* algorithm
 * Author: Saúl Valdelvira (2023)
*/
#pragma once
#ifndef HEAP_H
#define HEAP_H

#include "path_finding.h"
#include <stdbool.h>

typedef struct Heap {
        int n_elements;
        Node **elements;
} Heap;

int heap_add(Heap *heap, Node *node);

Node* heap_peek(Heap *heap);

Node* heap_pop(Heap *heap);

int heap_change_priority(Heap *heap, Node *node, double g, double h);

bool heap_exists(Heap *heap, Node *node);

#endif
