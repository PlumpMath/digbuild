///////////////////////////////////////////////////////////////////////////
// Copyright 2011 Evan Mezeske.
//
// This file is part of Digbuild.
// 
// Digbuild is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.
// 
// Digbuild is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Digbuild.  If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////

#ifndef RENDERER_H
#define RENDERER_H

#define MAJOR_OCCLUDER_CHUNK_COUNT 64

// it might possibly be acceptable to turn this a bit higher than 0, there will be artifacts
#define SAMPLES_NOT_OCCLUDED_THRESHOLD 0

// NOTE I'm not sure what would happen if this limit were reached
// probably Bad Things
#define MAX_OCCLUSION_QUERIES 8192

#include <GL/glew.h>

#include <set>

#include "camera.h"
#include "sdl_gl_window.h"
#include "world.h"
#include "player.h"
#include "renderer_material.h"

struct VertexBuffer : public boost::noncopyable
{
    typedef GLuint Index;

    VertexBuffer( const GLsizei num_elements = 0 );
    virtual ~VertexBuffer();

    void bind();
    void unbind();
    void draw_elements();

protected:

    struct BindGuard
    {
        BindGuard( VertexBuffer& vbo );
        ~BindGuard();

    protected:

        VertexBuffer& vbo_;
    };

    struct ClientStateGuard
    {
        ClientStateGuard( const GLenum state );
        ~ClientStateGuard();

    protected:

        GLenum state_;
    };

    struct TextureStateGuard
    {
        TextureStateGuard( const GLenum texture_unit, const GLenum state );
        ~TextureStateGuard();

    protected:

        GLenum
            texture_unit_,
            state_;
    };

    GLuint
        vbo_id_,
        ibo_id_;

    GLsizei num_elements_;
};

struct BlockVertex
{
    BlockVertex()
    {
    }

    BlockVertex(
        const Vector3f& position,
        const Vector3f& normal,
        const Vector3f& tangent,
        const Vector3f& texcoords,
        const Vector3f& lighting,
        const Vector3f& sunlighting
    ) :
        x_( position[0] ), y_( position[1] ), z_( position[2] ),
        nx_( normal[0] ), ny_( normal[1] ), nz_( normal[2] ),
        tx_( tangent[0] ), ty_( tangent[1] ), tz_( tangent[2] ),
        s_( texcoords[0] ), t_( texcoords[1] ), p_( texcoords[2] ),
        lr_( lighting[0] ), lg_( lighting[1] ), lb_( lighting[2] ),
        slr_( sunlighting[0] ), slg_( sunlighting[1] ), slb_( sunlighting[2] )
    {
    }

    GLfloat x_, y_, z_;       // Position
    GLfloat nx_, ny_, nz_;    // Normal
    GLfloat tx_, ty_, tz_;    // Tangent
    GLfloat s_, t_, p_;       // Texture coordinates
    GLfloat lr_, lg_, lb_;    // Light color
    GLfloat slr_, slg_, slb_; // Sunlight color

} __attribute__( ( packed ) );

typedef std::vector<BlockVertex> BlockVertexV;

struct ChunkVertexBuffer : public VertexBuffer
{
    ChunkVertexBuffer(
        const BlockVertexV& vertices,
        const GLenum vertex_usage = GL_STATIC_DRAW,
        const GLenum index_usage = GL_STATIC_DRAW
    );

    void render();
    void render_no_bind();
};

typedef boost::shared_ptr<ChunkVertexBuffer> ChunkVertexBufferSP;
typedef std::vector<Vector3f> Vector3fV;

struct SortableChunkVertexBuffer : public ChunkVertexBuffer
{
    SortableChunkVertexBuffer( const BlockVertexV& vertices );

    void render( const Camera& camera );

private:

    static const unsigned VERTICES_PER_FACE = 4;

    typedef std::pair<Scalar, unsigned> DistanceIndex;
    typedef std::vector<DistanceIndex> DistanceIndexV;
    typedef std::vector<VertexBuffer::Index> IndexV;

    void render_sorted( const DistanceIndexV distance_indices );

    Vector3fV centroids_;
};

typedef boost::shared_ptr<SortableChunkVertexBuffer> SortableChunkVertexBufferSP;
typedef std::set<BlockMaterial> BlockMaterialSet;

struct AABoxVertexBuffer : public VertexBuffer
{
    AABoxVertexBuffer( const AABoxf& aabb );

