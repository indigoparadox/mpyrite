
#include <stdlib.h> /* For atoi. */

#include "maug.h"

#include "astree.h"

#define PARSER_C
#include "parser.h"

int mpy_parser_is_numeric( const char* token, int token_sz ) {
   int i = 0;

   if( 0 >= token_sz ) {
      /* Empty is NOT numeric! */
      debug_printf( 0, "token \"%s\" not numeric: empty", token );
      return 0;
   }

   for( i = 0 ; token_sz > i ; i++ ) {
      if( 48 > token[i] || 57 < token[i] ) {
         debug_printf( 0, "token \"%s\" not numeric: %c", token, token[i] );
         return 0;
      }
   }

   return 1;
}

int mpy_parser_add_node_def( struct MPY_PARSER* parser ) {
   int16_t def_node_idx = -1;

   def_node_idx = astree_node_add_child( parser->tree, parser->tree_node_idx );
   astree_node( parser->tree, def_node_idx )->type = ASTREE_NODE_TYPE_FUNC_DEF;
   mpy_parser_node_idx( parser, def_node_idx );
   mpy_parser_state( parser, MPY_PARSER_STATE_FUNC_DEF );

   return 0;
}

int mpy_parser_add_node_if( struct MPY_PARSER* parser ) {
   int16_t if_node_idx = -1;

   assert( NULL != astree_node( parser->tree, parser->tree_node_idx ) );
   assert( ASTREE_NODE_TYPE_ASSIGN !=
      astree_node( parser->tree, parser->tree_node_idx )->type );
   assert( ASTREE_NODE_TYPE_OP !=
      astree_node( parser->tree, parser->tree_node_idx )->type );
   assert( ASTREE_NODE_TYPE_LITERAL !=
      astree_node( parser->tree, parser->tree_node_idx )->type );

   if_node_idx = astree_node_add_child( parser->tree, parser->tree_node_idx );
   debug_printf( 1, "adding node %d: if", if_node_idx );
   astree_node( parser->tree, if_node_idx )->type = ASTREE_NODE_TYPE_IF;
   mpy_parser_node_idx( parser, if_node_idx );
   mpy_parser_state( parser, MPY_PARSER_STATE_IF_COND )

   return 0;
}

int mpy_parser_add_node_while( struct MPY_PARSER* parser ) {
   int16_t while_node_idx = -1;

   while_node_idx = 
      astree_node_add_child( parser->tree, parser->tree_node_idx );
   debug_printf( 1, "adding node %d: while", while_node_idx );
   astree_node( parser->tree, while_node_idx )->type = ASTREE_NODE_TYPE_WHILE;
   mpy_parser_node_idx( parser, while_node_idx );
   mpy_parser_state( parser, MPY_PARSER_STATE_WHILE_COND )

   return 0;
}

#  define MPY_PARSER_STATEMENTS_ADD_CBS( token, type, add_cb ) add_cb,

mpy_parser_add_cb g_mpy_parser_add_cbs[] = {
   MPY_PARSER_STATEMENTS( MPY_PARSER_STATEMENTS_ADD_CBS )
   NULL
};

int mpy_parser_add_node_sequence( struct MPY_PARSER* parser ) {
   int16_t seq_node_idx = -1;

   seq_node_idx = astree_node_add_child( parser->tree, parser->tree_node_idx );
   debug_printf( 1, "adding seq node %d", seq_node_idx );
   astree_node( parser->tree, seq_node_idx )->type = ASTREE_NODE_TYPE_SEQUENCE;
   mpy_parser_node_idx( parser, seq_node_idx );

   return 0;
}

int mpy_parser_add_node_variable(
   struct MPY_PARSER* parser, const char* name, uint8_t node_type
) {
   int16_t var_node_idx = -1;

   var_node_idx = astree_node_add_child( parser->tree, parser->tree_node_idx );
   debug_printf( 1, "adding var node %d: %s", var_node_idx, name );
   astree_node( parser->tree, var_node_idx )->type = node_type;
   strncpy(
      parser->tree->nodes[var_node_idx].value.s, name, ASTREE_NODE_VALUE_SZ_MAX );
   mpy_parser_node_idx( parser, var_node_idx );

   return 0;
}

