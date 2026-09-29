
#include "texture.h"
#include "app_camera.h"

AppCamera::AppCamera( const int width, const int height, const int major, const int minor ) 
    : App(width, height, major, minor), m_camera() 
{
    // projection par defaut, adaptee a la fenetre
    m_camera.projection(window_width(), window_height(), 45);
}

AppCamera::~AppCamera( ) {}

int AppCamera::prerender( )
{
    update_camera( m_camera );
    
    // appelle la fonction update() de la classe derivee
    return update(global_time(), delta_time());
}


void update_camera( Orbiter& camera )
{
    // recupere les mouvements de la souris
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);

    // deplace la camera
    if(mb & SDL_BUTTON(1))
        camera.rotation(mx, my);    // tourne autour de l'objet
    else if(mb & SDL_BUTTON(3))
        camera.translation( float(mx) / float(window_width()), float(my) / float(window_height()));
                                    // deplace le point de rotation
    else if(mb & SDL_BUTTON(2))
        camera.move(mx);            // approche / eloigne l'objet
    
    // molette / scroll
    SDL_MouseWheelEvent wheel= wheel_event();
    if(wheel.y != 0)
    {
        clear_wheel_event();
        camera.move(8.f * wheel.y); // approche / eloigne l'objet
    }
    
    const char *orbiter_filename= "app_orbiter.txt";
    // copy / export / write orbiter
    if(key_state('c'))
    {
        clear_key_state('c');
        camera.write_orbiter(orbiter_filename);
    }
    // paste / read orbiter
    if(key_state('v'))
    {
        clear_key_state('v');
        
        Orbiter tmp;
        if(tmp.read_orbiter(orbiter_filename) == 0)
            // ne modifie camera que si la lecture est correcte...
            camera= tmp;
    }
    
    // screenshot
    if(key_state('s'))
    {
        static int calls= 1;
        
        clear_key_state('s');
        screenshot("app", calls++);
    }
}
