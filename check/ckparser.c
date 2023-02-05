
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
   struct ASTREE tree;

   astree_init( &tree );
   parser_parse_buffer(
      &parser, &tree, test_buffer_a, strlen( test_buffer_a ) );

   ck_assert_int_eq( parser.this_line_indent, 0 );

   iter = astree_node( &tree, 0 );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* def main */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF );

   /* ( foo ) */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* : */
   iter = astree_node( &tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* if */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_IF );

   /* > */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_COND );

   /* foo */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* 3 */
   iter = astree_node( &tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* : */
   iter = astree_node( &tree, iter->parent );
   iter = astree_node( &tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* print */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* + */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_OP );

   /* "hello! " */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* foo */
   iter = astree_node( &tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* if */
   iter = astree_node( &tree, 0 );
   iter = astree_node( &tree, iter->first_child );
   iter = astree_node( &tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_IF );

   /* == */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_COND );

   /* '__main__' */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* __name__ */
   iter = astree_node( &tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* : */
   iter = astree_node( &tree, iter->parent );
   iter = astree_node( &tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* main */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* 123 */
   iter = astree_node( &tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   astree_free( &tree );

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

