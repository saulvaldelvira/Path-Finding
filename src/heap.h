/**
 * Heap structure to support the A* algorithm
 *
 * Copyright (C) 2023 Saúl Valdelvira
 * License: MIT
 * Last update: 25-03-2022
*/
#pragma once
#ifndef HEAP_H
#define HEAP_H

#include "path_finding.h"
#include <stdbool.h>

typedef struct Heap {
        Node* elements[N_ROWS * N_COLS];
        int n_elements;
} Heap;

int heap_add(Heap *heap, Node *node);

Node* heap_peek(Heap *heap);

Node* heap_pop(Heap *heap);

int heap_change_priority(Heap *heap, Node *node, int g, int h);

bool heap_exists(Heap *heap, Node *node);

#endif