int mpy_parser_add_node_literal(
   struct MPY_PARSER* parser, const char* value, uint8_t value_type
) {
   int16_t value_node_idx = -1;

   /* Note that this does NOT move the parser's focus to the created node! */
   /* Nor does it change the parser's state! */

   value_node_idx =
      astree_node_add_child( parser->tree, parser->tree_node_idx );
   debug_printf( 1, "adding literal node %d: \"%s\"", value_node_idx, value );
   astree_node( parser->tree, value_node_idx )->type = ASTREE_NODE_TYPE_LITERAL;
   astree_node( parser->tree, value_node_idx )->value_type = value_type;
   mpy_parser_node_idx( parser, value_node_idx );

   switch( value_type ) {
   case ASTREE_VALUE_TYPE_INT:
      debug_printf( 3, "value: %d from %s", atoi( value ), value );
      parser->tree->nodes[value_node_idx].value.i = atoi( value );
      break;

   case ASTREE_VALUE_TYPE_FLOAT:
      parser->tree->nodes[value_node_idx].value.f = atof( value );
      break;

   case ASTREE_VALUE_TYPE_STRING:
      strncpy(
         parser->tree->nodes[value_node_idx].value.s,
         value,
         ASTREE_NODE_VALUE_SZ_MAX );
      break;
   }

   return 0;
}

int mpy_parser_add_node_call( struct MPY_PARSER* parser, const char* name ) {
   int16_t call_node_idx = -1;

   call_node_idx = astree_node_add_child( parser->tree, parser->tree_node_idx );
   astree_node( parser->tree, call_node_idx )->type =
      ASTREE_NODE_TYPE_FUNC_CALL;
   strncpy(
      astree_node( parser->tree, call_node_idx )->value.s,
      name, ASTREE_NODE_VALUE_SZ_MAX );
   mpy_parser_node_idx( parser, call_node_idx );

   return 0;
}

int mpy_parser_insert_node(
   struct MPY_PARSER* parser, uint8_t type, uint8_t value_type
) {
   int16_t node_idx = -1;

   node_idx = astree_node_insert_as_parent(
      parser->tree, parser->tree_node_idx );
   astree_node( parser->tree, node_idx )->type = type;
   astree_node( parser->tree, node_idx )->value_type = value_type;
   mpy_parser_node_idx( parser, node_idx );

   return 0;
}

