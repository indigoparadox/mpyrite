
#ifndef ASTREE_H
#define ASTREE_H

#define ASTREE_NODE_VALUE_SZ_MAX 255

#define ASTREE_NODE_TYPE_NONE          0
#define ASTREE_NODE_TYPE_SEQUENCE      1
#define ASTREE_NODE_TYPE_FUNC_DEF      2
#define ASTREE_NODE_TYPE_IF            4
#define ASTREE_NODE_TYPE_COND          5
#define ASTREE_NODE_TYPE_LITERAL       6
#define ASTREE_NODE_TYPE_VARIABLE      7
#define ASTREE_NODE_TYPE_FUNC_CALL     8
#define ASTREE_NODE_TYPE_OP            9
#define ASTREE_NODE_TYPE_ASSIGN        10
#define ASTREE_NODE_TYPE_FUNC_DEF_PARM 11

#define ASTREE_VALUE_TYPE_NONE      0
#define ASTREE_VALUE_TYPE_INT       1
#define ASTREE_VALUE_TYPE_FLOAT     2
#define ASTREE_VALUE_TYPE_STRING    3
#define ASTREE_VALUE_TYPE_GT        4
#define ASTREE_VALUE_TYPE_ADD       5
#define ASTREE_VALUE_TYPE_EQ        6

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
   int16_t prev_sibling;
};

struct ASTREE {
   struct ASTREE_NODE* nodes;
   int16_t nodes_sz;
};

#define astree_node( tree, node_idx ) \
   (0 <= node_idx ? &((tree)->nodes[node_idx]) : NULL)

int16_t astree_init( struct ASTREE* tree );
void astree_free( struct ASTREE* tree );
int16_t astree_node_find_free( struct ASTREE* tree );
void astree_node_initialize(
   struct ASTREE* tree, int16_t node_idx, int16_t parent_idx );
int16_t astree_node_insert_child_parent( 
   struct ASTREE* tree, int16_t parent_idx );
int16_t astree_node_add_child( struct ASTREE* tree, int16_t parent_idx );
void astree_dump( struct ASTREE* tree, int16_t node_idx, int16_t depth );

#endif /* !ASTREE_H */

