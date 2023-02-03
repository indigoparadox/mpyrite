
#include "maug.h"

#include "parser.h"

#include "astree.h"

int mpy_parser_parse( struct MPY_PARSER* parser, char c ) {
   int retval = 0;

   switch( c ) {

   default:
      parser->token[parser->token_sz++] = c;
      break;
   }

   return retval;
}

