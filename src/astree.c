
#include <stdlib.h>

#include "maug.h"

#define ASTREE_C
#include "astree.h"

struct ASTREE_NODE* astree_node_find_free() {
   uint16_t i = 0;
   struct ASTREE_NODE* new_nodes = NULL;

   for( i = 0 ; g_astree_nodes_sz > i ; i++ ) {
      if( g_astree_nodes[i].active ) {
         return &(g_astree_nodes[i]);
      }
   }

   /* Could not find free node! */
   assert( g_astree_nodes_sz * 2 > g_astree_nodes_sz );
   new_nodes = realloc( g_astree_nodes, g_astree_nodes_sz * 2 );
   assert( NULL != new_nodes );
   g_astree_nodes = new_nodes;

   return astree_node_find_free();
}

struct ASTREE_NODE* astree_node_add_child( struct ASTREE_NODE* parent ) {
   struct ASTREE_NODE* iter = NULL;

   if( NULL == parent->first_child ) {
      parent->first_child = astree_node_find_free();
      assert( NULL != parent->first_child );
      return parent->first_child;
   } else {
      for(
         iter = parent->first_child ;
         NULL != iter->next_sibling ;
         iter = iter->next_sibling
      ) {}

      iter->next_sibling = astree_node_find_free();
      assert( NULL != iter->next_sibling );
      return iter->next_sibling;
   }

}

