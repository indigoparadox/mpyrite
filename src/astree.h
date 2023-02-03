
#ifndef ASTREE_H
#define ASTREE_H

#define ASTREE_NODE_VALUE_SZ_MAX 255

#define ASTREE_NODE_TYPE_NONE       0
#define ASTREE_NODE_TYPE_SEQUENCE   1
#define ASTREE_NODE_TYPE_FUNC_DEF   2
#define ASTREE_NODE_TYPE_FUNC_PARM  3
#define ASTREE_NODE_TYPE_IF         4
#define ASTREE_NODE_TYPE_COND       5
#define ASTREE_NODE_TYPE_LITERAL    6
#define ASTREE_NODE_TYPE_VARIABLE   7
#define ASTREE_NODE_TYPE_FUNC_CALL  8

#define ASTREE_VALUE_TYPE_NONE      0
#define ASTREE_VALUE_TYPE_INT       1
#define ASTREE_VALUE_TYPE_FLOAT     2
#define ASTREE_VALUE_TYPE_STRING    3
#define ASTREE_VALUE_TYPE_GT        4

union ASTREE_NODE_VALUE {
   uint32_t u;
   int32_t i;
   double f;
   char s[ASTREE_NODE_VALUE_SZ_MAX + 1];
};

struct ASTREE_NODE {
   uint8_t active;
   uint8_t type;
   union ASTREE_NODE_VALUE value;
   uint8_t value_type;
   int16_t parent;
   int16_t first_child;
   int16_t next_sibling;
};

#define astree_set_node_type( node_idx, type_in ) \
   g_astree_nodes[node_idx].type = type_in;

#define astree_set_node_value_type( node_idx, type_in ) \
   g_astree_nodes[node_idx].value_type = type_in;

int16_t astree_node_find_free();
void astree_node_intialize( int16_t node_idx, int16_t parent_idx );
int16_t astree_node_insert_child_parent( int16_t parent_idx );
int16_t astree_node_add_child( int16_t parent_idx );

#ifdef ASTREE_C

struct ASTREE_NODE* g_astree_nodes = NULL;
int16_t g_astree_nodes_sz = 0;

#else

extern struct ASTREE_NODE* g_astree_nodes;
extern int16_t g_astree_nodes_sz;

#endif /* ASTREE_C */

#endif /* !ASTREE_H */

