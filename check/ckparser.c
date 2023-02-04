
#include <check.h>

const char test_buffer_a[] =
   "\n" \
   "def main( foo ):\n" \
   "    if foo > 3:\n" \
   "        print( \"hello! \" + foo )\n" \
   "\n" \
   "if '__main__' == __name__:\n" \
   "    main( 123 )\n" \
   "\n";

START_TEST( check_parser_a ) {
   int16_t i = 0;
   struct MPY_PARSER parser;

   for( i = 0 ; strlen( test_buffer_a ) > i ; i++ ) {
      mpy_parser_parse( &parser, test_buffer_a[i] );
   }

   ck_assert_int_eq( parser.this_line_indent, 0 );
}
END_TEST

Suite* parser_suite( void ) {
   Suite* s;
   TCase* tc_parser;

   s = suite_create( "parser" );

   /* Core test case */
   tc_parser = tcase_create( "parser" );

   tcase_add_test( tc_parser, check_parser_a ); 

   return s;
}

