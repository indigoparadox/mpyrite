
#include <stdlib.h>

#include "maug.h"

#define ASTREE_C
#include "astree.h"

int16_t astree_node_find_free() {
   uint16_t i = 0;
   struct ASTREE_NODE* new_nodes = NULL;

   for( i = 0 ; g_astree_nodes_sz > i ; i++ ) {
      if( !(g_astree_nodes[i].active) ) {
         return i;
      }
   }

   /* Could not find free node! */
   assert( g_astree_nodes_sz * 2 > g_astree_nodes_sz );
   new_nodes = realloc( g_astree_nodes,
      (g_astree_nodes_sz * 2) * sizeof( struct ASTREE_NODE ) );
   assert( NULL != new_nodes );
   g_astree_nodes = new_nodes;
   for( i = g_astree_nodes_sz ; g_astree_nodes_sz * 2 > i ; i++ ) {
      /* Make sure new nodes are inactive. */
      memset( &(g_astree_nodes[i]), '\0', sizeof( struct ASTREE_NODE ) );
   }
   g_astree_nodes_sz *= 2;

   /* Now that there are more nodes, try again! */
   return astree_node_find_free();
}

void astree_node_intialize( int16_t node_idx, int16_t parent_idx ) {
   g_astree_nodes[node_idx].parent = parent_idx;
   g_astree_nodes[node_idx].active = 1;
   g_astree_nodes[node_idx].first_child = -1;
   g_astree_nodes[node_idx].next_sibling = -1;
}

int16_t astree_node_insert_child_parent( int16_t parent_idx ) {
   int16_t node_idx_out = -1,
      prev_child_idx = -1;

   /* Grab the current child list. */
   prev_child_idx = g_astree_nodes[parent_idx].first_child;
   g_astree_nodes[parent_idx].first_child = -1;

   /* Replace with the new child and tack the list on to that. */
   node_idx_out = astree_node_add_child( parent_idx );
   if( 0 > node_idx_out ) {
      /* Problem! Put it back! */
      g_astree_nodes[parent_idx].first_child = prev_child_idx;
      goto cleanup;
   }

   g_astree_nodes[parent_idx].first_child = node_idx_out;
   g_astree_nodes[node_idx_out].first_child = prev_child_idx;

cleanup:

   return node_idx_out;
}

int16_t astree_node_add_child( int16_t parent_idx ) {
   int16_t iter = -1,
      node_out = -1;

   /* Grab a free node to append. */
   node_out = astree_node_find_free();
   if( 0 > node_out ) {
      goto cleanup;
   }

   /* Figure out where to append it. */
   if( 0 > g_astree_nodes[parent_idx].first_child ) {
      /* Create the first child. */
      g_astree_nodes[parent_idx].first_child = node_out;
   } else {
      for(
         iter = g_astree_nodes[parent_idx].first_child ;
         0 <= g_astree_nodes[iter].next_sibling ;
         iter = g_astree_nodes[iter].next_sibling
      ) {}

      /* Create the new sibling of the last child. */
      g_astree_nodes[iter].next_sibling = node_out;
   }

   astree_node_intialize( node_out, parent_idx );

cleanup:

   return node_out;
}

