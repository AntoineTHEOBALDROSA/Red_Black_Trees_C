#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
/*
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "basic.h"
#include <stddef.h>
*/

///////////////////////////////////////////////////////////////////////////////
//////// Red Black Trees  /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


typedef struct RBTreeNode {
    bool color; // We use 0 for black, 1 for red
    int key;
    struct RBTreeNode *right;
    struct RBTreeNode *left;
    struct RBTreeNode *p;
} RBTreeNode;

typedef struct RBTree {
    RBTreeNode* sentinel;
    RBTreeNode* root;
} RBTree;


RBTreeNode *RBTree_create_node(int key){
    RBTreeNode *n = malloc(sizeof(RBTreeNode));
    n->color = 0;
    n->key = key;
    n->left = NULL;
    n->right = NULL;
    n->p = NULL;
    return n;
}

RBTree *RBTree_create(int key){
    RBTreeNode *sentinel = malloc(sizeof(RBTreeNode));
    sentinel->color = 0;

    RBTreeNode *n = malloc(sizeof(RBTreeNode));
    n->color = 0;
    n->key = key;
    n->left = sentinel;
    n->right = sentinel;
    n->p = sentinel;

    RBTree *t = malloc(sizeof(RBTree));
    t->sentinel = sentinel;
    t->root = n;

    return t;
}

// we suppose x.right != NULL
void RBTree_LeftRotation(RBTree *t, RBTreeNode* x){
    assert(x->right != t->sentinel);
    RBTreeNode* y = x->right;
    x->right = y->left;
    if (y->left != t->sentinel) y->left->p = x;

    y->p = x->p;
    if (x->p == t->sentinel) t->root = y; // y might be the new root
    else if (x->p->left == x) x->p->left = y;
    else x->p->right = y;

    y->left = x;
    x->p = y;
}

// we suppose x.left != NULL
void RBTree_RightRotation(RBTree *t, RBTreeNode* x){
    assert(x->left != t->sentinel);
    RBTreeNode *y = x->left;
    // we change y's left child
    x->left = y->right;
    if (y->right != t->sentinel) y->right->p = x;
    // we change y's parent
    y->p = x->p;
    if (x->p == t->sentinel) t->root = y;
    else if (x->p->left == x) x->p->left = y;
    else x->p->right = y;
    x->p = y;
    y->right = x;
}

void RB_Insert_Fixup(RBTree *t, RBTreeNode* z){
    while(z->p->color == 1){ // while we break condition 4
        if (z->p == z->p->p->left) { // we are in a left subtree
            RBTreeNode *y = z->p->p->right; // uncle of z
            if (y->color == 1){
                z->p->color = 0;
                y->color = 0;
                z->p->p->color = 1;
                z = z->p->p;
            } else {
                if (z == z->p->right){
                    z = z->p;
                    RBTree_LeftRotation(t, z);
                }
                z->p->color = 0;
                z->p->p->color = 1;
                RBTree_RightRotation(t, z->p->p);
            }
        } else {
            RBTreeNode *y = z->p->p->left; // uncle of z
            if (y->color == 1){
                z->p->color = 0;
                y->color = 0;
                z->p->p->color = 1;
                z = z->p->p;
            } else {
                if (z == z->p->left){
                    z = z->p;
                    RBTree_RightRotation(t, z);
                }
                z->p->color = 0;
                z->p->p->color = 1;
                RBTree_LeftRotation(t, z->p->p);
            }
        }
    }
    t->root->color = 0;
}

void RBTree_Insert(RBTree *t, RBTreeNode* z){
    // We search where z should go 
    RBTreeNode *x = t->root;
    RBTreeNode *y = t->sentinel;
    while(x != t->sentinel){
        // we suppose that the keys are distinct
        y = x;
        if (x->key > z->key) x = x->left;
        else x = x->right;
    }
    z->p = y;
    if (y == t->sentinel) t->root = z;
    else if (y->key > z->key) y->left = z;
    else y->right = z;
    z->left = t->sentinel;
    z->right = t->sentinel;
    z->color = 1;
    RB_Insert_Fixup(t, z);
}

void RBTree_Transplant(RBTree *t, RBTreeNode *u, RBTreeNode *v){
    if (u->p == t->sentinel) t->root = v;
    else if(u->p->right == u) u->p->right = v;
    else u->p->left = v;
    v->p = u->p;
}

