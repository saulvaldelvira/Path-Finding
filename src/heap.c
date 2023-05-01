/**
 * Heap implementation
 *
 * Copyright (C) 2023 Saúl Valdelvira
 * License: MIT
 * Last update: 25-03-2022
*/
#include "heap.h"

static inline void heap_swap(Heap *heap,int p1, int p2){
        Node *tmp = heap->elements[p1];

        heap->elements[p1] = heap->elements[p2];
        heap->elements[p1]->heap_index = p1;

        heap->elements[p2] = tmp;
        heap->elements[p2]->heap_index = p2;
}

static void filter_up(Heap *heap, int pos){
        if (pos <= 0){
                return;
        }

        int father = (pos-1) / 2;
        double pos_f = heap->elements[pos]->g + heap->elements[pos]->h;
        double father_f = heap->elements[father]->g + heap->elements[father]->h;

        if (father_f < pos_f){
                return;
        }else if(father_f == pos_f){
		if (heap->elements[father]->h <= heap->elements[pos]->h){
			return;
		}
	}

        heap_swap(heap, father, pos);

	filter_up(heap, father);
}

static int lowest_child(Heap *heap, int pos){
	int l_child = pos * 2 + 1;
	int r_child = pos * 2 + 2;
	int size = heap->n_elements;

	if (l_child >= size){
		if (r_child >= size){
			return -1;
		}else {
			return r_child;
		}
	}else if (r_child >= size){
		return l_child;
	}else{
                double left_f = heap->elements[l_child]->g + heap->elements[l_child]->h;
                double right_f = heap->elements[r_child]->g + heap->elements[r_child]->h;
		if (left_f < right_f){
			return l_child;
		}else if(left_f == right_f){
			if (heap->elements[l_child]->h <= heap->elements[r_child]->h){
				return l_child;
			}
		}
		return r_child;
	}
}

static void filter_down(Heap *heap, int pos){
        int lowest = lowest_child(heap, pos);
        if (lowest == -1){
                return;
        }

        double lowest_f = heap->elements[lowest]->g + heap->elements[lowest]->h;
        double pos_f = heap->elements[pos]->g + heap->elements[pos]->h;
        if (lowest_f > pos_f){
                return;
        }else if(lowest_f == pos_f){
		if (heap->elements[lowest]->h >= heap->elements[pos]->h){
			return;
		}
	}

        heap_swap(heap, lowest, pos);
	filter_down(heap, lowest);
}

int heap_add(Heap *heap, Node *node){
        heap->elements[heap->n_elements++] = node;
        node->heap_index = heap->n_elements - 1;
        filter_up(heap, heap->n_elements - 1);
        return 1;
}

Node* heap_peek(Heap *heap){
        return heap->elements[0];
}

Node* heap_pop(Heap *heap){
        Node *ret = heap->elements[0];
        heap->elements[0] = heap->elements[--heap->n_elements];
        heap->elements[0]->heap_index = 0;
        filter_down(heap, 0);
        ret->heap_index = -1;
        return ret;
}

int heap_change_priority(Heap *heap, Node *node, double g, double h){
        if (node->heap_index == -1){
                return -1;
        }
        double old_f = heap->elements[node->heap_index]->g + heap->elements[node->heap_index]->h;
        heap->elements[node->heap_index]->g = g;
        heap->elements[node->heap_index]->h = h;

        if (g + h >= old_f){
                filter_down(heap, node->heap_index);
        }else{
                filter_up(heap, node->heap_index);
        }
        return 1;
}

bool heap_exists(Heap *heap, Node *node){
        if (node->heap_index == -1){
                return false;
        }
        if (heap->elements[node->heap_index] == node){
                return true;
        }
        return false;
}
