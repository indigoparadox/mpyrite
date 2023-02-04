
#include <stdlib.h> /* For atoi. */

#include "maug.h"

#include "astree.h"

#define PARSER_C
#include "parser.h"

int mpy_parser_is_numeric( const char* token, int token_sz ) {
   int i = 0;

   if( 0 >= token_sz ) {
      /* Empty is NOT numeric! */
      return 0;
   }

   for( i = 0 ; token_sz > i ; i++ ) {
      if( 48 > token[i] || 57 < token[i] ) {
         return 0;
      }
   }

   return 1;
}

int mpy_parser_add_node_def( struct MPY_PARSER* parser ) {
   int16_t def_node_idx = -1;

   def_node_idx = astree_node_add_child( parser->tree_node_idx );
   astree_set_node_type( def_node_idx, ASTREE_NODE_TYPE_FUNC_DEF );
   parser->tree_node_idx = def_node_idx;
   parser->state = MPY_PARSER_STATE_FUNC_DEF;

   return 0;
}

int mpy_parser_add_node_if( struct MPY_PARSER* parser ) {
   int16_t if_node_idx = -1;

   if_node_idx = astree_node_add_child( parser->tree_node_idx );
   astree_set_node_type( if_node_idx, ASTREE_NODE_TYPE_IF );
   parser->tree_node_idx = if_node_idx;
   parser->state = MPY_PARSER_STATE_IF_COND;

   return 0;
}

#  define MPY_PARSER_STATEMENTS_ADD_CBS( token, type, add_cb ) add_cb,

mpy_parser_add_cb g_mpy_parser_add_cbs[] = {
   MPY_PARSER_STATEMENTS( MPY_PARSER_STATEMENTS_ADD_CBS )
   NULL
};

int mpy_parser_add_node_sequence( struct MPY_PARSER* parser ) {
   int16_t seq_node_idx = -1;

   seq_node_idx = astree_node_add_child( parser->tree_node_idx );
   astree_set_node_type( seq_node_idx, ASTREE_NODE_TYPE_SEQUENCE );
   parser->tree_node_idx = seq_node_idx;

   return 0;
}

int mpy_parser_add_node_variable(
   struct MPY_PARSER* parser, const char* name
) {
   int16_t var_node_idx = -1;

   var_node_idx = astree_node_add_child( parser->tree_node_idx );
   astree_set_node_type( var_node_idx, ASTREE_NODE_TYPE_VARIABLE );

   return 0;
}

int mpy_parser_add_node_literal(
   struct MPY_PARSER* parser, const char* value, uint8_t value_type
) {
   int16_t value_node_idx = -1;

   /* Note that this does NOT move the parser's focus to the created node! */
   /* Nor does it change the parser's state! */

   value_node_idx = astree_node_add_child( parser->tree_node_idx );
   astree_set_node_type( value_node_idx, ASTREE_NODE_TYPE_LITERAL );
   astree_set_node_value_type( value_node_idx, value_type );

   switch( value_type ) {
   case ASTREE_VALUE_TYPE_INT:
      debug_printf( 3, "value: %d from %s", atoi( value ), value );
      g_astree_nodes[value_node_idx].value.i = atoi( value );
      break;

   case ASTREE_VALUE_TYPE_FLOAT:
      g_astree_nodes[value_node_idx].value.f = atof( value );
      break;

   case ASTREE_VALUE_TYPE_STRING:
      strncpy(
         g_astree_nodes[value_node_idx].value.s,
         value,
         ASTREE_NODE_VALUE_SZ_MAX );
      break;
   }

   return 0;
}

int mpy_parser_add_node_call( struct MPY_PARSER* parser, const char* name ) {
   int16_t call_node_idx = -1;

   call_node_idx = astree_node_add_child( parser->tree_node_idx );
   astree_set_node_type( call_node_idx, ASTREE_NODE_TYPE_FUNC_CALL );
   strncpy(
      g_astree_nodes[call_node_idx].value.s, name, ASTREE_NODE_VALUE_SZ_MAX );
   parser->tree_node_idx = call_node_idx;

   return 0;
}

int mpy_parser_add_node_parm( struct MPY_PARSER* parser, const char* name ) {
   int16_t parm_node_idx = -1;

   parm_node_idx = astree_node_add_child( parser->tree_node_idx );
   astree_set_node_type( parm_node_idx, ASTREE_NODE_TYPE_FUNC_PARM );
   strncpy(
      g_astree_nodes[parm_node_idx].value.s, name, ASTREE_NODE_VALUE_SZ_MAX );

   return 0;
}