// Return the minimum of the subtree having x as root
RBTreeNode * RBTree_Minimum(RBTree *t, RBTreeNode *x){
    if(x->left == t->sentinel) return x;
    else return RBTree_Minimum(t, x->left);
}

void RBTree_Delete_Fixup(RBTree *t, RBTreeNode *x){
    while (x != t->root && x->color == 0){
        if (x == x->p->left){ // if x is in a left subtree
            RBTreeNode *w = x->p->right; // w is x's sibling
            if (w->color == 1){ // case 1 : w is red
                w->color = 0;
                x->p->color = 1;
                RBTree_LeftRotation(t, x->p);
                w = x->p->right; // the sibling of x changed
            }
            if (w->left->color == 0 && w->right->color == 0){ // Case 2 : both child of w are black
                w->color = 1;
                x = x->p;
            } else { // at least one of w's child is red
                if (w->right->color == 0){ // case 3 : if the right child is black, ie left child is red
                    w->left->color = 0;
                    w->color = 1;
                    RBTree_RightRotation(t, w);
                    w = x->p->right; // the sibling of x changed
                }
                // case 4 : w.right is red (after applying case 3 we must be in case 4, hence there no "else")
                w->color = x->p->color;
                x->p->color = 0;
                w->right->color = 0;
                RBTree_LeftRotation(t, x->p);
                x = t->root;
            }
        } else {
            RBTreeNode *w = x->p->left; // w is x's sibling
            if (w->color == 1){ // case 1 : w is red
                w->color = 0;
                x->p->color = 1;
                RBTree_RightRotation(t, x->p);
                w = x->p->left; // the sibling of x changed
            }
            if (w->left->color == 0 && w->right->color == 0){ // Case 2 : both child of w are black
                w->color = 1;
                x = x->p;
            } else { // at least one of w's child is red
                if (w->left->color == 0){ // case 3 : if the left child is black, ie right child is red
                    w->right->color = 0;
                    w->color = 1;
                    RBTree_LeftRotation(t, w);
                    w = x->p->left; // the sibling of x changed
                }
                // case 4 : w.right is red (after applying case 3 we must be in case 4, hence there no "else")
                w->color = x->p->color;
                x->p->color = 0;
                w->left->color = 0;
                RBTree_RightRotation(t, x->p);
                x = t->root;
            }
        }
    } 
    x->color = 0;
}


void RBTree_Delete(RBTree *t, int k){
    RBTreeNode *z = t->root;
    while(z != t->sentinel && z->key != k){
        if(z->key > k) z = z->left;
        else z = z->right;
    }
    if (z == t->sentinel) return; // k is not in the tree

    RBTreeNode *y = z;
    bool y_original_color = y->color;
    RBTreeNode *x;
    if(z->right == t->sentinel){
        x = z->left;
        RBTree_Transplant(t, z, z->left);
    } else if (z->left == t->sentinel){
        x = z->right;
        RBTree_Transplant(t, z, z->right);
    } else {
        y = RBTree_Minimum(t, z->right);
        y_original_color = y->color;
        x = y->right;
        if (y != z->right){
            RBTree_Transplant(t, y, y->right);
            y->right = z->right;
            y->right->p = y;
        } else x->p = y;
        RBTree_Transplant(t, z, y);
        y->left = z->left;
        y->left->p = y;
        y->color = z->color;
    }
    if (y_original_color == 0) RBTree_Delete_Fixup(t, x);
}

void RBTree_printf_aux(RBTree *t, RBTreeNode *x){
    printf("Node(%d , ", x->key);
    if (x->left == t->sentinel){
        printf("V, ");
    } else {
        RBTree_printf_aux(t, x->left);
        printf(", ");
    }
    if (x->right == t->sentinel){
        printf("V");
    } else {
        RBTree_printf_aux(t, x->right);
        printf(", ");
    }
    printf(")");
}

void RBTree_printf(RBTree *t){
    if (t->root == t->sentinel) printf("Arbre vide\n");
    else RBTree_printf_aux(t, t->root);
    printf("\n");
}


int main(){
    
    RBTree *t = RBTree_create(10);
    RBTree_printf(t);

    for(int i = 0; i < 5; i++){
        RBTree_Insert(t, RBTree_create_node(i));
        RBTree_printf(t);
    }
    
    RBTree_Delete(t, 1);
    RBTree_printf(t);

    return 0;
}
   