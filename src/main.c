
#define MAUG_C
#include <maug.h>

#define RETROFLT_C
#include <retroflt.h>

#include "astree.h"
#include "parser.h"
#include "interp.h"

struct TEMPLATE_DATA {
   int init;
};

void mpy_loop( struct TEMPLATE_DATA* data ) {
   int input = 0;
   struct RETROFLAT_INPUT input_evt;

   /* Input */

   input = retroflat_poll_input( &input_evt );

   switch( input ) {
   case RETROFLAT_KEY_ESC:
      retroflat_quit( 0 );
      break;
   }

   /* Drawing */

   retroflat_draw_lock( NULL );

   retroflat_rect(
      NULL, RETROFLAT_COLOR_BLACK, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );

   retroflat_draw_release( NULL );
}

void mpy_dump_astree( struct ASTREE* tree, int16_t node_idx, int16_t depth ) {

   switch( tree->nodes[node_idx].type ) {
   case ASTREE_NODE_TYPE_SEQUENCE:
      debug_printf( 2, "\t%d: (idx: %d) sequence node", depth, node_idx );
      break;

   case ASTREE_NODE_TYPE_FUNC_DEF:
      debug_printf( 2, "\t%d: (idx: %d) function def node: %s", depth, node_idx,
         tree->nodes[node_idx].value.s );
      break;

   case ASTREE_NODE_TYPE_FUNC_CALL:
      debug_printf( 2, "\t%d: (idx: %d) function call node: %s", depth, node_idx,
         tree->nodes[node_idx].value.s );
      break;

   case ASTREE_NODE_TYPE_IF:
      debug_printf( 2, "\t%d: (idx: %d) if node", depth, node_idx );
      break;

   case ASTREE_NODE_TYPE_LITERAL:
      switch( tree->nodes[node_idx].value_type ) {
      case ASTREE_VALUE_TYPE_NONE:
         debug_printf( 2, "\t%d: (idx: %d) literal node: none", depth, node_idx );
         break;

      case ASTREE_VALUE_TYPE_INT:
         debug_printf( 2, "\t%d: (idx: %d) literal node: %d", depth, node_idx,
            tree->nodes[node_idx].value.i );
         break;

      case ASTREE_VALUE_TYPE_FLOAT:
         debug_printf( 2, "\t%d: (idx: %d) literal node: %f", depth, node_idx,
            tree->nodes[node_idx].value.f );
         break;

      case ASTREE_VALUE_TYPE_STRING:
         debug_printf( 2, "\t%d: (idx: %d) literal node: \"%s\"",
            depth, node_idx, tree->nodes[node_idx].value.s );
         break;
      }
      break;

   case ASTREE_NODE_TYPE_COND:
      switch( tree->nodes[node_idx].value_type ) {
      case ASTREE_VALUE_TYPE_GT:
         debug_printf( 2, "\t%d: (idx: %d) cond node: greater than", depth, node_idx );
         break;

      case ASTREE_VALUE_TYPE_EQ:
         debug_printf( 2, "\t%d: (idx: %d) cond node: equal to", depth, node_idx );
         break;
      }
      break;

   case ASTREE_NODE_TYPE_OP:
      switch( tree->nodes[node_idx].value_type ) {
      case ASTREE_VALUE_TYPE_ADD:
         debug_printf( 2, "\t%d: (idx: %d) op node: add", depth, node_idx );
         break;
      }
      break;

   case ASTREE_NODE_TYPE_VARIABLE:
      debug_printf( 2, "\t%d: (idx: %d) variable node: %s", depth, node_idx,
         tree->nodes[node_idx].value.s );
      break;

   default:
      debug_printf( 2, "\t%d: (idx: %d) unknown node", depth, node_idx );
      break;
   }

   if( 0 <= tree->nodes[node_idx].first_child ) {
      mpy_dump_astree( tree, tree->nodes[node_idx].first_child, depth + 1 );
   }

   if( 0 <= tree->nodes[node_idx].next_sibling ) {
      mpy_dump_astree( tree, tree->nodes[node_idx].next_sibling, depth );
   }
}

static int mpy_cli_f( const char* arg, char** p_script_path ) {
   if( 0 == strncmp( MAUG_CLI_SIGIL "f", arg, MAUG_CLI_SIGIL_SZ + 2 ) ) {
      /* The next arg must be the new var. */
   } else {
      assert( NULL == *p_script_path );
      *p_script_path = calloc( 1, strlen( arg ) + 1 );
      strncpy( *p_script_path, arg, strlen( arg ) );
   }
   return RETROFLAT_OK;
}

int main( int argc, char** argv ) {
   int retval = 0;
   struct RETROFLAT_ARGS args;
   struct TEMPLATE_DATA data;
   char* script_path = NULL;
   FILE* script_file = NULL;
   char* script_buf = NULL;
   int script_sz = 0,
      read_sz = 0;
   struct MPY_PARSER parser;
   struct ASTREE tree;

   /* === Setup === */

   retval = maug_add_arg(
      MAUG_CLI_SIGIL "f", MAUG_CLI_SIGIL_SZ + 2,
      "Script file to interpret.", 0,
      (maug_cli_cb)mpy_cli_f, NULL, &script_path );

   args.screen_w = 320;
   args.screen_h = 200;
   args.title = "mpy";
   args.assets_path = "";
   
   retval = retroflat_init( argc, argv, &args );
   if( RETROFLAT_OK != retval ) {
      goto cleanup;
   }

   assert( NULL != script_path );

   /* Read script file. */
   /* TODO: Convert this to import handler. */
   script_file = fopen( script_path, "r" );
   assert( NULL != script_file );
   fseek( script_file, 0, SEEK_END );
   script_sz = ftell( script_file );
   fseek( script_file, 0, SEEK_SET );
   script_buf = calloc( script_sz, 1 );
   assert( NULL != script_buf );
   read_sz = fread( script_buf, 1, script_sz, script_file );
   assert( script_sz == read_sz );
   fclose( script_file );

   if( 0 > astree_init( &tree ) ) {
      goto cleanup;
   }
   parser_parse_buffer( &parser, &tree, script_buf, script_sz );
   mpy_dump_astree( &tree, 0, 0 );

   data.init = 0;

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)mpy_loop, &data );

cleanup:

#ifndef MAUG_OS_WASM

   retroflat_shutdown( retval );

   astree_free( &tree );

   if( NULL != script_path ) {
      free( script_path );
   }

   if( NULL != script_buf ) {
      free( script_buf );
   }

#endif /* !MAUG_OS_WASM */

   return retval;
}
END_OF_MAIN()

