//  tests for lru_cache
//
//  Created by bbetke on 18/11/2014.
//  Copyright (c) 2014 bbetke. All rights reserved.

#include "../../lru_cache/lru_cache.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE main

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( lru_cache_tests )

// Tests for the requirements given

// A cache item consists of a KEY and a VALUE.
// When instantiated, the cache has a fixed limit on the number of items that can be stored.
BOOST_AUTO_TEST_CASE( test_capacity )
{
    lru_cache< std::string, int > cache( 5 );
    cache.put( "test_1", 3 );
    cache.put( "test_2", 9 );
    cache.put( "test_3", 8 );
    cache.put( "test_4", 10 );
    cache.put( "test_5", 2 );
    cache.put( "test_6", 6 );
    cache.put( "test_7", 27 );
    cache.put( "test_8", 28 );
    cache.put( "test_9", 29 );
    cache.put( "test_10", 11 );
    
    // expected capacity is 5
    BOOST_CHECK_EQUAL( cache.size(), 5 );
}

// The cache cannot store duplicate keys.
BOOST_AUTO_TEST_CASE( test_no_duplicates )
{
    lru_cache< std::string, int > cache( 5 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 1 );
    
    // expected capacity is 1 as these are all duplicates
    BOOST_CHECK_EQUAL( cache.size(), 1 );
}

// If a ‘put’ operation is performed using a KEY that already exists in the cache, the associated VALUE for that KEY is updated.
BOOST_AUTO_TEST_CASE( test_update_existing )
{
    lru_cache< std::string, int > cache( 5 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 1 );
    cache.put( "test_1", 2 );
    
    // expected capacity is 1 as these are all duplicates
    BOOST_CHECK_EQUAL( cache.size(), 1 );
    
    // expect the key to have been updated
    boost::optional< int > value = cache.get( "test_1" );
    BOOST_CHECK_EQUAL( value.get(), 2 );
}

// If an item is inserted when the cache is full, then
// the least-recently-used item is evicted prior to inserting the new item.
BOOST_AUTO_TEST_CASE( test_lru )
{
    lru_cache< std::string, int > cache( 5 );
    cache.put( "test_1", 3 );
    cache.put( "test_2", 9 );
    cache.put( "test_3", 8 );
    cache.put( "test_4", 10 );
    cache.put( "test_5", 2 );
    cache.put( "test_6", 6 );
    
    // expected capacity is 5
    BOOST_CHECK_EQUAL( cache.size(), 5 );
    
    // expect the least recently used key "test_1" to have been evicted
    boost::optional< int > value_1 = cache.get( "test_1" );
    boost::optional< int > value_2 = cache.get( "test_2" );
    boost::optional< int > value_3 = cache.get( "test_3" );
    boost::optional< int > value_4 = cache.get( "test_4" );
    boost::optional< int > value_5 = cache.get( "test_5" );
    boost::optional< int > value_6 = cache.get( "test_6" );
    
    // when trying to retreive the value of key "test_1" it should fail on optional as it has been evicted
    BOOST_REQUIRE( !value_1 );
    BOOST_REQUIRE( value_2 );
    BOOST_REQUIRE( value_3 );
    BOOST_REQUIRE( value_4 );
    BOOST_REQUIRE( value_5 );
    BOOST_REQUIRE( value_6 );
}

// Both ‘put’ and ‘get’ operations will make the item the most-recently-used item.
BOOST_AUTO_TEST_CASE( test_lru_with_get )
{
    lru_cache< std::string, int > cache( 5 );
    cache.put( "test_1", 3 );
    cache.put( "test_2", 9 );
    cache.put( "test_3", 8 );
    cache.put( "test_4", 10 );
    cache.put( "test_5", 2 );
    
    // test get operation will make it most recently used
    BOOST_REQUIRE( boost::optional< int >( cache.get( "test_1" ) ) );
    
    // test that "test_1" is not evicted after the get operation to make it mru
    cache.put( "test_6", 6 );
    
    // expect the least recently used key "test_2" to have been evicted as the lru
    boost::optional< int > value_2 = cache.get( "test_2" );
    boost::optional< int > value_3 = cache.get( "test_3" );
    boost::optional< int > value_4 = cache.get( "test_4" );
    boost::optional< int > value_5 = cache.get( "test_5" );
    boost::optional< int > value_6 = cache.get( "test_6" );
    boost::optional< int > value_1 = cache.get( "test_1" );
    
    // when trying to retreive the value of key "test_1" it should fail on optional as it has been evicted
    BOOST_REQUIRE( !value_2 );
    BOOST_REQUIRE( value_3 );
    BOOST_REQUIRE( value_4 );
    BOOST_REQUIRE( value_5 );
    BOOST_REQUIRE( value_6 );
    BOOST_REQUIRE( value_1 );
    
    // expect value to be updated after put operation
    BOOST_CHECK_EQUAL( value_6.get(), 6 );
    
    // expected capacity is 5
    BOOST_CHECK_EQUAL( cache.size(), 5 );
}

