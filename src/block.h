#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <assert.h>

#include <limits>
#include <vector>

#include "math.h"

enum BlockMaterial
{
    BLOCK_MATERIAL_AIR = 0,
    BLOCK_MATERIAL_GRASS,
    BLOCK_MATERIAL_DIRT,
    BLOCK_MATERIAL_CLAY,
    BLOCK_MATERIAL_STONE,
    BLOCK_MATERIAL_BEDROCK,
    BLOCK_MATERIAL_MAGMA,
    BLOCK_MATERIAL_TREE_TRUNK,
    BLOCK_MATERIAL_TREE_LEAF,
    BLOCK_MATERIAL_GLASS,
    BLOCK_MATERIAL_SIZE
};

struct BlockMaterialAttributes
{
    BlockMaterialAttributes( const bool translucent ) :
        translucent_( translucent )
    {
    }

    const bool translucent_;
};

const BlockMaterialAttributes& get_block_material_attributes( const BlockMaterial material );

struct Block
{
    static const Vector4i
        MIN_LIGHT_LEVEL,
        MAX_LIGHT_LEVEL;

    Block() :
        material_( BLOCK_MATERIAL_AIR ),
        sunlight_source_( 0 ),
        visited_( 0 ),
        light_level_r_( 0 ),
        light_level_g_( 0 ),
        light_level_b_( 0 )
    {
    }

    void set_material( const BlockMaterial material )
    {
        assert( material >= std::numeric_limits<uint8_t>::min() );
        assert( material <= std::numeric_limits<uint8_t>::max() );

        material_ = material;
    }

    BlockMaterial get_material() const { return BlockMaterial( material_ ); }
    const BlockMaterialAttributes& get_material_attributes() const { return get_block_material_attributes( get_material() ); }

    void set_sunlight_source( const bool sunlight_source ) { sunlight_source_ = sunlight_source; }
    bool is_sunlight_source() const { return sunlight_source_; }

    void set_visited( const bool visited ) { visited_ = visited; }
    bool is_visited() const { return visited_; }

    void set_light_level( const Vector4i& light_level )
    {
        assert( light_level_valid( light_level ) );
        light_level_r_ = light_level[0];
        light_level_g_ = light_level[1];
        light_level_b_ = light_level[2];
        light_level_s_ = light_level[3];
    }

    Vector4i get_light_level() const
    {
        return Vector4i( light_level_r_, light_level_g_, light_level_b_, light_level_s_ );
    }

private:

    bool light_level_valid( const Vector4i& light_level ) const
    {
        for ( int i = 0; i < Vector4i::Size; ++i )
        {
             if ( light_level[i] < MIN_LIGHT_LEVEL[i] || light_level[i] > MAX_LIGHT_LEVEL[i] )
             {
                 return false;
             }
        }

        return true;
    }

    uint8_t material_;

    uint8_t sunlight_source_ : 1;
    uint8_t visited_         : 1;
    uint8_t light_level_r_   : 4;
    uint8_t light_level_g_   : 4;
    uint8_t light_level_b_   : 4;
    uint8_t light_level_s_   : 4;
};

struct BlockFace
{
    enum { NUM_VERTICES = 4 };

    struct Vertex
    {
        Vertex()
        {
        }

        Vertex( const Vector3f& position, const Vector4f& lighting ) :
            position_( position ),
            lighting_( lighting )
        {
        }

        Vector3f position_;
        Vector4f lighting_;
    };

    BlockFace()
    {
    }

    BlockFace( const Vector3f& normal, const Vector3f& tangent, const BlockMaterial material ) :
        normal_( normal ),
        tangent_( tangent ),
        material_( material )
    {
    }

    Vertex vertices_[NUM_VERTICES];

    Vector3f
        normal_,
        tangent_;

    BlockMaterial material_;
};

typedef std::vector<BlockFace> BlockFaceV;

#endif // BLOCK_H