
#include <maug.h>
#include <retroflt.h>

#include "astree.h"
#include "parser.h"
#include "interp.h"
#include "callback.h"

int16_t mpy_print( struct INTERP* interp ) {
   struct INTERP_STACK_ITEM* item = NULL;

   item = interp_stack_pop( interp );
   assert( NULL != item );

   printf( "%s\n", item->value.s );

   return 0;
}

int16_t mpy_lock( struct INTERP* interp ) {
   retroflat_draw_lock( NULL );
   return 0;
}

int16_t mpy_release( struct INTERP* interp ) {
   retroflat_draw_release( NULL );
   return 0;
}

int16_t mpy_rect( struct INTERP* interp ) {
   struct INTERP_STACK_ITEM* item = NULL;
   int16_t x = 0,
      y = 0,
      w = 0,
      h = 0,
      c = 0;

   item = interp_stack_pop( interp );
   assert( NULL != item );

   item = interp_stack_pop( interp );
   assert( NULL != item );

   item = interp_stack_pop( interp );
   assert( NULL != item );

   item = interp_stack_pop( interp );
   assert( NULL != item );

   item = interp_stack_pop( interp );
   assert( NULL != item );

   retroflat_rect(
      NULL, RETROFLAT_COLOR_BLACK, 0, 0,
      retroflat_screen_w(), retroflat_screen_h(),
      RETROFLAT_FLAGS_FILL );

   return 0;
}

