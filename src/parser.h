
#ifndef PARSER_H
#define PARSER_H

#define MPY_PARSER_TOKEN_SZ_MAX 512

#define MPY_PARSER_STATE_NONE       0
#define MPY_PARSER_STATE_FUNC_DEF   1
#define MPY_PARSER_STATE_FUNC_PARMS 2
#define MPY_PARSER_STATE_IF_COND    3
#define MPY_PARSER_STATE_STRING     4
#define MPY_PARSER_STATE_STRING_SQ  5
#define MPY_PARSER_STATE_COMMENT    6

struct MPY_PARSER {
   uint8_t inside_indent;
   int this_line_indent;
   int last_line_indent;
   int indent_divisor;
   int state;
   char last_c;
   int16_t tree_node_idx;
   char token[MPY_PARSER_TOKEN_SZ_MAX];
   uint16_t token_sz;
   struct ASTREE* tree;
};

typedef int (*mpy_parser_add_cb)( struct MPY_PARSER* parser );

int mpy_parser_is_numeric( const char* token, int token_sz );
int mpy_parser_parse_token( struct MPY_PARSER* parser, char trig_c );
int mpy_parser_parse_c( struct MPY_PARSER* parser, char c );
int parser_parse_buffer(
   struct MPY_PARSER* parser, struct ASTREE* tree,
   const char* buffer, int buffer_sz );

#define MPY_PARSER_STATEMENTS( f ) \
   f( "def", ASTREE_NODE_TYPE_FUNC_DEF, mpy_parser_add_node_def ) \
   f( "if", ASTREE_NODE_TYPE_IF, mpy_parser_add_node_if )

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

#else

extern MAUG_CONST char* gc_mpy_parser_tokens[];
extern MAUG_CONST uint8_t gc_mpy_parser_types[];

#endif /* PARSER_C */

#endif /* !PARSER_H */

