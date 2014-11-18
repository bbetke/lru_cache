//  main.cpp
//  lru_cache
//
//  Created by bbetke on 18/11/2014.
//  Copyright (c) 2014 bbetke. All rights reserved.

#include <lru_cache.h>

#include <iostream>


int main( int argc, const char * argv[] )
{
    lru_cache< std::string, int > c( 5 );
    c.put( "test_1", 3 );
    c.put( "test_2", 9 );
    c.put( "test_3", 8 );
    c.put( "test_4", 10 );
    c.put( "test_5", 2 );
    c.put( "test_6", 6 );
    
    c.put( "test_7", 7 );
    c.put( "test_11", 11 );
    c.put( "test_11", 12 );
    c.put( "test_11", 13 );
    c.put( "test_11", 14 );
    
    c.put( "test_22", 22 );
    c.put( "test_23", 23 );
    c.put( "test_24", 24 );
    c.put( "test_25", 25 );
    c.put( "test_26", 26 );
    
    c.put( "test_11", 11 );
    c.put( "test_11", 12 );
    c.put( "test_11", 13 );
    
    if( !c.get( "test_0" ) )
    {
        std::cout << "not valid\n";
    }
    
    return 0;
}
