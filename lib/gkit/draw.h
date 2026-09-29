
#ifndef _DRAW_H
#define _DRAW_H

#include "mesh.h"
#include "orbiter.h"
#include "window.h"
#include "program.h"


//! \addtogroup objet3D

//! \file

//! dessine l'objet avec les transformations model, vue et projection.
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection );
//! applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet.
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture );

//! dessine l'objet avec les transformations vue et projection, definies par la camera. model est la transformation identite.
void draw( Mesh& m, Orbiter& camera );
//! dessine l'objet avec une transformation model. les transformations vue et projection sont celles de la camera
void draw( Mesh& m, const Transform& model, Orbiter& camera );

//! dessine l'objet avec les transformations vue et projection. model est l'identite. applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet.
void draw( Mesh& m, Orbiter& camera, const GLuint texture );
//! dessine l'objet avec une transformation model. les transformations vue et projection sont celles de la camera. applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet.
void draw( Mesh& m, const Transform& model, Orbiter& camera, const GLuint texture );

//! \name dessine des triangles d'un objet associés à une matière. cf gestion des matieres, Mesh::groups() et les classes Materials et Material.
//@{
//! dessine un groupe de triangles de l'objet associe a une matiere / couleur. 
void draw( const TriangleGroup& group, Mesh& mesh, Orbiter& camera );
//! dessine un groupe de triangles de l'objet associe a une matiere / couleur.
void draw( const TriangleGroup& group, Mesh& mesh, const Transform& model, Orbiter& camera );
//! dessine un groupe de triangles de l'objet associe a une matiere / couleur.
void draw( const TriangleGroup& group, Mesh& mesh, const Transform& model, const Transform& view, const Transform& projection );

//! dessine un groupe de triangles de l'objet associe a une matiere / couleur et une texture. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet.
void draw( const TriangleGroup& group, Mesh& mesh, Orbiter& camera, const GLuint texture );
//! dessine un groupe de triangles de l'objet associe a une matiere / couleur et une texture. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet.
void draw( const TriangleGroup& group, Mesh& mesh, const Transform& model, Orbiter& camera, const GLuint texture );
//! dessine un groupe de triangles de l'objet associe a une matiere / couleur et une texture. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet.
void draw( const TriangleGroup& group, Mesh& mesh, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture );
//@}

/*! representation des options / parametres d'un draw.
    permet de donner tous les parametres d'un draw de maniere flexible.

    exemple :
    \code
    Mesh objet= { ... };

    DrawParam param;
    param.light(Point(0, 20, 0), Red());
    param.camera(orbiter);
    param.draw(objet);
    \endcode

    ou de maniere encore plus compacte :
    \code
    DrawParam().light(Point(0, 20, 0), Red()).model(m).camera(orbiter).draw(objet);
    \endcode
    les parametres peuvent etre decrits dans un ordre quelconque, mais DrawParam::draw() doit etre appele en dernier.
 */
class DrawParam
{
public:
    //! constructeur par defaut.
    DrawParam( ) :  flags( USE_SUN ),
        m_model(), m_view(), m_projection(),
        m_light( Point(1, 1, 1) ), m_light_color( White() ),
        m_sun( normalize( Vector(0.5, 0.25, 0.5) ) ), m_sun_color( Color(1) ),
        m_sky( normalize( Vector(0, 1, 0) ) ), m_sky_color( Color(0.2) ),
        m_texture(0),
        m_alpha_min(0.3),
        m_normals_scale(0.2),
        m_material( Color(0.7) )
    {}

    //! modifie la transformation model utilisee pour afficher l'objet.
    DrawParam& model( const Transform& m ) { m_model= m; return *this; }
    //! modifie la transformation view utilisee pour afficher l'objet.
    DrawParam& view( const Transform& m ) { m_view= m; return *this; }
    //! modifie la transformation projection utilisee pour afficher l'objet.
    DrawParam& projection( const Transform& m ) { m_projection= m; return *this; }

    //! utilise les transformations view et projection definies par une camera.
    DrawParam& camera( Orbiter& o ) { m_view= o.view(); m_projection= o.projection(); return *this; }
    //! utilise les transformations view et projection definies par une camera. parametres explicites de la projection.
    DrawParam& camera( Orbiter& o, const int width, const int height, const float fov ) { m_view= o.view(); m_projection= o.projection(width, height, fov); return *this; }
    //! eclaire l'objet avec une source ponctuelle, de position p et de couleur c.
    DrawParam& light( const Point& p, const Color& c= White() ) { update(flags, USE_LIGHT); m_light= p; m_light_color=c; return *this; }
    //! active / desactive la source ponctuelle.
    DrawParam& light( const bool flag= true ) { update(flags, USE_LIGHT, flag); return *this; }
    //! eclaire l'objet avec une source directionnelle, de direction d et de couleur c.
    DrawParam& sun( const Vector& d, const Color& c ) { update(flags, USE_SUN); m_sun= d; m_sun_color=c; return *this; }
    //! active / desactive le soleil.
    DrawParam& sun( const bool flag= true ) { update(flags, USE_SUN, flag); return *this; }
    
