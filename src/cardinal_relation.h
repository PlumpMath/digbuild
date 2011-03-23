#ifndef CARDINAL_RELATION_H
#define CARDINAL_RELATION_H

#include <stdexcept>

#include "math.h"

enum CardinalRelation
{
    CARDINAL_RELATION_ABOVE,
    CARDINAL_RELATION_BELOW,
    CARDINAL_RELATION_NORTH,
    CARDINAL_RELATION_SOUTH,
    CARDINAL_RELATION_EAST,
    CARDINAL_RELATION_WEST,
    NUM_CARDINAL_RELATIONS
};

#define FOR_EACH_CARDINAL_RELATION( iterator_name )\
    for ( CardinalRelation iterator_name = CARDINAL_RELATION_ABOVE;\
          iterator_name != NUM_CARDINAL_RELATIONS;\
          iterator_name = CardinalRelation( int( iterator_name ) + 1 ) )

inline Vector3i cardinal_relation_vector( const CardinalRelation relation )
{
    switch ( relation )
    {
        case CARDINAL_RELATION_ABOVE: return Vector3i(  0,  1,  0 );
        case CARDINAL_RELATION_BELOW: return Vector3i(  0, -1,  0 );
        case CARDINAL_RELATION_NORTH: return Vector3i(  0,  0,  1 );
        case CARDINAL_RELATION_SOUTH: return Vector3i(  0,  0, -1 );
        case CARDINAL_RELATION_EAST:  return Vector3i(  1,  0,  0 );
        case CARDINAL_RELATION_WEST:  return Vector3i( -1,  0,  0 );
        default: throw std::runtime_error( "Invalid cardinal relation." );
    }
}

inline CardinalRelation cardinal_relation_reverse( const CardinalRelation relation )
{
    switch ( relation )
    {
        case CARDINAL_RELATION_ABOVE: return CARDINAL_RELATION_BELOW;
        case CARDINAL_RELATION_BELOW: return CARDINAL_RELATION_ABOVE;
        case CARDINAL_RELATION_NORTH: return CARDINAL_RELATION_SOUTH;
        case CARDINAL_RELATION_SOUTH: return CARDINAL_RELATION_NORTH;
        case CARDINAL_RELATION_EAST:  return CARDINAL_RELATION_WEST;
        case CARDINAL_RELATION_WEST:  return CARDINAL_RELATION_EAST;
        default: throw std::runtime_error( "Invalid cardinal relation." );
    }
}

inline CardinalRelation cardinal_relation_tangent( const CardinalRelation relation )
{
    switch ( relation )
    {
        case CARDINAL_RELATION_ABOVE: return CARDINAL_RELATION_NORTH;
        case CARDINAL_RELATION_BELOW: return CARDINAL_RELATION_SOUTH;
        case CARDINAL_RELATION_NORTH: return CARDINAL_RELATION_EAST;
        case CARDINAL_RELATION_SOUTH: return CARDINAL_RELATION_WEST;
        case CARDINAL_RELATION_EAST:  return CARDINAL_RELATION_ABOVE;
        case CARDINAL_RELATION_WEST:  return CARDINAL_RELATION_BELOW;
        default: throw std::runtime_error( "Invalid cardinal relation." );
    }
}

#endif // CARDINAL_RELATION_H
