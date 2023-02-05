
#ifndef INTERP_H
#define INTERP_H

#include "astree.h"

struct INTERP;

typedef int16_t (*interp_func_cb)( struct INTERP* );

struct INTERP {
   char** func_names;
   int16_t* func_idxs;
   int16_t func_sz;
   interp_func_cb* func_cbs;
   int16_t func_sz_max;
   int16_t pc;
   struct ASTREE* tree;
};

int16_t interp_init( struct INTERP* interp, struct ASTREE* tree );
void interp_free( struct INTERP* interp );
int16_t interp_tick( struct INTERP* interp );

#endif /* !INTERP_H */

