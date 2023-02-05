
#include <stdlib.h>

#include <maug.h>

#include "interp.h"

int16_t interp_init( struct INTERP* interp, struct ASTREE* tree ) {
   int16_t retval = 0;

   memset( interp, '\0', sizeof( struct INTERP ) );
   interp->tree = tree;
   interp->funcs_sz_max = 10;
   interp->funcs =
      calloc( interp->funcs_sz_max, sizeof( struct INTERP_FUNC ) );
   if( NULL == interp->funcs ) {
      retval = -1;
      goto cleanup;
   }
   interp->vars = calloc( interp->vars_sz_max, sizeof( struct INTERP_VAR ) );
   if( NULL == interp->vars ) {
      retval = -1;
      goto cleanup;
   }

cleanup:

   if( NULL != interp->funcs && retval ) {
      free( interp->funcs );
      interp->funcs = NULL;
   }

   if( NULL != interp->vars && retval ) {
      free( interp->vars );
      interp->vars = NULL;
   }

   return 0;
}

void interp_free( struct INTERP* interp ) {
   if( NULL != interp->funcs ) {
      free( interp->funcs );
      interp->funcs = NULL;
   }

   if( NULL != interp->vars ) {
      free( interp->vars );
      interp->vars = NULL;
   }
}

int16_t interp_dbl_funcs( struct INTERP* interp ) {
   struct INTERP_FUNC* new_funcs = NULL;

   debug_printf( 1, "function table limit exceeded; expanding to %d...",
      interp->funcs_sz_max * 2 );
   
   assert(
      interp->funcs_sz_max * 2 * sizeof( struct INTERP_FUNC ) >
      interp->funcs_sz_max * sizeof( struct INTERP_FUNC ) );
   new_funcs = realloc(
      interp->funcs,
      interp->funcs_sz_max * 2 * sizeof( struct INTERP_FUNC ) );
   assert( NULL != new_funcs );
   interp->funcs = new_funcs;
   
   interp->funcs_sz_max *= 2;

   /* TODO: Error handling beyond asserts() above. */
   return 0;
}

int16_t interp_set_func_pc(
   struct INTERP* interp, const char* func_name, int16_t func_pc
) {
   int16_t i = 0;

   for( i = 0 ; interp->funcs_sz > i ; i++ ) {
      if( 0 == strcmp( func_name, interp->funcs[i].name ) ) {
         debug_printf( 1, "function \"%s\" already exists, updating to %d...",
            func_name, func_pc );
         interp->funcs[i].type = INTERP_FUNC_PC;
         interp->funcs[i].exe.pc = func_pc;
         return 0;
      }
   }

   if( interp->funcs_sz + 1 >= interp->funcs_sz_max ) {
      interp_dbl_funcs( interp );
   }

   /* Add the new function. */
   debug_printf( 1, "adding new function \"%s\", pc = %d", func_name, func_pc );
   strncpy( interp->funcs[interp->funcs_sz].name, func_name,
      INTERP_FUNC_NAME_SZ_MAX );
   interp->funcs[interp->funcs_sz].type = INTERP_FUNC_PC;
   interp->funcs[interp->funcs_sz].exe.pc = func_pc;
   interp->funcs_sz++;

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

   interp_set_func_pc( interp, def->value.s, def_seq );

   return 0;
}

int16_t interp_set_func_cb(
   struct INTERP* interp, const char* func_name, interp_func_cb cb
) {
   /* TODO: Some kind of wrapper to unstack vars. */
   return 0;
}

int16_t interp_set_var_int(
   struct INTERP* interp, const char* var_name, int16_t value
) {

   return 0;
}

int16_t interp_set_var_str(
   struct INTERP* interp, const char* var_name, const char* value
) {
   int16_t i = 0;

   for( i = 0 ; interp->vars_sz ; i++ ) {

   }

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
         interp_set_var_int( interp, left->value.s, atoi( right->value.s ) );
      } else if( ASTREE_VALUE_TYPE_STRING == right->value_type ) {
         interp_set_var_str( interp, left->value.s, right->value.s );
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

