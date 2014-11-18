#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <stdio.h>
#include <iostream>

#include <boost/bimap.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/optional.hpp>

/// @class lru_cache
/// @brief Implementation for least recently used cache class
/// @note The container is not thread-safe.
/// @date Created by bbetke on 18/11/2014.

/// The unordered_set is a boost version of set which uses hash tables instead of trees to store the elements
/// The set_of are generally implemented using balanced binary trees so that lookup time has logarithmic complexity.
/// According to boost documentation this is generally okay, but in many cases a hash table can perform better,
/// as accessing data has constant complexity, on average.
/// To ensure that both ‘put’ and ‘get’ operations should be fast, even when the cache is very large (i.e. much faster than O(N)),
/// unordered_set has been selected for this purpose for its average constant complexity O(1)

template< typename KEY, typename VALUE >
class lru_cache
{
public:
    lru_cache( size_t a_max_items );
    ~lru_cache();
    
    /// using boost optional instead of std optional
    boost::optional< VALUE > get( const KEY& a_key );
    
    void put( const KEY& a_key, const VALUE& a_value );

    inline const size_t size() const
    {
        return m_cache.size();
    }
    
private:
    typedef boost::bimaps::bimap< boost::bimaps::unordered_set_of< KEY >, boost::bimaps::list_of< VALUE > > Container;
    typedef typename Container::value_type Element;
    Container m_cache;
    const size_t m_capacity;
};

template< typename KEY, typename VALUE >
lru_cache< KEY, VALUE >::lru_cache( size_t max_items ) :
    m_cache(),
    m_capacity( max_items )
{
    assert( m_capacity > 0 );
}

template< typename KEY, typename VALUE >
lru_cache< KEY, VALUE >::~lru_cache()
{
}

template< typename KEY, typename VALUE >
boost::optional< VALUE > lru_cache< KEY, VALUE >::get( const KEY& a_key )
{
    boost::optional< VALUE > val;

    // find the key if it exists
    const typename Container::left_iterator ci = m_cache.left.find( a_key );
    if( ci != m_cache.left.end() )
    {
        // update by moving the right element so that it is in the most recently used or rear position,
        // to do this we need to get the right iterator from the left, hence the call to project right
        m_cache.right.relocate( m_cache.right.end(), m_cache.project_right( ci ) );
        val = ci->second;
    }
    else
    {
        // key not found, cache miss, not going to throw or add in a default key value to the cache now
        // given that the hint in the class code was to use optional instead
    }
    
    return val;
}

template< typename KEY, typename VALUE >
void lru_cache< KEY, VALUE >::put( const KEY& a_key, const VALUE& a_value )
{
    if( m_cache.size() > m_capacity )
    {
        throw std::runtime_error( "lru cache capacity exceeded" );
    }
    
    // if the key already exists in the cache, then we will perform an update only
    const typename Container::left_iterator ci = m_cache.left.find( a_key );
    if( ci != m_cache.left.end() )
    {
        //m_cache.project_right( ci )->first = a_value;
        m_cache.left.replace_data( ci, a_value );
        m_cache.right.relocate( m_cache.right.end(), m_cache.project_right( ci ) );
    }
    else
    {
        // if the key doesn't exist, than we will need to make room for a new element, only if we are at capacity
        if( m_cache.size() == m_capacity )
        {
            // if the end right is the most recently used, then the front right is the least recently used,
            // so delete the lru right element as per the algorithm to make room for the new element
            m_cache.right.erase( m_cache.right.begin() );
        }
        
        // in either case inserting a new element will put it at the end and hence make it the most recently used
        m_cache.insert( Element( a_key, a_value ) );
    }
}

#endif
