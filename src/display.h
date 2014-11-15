#ifndef DISPLAY_H
#define DISPLAY_H

struct SDL_Surface;
struct ZigConfig;

#include <string>
#include <vector>
#include <list>

#include <SDL.h>
#include "vec2.h"
class Texture;

struct Res
{
	Res( int width, int height ) : w(width),h(height) {}
	bool operator==( Res const& other ) const
		{ return other.w==w && other.h==h; }
	bool operator<( Res const& other ) const
		{ return w<other.w || h<other.h; }
	int w; int h;
};


class Display
{
public:
    SDL_Window *m_Window;
    vec2 m_Extent[4];
	Display( bool fullscreen );
	virtual ~Display();

	void ChangeSettings( bool fullscreen );

	bool IsFullscreen() const
        { return false;}
    void HandleResize( int winw, int winh );

	void AddTexture( Texture* t );
	void RemoveTexture( Texture* t);	// OK to pass in null

	void TakeScreenshot();

private:
	void OpenMode( Res res, int depth, bool fullscreen );

    SDL_GLContext m_GLContext;


	std::list< Texture* > m_Textures;
};


#endif // DISPLAY_H

