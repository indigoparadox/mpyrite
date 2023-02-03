
#define MAUG_C
#include <maug.h>

#define RETROFLT_C
#include <retroflt.h>

#include "astree.h"

#include "parser.h"

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

void mpy_dump_astree( int16_t node_idx, int16_t depth ) {

   switch( g_astree_nodes[node_idx].type ) {
   case ASTREE_NODE_TYPE_SEQUENCE:
      debug_printf( 2, "%d: sequence node", depth );
      break;

   case ASTREE_NODE_TYPE_FUNC_DEF:
      debug_printf( 2, "%d: function def node: %s", depth,
         g_astree_nodes[node_idx].value.s );
      break;

   case ASTREE_NODE_TYPE_IF:
      debug_printf( 2, "%d: if node", depth );
      break;

   case ASTREE_NODE_TYPE_LITERAL:
      switch( g_astree_nodes[node_idx].value_type ) {
      case ASTREE_VALUE_TYPE_NONE:
         debug_printf( 2, "%d: literal node: none", depth );
         break;

      case ASTREE_VALUE_TYPE_INT:
         debug_printf( 2, "%d: literal node: %d", depth,
            g_astree_nodes[node_idx].value.i );
         break;

      case ASTREE_VALUE_TYPE_FLOAT:
         debug_printf( 2, "%d: literal node: %f", depth,
            g_astree_nodes[node_idx].value.f );
         break;

      case ASTREE_VALUE_TYPE_STRING:
         debug_printf( 2, "%d: literal node: %s", depth,
            g_astree_nodes[node_idx].value.s );
         break;
      }
      break;

   case ASTREE_NODE_TYPE_COND:
      switch( g_astree_nodes[node_idx].value_type ) {
      case ASTREE_VALUE_TYPE_GT:
         debug_printf( 2, "%d: cond node: greater than", depth );
         break;
      }
      break;

   case ASTREE_NODE_TYPE_FUNC_PARM:
      switch( g_astree_nodes[node_idx].value_type ) {
      case ASTREE_VALUE_TYPE_NONE:
         debug_printf( 2, "%d: function parm node: none", depth );
         break;

      case ASTREE_VALUE_TYPE_STRING:
         debug_printf( 2, "%d: function parm node: \"%s\"", depth,
            g_astree_nodes[node_idx].value.s );
         break;

      case ASTREE_VALUE_TYPE_INT:
         debug_printf( 2, "%d: function parm node: %d", depth,
            g_astree_nodes[node_idx].value.i );
         break;

      case ASTREE_VALUE_TYPE_FLOAT:
         debug_printf( 2, "%d: function parm node: %lf", depth,
            g_astree_nodes[node_idx].value.f );
         break;
      }
      break;

   default:
      debug_printf( 2, "%d: unknown node", depth );
      break;
   }

   if( 0 <= g_astree_nodes[node_idx].first_child ) {
      mpy_dump_astree( g_astree_nodes[node_idx].first_child, depth + 1 );
   }

   if( 0 <= g_astree_nodes[node_idx].next_sibling ) {
      mpy_dump_astree( g_astree_nodes[node_idx].next_sibling, depth );
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
      read_sz = 0,
      i = 0;
   struct MPY_PARSER parser;

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

   g_astree_nodes = calloc( 1, sizeof( struct ASTREE_NODE ) );
   g_astree_nodes_sz++;
   memset( &parser, '\0', sizeof( struct MPY_PARSER ) );
   astree_node_intialize( 0, -1 );
   astree_set_node_type( 0, ASTREE_NODE_TYPE_SEQUENCE );
   for( i = 0 ; script_sz > i ; i++ ) {
      mpy_parser_parse( &parser, script_buf[i] );
   }
   mpy_dump_astree( 0, 0 );

   data.init = 0;

   /* === Main Loop === */

   retroflat_loop( (retroflat_loop_iter)mpy_loop, &data );

cleanup:

#ifndef MAUG_OS_WASM

   retroflat_shutdown( retval );

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

