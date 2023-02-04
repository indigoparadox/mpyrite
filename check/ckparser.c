
#include <check.h>
#include <stdlib.h>

#include <maug.h>

#include <astree.h>
#include <parser.h>

const char test_buffer_a[] =
   "\n" \
   "# Test comment.\n" \
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
   struct ASTREE_NODE* iter = NULL;

   astree_init();

   memset( &parser, '\0', sizeof( struct MPY_PARSER ) );
   for( i = 0 ; strlen( test_buffer_a ) > i ; i++ ) {
      mpy_parser_parse( &parser, test_buffer_a[i] );
   }

   ck_assert_int_eq( parser.this_line_indent, 0 );

   iter = astree_get_node( 0 );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* def main */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF );

   /* ( foo ) */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* : */
   iter = astree_get_node( iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* if */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_IF );

   /* > */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_COND );

   /* foo */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* 3 */
   iter = astree_get_node( iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* : */
   iter = astree_get_node( iter->parent );
   iter = astree_get_node( iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* print */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* + */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_OP );

   /* "hello! " */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* foo */
   iter = astree_get_node( iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* if */
   iter = astree_get_node( 0 );
   iter = astree_get_node( iter->first_child );
   iter = astree_get_node( iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_IF );

   /* == */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_COND );

   /* '__main__' */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* __name__ */
   iter = astree_get_node( iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* : */
   iter = astree_get_node( iter->parent );
   iter = astree_get_node( iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* main */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* 123 */
   iter = astree_get_node( iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   astree_free();

}
END_TEST

Suite* parser_suite( void ) {
   Suite* s;
   TCase* tc_parser;

   s = suite_create( "parser" );

   /* Core test case */
   tc_parser = tcase_create( "parser" );

   tcase_add_test( tc_parser, check_parser_a ); 

   suite_add_tcase( s, tc_parser );

   return s;
}

