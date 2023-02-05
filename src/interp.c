
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
   int16_t i = 0;

   if( NULL != interp->func_names ) {
      for( i = 0 ; i < interp->func_sz ; i++ ) {
         free( interp->func_names[i] );
      }
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

int16_t interp_dbl_funcs( struct INTERP* interp ) {
   int16_t* new_func_idxs = NULL;
   char** new_func_names = NULL;
   interp_func_cb* new_func_cbs = NULL;

   debug_printf( 1, "function table limit exceeded; expanding to %d...",
      interp->func_sz_max * 2 );
   
   assert(
      interp->func_sz_max * 2 * sizeof( char** ) >
      interp->func_sz_max * sizeof( char** ) );
   new_func_names = realloc(
      interp->func_names, interp->func_sz_max * 2 * sizeof( char** ) );
   assert( NULL != new_func_names );
   interp->func_names = new_func_names;
   
   assert(
      interp->func_sz_max * 2 * sizeof( int16_t ) >
      interp->func_sz_max * sizeof( int16_t ) );
   new_func_idxs = realloc(
      interp->func_idxs, interp->func_sz_max * 2 * sizeof( int16_t ) );
   assert( NULL != new_func_idxs );
   interp->func_idxs = new_func_idxs;
   
   assert(
      interp->func_sz_max * 2 * sizeof( interp_func_cb ) >
      interp->func_sz_max * sizeof( interp_func_cb ) );
   new_func_cbs = realloc(
      interp->func_cbs, interp->func_sz_max * 2 * sizeof( interp_func_cb ) );
   assert( NULL != new_func_cbs );
   interp->func_cbs = new_func_cbs;

   interp->func_sz_max *= 2;

   /* TODO: Error handling beyond asserts() above. */
   return 0;
}

int16_t interp_set_func_pc(
   struct INTERP* interp, const char* func_name, int16_t func_name_sz,
   int16_t func_pc
) {
   int16_t i = 0;

   for( i = 0 ; interp->func_sz > i ; i++ ) {
      if( 0 == strcmp( func_name, interp->func_names[i] ) ) {
         debug_printf( 1, "function \"%s\" already exists, updating to %d...",
            func_name, func_pc );
         interp->func_idxs[i] = func_pc;
         return 0;
      }
   }

   if( interp->func_sz + 1 >= interp->func_sz_max ) {
      interp_dbl_funcs( interp );
   }

   /* Add the new function. */
   debug_printf( 1, "adding new function \"%s\", pc = %d", func_name, func_pc );
   interp->func_names[interp->func_sz] = calloc( func_name_sz + 1, 1 );
   assert( NULL != interp->func_names[interp->func_sz] );
   strncpy( interp->func_names[interp->func_sz], func_name, func_name_sz );
   interp->func_idxs[interp->func_sz] = func_pc;
   interp->func_sz++;

   return 0;
}

int16_t interp_set_func_def( struct INTERP* interp, struct ASTREE_NODE* def ) {
   int16_t def_seq = -1;
   struct ASTREE_NODE* seq = NULL;

   /* Find the function def's sequence node. */
   def_seq = def->first_child;
   seq = astree_node( interp->tree, def->first_child );
   while( NULL != seq && ASTREE_NODE_TYPE_SEQUENCE != seq->type ) {
      def_seq = seq->next_sibling;
      seq = astree_node( interp->tree, def_seq );
   }

   /* TODO: Error handling. */
   assert( NULL != seq );

   interp_set_func_pc( interp, def->value.s, strlen( def->value.s ), def_seq );

   return 0;
}

int16_t interp_set_func_cb(
   struct INTERP* interp, const char* func_name, int16_t func_name_sz,
   interp_func_cb cb
) {
   /* TODO: Some kind of wrapper to unstack vars. */
   return 0;
}

int16_t interp_set_var_int(
   struct INTERP* interp, const char* var_name, int16_t var_name_sz,
   int16_t value
) {

   return 0;
}

int16_t interp_set_var_str(
   struct INTERP* interp, const char* var_name, int16_t var_name_sz,
   const char* value
) {
   return 0;
}

int16_t interp_tick( struct INTERP* interp ) {
   struct ASTREE_NODE* left = NULL;
   struct ASTREE_NODE* right = NULL;
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;

   assert( NULL != interp );
   assert( NULL != interp->tree );
   assert( NULL != interp->tree->nodes );

   debug_printf( 1, "executing %d...", interp->pc );

   if( 0 > interp->pc ) {
      error_printf( "encountered dead end!" );
      return -1;
   }

   iter = astree_node( interp->tree, interp->pc );
   assert( NULL != iter );

   switch( iter->type ) {
   case ASTREE_NODE_TYPE_IF:
      break;

   case ASTREE_NODE_TYPE_FUNC_DEF:
      interp_set_func_def( interp, iter );
      interp->pc = iter->next_sibling;
      break;

   case ASTREE_NODE_TYPE_ASSIGN:
      /* TODO: Nested assignment. */
      left = astree_node( interp->tree, iter->first_child );
      assert( NULL != left );
      right = astree_node( interp->tree, left->next_sibling );
      assert( NULL != right );
      if( ASTREE_VALUE_TYPE_INT == right->value_type ) {
         interp_set_var_int( interp, left->value.s, strlen( left->value.s ),
            atoi( right->value.s ) );
      } else if( ASTREE_VALUE_TYPE_STRING == right->value_type ) {
         interp_set_var_str( interp, left->value.s, strlen( left->value.s ),
            right->value.s );
      } else {
         retval = -1;
         goto cleanup;
      }
      interp->pc = iter->next_sibling;
      break;

   default:
      /* By default, descend into node. */
      interp->pc = iter->first_child;
      break;
   }

cleanup:

   if( retval ) {
      error_printf( "error on node: %d", interp->pc );
   }

   return retval;
}

