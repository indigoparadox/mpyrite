
#include <stdlib.h>

#include "maug.h"

#define ASTREE_C
#include "astree.h"

int16_t astree_init( struct ASTREE* tree ) {
   int16_t retval = 0;

   memset( tree, '\0', sizeof( struct ASTREE ) );

   tree->nodes_sz = 1;
   tree->nodes = calloc( tree->nodes_sz, sizeof( struct ASTREE_NODE ) );
   if( NULL == tree->nodes ) {
      error_printf( "could not allocate syntax tree!" );
      retval = -1;
      goto cleanup;
   }

   astree_node_initialize( tree, 0, -1 );
   astree_node( tree, 0 )->type = ASTREE_NODE_TYPE_SEQUENCE;

cleanup:
   return retval;
}

void astree_free( struct ASTREE* tree ) {
   free( tree->nodes );
   tree->nodes_sz = 0;
}

int16_t astree_node_find_free( struct ASTREE* tree ) {
   uint16_t i = 0;
   struct ASTREE_NODE* new_nodes = NULL;

   for( i = 0 ; tree->nodes_sz > i ; i++ ) {
      if( !(tree->nodes[i].active) ) {
         return i;
      }
   }

   /* Could not find free node! */
   assert( tree->nodes_sz * 2 > tree->nodes_sz );
   new_nodes = realloc( tree->nodes,
      (tree->nodes_sz * 2) * sizeof( struct ASTREE_NODE ) );
   assert( NULL != new_nodes );
   tree->nodes = new_nodes;
   for( i = tree->nodes_sz ; tree->nodes_sz * 2 > i ; i++ ) {
      /* Make sure new nodes are inactive. */
      memset( &(tree->nodes[i]), '\0', sizeof( struct ASTREE_NODE ) );
   }
   tree->nodes_sz *= 2;

   /* Now that there are more nodes, try again! */
   return astree_node_find_free( tree );
}

void astree_node_initialize(
   struct ASTREE* tree, int16_t node_idx, int16_t parent_idx
) {
   assert( parent_idx != node_idx );
   tree->nodes[node_idx].parent = parent_idx;
   tree->nodes[node_idx].active = 1;
   tree->nodes[node_idx].first_child = -1;
   tree->nodes[node_idx].next_sibling = -1;
   tree->nodes[node_idx].prev_sibling = -1;
}

int16_t astree_node_insert_as_parent( struct ASTREE* tree, int16_t node_idx ) {
   int16_t node_idx_out = -1;
   struct ASTREE_NODE* node_old = NULL,
      * node_new = NULL,
      * node_prev_sibling = NULL,
      * node_next_sibling = NULL,
      * node_parent = NULL;

   node_old = astree_node( tree, node_idx );
   assert( NULL != node_old );

   /* Grab a free node to insert. */
   node_idx_out = astree_node_find_free( tree );
   if( 0 > node_idx_out ) {
      goto cleanup;
   }

   debug_printf( 1, "inserting %d above %d...", node_idx_out, node_idx );

   astree_node_initialize( tree, node_idx_out, node_old->parent );
   node_new = astree_node( tree, node_idx_out );

   node_new->prev_sibling = node_old->prev_sibling;
   node_old->prev_sibling = -1;
   node_new->next_sibling = node_old->next_sibling;
   node_old->next_sibling = -1;
   node_old->parent = node_idx_out;
   node_new->first_child = node_idx;

   node_parent = astree_node( tree, node_new->parent );
   assert( NULL != node_parent );
   if( node_parent->first_child == node_idx ) {
      node_parent->first_child = node_idx_out;
   }

   assert( node_new->parent != node_idx_out );
   assert( node_old->parent != node_idx );

   node_prev_sibling = astree_node( tree, node_new->prev_sibling );
   if( NULL != node_prev_sibling ) {
      node_prev_sibling->next_sibling = node_idx_out;
   }

   node_next_sibling = astree_node( tree, node_new->next_sibling );
   if( NULL != node_next_sibling ) {
      node_next_sibling->prev_sibling = node_idx_out;
   }

cleanup:

   return node_idx_out;
}