// Both ‘put’ and ‘get’ operations will make the item the most-recently-used item.
BOOST_AUTO_TEST_CASE( test_lru_with_put )
{
    lru_cache< std::string, int > cache( 5 );
    cache.put( "test_1", 3 );
    cache.put( "test_2", 9 );
    cache.put( "test_3", 8 );
    cache.put( "test_4", 10 );
    cache.put( "test_5", 2 );
    
    // test put operation will make it most recently used
    cache.put( "test_1", 7 );
    
    // test that "test_1" is not evicted after the put operation to make it mru
    cache.put( "test_6", 6 );
    
    // expect the least recently used key "test_2" to have been evicted as the lru
    boost::optional< int > value_2 = cache.get( "test_2" );
    boost::optional< int > value_3 = cache.get( "test_3" );
    boost::optional< int > value_4 = cache.get( "test_4" );
    boost::optional< int > value_5 = cache.get( "test_5" );
    boost::optional< int > value_6 = cache.get( "test_6" );
    boost::optional< int > value_1 = cache.get( "test_1" );
    
    // when trying to retreive the value of key "test_1" it should fail on optional as it has been evicted
    BOOST_REQUIRE( !value_2 );
    BOOST_REQUIRE( value_3 );
    BOOST_REQUIRE( value_4 );
    BOOST_REQUIRE( value_5 );
    BOOST_REQUIRE( value_6 );
    BOOST_REQUIRE( value_1 );
    
    // expect value to be updated after put operation
    BOOST_CHECK_EQUAL( value_1.get(), 7 );
    
    // expected capacity is 5
    BOOST_CHECK_EQUAL( cache.size(), 5 );
}

// test ‘put’ and ‘get’ operations will make the item the most-recently-used item.
BOOST_AUTO_TEST_CASE( test_lru_with_get_and_put )
{
    lru_cache< std::string, int > cache( 5 );
    cache.put( "test_1", 3 );
    cache.put( "test_2", 9 );
    cache.put( "test_3", 8 );
    cache.put( "test_4", 10 );
    cache.put( "test_5", 2 );
    
    // test get operation will make it most recently used
    cache.get( "test_1" );
    
    // test that "test_2" is not evicted after the put operation to make it mru
    cache.put( "test_2", 7 );
    
    // add some new elements into cache
    cache.put( "test_6", 8 );
    cache.put( "test_7", 10 );
    cache.put( "test_8", 2 );
    
    // expect both "test_1" and "test_2" keys to be around as they were least recently promoted to the mru
    boost::optional< int > value_3 = cache.get( "test_3" );
    boost::optional< int > value_4 = cache.get( "test_4" );
    boost::optional< int > value_5 = cache.get( "test_5" );

    boost::optional< int > value_1 = cache.get( "test_1" );
    boost::optional< int > value_2 = cache.get( "test_2" );
    boost::optional< int > value_6 = cache.get( "test_6" );
    boost::optional< int > value_7 = cache.get( "test_7" );
    boost::optional< int > value_8 = cache.get( "test_8" );
    
    
    // when trying to retreive the values of keys "test_3", "test_4", and "test_5", they should fail on optional as they should have been evicted
    BOOST_REQUIRE( !value_3 );
    BOOST_REQUIRE( !value_4 );
    BOOST_REQUIRE( !value_5 );
    
    BOOST_REQUIRE( value_1 );
    BOOST_REQUIRE( value_2 );
    BOOST_REQUIRE( value_6 );
    BOOST_REQUIRE( value_7 );
    BOOST_REQUIRE( value_8 );
    
    // expect value to be updated after put operation
    BOOST_CHECK_EQUAL( value_2.get(), 7 );
    
    // expected capacity is 5
    BOOST_CHECK_EQUAL( cache.size(), 5 );
}

BOOST_AUTO_TEST_SUITE_END()