void mpy_parser_reset_after_var( struct MPY_PARSER* parser ) {
   if(
      ASTREE_NODE_TYPE_FUNC_CALL ==
         astree_node( parser->tree, parser->tree_node_idx )->type
   ) {
      debug_printf( 1, "auto-resetting to func call parms state" );
      mpy_parser_state( parser, MPY_PARSER_STATE_FUNC_CALL_PARMS )
   } else if(
      ASTREE_NODE_TYPE_FUNC_DEF ==
         astree_node( parser->tree, parser->tree_node_idx )->type
   ) {
      debug_printf( 1, "auto-resetting to func def parms state" );
      mpy_parser_state( parser, MPY_PARSER_STATE_FUNC_DEF_PARMS )
   } else if(
      ASTREE_NODE_TYPE_IF ==
         astree_node( parser->tree, parser->tree_node_idx )->type
   ) {
      debug_printf( 1, "auto-resetting to if condition state" );
      mpy_parser_state( parser, MPY_PARSER_STATE_IF_COND )
   } else if(
      ASTREE_NODE_TYPE_WHILE ==
         astree_node( parser->tree, parser->tree_node_idx )->type
   ) {
      debug_printf( 1, "auto-resetting to while condition state" );
      mpy_parser_state( parser, MPY_PARSER_STATE_WHILE_COND )
   } else if(
      ASTREE_NODE_TYPE_ASSIGN ==
         astree_node( parser->tree, parser->tree_node_idx )->type
   ) {
      /* Only allow assigns off of sequences, for now... */
      mpy_parser_rewind( parser, ASTREE_NODE_TYPE_SEQUENCE );
      mpy_parser_state( parser, MPY_PARSER_STATE_ASSIGN )
   } else if(
      ASTREE_NODE_TYPE_COND ==
         astree_node( parser->tree, parser->tree_node_idx )->type ||
      ASTREE_NODE_TYPE_OP ==
         astree_node( parser->tree, parser->tree_node_idx )->type
   ) {
      debug_printf( 1, "rewinding up to prev if/func call/func def" );
      while(
         ASTREE_NODE_TYPE_IF !=
            astree_node( parser->tree, parser->tree_node_idx )->type &&
         /* XXX Func def? this might be invalid... */
         ASTREE_NODE_TYPE_FUNC_DEF !=
            astree_node( parser->tree, parser->tree_node_idx )->type &&
         ASTREE_NODE_TYPE_FUNC_CALL !=
            astree_node( parser->tree, parser->tree_node_idx )->type
      ) {
         /* Rewind */
         mpy_parser_node_idx( parser,
            parser->tree->nodes[parser->tree_node_idx].parent );
         assert( 0 <= parser->tree_node_idx );
      }
      mpy_parser_reset_after_var( parser );
   } else if(
      ASTREE_NODE_TYPE_LITERAL ==
         astree_node( parser->tree, parser->tree_node_idx )->type ||
      ASTREE_NODE_TYPE_VARIABLE ==
         astree_node( parser->tree, parser->tree_node_idx )->type
   ) {
      /* XXX: Test this out. */
      mpy_parser_state_reset( parser );
   } else {
      /* debug_printf( 1, "auto-resetting to none state" );
      mpy_parser_state( parser, MPY_PARSER_STATE_NONE ) */
      debug_printf( 1, "Bad reset?" );
   }
}

void mpy_parser_rewind( struct MPY_PARSER* parser, int node_type ) {
   while(
      node_type != astree_node( parser->tree, parser->tree_node_idx )->type
   ) {
      /* Rewind */
      mpy_parser_node_idx( parser,
         parser->tree->nodes[parser->tree_node_idx].parent );
#ifdef DEBUG
      if( 0 > parser->tree_node_idx ) {
         astree_dump( parser->tree, 0, 0 );
      }
#endif /* DEBUG */
      assert( 0 <= parser->tree_node_idx );
   }
}

