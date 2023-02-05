
#include <check.h>
#include <stdlib.h>

#include <maug.h>

#include <astree.h>
#include <parser.h>

struct MPY_PARSER g_parser;
struct ASTREE g_tree;

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

const char test_buffer_func_def[] = "def main( foo ):\n";

START_TEST( check_parser_func_def ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_func_def,
      strlen( test_buffer_func_def ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* def main */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF );

   /* ( foo ) */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* : */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   ck_assert_int_eq( g_parser.this_line_indent, 0 );
   ck_assert_int_eq( g_parser.tree_node_idx, 3 );
}
END_TEST

START_TEST( check_parser_a ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;

   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_a, strlen( test_buffer_a ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   ck_assert_int_eq( g_parser.this_line_indent, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* def main */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF );

   /* ( foo ) */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* : */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* if */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_IF );

   /* > */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_COND );

   /* foo */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* 3 */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* : */
   iter = astree_node( &g_tree, iter->parent );
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* print */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* + */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_OP );

   /* "hello! " */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* foo */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* if */
   iter = astree_node( &g_tree, 0 ); /* seq */
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->first_child ); /* main func */
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->next_sibling ); /* if */
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_IF );

   /* == */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_COND );

   /* '__main__' */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* __name__ */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* : */
   iter = astree_node( &g_tree, iter->parent );
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* main */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* 123 */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );
}
END_TEST

static void parser_setup() {
   astree_init( &g_tree );
}

static void parser_teardown() {
   astree_free( &g_tree );
}

Suite* parser_suite( void ) {
   Suite* s;
   TCase* tc_parser;

   s = suite_create( "parser" );

   /* Core test case */
   tc_parser = tcase_create( "parser" );

   tcase_add_test( tc_parser, check_parser_func_def ); 
   tcase_add_test( tc_parser, check_parser_a ); 

   tcase_add_checked_fixture( tc_parser, parser_setup, parser_teardown );

   suite_add_tcase( s, tc_parser );

   return s;
}