    //! eclaire l'objet avec une source hemispherique de couleur c. le zenith se trouve dans la direction d.
    DrawParam& sky( const Vector& d, const Color& c ) { update(flags, USE_SKY); m_sky= d; m_sky_color=c; return *this; }
    //! active / desactive le ciel
    DrawParam& sky( const bool flag= true ) { update(flags, USE_SKY, flag); return *this; }
    
    //! plaque une texture opaque a la surface de l'objet.
    DrawParam& texture( const GLuint t ) { update(flags, USE_TEXTURE, (t > 0)); m_texture= t; return *this; }
    
    //! utilise une texture semi transparente, si l'alpha du texel est plus petit que a, le pixel est transparent. desactive aussi les calculs d'eclairage.
    DrawParam& alpha_texture( const GLuint t, const float a= 0.5 ) { update(flags, USE_ALPHATEST, (a > 0)); m_alpha_min= a; m_texture= t; return *this; }
    
    //! utilise une source de lumire pour eclairer l'objet, ou pas si flag= false.
    DrawParam& shading( const bool flag= true ) { update(flags, USE_SHADING, flag);  return *this; }
    
    //! utilise une matiere pour dessiner un objet.
    DrawParam& material( const Material& material ) { update(flags, USE_SHADING | USE_MATERIAL); m_material= material; return *this; }
    //! active / desactive la matiere pour dessiner un objet.
    DrawParam& material( const bool flag= true ) { update(flags, USE_MATERIAL, flag); return *this; }
    
    //! visualise les normales des sommets des triangles et les normales geometrique des triangles
    DrawParam& debug_normals( const float s= 0.2 ) { update(flags, DEBUG_NORMALS, (s > 0)); m_normals_scale= s; return *this;}
    //! visualise les aretes des triangles.
    DrawParam& wireframe( const bool flag= true ) { update(flags, DEBUG_NORMALS,  flag); m_normals_scale= 0; return *this;}
    //! visualise les coordonnees de textures des sommets des triangles.
    DrawParam& debug_texcoords( const bool flag= true ) { update(flags, DEBUG_TEXCOORDS, flag); return *this;}
    
    //! dessine l'objet avec l'ensemble des parametres definis.
    void draw( Mesh& mesh );
    void draw( Mesh& mesh, const Transform& model );
    void draw( const TriangleGroup& group, Mesh& mesh );
    
protected:
    void draw( const unsigned first, const unsigned count, Mesh& mesh );

    //! flags pour identifier les parametres actifs du draw / generer le shader
    enum : unsigned
    {
        USE_TEXCOORD = 1,
        USE_NORMAL= 2,
        USE_COLOR= 4,
        USE_TEXTURE= 8,
        USE_ALPHATEST= 16,
        USE_SHADING= 32,
        USE_LIGHT= 64,
        USE_SUN= 128,
        USE_SKY= 256,
        USE_ENVMAP= 512,
        USE_MATERIAL= 1024,
        
        DEBUG_NORMALS= 1u << 20,
        DEBUG_TEXCOORDS= 1u << 21
    };
    
    unsigned flags;
    
    unsigned create_flags( const bool use_texcoord, const bool use_normal, const bool use_color );
    unsigned update( unsigned &flags, const unsigned option, const bool use= true );
    bool flag( const unsigned option );
    
    //! construit un shader program configure pour l'ensemble d'options.
    GLuint create_program( const GLenum primitives, const unsigned flags );
    //! construit un shader program / debug des normales.
    GLuint create_debug_normals_program( const GLenum primitives, const unsigned flags );
    //! construit un shader program / debug des coordonnees de texture.
    GLuint create_debug_texcoords_program( const GLenum primitives, const unsigned flags );
    
    Transform m_model;
    Transform m_view;
    Transform m_projection;

    Point m_light;
    Color m_light_color;
    
    Vector m_sun;
    Color m_sun_color;
    Vector m_sky;
    Color m_sky_color;
    
    GLuint m_texture;
    GLuint m_envmap;
    float m_alpha_min;
    float m_normals_scale;
    
    Material m_material;
};


void draw( Mesh& m, DrawParam& param );

#endif
