
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
   "def main( foo, fii, faa ):\n" \
   "    while 1:\n" \
   "        realplus = realplus + 1\n" \
   "        if foo > 3:\n" \
   "            x = \"the x\"\n" \
   "            print( \"hello! \" + foo )\n" \
   "\n" \
   "if '__main__' == __name__:\n" \
   "    main( 123, \"test\" )\n" \
   "\n";

const char test_buffer_assign_op[] = "plusop = plusop + 1\n";

const char test_buffer_while[] = "while 1:\n    whileplus = whileplus + 3\n";

const char test_buffer_while_gt[] = \
   "while 3 > whilegt:\n" \
   "    whilegt = whilegt + 1\n";

const char test_buffer_if_else[] = \
   "if x > 3:\n" \
   "    ielse = 0\n" \
   "else:\n" \
   "    ielse = 6\n";

const char test_buffer_func_def[] = "def main( foo ):\n";

const char test_buffer_func_def_mult[] = "def main( foo, fii, faa ):\n";

const char test_buffer_assign[] = "foo = \"test\"\n";

const char test_buffer_func_call[] = "rect( \"red\", 0, 0, 320, 200 )";

const char test_buffer_func_call_op[] = "print_nums( 3 + 2, 4 + 5 )";

const char test_buffer_func_call_mult[] =
   "lock()\n" \
   "rect( \"red\", 0, 0, 320, 200 )\n" \
   "little_rect( 0, 0, \"blue\" )\n" \
   "print( \"hello!\" )\n" \
   "release()\n";

START_TEST( check_parser_func_call ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_func_call,
      strlen( test_buffer_func_call ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* rect( */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* "red", */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 0, */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 0, */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 320, */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 200 ) */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );
}
END_TEST

START_TEST( check_parser_func_call_op ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_func_call_op,
      strlen( test_buffer_func_call_op ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* print( */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* + */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_OP );

   /* 3 */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 2 */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* + */
   iter = astree_node( &g_tree, iter->parent );
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_OP );

   /* 4 */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 5 */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );
}
END_TEST

START_TEST( check_parser_func_call_mult ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_func_call_mult,
      strlen( test_buffer_func_call_mult ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* lock() */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* rect( */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* "red", */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 0, */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 0, */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 320, */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 200 ) */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* little_rect( */
   iter = astree_node( &g_tree, iter->parent );
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_CALL );

   /* 0, */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* 0, */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* "blue", */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );
}
END_TEST

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
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF_PARM );

   /* : */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   ck_assert_int_eq( g_parser.this_line_indent, 0 );
   ck_assert_int_eq( g_parser.tree_node_idx, 3 );
}
END_TEST

START_TEST( check_parser_func_def_mult ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_func_def_mult,
      strlen( test_buffer_func_def_mult ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* def main */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF );

   /* ( foo, */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF_PARM );

   /* fii, */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF_PARM );

   /* faa ) */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF_PARM );

   /* : */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   ck_assert_int_eq( g_parser.this_line_indent, 0 );
   ck_assert_int_eq( g_parser.tree_node_idx, 5 );
}
END_TEST

START_TEST( check_parser_assign ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_assign,
      strlen( test_buffer_assign ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* = */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_ASSIGN );

   /* foo */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* "test" */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   ck_assert_int_eq( g_parser.this_line_indent, 0 );
   ck_assert_int_eq( g_parser.tree_node_idx, 0 );
}
END_TEST

START_TEST( check_parser_while ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_while,
      strlen( test_buffer_while ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* while */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_WHILE );

   /* 1 */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* : */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* = */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_ASSIGN );

   /* whileplus */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* + */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_OP );

   /* whileplus */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* 3 */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );
}
END_TEST

