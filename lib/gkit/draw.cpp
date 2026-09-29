
#include "image_io.h"
#include "texture.h"

#include "draw.h"
#include "window.h"
#include "program.h"
#include "uniforms.h"

//avec texture
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture )
{
    DrawParam param;
    param.model(model).view(view).projection(projection);
    param.texture(texture);
    param.draw(m);
}

void draw( Mesh& m, const Transform& model, Orbiter& camera, const GLuint texture )
{
    // recupere les transformations
    Transform view= camera.view();
    Transform projection= camera.projection(window_width(), window_height(), 45);
    
    // affiche l'objet
    draw(m, model, view, projection, texture);
}

void draw( Mesh& m, Orbiter& camera, const GLuint texture )
{
    draw(m, Identity(), camera, texture);
}


// sans texture
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection )
{
    DrawParam param;
    param.model(model).view(view).projection(projection);
    param.draw(m);
}

void draw( Mesh& m, const Transform& model, Orbiter& camera )
{
    // recupere les transformations
    Transform view= camera.view();
    Transform projection= camera.projection(window_width(), window_height(), 45);
    
    // affiche l'objet
    draw(m, model, view, projection);
}

void draw( Mesh& m, Orbiter& camera )
{
    // affiche l'objet
    draw(m, Identity(), camera);
}


// groupe de triangles + matiere, sans texture
void draw( const TriangleGroup& group, Mesh& m, const Transform& model, const Transform& view, const Transform& projection )
{
    DrawParam param;
    param.model(model).view(view).projection(projection);
    param.draw(group, m);
}    

void draw( const TriangleGroup& group, Mesh& m, const Transform& model, Orbiter& camera )
{
    // recupere les transformations
    Transform view= camera.view();
    Transform projection= camera.projection(window_width(), window_height(), 45);
    
    // dessine les triangles
    draw(group, m, model, view, projection);
}

void draw( const TriangleGroup& group, Mesh& m, Orbiter& camera )
{
    draw(group, m, Identity(), camera);
}


// groupe de triangles + matiere et texture
void draw( const TriangleGroup& group, Mesh& m, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture )
{
    DrawParam param;
    param.model(model).view(view).projection(projection);
    param.texture(texture);    
    param.draw(group, m);
}

void draw( const TriangleGroup& group, Mesh& m, const Transform& model, Orbiter& camera, const GLuint texture )
{
    // recupere les transformations
    Transform view= camera.view();
    Transform projection= camera.projection(window_width(), window_height(), 45);
    
    // dessine les triangles
    draw(group, m, model, view, projection, texture);
}

void draw( const TriangleGroup& group, Mesh& m, Orbiter& camera, const GLuint texture )
{
    draw(group, m, Identity(), camera, texture);
}


void draw( Mesh& m, DrawParam& param )
{
    param.draw(m);
}


GLuint DrawParam::create_program( const GLenum primitives, const unsigned flags )
{
    std::string definitions;

    if(flags & USE_TEXCOORD)
        definitions.append("#define USE_TEXCOORD\n");
    if(flags & USE_NORMAL)
        definitions.append("#define USE_NORMAL\n");
    if(flags & USE_COLOR)
        definitions.append("#define USE_COLOR\n");
    
    if((flags & USE_TEXCOORD) && (flags & USE_ALPHATEST))
        definitions.append("#define USE_ALPHATEST\n");
    
    if(flags & USE_SHADING)
    {
        definitions.append("#define USE_SHADING\n");
        
        if(flags & USE_MATERIAL)
            definitions.append("#define USE_MATERIAL\n");
        if((flags & USE_LIGHT))
            definitions.append("#define USE_LIGHT\n");
        if(flags & USE_SUN)
            definitions.append("#define USE_SUN\n");
        if(flags & USE_SKY)
            definitions.append("#define USE_SKY\n");
    }

    //~ printf("--\n%s", definitions.c_str());
    const char *filename= smart_path("data/shaders/mesh.glsl");
    bool use_mesh_color= (primitives == GL_POINTS || primitives == GL_LINES || primitives == GL_LINE_STRIP || primitives == GL_LINE_LOOP);
    if(use_mesh_color) 
        filename= smart_path("data/shaders/mesh_color.glsl");
    
    PipelineProgram *cache= PipelineCache::manager().find(filename, definitions.c_str());
    return cache->program;
}

GLuint DrawParam::create_debug_normals_program( const GLenum primitives, const unsigned flags )
{
    const char *filename= smart_path("data/shaders/normals.glsl");
    bool use_mesh_color= (primitives == GL_POINTS || primitives == GL_LINES || primitives == GL_LINE_STRIP || primitives == GL_LINE_LOOP);
    if(use_mesh_color) 
        // pas la peine, les normales ne sont definies que pour les triangles...
        return 0;
    
    PipelineProgram *cache= PipelineCache::manager().find(filename);
    return cache->program;
}

GLuint DrawParam::create_debug_texcoords_program( const GLenum primitives, const unsigned flags )
{
    const char *filename= smart_path("data/shaders/texcoords.glsl");
    PipelineProgram *cache= PipelineCache::manager().find(filename);
    return cache->program;
}

unsigned DrawParam::create_flags( const bool use_texcoord, const bool use_normal, const bool use_color )
{
    unsigned options= flags;
    update(options, USE_TEXCOORD, use_texcoord);
    update(options, USE_NORMAL, use_normal);
    update(options, USE_COLOR, use_color);
    
    return options;
}

unsigned DrawParam::update( unsigned &flags, const unsigned option, const bool use )
{
    if(use)
        flags= flags | option;
    else
        flags= flags & ~option;
    return flags;
}
    
bool DrawParam::flag( const unsigned option )
{
    return flags & option;
}

