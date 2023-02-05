
#include <stdlib.h>

#include <maug.h>

#include "interp.h"

int16_t interp_init( struct INTERP* interp, struct ASTREE* tree ) {
   int16_t retval = 0;

   memset( interp, '\0', sizeof( struct INTERP ) );
   interp->tree = tree;
   interp->func_sz_max = 10;
   interp->func_names = calloc( interp->func_sz_max, sizeof( char* ) );
   if( NULL == interp->func_names ) {
      retval = -1;
      goto cleanup;
   }
   interp->func_idxs = calloc( interp->func_sz_max, sizeof( int16_t ) );
   if( NULL == interp->func_idxs ) {
      retval = -1;
      goto cleanup;
   }
   interp->func_cbs = calloc( interp->func_sz_max, sizeof( interp_func_cb ) );
   if( NULL == interp->func_cbs ) {
      retval = -1;
      goto cleanup;
   }

cleanup:

   if( NULL != interp->func_names && retval ) {
      free( interp->func_names );
      interp->func_names = NULL;
   }

   if( NULL != interp->func_idxs && retval ) {
      free( interp->func_idxs );
      interp->func_idxs = NULL;
   }

   if( NULL != interp->func_cbs && retval ) {
      free( interp->func_cbs );
      interp->func_cbs = NULL;
   }

   return 0;
}

void interp_free( struct INTERP* interp ) {
   if( NULL != interp->func_names ) {
      free( interp->func_names );
      interp->func_names = NULL;
   }

   if( NULL != interp->func_idxs ) {
      free( interp->func_idxs );
      interp->func_idxs = NULL;
   }

   if( NULL != interp->func_cbs ) {
      free( interp->func_cbs );
      interp->func_cbs = NULL;
   }
}

int16_t interp_tick( struct INTERP* interp ) {

   assert( NULL != interp );
   assert( NULL != interp->tree );
   assert( NULL != interp->tree->nodes );

   debug_printf( 1, "executing %d...", interp->pc );

   if( 0 > interp->pc ) {
      error_printf( "encountered dead end!" );
      return -1;
   }

   switch( astree_node( interp->tree, interp->pc )->type ) {
   case ASTREE_NODE_TYPE_IF:
      break;

   default:
      /* By default, descend into node. */
      interp->pc = astree_node( interp->tree, interp->pc )->first_child;
      break;
   }

   return 0;
}

