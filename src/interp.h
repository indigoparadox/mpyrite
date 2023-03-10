
#ifndef INTERP_H
#define INTERP_H

#include "astree.h"

struct INTERP;

typedef int16_t (*interp_func_cb)( struct INTERP* );

#define INTERP_FUNC_NAME_SZ_MAX 255
#define INTERP_VAR_NAME_SZ_MAX 255
#define INTERP_FUNC_ARGS_SZ_MAX 24
#define INTERP_FUNC_ARG_NAME_SZ_MAX 255

#define INTERP_FUNC_PC 0
#define INTERP_FUNC_CB 1

struct INTERP_STACK_ITEM {
   uint8_t type;
   union ASTREE_NODE_VALUE value;  
};

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
   int16_t prev_pc;
   struct ASTREE* tree;
   /* TODO: Implement scope. */
   struct INTERP_VAR* vars;
   uint32_t vars_sz;
   uint32_t vars_sz_max;
   struct INTERP_STACK_ITEM* stack;
   uint32_t stack_sz;
   uint32_t stack_sz_max;
};

#define interp_set_pc( interp, new_pc ) \
   interp->prev_pc = interp->pc; \
   interp->pc = new_pc;

/**
 * \brief Determine if this tree node is being executed from a parent
 *        (first-pass) or returning from a child (non-first-pass).
 */
#define interp_is_first_pass( interp, node ) \
   (node->prev_sibling == interp->prev_pc || node->parent == interp->prev_pc)

int16_t interp_init( struct INTERP* interp, struct ASTREE* tree );
void interp_free( struct INTERP* interp );
int16_t interp_set_func(
   struct INTERP* interp, const char* func_name, void* func_pc_cb,
   uint8_t func_type );
int16_t interp_set_func_cb(
   struct INTERP* interp, const char* func_name, interp_func_cb cb );
int16_t interp_set_var_int(
   struct INTERP* interp, const char* var_name, int16_t value );
int16_t interp_set_var_str(
   struct INTERP* interp, const char* var_name, const char* value );
int16_t interp_call_func( struct INTERP* interp, const char* name );
int16_t interp_stack_push_str( struct INTERP* interp, const char* value );
int16_t interp_stack_push_int( struct INTERP* interp, int16_t value );
struct INTERP_STACK_ITEM* interp_stack_pop( struct INTERP* interp );
int16_t interp_tick( struct INTERP* interp );

#endif /* !INTERP_H */