int mpy_parser_parse_token( struct MPY_PARSER* parser, char trig_c ) {
   uint16_t i = 0;
   int retval = 0;
   struct ASTREE_NODE* cnode = &(parser->tree->nodes[parser->tree_node_idx]);

   if( 0 >= parser->token_sz ) {
      if( '\n' == trig_c || '\r' == trig_c ) {
         debug_printf( 1, "empty token, trig: '\\n'" );
      } else {
         debug_printf( 1, "empty token, trig: '%c'", trig_c );
      }
      goto cleanup;
   }

   if( '\n' == trig_c || '\r' == trig_c ) {
      debug_printf( 1, "token: %s, trig: '\\n', state: %s, prev_state: %s",
         parser->token,
         gc_mpy_parser_state_tokens[parser->state],
         gc_mpy_parser_state_tokens[parser->prev_state] );
   } else {
      debug_printf( 1, "token: %s, trig: '%c'", parser->token, trig_c );
   }

   if( MPY_PARSER_STATE_FUNC_DEF == parser->state ) {
      /* XXX */
      /* Function definition name found. */
      debug_printf( 1, "def parm?: %s", parser->token );
      if( 0 < strlen( parser->token ) ) {
         debug_printf( 1, "func name: %s", parser->token );
         strncpy( cnode->value.s, parser->token, ASTREE_NODE_VALUE_SZ_MAX );
      }

      goto cleanup;

   } else if(
      MPY_PARSER_STATE_FUNC_DEF_PARMS == parser->prev_state ||
      MPY_PARSER_STATE_FUNC_CALL_PARMS == parser->prev_state ||
      MPY_PARSER_STATE_IF_COND == parser->prev_state ||
      MPY_PARSER_STATE_WHILE_COND == parser->prev_state ||
      ((
         MPY_PARSER_STATE_ASSIGN == parser->prev_state
      ) && (
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state ||
         MPY_PARSER_STATE_NUM == parser->state ||
         MPY_PARSER_STATE_FLOAT == parser->state ||
         MPY_PARSER_STATE_VAR == parser->state
      ))
   ) {

      debug_printf( 1, "if cond or func parm?: %s", parser->token );
      if( MPY_PARSER_STATE_NUM == parser->state ) {
         mpy_parser_add_node_literal(
            parser, parser->token, ASTREE_VALUE_TYPE_INT );

      } else if( MPY_PARSER_STATE_FLOAT == parser->state ) {
         mpy_parser_add_node_literal(
            parser, parser->token, ASTREE_VALUE_TYPE_FLOAT );

      } else if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         mpy_parser_add_node_literal(
            parser, parser->token, ASTREE_VALUE_TYPE_STRING );

      } else {
         mpy_parser_add_node_variable( parser, parser->token,
            parser->prev_state == MPY_PARSER_STATE_FUNC_DEF_PARMS ?
               ASTREE_NODE_TYPE_FUNC_DEF_PARM : ASTREE_NODE_TYPE_VARIABLE );
      }
      mpy_parser_reset_after_var( parser );
      goto cleanup;

#if 0
   } else if(
      (MPY_PARSER_STATE_STRING == parser->state && '"' == trig_c) ||
      (MPY_PARSER_STATE_STRING_SQ == parser->state && '\'' == trig_c)
   ) {
      /* Terminating a string. */
      mpy_parser_add_node_literal(
         parser, parser->token, ASTREE_VALUE_TYPE_STRING );
      mpy_parser_reset_after_var( parser );
      goto cleanup;
#endif
   } else if(
      (MPY_PARSER_STATE_VAR == parser->state && '(' == trig_c)
   ) {
      /* TODO: Assert valid conditions. */
      retval = mpy_parser_add_node_call( parser, parser->token );
   }

   /* Otherwise look for built-in statement. */

   for( i = 0 ; '\0' != gc_mpy_parser_tokens[i][0] ; i++ ) {
      debug_printf( 1, "%s vs %s", 
            gc_mpy_parser_tokens[i], parser->token );
      if(
         strlen( gc_mpy_parser_tokens[i] ) == strlen( parser->token ) &&
         0 == strncmp(
            gc_mpy_parser_tokens[i], parser->token,
            strlen( gc_mpy_parser_tokens[i] ) + 1 )
      ) {
         debug_printf( 1, "found token: %s", parser->token );
         retval = g_mpy_parser_add_cbs[i]( parser );
         goto cleanup;
      }
   }

   /* Otherwise it must be a function call or variable. */
   if( ' ' == trig_c || ',' == trig_c ) {
      retval = mpy_parser_add_node_variable(
         parser, parser->token, ASTREE_NODE_TYPE_VARIABLE );
   }

cleanup:

   parser->token_sz = 0;
   parser->token[0] = '\0';

   return retval;
}

int mpy_parser_append_token( struct MPY_PARSER* parser, char c ) {
   parser->token[parser->token_sz++] = c;
   parser->token[parser->token_sz] = '\0';

   debug_printf( 0, "state: %s, prev_state: %s, token_sz: %d, c: %c",
      gc_mpy_parser_state_tokens[parser->state],
      gc_mpy_parser_state_tokens[parser->prev_state],
      parser->token_sz,
      c );

   if(
      1 == parser->token_sz && (
      MPY_PARSER_STATE_IF_COND == parser->state ||
      MPY_PARSER_STATE_WHILE_COND == parser->state ||
      MPY_PARSER_STATE_FUNC_DEF_PARMS == parser->state ||
      MPY_PARSER_STATE_FUNC_CALL_PARMS == parser->state ||
      MPY_PARSER_STATE_ASSIGN == parser->state ||
      MPY_PARSER_STATE_NONE == parser->state)
   ) {
      /* Parser is looking for a new literal, variable, or function call. */
      if( mpy_parser_is_numeric( parser->token, parser->token_sz ) ) {
         mpy_parser_state( parser, MPY_PARSER_STATE_NUM )
      } else {
         /* Assume it's a variable (but it may be a function call if we get a
          * ( later! */
         mpy_parser_state( parser, MPY_PARSER_STATE_VAR )
      }
   }

   return 0;
}

