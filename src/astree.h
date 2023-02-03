
#ifndef ASTREE_H
#define ASTREE_H

#define ASTREE_NODE_TYPE_NONE       0
#define ASTREE_NODE_TYPE_SEQUENCE   1

struct ASTREE_NODE {
   uint8_t active;
   uint8_t type;
   struct ASTREE_NODE* parent;
   struct ASTREE_NODE* first_child;
   struct ASTREE_NODE* next_sibling;
};

#ifdef ASTREE_C

struct ASTREE_NODE* g_astree_nodes = NULL;
uint16_t g_astree_nodes_sz = 0;

#else

extern struct ASTREE_NODE* g_astree_nodes;
extern uint16_t g_astree_nodes_sz;

#endif /* ASTREE_C */

#endif /* !ASTREE_H */