int16_t astree_node_add_child(
   struct ASTREE* tree, int16_t parent_idx, uint8_t type,
   uint8_t indent, const void* value, uint16_t value_sz
) {
   int16_t iter_idx = -1,
      node_out_idx = -1;
   struct ASTREE_NODE* node_out = NULL;

   /* Grab a free node to append. */
   node_out_idx = astree_node_find_free( tree );
   if( 0 > node_out_idx ) {
      goto cleanup;
   }

   /* Figure out where to append it. */
   if( 0 > tree->nodes[parent_idx].first_child ) {
      /* Create the first child. */
      tree->nodes[parent_idx].first_child = node_out_idx;
   } else {
      for(
         iter_idx = tree->nodes[parent_idx].first_child ;
         0 <= tree->nodes[iter_idx].next_sibling ;
         iter_idx = tree->nodes[iter_idx].next_sibling
      ) {}

      /* Create the new sibling of the last child. */
      tree->nodes[iter_idx].next_sibling = node_out_idx;
      assert( -1 != iter_idx );
   }

   astree_node_initialize( tree, node_out_idx, parent_idx );
   node_out = astree_node( tree, node_out_idx );
   assert( NULL != node_out );
   node_out->type = type;
   node_out->indent = indent;
   if( NULL != value && value_sz < sizeof( union ASTREE_NODE_VALUE ) ) {
      memcpy( &(node_out->value), value, value_sz );
   }

   tree->nodes[node_out_idx].prev_sibling = iter_idx;

cleanup:

   return node_out_idx;
}

