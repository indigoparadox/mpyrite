
#ifndef INTERP_H
#define INTERP_H

#include "astree.h"

struct INTERP;

typedef int16_t (*interp_func_cb)( struct INTERP* );

#define INTERP_VAR_NAME_SZ_MAX 255

struct INTERP_VAR {
   char name[INTERP_VAR_NAME_SZ_MAX + 1];
   union ASTREE_NODE_VALUE value;  
};

struct INTERP {
   char** func_names;
   int16_t* func_idxs;
   int16_t func_sz;
   interp_func_cb* func_cbs;
   int16_t func_sz_max;
   int16_t pc;
   struct ASTREE* tree;
   /* TODO: Implement scope. */
   struct INTERP_VAR* vars;
   int16_t vars_sz;
};

int16_t interp_init( struct INTERP* interp, struct ASTREE* tree );
void interp_free( struct INTERP* interp );
int16_t interp_dbl_funcs( struct INTERP* interp );
int16_t interp_set_func_pc(
   struct INTERP* interp, const char* func_name, int16_t func_name_sz,
   int16_t func_pc );
int16_t interp_set_func_def( struct INTERP* interp, struct ASTREE_NODE* def );
int16_t interp_set_var_int(
   struct INTERP* interp, const char* var_name, int16_t var_name_sz,
   int16_t value );
int16_t interp_set_var_str(
   struct INTERP* interp, const char* var_name, int16_t var_name_sz,
   const char* value );
int16_t interp_tick( struct INTERP* interp );

#endif /* !INTERP_H */

