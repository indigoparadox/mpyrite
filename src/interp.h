
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
int16_t interp_dbl_funcs( struct INTERP* interp );
int16_t interp_add_func_pc(
   struct INTERP* interp, const char* func_name, int16_t func_name_sz,
   int16_t func_pc );
int16_t interp_add_func_def( struct INTERP* interp, struct ASTREE_NODE* def );
int16_t interp_tick( struct INTERP* interp );

#endif /* !INTERP_H */

