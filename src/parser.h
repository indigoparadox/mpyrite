
#ifndef PARSER_H
#define PARSER_H

#define MPY_PARSER_TOKEN_SZ_MAX 512

#define MPY_PARSER_STATES( f ) \
   f( MPY_PARSER_STATE_NONE,              0  ) \
   f( MPY_PARSER_STATE_FUNC_DEF,          1  ) \
   f( MPY_PARSER_STATE_FUNC_DEF_PARMS,    2  ) \
   f( MPY_PARSER_STATE_IF_COND,           3  ) \
   f( MPY_PARSER_STATE_STRING,            4  ) \
   f( MPY_PARSER_STATE_STRING_SQ,         5  ) \
   f( MPY_PARSER_STATE_COMMENT,           6  ) \
   f( MPY_PARSER_STATE_NUM,               7  ) \
   f( MPY_PARSER_STATE_FLOAT,             8  ) \
   f( MPY_PARSER_STATE_ASSIGN,            9  ) \
   f( MPY_PARSER_STATE_EQ_SIGN,           10 ) \
   f( MPY_PARSER_STATE_VAR,               11 ) \
   f( MPY_PARSER_STATE_FUNC_CALL_PARMS,   12 ) \
   f( MPY_PARSER_STATE_WHILE_COND,        13 )

#define mpy_parser_state( parser, new_state ) \
   debug_printf( 1, "new state: %s (prev: %s)", \
      gc_mpy_parser_state_tokens[new_state], \
      gc_mpy_parser_state_tokens[parser->state] ); \
   parser->prev_state = parser->state; \
   parser->state = new_state;

#define mpy_parser_node_idx( parser, new_idx ) \
   debug_printf( 1, "new node idx: %d (prev: %d)", \
      new_idx, parser->tree_node_idx ); \
   parser->tree_node_idx = new_idx;

struct MPY_PARSER {
   uint8_t inside_indent;
   int this_line_indent;
   int prev_line_indent;
   int indent_divisor;
   int state;
   int prev_state;
   int16_t tree_node_idx;
   char token[MPY_PARSER_TOKEN_SZ_MAX];
   uint16_t token_sz;
   struct ASTREE* tree;
   int line_num;
};

typedef int (*mpy_parser_add_cb)( struct MPY_PARSER* parser );

int mpy_parser_is_numeric( const char* token, int token_sz );
void mpy_parser_rewind( struct MPY_PARSER* parser, int node_type );
int mpy_parser_parse_token( struct MPY_PARSER* parser, char trig_c );
int mpy_parser_parse_c( struct MPY_PARSER* parser, char c );
int parser_parse_buffer(
   struct MPY_PARSER* parser, struct ASTREE* tree,
   const char* buffer, int buffer_sz );

#define MPY_PARSER_STATEMENTS( f ) \
   f( "def", ASTREE_NODE_TYPE_FUNC_DEF, mpy_parser_add_node_def ) \
   f( "if", ASTREE_NODE_TYPE_IF, mpy_parser_add_node_if ) \
   f( "while", ASTREE_NODE_TYPE_WHILE, mpy_parser_add_node_while )

#ifdef PARSER_C

#  define MPY_PARSER_STATEMENTS_TOKENS( token, type, add_cb ) token,

MAUG_CONST char* gc_mpy_parser_tokens[] = {
   MPY_PARSER_STATEMENTS( MPY_PARSER_STATEMENTS_TOKENS )
   ""
};

#  define MPY_PARSER_STATEMENTS_TYPES( token, type, add_cb ) type,

MAUG_CONST uint8_t gc_mpy_parser_types[] = {
   MPY_PARSER_STATEMENTS( MPY_PARSER_STATEMENTS_TYPES )
   0
};

#  define MPY_PARSER_STATES_VALS( name, val ) \
   MAUG_CONST uint16_t name = val;

MPY_PARSER_STATES( MPY_PARSER_STATES_VALS )

#  define MPY_PARSER_STATES_TOKENS( name, val ) #name,

MAUG_CONST char* gc_mpy_parser_state_tokens[] = {
   MPY_PARSER_STATES( MPY_PARSER_STATES_TOKENS )
   ""
};

#else

extern MAUG_CONST char* gc_mpy_parser_tokens[];
extern MAUG_CONST uint8_t gc_mpy_parser_types[];

#endif /* PARSER_C */

#endif /* !PARSER_H */

