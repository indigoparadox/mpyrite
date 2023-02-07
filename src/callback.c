
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
   RETROFLAT_COLOR color;

   item = interp_stack_pop( interp );
   assert( NULL != item );
   h = item->value.i;

   item = interp_stack_pop( interp );
   assert( NULL != item );
   w = item->value.i;

   item = interp_stack_pop( interp );
   assert( NULL != item );
   y = item->value.i;

   item = interp_stack_pop( interp );
   assert( NULL != item );
   x = item->value.i;

   item = interp_stack_pop( interp );
   assert( NULL != item );
   c = item->value.i;

   /* TODO: Color LUT. */
   switch( c ) {
   case 2:
      color = RETROFLAT_COLOR_RED;
      break;
   default:
      color = RETROFLAT_COLOR_BLACK;
      break;
   }

   retroflat_rect( NULL, color, x, y, w, h, RETROFLAT_FLAGS_FILL );

   return 0;
}

