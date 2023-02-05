
#include <check.h>
#include <stdlib.h>

#include <maug.h>

#include <astree.h>

struct ASTREE g_tree;

START_TEST( check_astree_node_find_free ) {
   int16_t node_id = -1;

   node_id = astree_node_find_free( &g_tree );

   ck_assert_int_eq( node_id, 1 );
}
END_TEST

START_TEST( check_astree_node_add ) {
   int16_t node_id = -1;
   int i = 0,
    j = 0;

   /* Test adding children. */
   for( i = 0 ; i < _i ; i++ ) {
      node_id = astree_node_add_child( &g_tree, i );

      ck_assert_int_eq( node_id, i + 1 );
      ck_assert_int_eq( astree_node( &g_tree, node_id )->parent, i );
      ck_assert_int_eq( astree_node( &g_tree, node_id )->first_child, -1 );
      ck_assert_int_eq( astree_node( &g_tree, node_id )->next_sibling, -1 );
   }

   /* Test adding siblings. */
   for( j = i ; j < (_i * 2) ; j++ ) {
      node_id = astree_node_add_child( &g_tree, i );

      ck_assert_int_eq( node_id, j + 1 );
      ck_assert_int_eq( astree_node( &g_tree, node_id )->parent, i );
      ck_assert_int_eq( astree_node( &g_tree, node_id )->first_child, -1 );
      ck_assert_int_eq( astree_node( &g_tree, node_id )->next_sibling, -1 );
   }
}
END_TEST

static void astree_setup() {
   astree_init( &g_tree );
}

static void astree_teardown() {
   astree_free( &g_tree );
}

Suite* astree_suite( void ) {
   Suite* s;
   TCase* tc_astree;

   s = suite_create( "astree" );

   /* Core test case */
   tc_astree = tcase_create( "astree" );

   tcase_add_test( tc_astree, check_astree_node_find_free ); 
   tcase_add_loop_test( tc_astree, check_astree_node_add, 0, 5 ); 

   tcase_add_checked_fixture( tc_astree, astree_setup, astree_teardown );

   suite_add_tcase( s, tc_astree );

   return s;
}

