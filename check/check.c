
#include <check.h>

#define main_add_test_proto( suite_name ) \
   Suite* suite_name ## _suite();

#define main_add_test( suite_name ) \
   for( i = 0 ; argc > i ; i++ ) { \
      if( \
         1 < argc && \
         0 != strncmp( #suite_name, argv[i], strlen( #suite_name ) ) \
      ) { \
         continue; \
      } \
      Suite* s_ ## suite_name = suite_name ## _suite(); \
      SRunner* sr_ ## suite_name = srunner_create( s_ ## suite_name ); \
      if( nofork ) { \
         srunner_set_fork_status( sr_ ## suite_name, CK_NOFORK ); \
      } \
      srunner_run_all( sr_ ## suite_name, verbose ); \
      number_failed += srunner_ntests_failed( sr_ ## suite_name ); \
      srunner_free( sr_ ## suite_name ); \
      break; \
   }

main_add_test_proto( parser )

int main( int argc, char* argv[] ) {
   int number_failed = 0,
      i = 0,
      nofork = 0,
      verbose = CK_VERBOSE;

   main_add_test( parser );

   return( number_failed == 0 ) ? 0 : 1;
}