void mpy_parser_check_indent( struct MPY_PARSER* parser, char c ) {
   int indent_diff = 0;
   struct ASTREE_NODE* iter = NULL,
      * iter_parent = NULL;
   int16_t term_node_idx = -1;

   if( parser->inside_indent && ' ' != c ) {
      parser->inside_indent = 0;
      debug_printf( 1,
         "line %d: ending indent at: %d",
         parser->line_num, parser->this_line_indent );
      if( 0 == parser->indent_divisor ) {
         /* Set indent divisor based on first indented line. */
         parser->indent_divisor = parser->this_line_indent;
      }
      if( parser->this_line_indent < parser->prev_line_indent ) {
         /* Indent reduced, so rewind upwards. */

         indent_diff = parser->prev_line_indent;
         do {
            /* Rewind upwards each level, one by one. */
            debug_printf( 1,
               "line %d: rewinding up from indent %d to indent %d",
               parser->line_num,
               indent_diff, indent_diff - parser->indent_divisor );

            iter = astree_node( parser->tree, parser->tree_node_idx );
            assert( NULL != iter );
            iter_parent = astree_node( parser->tree, iter->parent );
            assert( NULL != iter_parent );
            do {

               /* We need to test whether the next sibling is -1 here, because
                * we're walking back up and this is the node that was last
                * traversed.
                */
               if(
                  NULL != iter &&
                  ASTREE_NODE_TYPE_SEQUENCE == iter->type &&
                  0 > iter->next_sibling
               ) {
                  /* Add sequence terminator. */
                  term_node_idx = astree_node_add_child(
                     parser->tree, parser->tree_node_idx );
                  debug_printf( 1, "adding seq term node %d", term_node_idx );
                  astree_node( parser->tree, term_node_idx )->type =
                     ASTREE_NODE_TYPE_SEQ_TERM;
                  astree_node( parser->tree, term_node_idx )->value.i =
                     parser->tree_node_idx;
               } else if( NULL != iter ) {
                  debug_printf( 1, "not adding term at node %d",
                     parser->tree_node_idx );
               }

               /* Do this loop at least once to get out of current sequence. */
               mpy_parser_node_idx( parser,
                  parser->tree->nodes[parser->tree_node_idx].parent );
               assert( 0 <= parser->tree_node_idx );
               iter = astree_node( parser->tree, parser->tree_node_idx );
               assert( NULL != iter );
               iter_parent = astree_node( parser->tree, iter->parent );

            } while( ASTREE_NODE_TYPE_SEQUENCE != iter->type );

            indent_diff -= parser->indent_divisor;
            debug_printf( 1, "active node index is now: %d",
            parser->tree_node_idx );
         } while( 0 < indent_diff );
            
         /* Found the sequence node, move up to its parent
          * func def/if/while/etc before we add more neighbors.
          */
          /*
         assert( NULL != astree_node( parser->tree, iter->parent ) );
         mpy_parser_node_idx( parser, iter->parent );
         iter = astree_node( parser->tree, parser->tree_node_idx );
         assert( NULL != iter );
         mpy_parser_node_idx( parser, iter->parent );
         */
      }
   }
}

