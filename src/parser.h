
#ifndef PARSER_H
#define PARSER_H

#define MPY_PARSER_TOKEN_SZ_MAX 512

struct MPY_PARSER {
   struct ASTREE_NODE* tree;
   char token[MPY_PARSER_TOKEN_SZ_MAX];
   uint16_t token_sz;
};

int mpy_parser_parse( struct MPY_PARSER* parser, char c );

#define MPY_PARSER_STATEMENTS( f ) \
   f( "def", ASTREE_NODE_TYPE_FUNC_DEF )

#ifdef PARSER_C

#  define MPY_PARSER_STATEMENTS_TOKENS( token, type ) token,

MAUG_CONST char* g_parser_tokens[] = {
   MPY_PARSER_STATEMENTS( MPY_PARSER_STATEMENTS_TOKENS )
   ""
};

#  define MPY_PARSER_STATEMENTS_TYPES( token, type ) type,

MAUG_CONST uint8_t g_parser_types[] = {
   MPY_PARSER_STATEMENTS( MPY_PARSER_STATEMENTS_TYPES )
   0
};

#else

extern MAUG_CONST char* g_parser_tokens[];
extern MAUG_CONST uint8_t g_parser_types[];

#endif /* PARSER_C */

#endif /* !PARSER_H */

