
#ifndef INTERP_H
#define INTERP_H

#include "astree.h"

struct INTERP;

typedef int16_t (*interp_func_cb)( struct INTERP* );

#define INTERP_FUNC_NAME_SZ_MAX 255
#define INTERP_VAR_NAME_SZ_MAX 255

#define INTERP_FUNC_PC 0
#define INTERP_FUNC_CB 1

struct INTERP_VAR {
   char name[INTERP_VAR_NAME_SZ_MAX + 1];
   uint8_t type;
   union ASTREE_NODE_VALUE value;  
};

union INTERP_FUNC_EXEC {
   int16_t pc;
   interp_func_cb cb;
};

struct INTERP_FUNC {
   char name[INTERP_FUNC_NAME_SZ_MAX];
   uint8_t type;
   union INTERP_FUNC_EXEC exe;
};

struct INTERP {
   struct INTERP_FUNC* funcs;
   uint32_t funcs_sz;
   uint32_t funcs_sz_max;
   int16_t pc;
   struct ASTREE* tree;
   /* TODO: Implement scope. */
   struct INTERP_VAR* vars;
   uint32_t vars_sz;
   uint32_t vars_sz_max;
};

int16_t interp_init( struct INTERP* interp, struct ASTREE* tree );
void interp_free( struct INTERP* interp );
int16_t interp_dbl_funcs( struct INTERP* interp );
int16_t interp_set_func_pc(
   struct INTERP* interp, const char* func_name, int16_t func_pc );
int16_t interp_set_func_def( struct INTERP* interp, struct ASTREE_NODE* def );
int16_t interp_set_var_int(
   struct INTERP* interp, const char* var_name, int16_t value );
int16_t interp_set_var_str(
   struct INTERP* interp, const char* var_name, const char* value );
int16_t interp_tick( struct INTERP* interp );

#endif /* !INTERP_H */