int mpy_parser_insert_node_cond( struct MPY_PARSER* parser, uint8_t cond ) {
   int16_t cond_node_idx = -1;

   cond_node_idx = astree_node_insert_child_parent( parser->tree_node_idx );
   astree_set_node_type( cond_node_idx, ASTREE_NODE_TYPE_COND );
   astree_set_node_value_type( cond_node_idx, cond );
   parser->tree_node_idx = cond_node_idx;

   return 0;
}

int mpy_parser_parse_token( struct MPY_PARSER* parser, char trig_c ) {
   uint16_t i = 0;
   int retval = 0;
   struct ASTREE_NODE* cnode = &(g_astree_nodes[parser->tree_node_idx]);

   if( MPY_PARSER_STATE_FUNC_DEF == parser->state ) {
      /* Function definition name found. */
      if( 0 < strlen( parser->token ) ) {
         debug_printf( 1, "func name: %s", parser->token );
         strncpy( cnode->value.s, parser->token, ASTREE_NODE_VALUE_SZ_MAX );
      }

      goto cleanup;

   } else if( MPY_PARSER_STATE_IF_COND == parser->state ) {
      /* TODO */
      if( mpy_parser_is_numeric( parser->token, parser->token_sz ) ) {
         mpy_parser_add_node_literal(
            parser, parser->token, ASTREE_VALUE_TYPE_INT );
      }
      goto cleanup;

   } else if( MPY_PARSER_STATE_FUNC_PARMS == parser->state ) {

      mpy_parser_add_node_parm( parser, parser->token );
      goto cleanup;

   } else if(
      MPY_PARSER_STATE_STRING == parser->state ||
      MPY_PARSER_STATE_STRING_SQ == parser->state
   ) {
      /* Terminating a string. */
      mpy_parser_add_node_literal(
         parser, parser->token, ASTREE_VALUE_TYPE_STRING );
      goto cleanup;
   }

   /* Otherwise look for built-in statement. */

   for( i = 0 ; '\0' != gc_mpy_parser_tokens[i][0] ; i++ ) {
      if(
         strlen( gc_mpy_parser_tokens[i] ) == strlen( parser->token ) &&
         0 == strncmp(
            gc_mpy_parser_tokens[i], parser->token,
            strlen( gc_mpy_parser_tokens[i] ) )
      ) {
         debug_printf( 1, "found token: %s", parser->token );
         retval = g_mpy_parser_add_cbs[i]( parser );
         goto cleanup;
      }
   }

   /* Otherwise it must be a function call. */
   if( 0 < parser->token_sz ) {
      debug_printf( 1, "function call: %s", parser->token );
      retval = mpy_parser_add_node_call( parser, parser->token );
   }

cleanup:

   parser->token_sz = 0;
   parser->token[0] = '\0';

   return retval;
}

int mpy_parser_append_token( struct MPY_PARSER* parser, char c ) {
   parser->token[parser->token_sz++] = c;
   parser->token[parser->token_sz] = '\0';
   return 0;
}

