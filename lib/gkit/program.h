
#ifndef _PROGRAM_H
#define _PROGRAM_H

#include <vector>
#include <string>

#include "glcore.h"


//! \addtogroup openGL utilitaires openGL
///@{

//! \file
//! shader program openGL

//! cree un shader program. a detruire avec release_program( ).\n
//! charge un seul fichier, les shaders sont separes par \#ifdef VERTEX_SHADER / \#endif et \#ifdef FRAGMENT_SHADER / \#endif.\n
//! renvoie l'identifiant openGL du program et le program est selectionne (cf glUseProgram( )).
//! \param filename nom du fichier source.
//! \param definitions chaine de caracteres pouvant comporter plusieurs lignes "#define what value\n".
GLuint read_program( const char *filename, const char *definitions= "" );

//! detruit les shaders et le program.
//! \param program shader program a detruire.
int release_program( const GLuint program );

//! recharge et recompile les shaders du program. utilise le cache pour retrouver la configuration et les sources.
//! \param program shader program a recompiler
int reload_program( const GLuint program );

//! recharge et recompile les shaders du program. utilise les definitions supplementaires. utilise le cache pour retrouver la configuration et les sources.
//! \param program shader program a recompiler
//! \param definitions cf read_program
int reload_program( const GLuint program, const char *definitions );

//! renvoie les erreurs de compilation.
int program_format_errors( const GLuint program, std::string& errors );

//! affiche les erreurs de compilation.
int program_print_errors( const GLuint program, const char *filename= "" );

//! renvoie vrai si le programme est pret. (pas d'erreurs de compilation des shaders, pas d'erreur de link).
bool program_ready( const GLuint program );

//! renvoie vrai si le programme n'est pas pret.
bool program_errors( const GLuint program );


//! description d'un shader program compile.
struct PipelineProgram
{
    std::string filename;
    std::string definitions;
    GLuint program;
};

//! ensemble de shader programs compiles. singleton.
class PipelineCache
{
public:
    ~PipelineCache( ) 
    {
        printf("[pipeline cache] %d programs.\n", int(m_programs.size()));
        // le contexte est deja detruit lorsque ce destructeur est appele... 
        // trop tard pour detruire les programs et les shaders...
        
        //~ dump();
        for(auto& cache : m_programs)
            delete cache;
    }
    
    //! renvoie un shader program compile. et l'ajoute dans le cache, si necessaire...
    PipelineProgram *find( const char *filename, const char *definitions= "" )
    {
        // unordered_map sans doute plus rapide ? mais il y a peu de shaders utilises... 
        for(auto cache : m_programs)
        {
            if(cache->filename == filename && cache->definitions == definitions)
                return cache;
        }
        
        return insert(filename, definitions);
    }
    
    //! renvoie un shader program compile. et l'ajoute dans le cache, si necessaire...
    PipelineProgram *find( const GLuint program, const char *filename, const char *definitions= "" )
    {
        for(auto cache : m_programs)
        {
            if(cache->program == program && cache->filename == filename && cache->definitions == definitions)
                return cache;
        }
        
        return insert(program, filename, definitions);
    }
    
    //! recharge et recompile un programme du cache.
    PipelineProgram *reload( const GLuint program, const char *definitions= "" )
    {
        int update_program( const GLuint program, const char *filename, const char *definitions );
        
        for(auto cache : m_programs)
        {
            if(cache->program == program)
            {
                if(definitions && definitions[0])
                    // modifier le cache avec les nouvelles definitions, uniquement si elles sont non nulles.
                    cache->definitions= definitions;
                
                // recharge et recompile le shader.
                update_program(program, cache->filename.c_str(), cache->definitions.c_str());
                program_print_errors(program);
                return cache;
            }
        }
        
        // ne devrait pas arriver...
        return nullptr;
    }
    
    //! recompile tous les shaders. du cache.
    void reload( )
    {
        int update_program( const GLuint program, const char *filename, const char *definitions );
        
        for(auto cache : m_programs)
        {
            update_program(cache->program, cache->filename.c_str(), cache->definitions.c_str());
            program_print_errors(cache->program);
        }
    }
    
    // debug
    void dump( )
    {
        for(unsigned i= 0; i < m_programs.size(); i++)
        {
            PipelineProgram *cache= m_programs[i];
            printf("[%d] '%s'\n", i, cache->filename.c_str());
            if(!cache->definitions.empty())
                printf("  %s\n", cache->definitions.c_str());
        }
    }
    
    //! acces au singleton.
    static PipelineCache& manager( ) 
    {
        static PipelineCache cache;
        return cache;
    }
    
protected:
    //! constructeur prive. 
    PipelineCache( ) : m_programs() {}
    
    //! compile un shader et l'ajoute au cache.
    PipelineProgram *insert( const char *filename, const char *definitions= "" )
    {
        int update_program( const GLuint program, const char *filename, const char *definitions );
        
        // cree le programme s'il n'existe pas deja...
        GLuint program= glCreateProgram();
        update_program(program, filename, definitions);
        program_print_errors(program);
        
        return insert(program, filename, definitions);
    }
    
    //! ajoute un shader dans le cache.
    PipelineProgram *insert( const GLuint program, const char *filename, const char *definitions= "" )
    {
        PipelineProgram *cache= new PipelineProgram { filename, definitions, program };
        m_programs.push_back(cache);
        
        return cache;
    }
    
    std::vector<PipelineProgram *> m_programs;
};


///@}
#endif
