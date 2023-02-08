
#include <stdlib.h>

#include <maug.h>

#include "interp.h"

static int16_t interp_dbl_funcs( struct INTERP* interp ) {
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

static int16_t interp_dbl_vars( struct INTERP* interp ) {
   struct INTERP_VAR* new_vars = NULL;

   debug_printf( 1, "variable table limit exceeded; expanding to %d...",
      interp->vars_sz_max * 2 );
   
   assert(
      interp->vars_sz_max * 2 * sizeof( struct INTERP_VAR ) >
      interp->vars_sz_max * sizeof( struct INTERP_VAR ) );
   new_vars = realloc(
      interp->vars,
      interp->vars_sz_max * 2 * sizeof( struct INTERP_VAR ) );
   assert( NULL != new_vars );
   interp->vars = new_vars;
   
   interp->vars_sz_max *= 2;

   /* TODO: Error handling beyond asserts() above. */
   return 0;
}

static int16_t interp_dbl_stack( struct INTERP* interp ) {
   struct INTERP_STACK_ITEM* new_stack = NULL;

   debug_printf( 1, "variable table limit exceeded; expanding to %d...",
      interp->stack_sz_max * 2 );
   
   assert(
      interp->stack_sz_max * 2 * sizeof( struct INTERP_VAR ) >
      interp->stack_sz_max * sizeof( struct INTERP_VAR ) );
   new_stack = realloc(
      interp->stack,
      interp->stack_sz_max * 2 * sizeof( struct INTERP_VAR ) );
   assert( NULL != new_stack );
   interp->stack = new_stack;
   
   interp->stack_sz_max *= 2;

   /* TODO: Error handling beyond asserts() above. */
   return 0;
}

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
   interp->vars_sz_max = 10;
   interp->vars = calloc( interp->vars_sz_max, sizeof( struct INTERP_VAR ) );
   if( NULL == interp->vars ) {
      retval = -1;
      goto cleanup;
   }
   interp->stack_sz_max = 10;
   interp->stack = calloc(
      interp->stack_sz_max, sizeof( struct INTERP_STACK_ITEM ) );
   if( NULL == interp->stack ) {
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

   if( NULL != interp->stack && retval ) {
      free( interp->stack );
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

   if( NULL != interp->stack ) {
      free( interp->stack );
      interp->stack = NULL;
   }
}

int16_t interp_set_func(
   struct INTERP* interp, const char* func_name, void* func_pc_cb,
   uint8_t func_type
) {
   uint32_t i = 0;
   int16_t* func_pc = (int16_t*)(func_pc_cb);
   interp_func_cb func_cb = (interp_func_cb)(func_pc_cb);

   for( i = 0 ; interp->funcs_sz > i ; i++ ) {
      if( 0 == strcmp( func_name, interp->funcs[i].name ) ) {
         interp->funcs[i].type = func_type;
         switch( func_type ) {
         case INTERP_FUNC_PC:
            debug_printf( 1,
               "function \"%s\" already exists, updating to: %d...",
               func_name, *func_pc );
            interp->funcs[i].exe.pc = *func_pc;
            break;

         case INTERP_FUNC_CB:
            debug_printf( 1,
               "function \"%s\" already exists, updating to: %p...",
               func_name, *func_cb );
            interp->funcs[i].exe.cb = *func_cb;
            break;
         }
         return 0;
      }
   }

   if( interp->funcs_sz + 1 >= interp->funcs_sz_max ) {
      interp_dbl_funcs( interp );
   }

   /* Add the new function. */
   memset( interp->funcs[interp->funcs_sz].name, '\0',
      INTERP_FUNC_NAME_SZ_MAX );
   strncpy( interp->funcs[interp->funcs_sz].name, func_name,
      INTERP_FUNC_NAME_SZ_MAX );
   interp->funcs[interp->funcs_sz].type = func_type;
   switch( func_type ) {
   case INTERP_FUNC_PC:
      debug_printf( 1, "adding new function: \"%s\", pc = %d",
         func_name, *func_pc );
      interp->funcs[interp->funcs_sz].exe.pc = *func_pc;
      break;

   case INTERP_FUNC_CB:
      debug_printf( 1, "adding new function: \"%s\", cb = %p",
         func_name, *func_cb );
      interp->funcs[interp->funcs_sz].exe.cb = *func_cb;
      break;
   }
   interp->funcs_sz++;

   return 0;
}

#if 0
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
#endif

int16_t interp_set_var_int(
   struct INTERP* interp, const char* var_name, int16_t value
) {
   uint32_t i = 0;

   for( i = 0 ; interp->vars_sz > i ; i++ ) {
      if( 0 == strcmp( var_name, interp->vars[i].name ) ) {
         debug_printf( 1,
            "variable \"%s\" already exists, updating to %d...",
            var_name, value );
         interp->vars[i].type = ASTREE_VALUE_TYPE_INT;
         interp->vars[i].value.i = value;
         return 0;
      }
   }

   if( interp->vars_sz + 1 >= interp->vars_sz_max ) {
      interp_dbl_vars( interp );
   }

   /* Add the new variable. */
   debug_printf( 1, "adding new variable \"%s\", %d", var_name, value );
   memset( interp->vars[interp->vars_sz].name, '\0', INTERP_VAR_NAME_SZ_MAX );
   strncpy( interp->vars[interp->vars_sz].name, var_name,
      INTERP_FUNC_NAME_SZ_MAX );
   interp->vars[interp->vars_sz].type = ASTREE_VALUE_TYPE_INT;
   interp->vars[interp->vars_sz].value.i = value;
   interp->vars_sz++;

   return 0;
}

int16_t interp_set_var_str(
   struct INTERP* interp, const char* var_name, const char* value
) {
   uint32_t i = 0;

   for( i = 0 ; interp->vars_sz > i ; i++ ) {
      if( 0 == strcmp( var_name, interp->vars[i].name ) ) {
         debug_printf( 1,
            "variable \"%s\" already exists, updating to \"%s\"...",
            var_name, value );
         interp->vars[i].type = ASTREE_VALUE_TYPE_STRING;
         memset( interp->vars[i].value.s, '\0', ASTREE_NODE_VALUE_SZ_MAX );
         strncpy( interp->vars[i].value.s, value, ASTREE_NODE_VALUE_SZ_MAX );
         return 0;
      }
   }

   if( interp->vars_sz + 1 >= interp->vars_sz_max ) {
      interp_dbl_vars( interp );
   }

   /* Add the new variable. */
   debug_printf( 1, "adding new variable \"%s\", \"%s\"", var_name, value );
   memset( interp->vars[interp->vars_sz].name, '\0', INTERP_VAR_NAME_SZ_MAX );
   strncpy( interp->vars[interp->vars_sz].name, var_name,
      INTERP_FUNC_NAME_SZ_MAX );
   interp->vars[interp->vars_sz].type = ASTREE_VALUE_TYPE_STRING;
   memset( interp->vars[interp->vars_sz].value.s, '\0', 
      ASTREE_NODE_VALUE_SZ_MAX );
   strncpy( interp->vars[interp->vars_sz].value.s, value,
      ASTREE_NODE_VALUE_SZ_MAX );
   interp->vars_sz++;

   return 0;
}

struct INTERP_VAR* interp_get_var( struct INTERP* interp, const char* name ) {
   uint32_t i = 0;

   for( i = 0 ; interp->vars_sz > i ; i++ ) {
      if( 0 == strcmp( name, interp->vars[i].name ) ) {
         return &(interp->vars[i]);
      }
   }

   return NULL;
}

void interp_call_ret( struct INTERP* interp ) {
   struct INTERP_STACK_ITEM* ret = NULL;

   debug_printf( 1, "attempting to return..." );

   ret = interp_stack_pop( interp );
   if( NULL != ret ) {
      assert( ASTREE_VALUE_TYPE_INT == ret->type );
      interp_set_pc( interp, ret->value.i );
   }
}

int16_t interp_call_func( struct INTERP* interp, const char* name ) {
   uint32_t i = 0;
   struct INTERP_FUNC* func = NULL;
   int16_t retval = 0;

   debug_printf( 1, "calling function: %s()", name );

   for( i = 0 ; interp->funcs_sz > i ; i++ ) {
      if( 0 == strcmp( name, interp->funcs[i].name ) ) {
          func = &(interp->funcs[i]);
      }
   }

   if( NULL == func ) {
      error_printf( "could not find function: %s()", name );
      retval = -1;
   }

   switch( func->type ) {
   case INTERP_FUNC_CB:
      assert( NULL != func->exe.cb );
      func->exe.cb( interp );

      /* Return immediately since callback functions are all in one tick. */
      interp_call_ret( interp );
      break;

   case INTERP_FUNC_PC:
      interp_set_pc( interp, func->exe.pc );
      
      /* Call the ret at a dead end if possible. */
      break;
   }

   return retval;
}

int16_t interp_stack_push_str( struct INTERP* interp, const char* value ) {
   if( interp->stack_sz + 1 >= interp->stack_sz_max ) {
      interp_dbl_stack( interp );
   }

   /* Add the new variable. */
   debug_printf( 1, "pushing to stack: \"%s\"", value );
   interp->stack[interp->stack_sz].type = ASTREE_VALUE_TYPE_STRING;
   memset( interp->stack[interp->stack_sz].value.s, '\0',
      ASTREE_NODE_VALUE_SZ_MAX );
   strncpy( interp->stack[interp->stack_sz].value.s, value,
      ASTREE_NODE_VALUE_SZ_MAX );
   interp->stack_sz++;

   return 0;
}

int16_t interp_stack_push_int( struct INTERP* interp, int16_t value ) {
   if( interp->stack_sz + 1 >= interp->stack_sz_max ) {
      interp_dbl_stack( interp );
   }

   /* Add the new variable. */
   debug_printf( 1, "pushing to stack: %d", value );
   interp->stack[interp->stack_sz].type = ASTREE_VALUE_TYPE_INT;
   interp->stack[interp->stack_sz].value.i = value;
   interp->stack_sz++;

   return 0;
}

struct INTERP_STACK_ITEM* interp_stack_pop( struct INTERP* interp ) {

   /* Shrink the stack so the sz == the idx of the old last item. */
   assert( 0 < interp->stack_sz );
   interp->stack_sz--;

#ifdef DEBUG
   /* Helpful trace messages. */
   switch( interp->stack[interp->stack_sz].type ) {
   case ASTREE_VALUE_TYPE_INT:
      debug_printf( 1, "popping from stack: %d", 
         interp->stack[interp->stack_sz].value.i );
      break;

   case ASTREE_VALUE_TYPE_STRING:
      debug_printf( 1, "popping from stack: %s", 
         interp->stack[interp->stack_sz].value.s );
      break;
   }
#endif /* DEBUG */

   return &(interp->stack[interp->stack_sz]);
}

int16_t interp_tick( struct INTERP* interp ) {
   struct ASTREE_NODE* left = NULL;
   struct ASTREE_NODE* right = NULL;
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   struct INTERP_VAR* var = NULL;
   struct INTERP_STACK_ITEM* item1 = NULL,
      * item2 = NULL;

   assert( NULL != interp );
   assert( NULL != interp->tree );
   assert( NULL != interp->tree->nodes );

   debug_printf( 1, "executing %d (prev: %d)...", interp->pc, interp->prev_pc );

   if( 0 > interp->pc ) {
      /* Try to return from a function call. */
      /* TODO: Note that pushing return value on the stack should go before
       *       ret PC... maybe make return stack its own thing? */
      interp_call_ret( interp );
      if( 0 > interp->pc ) {
         /* Return failed! */
         error_printf( "encountered dead end!" );
      }
      return -1;
   }

   iter = astree_node( interp->tree, interp->pc );
   assert( NULL != iter );

   switch( iter->type ) {
   case ASTREE_NODE_TYPE_WHILE:
   case ASTREE_NODE_TYPE_IF:
      if( interp->prev_pc != iter->first_child ) {
         /* Need to get the result onto the stack! */
         debug_printf( 1, "descending into if condition..." );
         interp_set_pc( interp, iter->first_child );
      } else {
         /* If condition is not necessarily a comparison... "if 1:" is valid! */

         /* Pop and evaluate. */
         item1 = interp_stack_pop( interp );
         assert( NULL != item1 );
         if(
            (ASTREE_VALUE_TYPE_INT == item1->type && 0 != item1->value.i) ||
            (ASTREE_VALUE_TYPE_STRING == item1->type &&
               '\0' != item1->value.s[0])
         ) {
            /* If is TRUE. */
            debug_printf( 1, "if condition TRUE" );
            interp_set_pc( interp,
               astree_node( interp->tree, iter->first_child )->next_sibling );
         } else {
            /* If is FALSE. */
            debug_printf( 1, "if condition FALSE" );
            interp_set_pc( interp, iter->next_sibling );
         }
      }
      break;

   case ASTREE_NODE_TYPE_SEQ_TERM:
      /* Check the sequence parent to figure out what to do next. */
      /* TODO: Integrate function closure return hoop-jumping into this. */
      left = astree_node( interp->tree, iter->value.i );
      assert( NULL != left );
      right = astree_node( interp->tree, left->parent );
      assert( NULL != left ); /* TODO: This won't hold for root seq. */
      switch( right->type ) {
      case ASTREE_NODE_TYPE_WHILE:
         debug_printf( 1, "while sequence termination, return to: %d",
            iter->value.i );
         interp_set_pc( interp, left->parent );
         break;
      default:
         /* Just go to (presumably -1) child so handler above can handle it. */
         /* TODO: Integrate that handler here. */
         interp_set_pc( interp, iter->first_child );
         break;
      }
      break;

   case ASTREE_NODE_TYPE_COND:
      if( interp->prev_pc == iter->first_child ) {
         /* Left should now be on the stack, so get right. */
         debug_printf( 1, "descending into right node..." );
         interp_set_pc( interp,
            astree_node( interp->tree, iter->first_child )->next_sibling );

      } else if(
         interp->prev_pc ==
            astree_node( interp->tree, iter->first_child )->next_sibling
      ) {
         /* Both should now be on the stack. */
         item1 = interp_stack_pop( interp );
         assert( NULL != item1 );
         item2 = interp_stack_pop( interp );
         assert( NULL != item2 );

         /* Compare item1 and item2. */
         /* TODO: Fill out this evaluation table for all types. */
         if(
            (ASTREE_VALUE_TYPE_EQ == iter->value_type && (

               (ASTREE_VALUE_TYPE_STRING == item1->type &&
                  ASTREE_VALUE_TYPE_STRING == item2->type &&
                  0 == strncmp(
                     item1->value.s, item2->value.s, ASTREE_NODE_VALUE_SZ_MAX ))

               || (ASTREE_VALUE_TYPE_INT == item1->type &&
                  ASTREE_VALUE_TYPE_INT == item2->type &&
                  item1->value.i == item2->value.i)
            
            )) || (ASTREE_VALUE_TYPE_GT == iter->value_type && (

               (ASTREE_VALUE_TYPE_INT == item1->type &&
                  ASTREE_VALUE_TYPE_INT == item2->type &&
                  item1->value.i > item2->value.i)
            
            ))
         ) {
            /* Comparison is TRUE. */
            interp_stack_push_int( interp, 1 );
         } else {
            /* Comparison is FALSE. */
            interp_stack_push_int( interp, 0 );
         }

         /* All children evaluated, so go back up! */
         debug_printf( 1, "ascending to parent..." );
         interp_set_pc( interp, iter->parent );

      } else {
         /* First visit, need to get left on the stack. */
         debug_printf( 1, "descending into left node..." );
         interp_set_pc( interp, iter->first_child );
      }
      break;

   case ASTREE_NODE_TYPE_LITERAL:
      /* Push the literal onto the stack. */
      switch( iter->value_type ) {
      case ASTREE_VALUE_TYPE_INT:
         interp_stack_push_int( interp, iter->value.i );
         break;

      case ASTREE_VALUE_TYPE_STRING:
         interp_stack_push_str( interp, iter->value.s );
         break;

      case ASTREE_VALUE_TYPE_FLOAT:
         /* TODO */
         break;
      }

      /* Bottom-level, so go back up! */
      debug_printf( 1, "ascending to parent..." );
      interp_set_pc( interp, iter->parent );
      break;

   case ASTREE_NODE_TYPE_VARIABLE:
      /* Push the variable value onto the stack. */
      var = interp_get_var( interp, iter->value.s );
      assert( NULL != var );
      switch( var->type ) {
      case ASTREE_VALUE_TYPE_STRING:
         debug_printf( 1, "var %s value: %s", iter->value.s, var->value.s );
         interp_stack_push_str( interp, var->value.s );
         break;
         break;

      case ASTREE_VALUE_TYPE_INT:
         debug_printf( 1, "var %s value: %d", iter->value.s, var->value.i );
         interp_stack_push_int( interp, var->value.i );
         break;

      case ASTREE_VALUE_TYPE_FLOAT:
         debug_printf( 1, "var %s value: %f", iter->value.s, var->value.f );
         /* TODO */
         break;
      }

      /* Bottom-level, so go back up! */
      debug_printf( 1, "ascending to parent..." );
      interp_set_pc( interp, iter->parent );
      break;

   case ASTREE_NODE_TYPE_FUNC_DEF:
      if(
         iter->prev_sibling == interp->prev_pc ||
         iter->parent == interp->prev_pc
      ) {
         /* Defining the function... */
         debug_printf( 1, "defining function: %s", iter->value.s );
         /* interp_set_func_def( interp, iter ); */
         interp_set_func(
            interp, iter->value.s, &(interp->pc), INTERP_FUNC_PC );
         interp_set_pc( interp, iter->next_sibling );

      } else {
         /* If we're being called from a non-adjacent instruction, we're
          * probably being asked to run (or there's a bug(!))
          */
         debug_printf( 1, "preparing function: %s", iter->value.s );

         /* Pop args from stack into vars. */

         /* We need to pop the stack into vars in reverse order, so start
          * at the end.
          */
         left = astree_node( interp->tree, iter->first_child );
         assert( NULL != left ); /* At least have the seq node, if no args! */
         while(
            ASTREE_NODE_TYPE_SEQUENCE !=
               astree_node( interp->tree, left->next_sibling )->type
         ) {
            left = astree_node( interp->tree, left->next_sibling );
         }
         assert( NULL != left );
         assert(
            ASTREE_NODE_TYPE_SEQUENCE ==
            astree_node( interp->tree, left->next_sibling )->type );
         /* Prime function seq to run next cycle. */
         interp_set_pc( interp, left->next_sibling );

         /* Cycle through the parms list backwards from last parm. */
         while( NULL != left ) {
            debug_printf( 1, "left type %d: popping to parms...", left->type );
            if( ASTREE_NODE_TYPE_FUNC_DEF_PARM == left->type ) {
               /* Pop stack nodes pushed by last few ticks and push to parms. */
               item1 = interp_stack_pop( interp );
               switch( item1->type ) {
               case ASTREE_VALUE_TYPE_INT:
                  interp_set_var_int( interp, left->value.s, item1->value.i );
                  break;

               case ASTREE_VALUE_TYPE_STRING:
                  interp_set_var_str( interp, left->value.s, item1->value.s );
                  break;
               }
            }
            right = left;

            /* Iterate through arg nodes backwards. */
            debug_printf( 1, "next prev: %d", left->prev_sibling );
            left = astree_node( interp->tree, left->prev_sibling );
         }
      }
      break;

   case ASTREE_NODE_TYPE_FUNC_CALL:

      if( interp_is_first_pass( interp, iter ) ) {
         debug_printf( 1, "function call first pass for: %s()",
            iter->value.s );

         /* Push the return PC before we parse potential args so it's below
          * them on the stack.
          */
         debug_printf( 1, "pushing return pc to stack..." );
         interp_stack_push_int( interp, iter->next_sibling );

         if( 0 <= iter->first_child ) {
            /* There are args, so push the arg PC to the stack so the next pass
             * will be able to pop it (see below) and see that we parsed an arg.
             * Then descend to push the args. */

            debug_printf( 1, "descending to parse first arg..." );
            interp_set_pc( interp, iter->first_child );
         } else {
            /* Call the function with no args. */
            interp_call_func( interp, iter->value.s );
         }
      } else {
         assert( -1 != iter->first_child );
         /* We're coming back from parsing an arg, so we know we have at least
          * one child node.
          */
         left = astree_node( interp->tree, iter->first_child );
         assert( NULL != left );
         if( interp->prev_pc != iter->first_child ) {
            /* We haven't returned from the first child, so it must be a
             * subsequent sibling!
             */
            debug_printf( 1,
               "previous pc: %d, iter node first child: %d",
               interp->prev_pc, iter->first_child );
            debug_printf( 1, "left ns: %d", left->next_sibling );
            while( NULL != left && interp->prev_pc != left->next_sibling ) {
               assert( NULL != left );
               debug_printf( 1, "paging arg to %d (looking for %d)...",
                  left->next_sibling, interp->prev_pc );
               left = astree_node( interp->tree, left->next_sibling );
            }

            /* Left should now be the arg before the last parsed. */
            assert( NULL != left );
            left = astree_node( interp->tree, left->next_sibling );
            /* Now it should be the last parsed. */
            debug_printf( 1, "left node next sibling: %d", left->next_sibling );
         }

         if( NULL != left && 0 <= left->next_sibling ) {
            debug_printf( 1, "descending into arg: %d", left->next_sibling );
            interp_set_pc( interp, left->next_sibling );
         } else {
            debug_printf( 1, "calling function!" );
            /* Call the function with the stack arranged on first pass. */
            interp_call_func( interp, iter->value.s );
         }
      }
      break;

   case ASTREE_NODE_TYPE_ASSIGN:
      left = astree_node( interp->tree, iter->first_child );
      assert( NULL != left );
      right = astree_node( interp->tree, left->next_sibling );
      assert( NULL != right );

      if( interp_is_first_pass( interp, iter ) ) {
         /* Descend into right and evaluate it on next tick. */
         debug_printf( 1, "descending into assignment..." );
         interp_set_pc( interp, left->next_sibling );
      
      } else if( interp->prev_pc == left->next_sibling ) {
         debug_printf( 1, "assignining from stack..." );

         /* Create a variable with name from left and value from right. */
         item1 = interp_stack_pop( interp );
         switch( item1->type ) {
         case ASTREE_VALUE_TYPE_INT:
            interp_set_var_int( interp, left->value.s, item1->value.i );
            break;

         case ASTREE_VALUE_TYPE_STRING:
            interp_set_var_str( interp, left->value.s, item1->value.s );
            break;

         default:
            /* Invalid value. */
            error_printf( "invalid value type: %d", iter->value_type );
            retval = -1;
            goto cleanup;
         }

         interp_set_pc( interp, iter->next_sibling );
      }
      break;

   case ASTREE_NODE_TYPE_OP:
      left = astree_node( interp->tree, iter->first_child );
      assert( NULL != left );
      right = astree_node( interp->tree, left->next_sibling );
      assert( NULL != right );

      if( interp_is_first_pass( interp, iter ) ) {
         /* Put left node on the stack. */
         debug_printf( 1, "descending into left node..." );
         interp_set_pc( interp, iter->first_child );
      
      } else if( interp->prev_pc == iter->first_child ) {
         /* Put right node on the stack. */
         debug_printf( 1, "descending into right node..." );
         interp_set_pc( interp, left->next_sibling );

      } else if( interp->prev_pc == left->next_sibling ) {
         /* Perform op and return to parent! */
         item1 = interp_stack_pop( interp );
         item2 = interp_stack_pop( interp );

         /* TODO: Handle different types! */
         interp_stack_push_int( interp, item1->value.i + item2->value.i );

         debug_printf( 1, "ascending to parent..." );
         interp_set_pc( interp, iter->parent );
      }
      break;

   default:
      /* By default, descend into node. */
      interp_set_pc( interp, iter->first_child );
      break;
   }

cleanup:

   if( 0 > retval ) {
      error_printf( "error on node: %d", interp->pc );
   }

   return retval;
}