int mpy_parser_parse( struct MPY_PARSER* parser, char c ) {
   int retval = 0,
      indent_diff = 0;

   if( parser->inside_indent && ' ' != c ) {
      parser->inside_indent = 0;
      debug_printf( 1, "ending indent at: %d", parser->this_line_indent );
      if( 0 == parser->indent_divisor ) {
         /* Set indent divisor based on first indented line. */
         parser->indent_divisor = parser->this_line_indent;
      }
      if( parser->this_line_indent < parser->last_line_indent ) {
         /* Indent reduced, so rewind upwards. */

         indent_diff = parser->last_line_indent;
         do {
            /* Rewind upwards each level, one by one. */
            debug_printf( 1, "rewinding up from indent %d to indent %d",
               indent_diff, indent_diff - parser->indent_divisor );
            do {
               parser->tree_node_idx =
                  g_astree_nodes[parser->tree_node_idx].parent;
               assert( 0 <= parser->tree_node_idx );
            } while(
               ASTREE_NODE_TYPE_SEQUENCE !=
               g_astree_nodes[parser->tree_node_idx].type
            );
            indent_diff -= parser->indent_divisor;
         } while( 0 < indent_diff );
      }
   }

   switch( c ) {

   case '\r':
   case '\n':
      retval = mpy_parser_parse_token( parser, c );
      parser->last_line_indent = parser->this_line_indent;
      parser->this_line_indent = 0;
      parser->inside_indent = 1;
      break;

   case '\'':
      /* TODO: Handle escaping. */
      if( MPY_PARSER_STATE_STRING_SQ == parser->state ) {
         retval = mpy_parser_parse_token( parser, c );
         parser->state = MPY_PARSER_STATE_NONE;
      } else if( MPY_PARSER_STATE_STRING == parser->state ) {
         retval = mpy_parser_append_token( parser, c );
      } else if( MPY_PARSER_STATE_NONE == parser->state ) {
         parser->state = MPY_PARSER_STATE_STRING_SQ;
      }
      break;

   case '"':
      /* TODO: Handle escaping. */
      if( MPY_PARSER_STATE_STRING == parser->state ) {
         retval = mpy_parser_parse_token( parser, c );
         parser->state = MPY_PARSER_STATE_NONE;
      } else if( MPY_PARSER_STATE_STRING_SQ == parser->state ) {
         retval = mpy_parser_append_token( parser, c );
      } else {
         parser->state = MPY_PARSER_STATE_STRING;
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
      } else if( MPY_PARSER_STATE_FUNC_PARMS == parser->state ) {
         /* Do nothing. */
      } else {
         /* Parse last token. */
         retval = mpy_parser_parse_token( parser, c );
      }
      break;

   case '>':
      if( MPY_PARSER_STATE_IF_COND == parser->state ) {
         retval = mpy_parser_insert_node_cond( parser, ASTREE_VALUE_TYPE_GT );
      } else {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );
      }
      break;

   case ',':
      /* TODO */
      break;

   case '+':
      /* TODO */
      break;

   case ':':
      if( MPY_PARSER_STATE_IF_COND == parser->state ) {
         /* Start if statement body. */
         debug_printf( 1, "conditions done; rewinding up to if statement" );
         while(
            ASTREE_NODE_TYPE_IF !=
            g_astree_nodes[parser->tree_node_idx].type
         ) {
            /* Rewind */
            parser->tree_node_idx =
               g_astree_nodes[parser->tree_node_idx].parent;
            assert( 0 <= parser->tree_node_idx );
         }
         parser->state = MPY_PARSER_STATE_NONE;
         retval = mpy_parser_add_node_sequence( parser );

      } else if(
         MPY_PARSER_STATE_NONE == parser->state
      ) {
         /* Start function definition body. */
         /* TODO: Merge with MPY_PARSER_STATE_IF_COND above. */
         debug_printf( 1,
            "function params done; rewinding up to def statement" );
         while(
            ASTREE_NODE_TYPE_FUNC_DEF !=
            g_astree_nodes[parser->tree_node_idx].type
         ) {
            /* Rewind */
            parser->tree_node_idx =
               g_astree_nodes[parser->tree_node_idx].parent;
            assert( 0 <= parser->tree_node_idx );
         }
         parser->state = MPY_PARSER_STATE_NONE;
         retval = mpy_parser_add_node_sequence( parser );
      } else {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );
      }
      break;

   case '(':
      if( MPY_PARSER_STATE_FUNC_DEF == parser->state ) {
         /* Parse function name token. */
         retval = mpy_parser_parse_token( parser, c );
         parser->state = MPY_PARSER_STATE_FUNC_PARMS;

      } else if(
         0 < parser->token_sz &&
         MPY_PARSER_STATE_NONE == parser->state
      ) {
         /* Probably a function invocation? */
         retval = mpy_parser_parse_token( parser, c );
         parser->state = MPY_PARSER_STATE_FUNC_PARMS;

      } else {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );
      }
      break;

   case ')':
      if( MPY_PARSER_STATE_FUNC_PARMS == parser->state ) {
         /* End function parms. */
         retval = mpy_parser_parse_token( parser, c );
         parser->state = MPY_PARSER_STATE_NONE;
      } else {
         /* Just a normal char. */
         retval = mpy_parser_append_token( parser, c );
      }
      break;

   default:
      retval = mpy_parser_append_token( parser, c );
      break;
   }

   return retval;
}

