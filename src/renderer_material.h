#ifndef RENDERER_MATERIAL_H
#define RENDERER_MATERIAL_H

#include <GL/glew.h>

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "math.h"
#include "shader.h"

struct Texture : public boost::noncopyable
{
    Texture();
    Texture( const std::string& filename );
    ~Texture();

    GLuint texture_id() const { return texture_id_; }
    const Vector2i& size() const { return size_; }

private:

    GLuint texture_id_;

    Vector2i size_;
};

struct RendererMaterial
{
    static const std::string
        TEXTURE_DIRECTORY,
        SHADER_DIRECTORY;

    RendererMaterial( const std::string& name );

    const Texture& texture() const { return texture_; }
    const Texture& specular_map() const { return specular_map_; }
    const Texture& bump_map() const { return bump_map_; }
    const Shader& shader() const { return shader_; }

private:

    Texture
        texture_,
        specular_map_,
        bump_map_;

    Shader shader_;
};

typedef boost::shared_ptr<RendererMaterial> RendererMaterialSP;
typedef std::vector<RendererMaterialSP> RendererMaterialV;

#endif // RENDERER_MATERIAL_H