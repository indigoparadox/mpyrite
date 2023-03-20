
#define MAUG_C
#include <maug.h>

#define RETROFLT_C
#include <retroflt.h>

#include "astree.h"
#include "parser.h"
#include "interp.h"
#include "callback.h"

struct MPY_DATA {
   int init;
   struct INTERP* interp;
};

void mpy_loop( struct MPY_DATA* data ) {
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

   if( 0 > interp_tick( data->interp ) ) {
      /* Quit on nonzero return. */
      retroflat_quit( 0 );
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
   struct MPY_DATA data;
   char* script_path = NULL;
   FILE* script_file = NULL;
   char* script_buf = NULL;
   int script_sz = 0,
      read_sz = 0;
   struct MPY_PARSER parser;
   struct ASTREE tree;
   struct INTERP interp;

   /* === Setup === */

   retval = maug_add_arg(
      MAUG_CLI_SIGIL "f", MAUG_CLI_SIGIL_SZ + 2,
      "Script file to interpret.", 0,
      (maug_cli_cb)mpy_cli_f, NULL, &script_path );

   maug_mzero( &args, sizeof( struct RETROFLAT_ARGS ) );

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
   if( 0 > parser_parse_buffer( &parser, &tree, script_buf, script_sz ) ) {
      goto cleanup;
   }
   astree_dump( &tree, 0, 0 );

   interp_init( &interp, &tree );

   interp_set_var_str( &interp, "__name__", "__main__" );
   
   interp_set_func( &interp, "print", &mpy_print, INTERP_FUNC_CB );
   interp_set_func( &interp, "lock", &mpy_lock, INTERP_FUNC_CB );
   interp_set_func( &interp, "release", &mpy_release, INTERP_FUNC_CB );
   interp_set_func( &interp, "rect", &mpy_rect, INTERP_FUNC_CB );

   maug_mzero( &data, sizeof( struct MPY_DATA ) );
   data.init = 0;
   data.interp = &interp;

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

   interp_free( &interp );

#endif /* !MAUG_OS_WASM */

   return retval;
}
END_OF_MAIN()