void astree_dump( struct ASTREE* tree, int16_t node_idx, int16_t depth ) {

   if( 0 == depth ) {
      debug_printf( ASTREE_DUMP_DEBUG_LVL, "--- TREE MAP ---" );
   }

   switch( tree->nodes[node_idx].type ) {
   case ASTREE_NODE_TYPE_SEQUENCE:
      debug_printf( ASTREE_DUMP_DEBUG_LVL,
         "\t%d: (idx: %d) sequence node", depth, node_idx );
      break;

   case ASTREE_NODE_TYPE_SEQ_TERM:
      debug_printf( ASTREE_DUMP_DEBUG_LVL,
         "\t%d: (idx: %d) sequence term node: %d", depth, node_idx,
         tree->nodes[node_idx].value.i );
      break;

   case ASTREE_NODE_TYPE_FUNC_DEF:
      debug_printf( ASTREE_DUMP_DEBUG_LVL,
         "\t%d: (idx: %d) function def node: %s", depth, node_idx,
         tree->nodes[node_idx].value.s );
      break;

   case ASTREE_NODE_TYPE_FUNC_CALL:
      debug_printf( ASTREE_DUMP_DEBUG_LVL, 
         "\t%d: (idx: %d) function call node: %s", depth, node_idx,
         tree->nodes[node_idx].value.s );
      break;

   case ASTREE_NODE_TYPE_IF:
      debug_printf( ASTREE_DUMP_DEBUG_LVL, 
         "\t%d: (idx: %d) if node", depth, node_idx );
      break;

   case ASTREE_NODE_TYPE_WHILE:
      debug_printf( ASTREE_DUMP_DEBUG_LVL, 
         "\t%d: (idx: %d) while node", depth, node_idx );
      break;

   case ASTREE_NODE_TYPE_LITERAL:
      switch( tree->nodes[node_idx].value_type ) {
      case ASTREE_VALUE_TYPE_NONE:
         debug_printf( ASTREE_DUMP_DEBUG_LVL, 
            "\t%d: (idx: %d) literal node: none", depth, node_idx );
         break;

      case ASTREE_VALUE_TYPE_INT:
         debug_printf( ASTREE_DUMP_DEBUG_LVL, 
            "\t%d: (idx: %d) literal node: %d", depth, node_idx,
            tree->nodes[node_idx].value.i );
         break;

      case ASTREE_VALUE_TYPE_FLOAT:
         debug_printf( ASTREE_DUMP_DEBUG_LVL,
            "\t%d: (idx: %d) literal node: %f", depth, node_idx,
            tree->nodes[node_idx].value.f );
         break;

      case ASTREE_VALUE_TYPE_STRING:
         debug_printf( ASTREE_DUMP_DEBUG_LVL, "\t%d: (idx: %d) literal node: \"%s\"",
            depth, node_idx, tree->nodes[node_idx].value.s );
         break;
      }
      break;

   case ASTREE_NODE_TYPE_COND:
      switch( tree->nodes[node_idx].value_type ) {
      case ASTREE_VALUE_TYPE_GT:
         debug_printf( ASTREE_DUMP_DEBUG_LVL,
            "\t%d: (idx: %d) cond node: greater than", depth, node_idx );
         break;

      case ASTREE_VALUE_TYPE_LT:
         debug_printf( ASTREE_DUMP_DEBUG_LVL,
            "\t%d: (idx: %d) cond node: less than", depth, node_idx );
         break;

      case ASTREE_VALUE_TYPE_EQ:
         debug_printf( ASTREE_DUMP_DEBUG_LVL,
            "\t%d: (idx: %d) cond node: equal to", depth, node_idx );
         break;
      }
      break;

   case ASTREE_NODE_TYPE_OP:
      switch( tree->nodes[node_idx].value_type ) {
      case ASTREE_VALUE_TYPE_ADD:
         debug_printf( ASTREE_DUMP_DEBUG_LVL,
            "\t%d: (idx: %d) op node: add", depth, node_idx );
         break;

      case ASTREE_VALUE_TYPE_SUBTRACT:
         debug_printf( ASTREE_DUMP_DEBUG_LVL,
            "\t%d: (idx: %d) op node: subtract", depth, node_idx );
         break;

      case ASTREE_VALUE_TYPE_MULTIPLY:
         debug_printf( ASTREE_DUMP_DEBUG_LVL,
            "\t%d: (idx: %d) op node: multiply", depth, node_idx );
         break;

      case ASTREE_VALUE_TYPE_DIVIDE:
         debug_printf( ASTREE_DUMP_DEBUG_LVL,
            "\t%d: (idx: %d) op node: divide", depth, node_idx );
         break;

      case ASTREE_VALUE_TYPE_MODULO:
         debug_printf( ASTREE_DUMP_DEBUG_LVL,
            "\t%d: (idx: %d) op node: modulo", depth, node_idx );
         break;
      }
      break;

   case ASTREE_NODE_TYPE_VARIABLE:
      debug_printf( ASTREE_DUMP_DEBUG_LVL,
         "\t%d: (idx: %d) variable node: %s", depth, node_idx,
         tree->nodes[node_idx].value.s );
      break;

   case ASTREE_NODE_TYPE_ASSIGN:
      debug_printf( ASTREE_DUMP_DEBUG_LVL,
         "\t%d: (idx: %d) assign node", depth, node_idx );
      break;

   case ASTREE_NODE_TYPE_FUNC_DEF_PARM:
      debug_printf( ASTREE_DUMP_DEBUG_LVL,
         "\t%d: (idx: %d) function def parm node: %s (< %d, > %d)",
         depth, node_idx, tree->nodes[node_idx].value.s,
         astree_node( tree, node_idx )->prev_sibling, 
         astree_node( tree, node_idx )->next_sibling );
      break;

   default:
      debug_printf( ASTREE_DUMP_DEBUG_LVL,
         "\t%d: (idx: %d) unknown node", depth, node_idx );
      break;
   }

   debug_printf( 0, "first_child: %d, next_sibling: %d, prev_sibling: %d",
      tree->nodes[node_idx].first_child,
      tree->nodes[node_idx].next_sibling,
      tree->nodes[node_idx].prev_sibling );

   if( 0 <= tree->nodes[node_idx].first_child ) {
      astree_dump( tree, tree->nodes[node_idx].first_child, depth + 1 );
   }

   if( 0 <= tree->nodes[node_idx].next_sibling ) {
      astree_dump( tree, tree->nodes[node_idx].next_sibling, depth );
   }

   if( 0 == depth ) {
      debug_printf( ASTREE_DUMP_DEBUG_LVL, "--- END TREE MAP ---" );
   }
}

