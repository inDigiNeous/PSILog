#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../logger.h"

TEST_CASE( "stupid/1=2", "Prove that one equals 2" ){
	int one = 1;
	REQUIRE( one == 2 );
}