START_TEST( check_parser_while_gt ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_while_gt,
      strlen( test_buffer_while_gt ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* while */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_WHILE );

   /* > */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_COND );

   /* 3 */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* whilegt */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* : */
   iter = astree_node( &g_tree, iter->parent );
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* = */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_ASSIGN );

   /* whilegt */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* + */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_OP );

   /* whilegt */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* 1 */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );
}
END_TEST

START_TEST( check_parser_if_else ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_if_else,
      strlen( test_buffer_if_else ) );

   astree_dump( &g_tree, 0, 0 );

   ck_assert_int_eq( retval, 0 );

   iter = astree_node( &g_tree, 0 );
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

   /* x */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* 3 */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* : */
   iter = astree_node( &g_tree, iter->parent );
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

}
END_TEST

START_TEST( check_parser_assign_op ) {
   struct ASTREE_NODE* iter = NULL;
   int16_t retval = 0;
   
   retval = parser_parse_buffer(
      &g_parser, &g_tree, test_buffer_assign_op,
      strlen( test_buffer_assign_op ) );
   ck_assert_int_eq( retval, 0 );

   astree_dump( &g_tree, 0, 0 );

   iter = astree_node( &g_tree, 0 );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* = */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_ASSIGN );

   /* x */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* + */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_OP );

   /* x */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* 1 */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );
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

   /* ( foo */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF_PARM );

   /* , fii */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF_PARM );

   /* , faa ) */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_FUNC_DEF_PARM );

   /* : */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* while */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_WHILE );

   /* 1 */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* : */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_SEQUENCE );

   /* = */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_ASSIGN );

   /* realplus */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* + */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_OP );

   /* realplus */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* 3 */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* if */
   iter = astree_node( &g_tree, iter->parent );
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->parent );
   ck_assert_ptr_ne( iter, NULL );
   iter = astree_node( &g_tree, iter->next_sibling );
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

   /* = */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_ASSIGN );
   
   /* x */
   iter = astree_node( &g_tree, iter->first_child );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_VARIABLE );

   /* 9 */
   iter = astree_node( &g_tree, iter->next_sibling );
   ck_assert_ptr_ne( iter, NULL );
   ck_assert_int_eq( iter->type, ASTREE_NODE_TYPE_LITERAL );

   /* print */
   iter = astree_node( &g_tree, iter->parent );
   iter = astree_node( &g_tree, iter->next_sibling );
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

   /* , "test" */
   iter = astree_node( &g_tree, iter->next_sibling );
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
   TCase* tc_func,
      * tc_assign,
      * tc_control,
      * tc_integ;

   s = suite_create( "parser" );

   /* Core test case */
   tc_func = tcase_create( "function" );
   tc_assign = tcase_create( "assign" );
   tc_control = tcase_create( "control" );
   tc_integ = tcase_create( "integration" );

   tcase_add_test( tc_func, check_parser_func_call_mult );
   tcase_add_test( tc_func, check_parser_func_call_op );
   tcase_add_test( tc_func, check_parser_func_call );
   tcase_add_test( tc_func, check_parser_func_def_mult ); 
   tcase_add_test( tc_func, check_parser_func_def ); 
   tcase_add_test( tc_assign, check_parser_assign ); 
   tcase_add_test( tc_control, check_parser_while );
   tcase_add_test( tc_control, check_parser_while_gt );
   tcase_add_test( tc_control, check_parser_if_else );
   tcase_add_test( tc_assign, check_parser_assign_op );
   tcase_add_test( tc_integ, check_parser_a ); 

   tcase_add_checked_fixture( tc_func, parser_setup, parser_teardown );
   tcase_add_checked_fixture( tc_assign, parser_setup, parser_teardown );
   tcase_add_checked_fixture( tc_control, parser_setup, parser_teardown );
   tcase_add_checked_fixture( tc_integ, parser_setup, parser_teardown );

   suite_add_tcase( s, tc_func );
   suite_add_tcase( s, tc_assign );
   suite_add_tcase( s, tc_control );
   suite_add_tcase( s, tc_integ );

   return s;
}