void DrawParam::draw( Mesh& mesh )
{
    if(mesh.index_count())
        draw(0, mesh.index_count(), mesh);
    else
        draw(0, mesh.vertex_count(), mesh);
}

void DrawParam::draw( Mesh& mesh, const Transform& model )
{
    m_model= model;
    draw(mesh);
}

void DrawParam::draw( const TriangleGroup& group, Mesh& mesh )
{
    draw(group.first, group.n, mesh);
}

void DrawParam::draw( const unsigned first, const unsigned count, Mesh& mesh )
{
    bool use_texture= m_texture;
    if(flag(USE_MATERIAL))
        use_texture= use_texture || m_material.diffuse_texture || m_material.specular_texture  || m_material.ns_texture;
    bool use_texcoord= use_texture && mesh.has_texcoord();
    bool use_normal= mesh.has_normal();
    bool use_color= mesh.has_color();
    
    Transform mv= m_view * m_model;
    Transform mvp= m_projection * mv;
    
    GLuint program= 0;
    if(flag(DEBUG_TEXCOORDS))
    {
        program= create_debug_texcoords_program( mesh.primitives(), create_flags(use_texcoord, use_normal, use_color) );
        if(program > 0)
        {
            use_color= false;
            use_normal= false;
            use_texcoord= true;
            update(flags, USE_LIGHT | USE_TEXTURE | USE_ALPHATEST, false);
            
            glUseProgram(program);
            //~ program_use_texture(program, "diffuse_color", 0, m_debug_texture);
            program_uniform(program, "mvpMatrix", mvp);
            program_uniform(program, "mvMatrix", mv);
            
            mesh.draw(first, count, program);
            return;
        }
    }
    
    //
    program= create_program( mesh.primitives(), create_flags(use_texcoord, use_normal, use_color) );
    assert(program > 0);
    
    glUseProgram(program);
    if(!use_color)
        program_uniform(program, "mesh_color", mesh.default_color());
    
    program_uniform(program, "mvpMatrix", mvp);
    
    // utiliser une texture, elle ne sera visible que si le mesh a des texcoords...
    if(use_texcoord && m_texture > 0)
        program_use_texture(program, "diffuse_texture", 0, m_texture);
    
    if(flag(USE_ALPHATEST))
        program_uniform(program, "alpha_min", m_alpha_min);
    
    if(use_normal)
        program_uniform(program, "normalMatrix", mv.normal()); // transforme les normales dans le repere camera.
    if(!use_normal || flag(USE_LIGHT | USE_MATERIAL))
        program_uniform(program, "mvMatrix", mv);
    
    if(flag(USE_SHADING))
    {
        if(flag(USE_MATERIAL))
        {
            program_uniform(program, "diffuse_color", m_material.diffuse);
            program_uniform(program, "specular_color", m_material.specular);
            program_uniform(program, "ns", m_material.ns);
            
            if(use_texcoord)
            {
                // texture blanche uniforme par defaut
                static GLuint white_texture= 0;
                if(white_texture == 0)
                {
                    ImageData data(16, 16, 4);
                    for(unsigned y= 0; y < data.height; y++)
                    for(unsigned x= 0; x < data.width; x++)
                    {
                        unsigned char *pixel= data.pixels.data() + data.offset(x, y);
                        for(unsigned i= 0; i < data.channels; i++)
                            pixel[i]= 255;
                    }
                    
                    white_texture= make_texture(0, data);
                }
                
                if(m_material.diffuse_texture > 0)
                    program_use_texture(program, "diffuse_texture", 0, m_material.diffuse_texture);
                else
                    program_use_texture(program, "diffuse_texture", 0, white_texture);
                
                if(m_material.specular_texture > 0)
                    program_use_texture(program, "specular_texture", 1, m_material.specular_texture);
                else
                    program_use_texture(program, "specular_texture", 1, white_texture);
                
                if(m_material.specular_texture > 0)
                    program_use_texture(program, "ns_texture", 2, m_material.specular_texture);
                else
                    program_use_texture(program, "ns_texture", 2, white_texture);
            }
        }
        
        if(flag(USE_LIGHT))
        {
            program_uniform(program, "light", m_view(m_light));       // transforme la position de la source dans le repere camera, comme les normales
            program_uniform(program, "light_color", m_light_color);
        }
        if(flag(USE_SUN))
        {
            program_uniform(program, "sun", m_view(m_sun));       // transforme la direction de la source dans le repere camera, comme les normales
            program_uniform(program, "sun_color", m_sun_color);
        }
        if(flag(USE_SKY))
        {
            program_uniform(program, "sky", m_view(m_sky));       // transforme la direction de la source dans le repere camera, comme les normales
            program_uniform(program, "sky_color", m_sky_color);
        }
    }
    
    mesh.draw(first, count, program);
    
    // dessine les normales par dessus...
    if(flag(DEBUG_NORMALS))
    {
        program= create_debug_normals_program( mesh.primitives(), create_flags(use_texcoord, use_normal, use_color) );
        if(program > 0)
        {
            use_color= false;
            use_normal= true;
            use_texcoord= false;
            update(flags, USE_SHADING | USE_LIGHT | USE_TEXTURE | USE_ALPHATEST, false);
            
            static float scale= 1;
            if(key_state('p') || key_state(SDLK_KP_PLUS))
                scale+= 0.005f;
            if(key_state('m') || key_state(SDLK_KP_MINUS))
                scale-= 0.005f;
            if(scale < 0.1f)
                scale= 0.1f;
            
            glUseProgram(program);
            program_uniform(program, "mvpMatrix", mvp);
            program_uniform(program, "normalMatrix", mv.normal()); // transforme les normales dans le repere camera.
            program_uniform(program, "scale", scale * m_normals_scale);

            mesh.draw(first, count, program);
        }
    }
}
