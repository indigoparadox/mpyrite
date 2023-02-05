
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
   tree->nodes[node_idx].parent = parent_idx;
   tree->nodes[node_idx].active = 1;
   tree->nodes[node_idx].first_child = -1;
   tree->nodes[node_idx].next_sibling = -1;
}

int16_t astree_node_insert_child_parent(
   struct ASTREE* tree, int16_t parent_idx
) {
   int16_t node_idx_out = -1,
      prev_child_idx = -1;

   /* Grab the current child list. */
   prev_child_idx = tree->nodes[parent_idx].first_child;
   tree->nodes[parent_idx].first_child = -1;

   /* Replace with the new child and tack the list on to that. */
   node_idx_out = astree_node_add_child( tree, parent_idx );
   if( 0 > node_idx_out ) {
      /* Problem! Put it back! */
      tree->nodes[parent_idx].first_child = prev_child_idx;
      goto cleanup;
   }

   debug_printf( 1, "inserting %d between %d and %d...",
      node_idx_out, parent_idx, prev_child_idx );

   tree->nodes[parent_idx].first_child = node_idx_out;
   tree->nodes[node_idx_out].first_child = prev_child_idx;

cleanup:

   return node_idx_out;
}

int16_t astree_node_add_child( struct ASTREE* tree, int16_t parent_idx ) {
   int16_t iter = -1,
      node_out = -1;

   /* Grab a free node to append. */
   node_out = astree_node_find_free( tree );
   if( 0 > node_out ) {
      goto cleanup;
   }

   /* Figure out where to append it. */
   if( 0 > tree->nodes[parent_idx].first_child ) {
      /* Create the first child. */
      tree->nodes[parent_idx].first_child = node_out;
   } else {
      for(
         iter = tree->nodes[parent_idx].first_child ;
         0 <= tree->nodes[iter].next_sibling ;
         iter = tree->nodes[iter].next_sibling
      ) {}

      /* Create the new sibling of the last child. */
      tree->nodes[iter].next_sibling = node_out;
   }

   astree_node_initialize( tree, node_out, parent_idx );

cleanup:

   return node_out;
}