int mpy_parser_parse_c( struct MPY_PARSER* parser, char c ) {
   int retval = 0;

   mpy_parser_check_indent( parser, c );

   switch( c ) {

   case '#':
      if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );
      } else {
         mpy_parser_state( parser, MPY_PARSER_STATE_COMMENT )
      }
      break;

   case '\r':
   case '\n':
      parser->line_num++;
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* mpy_parser_reset_after_var( parser ); */
         mpy_parser_state( parser, MPY_PARSER_STATE_NONE );
      } else {
         retval = mpy_parser_parse_token( parser, c );
         if(
            MPY_PARSER_STATE_ASSIGN == parser->state ||
            (MPY_PARSER_STATE_ASSIGN == parser->prev_state && (
               MPY_PARSER_STATE_NUM == parser->state ||
               MPY_PARSER_STATE_FLOAT == parser->state
            ))
         ) {
            mpy_parser_state( parser, MPY_PARSER_STATE_NONE );
            debug_printf( 1, "assign done; rewinding up to sequence" );
            mpy_parser_rewind( parser, ASTREE_NODE_TYPE_SEQUENCE );
         }
         parser->prev_line_indent = parser->this_line_indent;
         parser->this_line_indent = 0;
         parser->inside_indent = 1;
      }
      break;

   case '\'':
      /* TODO: Handle escaping. */
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */
      } else if( MPY_PARSER_STATE_STRING_SQ == parser->state ) {
         debug_printf( 1, "end sq string: %s", parser->token );
         retval = mpy_parser_parse_token( parser, c );
      } else if( MPY_PARSER_STATE_STRING == parser->state ) {
         retval = mpy_parser_append_token( parser, c );
      } else {
         mpy_parser_state( parser, MPY_PARSER_STATE_STRING_SQ )
      }
      break;

   case '"':
      /* TODO: Handle escaping. */
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */
      } else if( MPY_PARSER_STATE_STRING == parser->state ) {
         retval = mpy_parser_parse_token( parser, c );
      } else if( MPY_PARSER_STATE_STRING_SQ == parser->state ) {
         retval = mpy_parser_append_token( parser, c );
      } else {
         mpy_parser_state( parser, MPY_PARSER_STATE_STRING )
      }
      break;

   case ' ':
      if( parser->inside_indent ) {
         /* Add to indentation. */
         parser->this_line_indent++;
      } else if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         /* Add to string. */
         retval = mpy_parser_append_token( parser, c );
      } else if(
         MPY_PARSER_STATE_VAR == parser->state ||
         MPY_PARSER_STATE_NUM == parser->state
      ) {
         /* Might be a statement (or empty, but parse_token() will filter). */
         retval = mpy_parser_parse_token( parser, c );
      } else {
         /* Do nothing. */
      }
      break;

   case '<':
   case '>':
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */

      } else if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );

      } else if(
         MPY_PARSER_STATE_IF_COND == parser->state ||
         MPY_PARSER_STATE_WHILE_COND == parser->state ||
         MPY_PARSER_STATE_FUNC_CALL_PARMS == parser->state
      ) {
         /* Start comparison under if/while/func call. */
         retval = mpy_parser_insert_node(
            parser, ASTREE_NODE_TYPE_COND,
               '>' == c ? ASTREE_VALUE_TYPE_GT : ASTREE_VALUE_TYPE_LT );

      } else {
         retval = MPY_PARSER_ERROR_UNEXPECTED_C;
      }
      break;

   case ',':
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */

      } else if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );

      } else if(
         MPY_PARSER_STATE_FUNC_DEF_PARMS == parser->state ||
         MPY_PARSER_STATE_FUNC_DEF_PARMS == parser->prev_state
      ) {
         /* Add function parm. */
         retval = mpy_parser_parse_token( parser, c );
         debug_printf( 1, "parm done; rewinding up to function def" );
         mpy_parser_rewind( parser, ASTREE_NODE_TYPE_FUNC_DEF );

      } else if(
         MPY_PARSER_STATE_FUNC_CALL_PARMS == parser->state ||
         MPY_PARSER_STATE_FUNC_CALL_PARMS == parser->prev_state
      ) {
         /* Add function parm. */
         retval = mpy_parser_parse_token( parser, c );
         debug_printf( 1, "parm done; rewinding up to function call" );
         mpy_parser_rewind( parser, ASTREE_NODE_TYPE_FUNC_CALL );

      } else {
         retval = MPY_PARSER_ERROR_UNEXPECTED_C;
      }
      break;

   case '-':
   case '*':
   case '/':
   case '+':
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */
      } else if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );

      } else if(
         MPY_PARSER_STATE_FUNC_CALL_PARMS == parser->state ||
         MPY_PARSER_STATE_ASSIGN == parser->state ||
         MPY_PARSER_STATE_ASSIGN == parser->prev_state
      ) {
         /* Inside of an assignment rvalue or func call parms. */
         retval = mpy_parser_parse_token( parser, c );
         retval = mpy_parser_insert_node(
            parser, ASTREE_NODE_TYPE_OP, mpy_parser_sym_to_op( c ) );

      } else {
         retval = MPY_PARSER_ERROR_UNEXPECTED_C;
      }
      break;

   case '=':
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */

      } else if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );

      } else if(
         MPY_PARSER_STATE_IF_COND == parser->prev_state &&
         MPY_PARSER_STATE_EQ_SIGN == parser->state
      ) {
         /* == detected, testing for equality under if. */
         retval = mpy_parser_insert_node(
            parser, ASTREE_NODE_TYPE_COND, ASTREE_VALUE_TYPE_EQ );
         mpy_parser_state( parser, MPY_PARSER_STATE_IF_COND );

      } else if(
         MPY_PARSER_STATE_WHILE_COND == parser->prev_state &&
         MPY_PARSER_STATE_EQ_SIGN == parser->state
      ) {
         /* == detected, testing for equality under while. */
         retval = mpy_parser_insert_node(
            parser, ASTREE_NODE_TYPE_COND, ASTREE_VALUE_TYPE_EQ );
         mpy_parser_state( parser, MPY_PARSER_STATE_WHILE_COND );

      } else if(
         MPY_PARSER_STATE_IF_COND == parser->state ||
         MPY_PARSER_STATE_WHILE_COND == parser->state
      ) {
         /* = detected under if/while, another = will be equality test. */
         /* TODO: Allow assignment in while/if cond. */
         mpy_parser_state( parser, MPY_PARSER_STATE_EQ_SIGN );

      } else if( MPY_PARSER_STATE_VAR == parser->state ) {
         /* Single = after var, assign detected. */
         retval = mpy_parser_parse_token( parser, c );
         retval = mpy_parser_insert_node(
            parser, ASTREE_NODE_TYPE_ASSIGN, 0 );
         mpy_parser_state( parser, MPY_PARSER_STATE_ASSIGN );

      } else {
         retval = MPY_PARSER_ERROR_UNEXPECTED_C;
      }
      break;

   case ':':
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */

      } else if(
         MPY_PARSER_STATE_WHILE_COND == parser->state ||
         MPY_PARSER_STATE_WHILE_COND == parser->prev_state
      ) {
         retval = mpy_parser_parse_token( parser, c );

         /* Start while statement body. */
         debug_printf( 1, "conditions done; rewinding up to while statement" );
         mpy_parser_rewind( parser, ASTREE_NODE_TYPE_WHILE );
         mpy_parser_state( parser, MPY_PARSER_STATE_NONE )
         retval = mpy_parser_add_node_sequence( parser );

      } else if(
         MPY_PARSER_STATE_IF_COND == parser->state ||
         MPY_PARSER_STATE_IF_COND == parser->prev_state
      ) {
         retval = mpy_parser_parse_token( parser, c );

         /* Start if statement body. */
         debug_printf( 1, "conditions done; rewinding up to if statement" );
         mpy_parser_rewind( parser, ASTREE_NODE_TYPE_IF );
         mpy_parser_state( parser, MPY_PARSER_STATE_NONE )
         retval = mpy_parser_add_node_sequence( parser );

      } else if(
         MPY_PARSER_STATE_NONE == parser->state && (
            MPY_PARSER_STATE_VAR == parser->prev_state ||
            MPY_PARSER_STATE_NUM == parser->prev_state ||
            MPY_PARSER_STATE_FLOAT == parser->prev_state ||
            MPY_PARSER_STATE_STRING == parser->prev_state ||
            MPY_PARSER_STATE_STRING_SQ == parser->prev_state
         )
      ) {
         /* ')' resets state to NONE, this ist probably function def.
          * Start function definition body.
          */
         /* TODO: Merge with MPY_PARSER_STATE_IF_COND above. */
         debug_printf( 1,
            "function params done; rewinding up to def statement" );
         mpy_parser_rewind( parser, ASTREE_NODE_TYPE_FUNC_DEF );
         mpy_parser_state( parser, MPY_PARSER_STATE_NONE )
         retval = mpy_parser_add_node_sequence( parser );

      } else if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );

      } else {
         /* TODO: Create a tuple/precedence node. */
         retval = MPY_PARSER_ERROR_UNEXPECTED_C;
      }
      break;

   case '(':
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */

      } else if( MPY_PARSER_STATE_FUNC_DEF == parser->state ) {
         /* We're between the function name after "def" and the parms list. */
         /* Parse function name token. */
         retval = mpy_parser_parse_token( parser, c );
         mpy_parser_state( parser, MPY_PARSER_STATE_FUNC_DEF_PARMS )

      } else if( MPY_PARSER_STATE_VAR == parser->state ) {
         /* Probably a function invocation? */
         retval = mpy_parser_parse_token( parser, c );
         mpy_parser_state( parser, MPY_PARSER_STATE_FUNC_CALL_PARMS )

      } else if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );
      
      } else {
         retval = MPY_PARSER_ERROR_UNEXPECTED_C;
      }
      break;

   case ')':
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */

      } else if( MPY_PARSER_STATE_FUNC_DEF_PARMS == parser->prev_state ) {
         /* End function parms. */
         debug_printf( 1,
            "function def params done; tree_idx: %d", parser->tree_node_idx );
         retval = mpy_parser_parse_token( parser, c );
         mpy_parser_state( parser, MPY_PARSER_STATE_NONE );

      } else if(
         MPY_PARSER_STATE_FUNC_CALL_PARMS == parser->state ||
         MPY_PARSER_STATE_FUNC_CALL_PARMS == parser->prev_state
      ) {
         /* End function parms. */
         debug_printf( 1,
            "function call params done; tree_idx: %d", parser->tree_node_idx );
         retval = mpy_parser_parse_token( parser, c );
         mpy_parser_state( parser, MPY_PARSER_STATE_NONE );
          
         /* TODO: What else might we be rewinding up to from a function call?
          *       Maybe another function call's parms?
          */
         mpy_parser_rewind( parser, ASTREE_NODE_TYPE_SEQUENCE );

      } else if(
         MPY_PARSER_STATE_STRING == parser->state ||
         MPY_PARSER_STATE_STRING_SQ == parser->state
      ) {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );

      } else {
         retval = MPY_PARSER_ERROR_UNEXPECTED_C;
      }
      break;

   default:
      if( MPY_PARSER_STATE_COMMENT == parser->state ) {
         /* Do nothing. */
      } else {
         retval = mpy_parser_append_token( parser, c );
      }
      break;
   }

   if( MPY_PARSER_ERROR_UNEXPECTED_C == retval ) {
      /* Invalid character declared above, so elaborate. */
      astree_dump( parser->tree, 0, 0 );
      error_printf( "unexpected '%c' detected! state: %s, prev_state: %s",
         c,
         gc_mpy_parser_state_tokens[parser->state],
         gc_mpy_parser_state_tokens[parser->prev_state] );
   }

   if( retval ) {
      error_printf( "invalid state on line: %d", parser->line_num );
   }

   return retval;
}

int parser_parse_buffer(
   struct MPY_PARSER* parser, struct ASTREE* tree,
   const char* buffer, int buffer_sz
) {
   int i = 0;
   int16_t retval = 0;

   memset( parser, '\0', sizeof( struct MPY_PARSER ) );
   parser->tree = tree;
   for( i = 0 ; buffer_sz > i ; i++ ) {
      retval = mpy_parser_parse_c( parser, buffer[i] );
      if( retval ) {
         return retval;
      }
   }

   return 0;
}