    void render();
};

struct ChunkRenderer
{
    ChunkRenderer( const Vector3f& centroid = Vector3f(), const AABoxf& aabb = AABoxf() );

    void render_opaque();
    void render_translucent( const Camera& camera );
    void render_aabb();
    void rebuild( const Chunk& chunk );

    bool has_translucent_materials() const { return translucent_vbo_; }
    const Vector3f& get_centroid() const { return centroid_; }
    const AABoxf& get_aabb() const { return aabb_; }
    unsigned get_num_triangles() const { return num_triangles_; }

protected:

    void get_vertices_for_face( const BlockFace& face, BlockVertexV& vertices ) const;

    ChunkVertexBufferSP opaque_vbo_;

    SortableChunkVertexBufferSP translucent_vbo_;

    AABoxVertexBuffer aabb_vbo_;

    Vector3f centroid_;

    AABoxf aabb_;

    unsigned num_triangles_;
};

typedef boost::shared_ptr<ChunkRenderer> ChunkRendererSP;

struct SkydomeVertexBuffer : public VertexBuffer
{
    static const Scalar RADIUS = 10.0f;

    SkydomeVertexBuffer();

    void render();
};

struct StarVertexBuffer : public VertexBuffer
{
    static const Scalar RADIUS = 10.0f;

    StarVertexBuffer( const Sky::StarV& stars );

    void render();
};

typedef boost::shared_ptr<StarVertexBuffer> StarVertexBufferSP;

struct SkyRenderer
{
    SkyRenderer();

    void render( const Sky& sky );

protected:

    void rotate_sky( const Vector2f& angle ) const;
    void render_celestial_body( const GLuint texture_id, const Vector3f& color ) const;

    Texture
        sun_texture_,
        moon_texture_;

    SkydomeVertexBuffer skydome_vbo_;

    Shader skydome_shader_;

    StarVertexBufferSP star_vbo_;
};

struct Renderer
{
    Renderer();

    typedef std::pair<Scalar, ChunkRenderer*> DistanceChunkPair;
    typedef std::list<DistanceChunkPair> DistanceChunkPairL;

    void note_chunk_changes( const Chunk& chunk );

#ifdef DEBUG_COLLISIONS
    void render( const SDL_GL_Window& window, const Camera& camera, const World& world, const Player& player );
#else
    void render( const SDL_GL_Window& window, const Camera& camera, const World& world );
#endif

    unsigned get_num_chunks_drawn() const { return chunks_visible_.size(); }
    unsigned get_num_triangles_drawn() const { return num_triangles_drawn_; }
    unsigned get_num_chunks_in_frustum() const { return chunks_in_frustum_.size(); }

protected:

    void render_sky( const Sky& sky );
    void render_visible_opaque_chunks();
    void render_visible_translucent_chunks( const Camera& camera );
    void render_depth_buffer_initialization();
    void render_crosshairs( const SDL_GL_Window& window );
#ifdef DEBUG_COLLISIONS
    void render_collisions( const Player& player );
#endif

    void queue_chunk_for_rendering( const DistanceChunkPair& chunk );

    void set_render_state_for_sky();
    void set_render_state_for_depth_buffer_initialization();
    void set_render_state_for_occlusion_queries();
    void set_render_state_for_rendering_opaque( const Camera& camera, const Sky& sky );
    void set_render_state_for_rendering_translucent( const Camera& camera, const Sky& sky );

    void update_frustum_lists( const Camera& camera );
    void update_chunks_visible_lists();

    gmtl::Matrix44f get_opengl_matrix( const GLenum matrix );

    RendererMaterialManager material_manager_;

    typedef std::map<Vector3i, ChunkRendererSP, VectorLess<Vector3i> > ChunkRendererMap;
    ChunkRendererMap chunk_renderers_;

    SkyRenderer sky_renderer_;

    DistanceChunkPairL chunks_in_frustum_          ,
                       chunks_visible_             ,
                       translucent_chunks_visible_ ;
                       //chunks_entering_frustum_  ,
                       //chunks_leaving_frustum_   ,

    std::vector<GLuint> occlusion_queries_;
    std::vector<GLuint>::iterator opaque_chunk_occlusion_queries_begin,
                                  translucent_chunk_occlusion_queries_begin;

    unsigned num_triangles_drawn_;
};

#endif // RENDERER_H
