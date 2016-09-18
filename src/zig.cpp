
#include "zig.h"



#include "agent.h"
#include "agentmanager.h"
#include "bullets.h"
#include "completionscreen.h"
#include "controller.h"
#include "display.h"
#include "drawing.h"
#include "dudes.h"
#include "gameover.h"
#include "highscores.h"
#include "highscorescreen.h"
#include "image.h"
#include "level.h"
#include "leveldef.h"
#include "mathutil.h"
#include "optionsscreen.h"
#include "player.h"
#include "proceduraltextures.h"
#include "resources.h"
#include "soundexplore.h"
#include "soundmgr.h"
#include "texture.h"
#include "titlescreen.h"
#include "util.h"
#include "wobbly.h"
#include "log.h"
#include "mainloop.h"
#include <SDL.h>

#ifdef ZIG_INSTALL_DIR
#include <unistd.h>
#endif


#ifdef CRIPPLED
	#include "crippleclock.h"
	static bool s_LaunchWeb = false;
#endif // CRIPPLED

ZigConfig g_Config;
std::vector<LevelDef> g_LevelDefs;

Player* g_Player=0;
Level* g_CurrentLevel=0;
Texture* g_Font=0;

Texture* g_Textures[TX_NUMTEXTURES] = {0};

AgentManager* g_Agents = 0;
Display* g_Display=0;
ControllerMgr* g_ControllerMgr = 0;
HighScores* g_HighScores = 0;

GameState* g_GameState = 0;

static std::string s_ZigUserDir;

static void PlayGame();
static void InitZigUserDir();

static void InitTextures();
static void FreeTextures();

int main( int argc, char*argv[] )
{
	try
	{
		InitZigUserDir();
        log_open(JoinPath(ZigUserDir(),"log.txt").c_str());
        log_infof("Started\n");

		g_Config.Init( argc, argv );


		// set up memory pooling system for agents
		Agent_Startup();

        //log_open("-");
		Resources::Init();

		if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER) != 0)
		{
			Wobbly e("SDL_Init() failed: %s\n", SDL_GetError() );
			throw e;
		}

//		SDL_WM_SetCaption( "Zig", 0 );
// TODO: SDL_SetWindowTitle(

#ifdef ZIG_INSTALL_DIR
		if( chdir( ZIG_INSTALL_DIR ) < 0 )
		{
			throw Wobbly( "Couldn't change to " ZIG_INSTALL_DIR );
		}
#endif
        g_ControllerMgr = new ControllerMgr();
        g_HighScores = new HighScores();
		g_Display = new Display( g_Config.fullscreen );

        InitTextures();

		//----------------------------------------------
		// Sound Init
#if !defined(DISABLE_SOUND)
		if( !g_Config.nosound )
		{
			try
			{
				RealSoundMgr::Create();
			}
			catch( Wobbly& e )
			{
				log_errorf("Error starting sound: %s - running silent\n",e.what() );
			}
		}
#endif
		if( !SoundMgr::Running() )
			NullSoundMgr::Create();

		//----------------------------------------------



		srand( SDL_GetTicks() );


		//----------------------------------------------
		{
			std::string levelfile = Resources::Map( "levels" );
			LevelParser parser( levelfile, g_LevelDefs );
		}

#ifdef CRIPPLED
		CrippleClock::Init();
#endif	// CRIPPLED

#if 0
        {
            SoundExplore exp;
            exp.Run();
        }
#endif

		//----------------------------------------------
		// MAIN
		//----------------------------------------------

		bool quit = false;

        mainloop();
        quit=true;

		while( !quit )
		{
#ifdef CRIPPLED
			if( CrippleClock::Expired() )
			{
				quit = true;
			}
#endif	// CRIPPLED
			// looping title sequence
			bool go=false;
			while( !quit && !go )
			{
				TitleScreen t;
				t.Run();

				if( t.Result() == CANCEL )
					quit=true;
				else if( t.Result() == STARTGAME )
					go=true;
				else if( t.Result() == TIMEOUT )
				{
					{
						// Demo mode
                        assert(!g_GameState);
                        g_GameState = new GameState();
						Player player(true);
						int num = (int)(Rnd(0.0f,(float)(g_LevelDefs.size()-2))+0.5f);
						Level l( g_LevelDefs[num], num+1, true);
						l.Run();
                        delete g_GameState;
                        g_GameState = 0;
					}

					HighScoreScreen scorescreen;
					scorescreen.Run();
				}
				else if( t.Result() == CONFIG )
				{
					OptionsScreen o;
					o.Run();
				}
			}

			if( !quit )
				PlayGame();
		}
#ifdef CRIPPLED
		{
			NagScreen n;
			n.Run();
			s_LaunchWeb = n.LaunchWeb();
		}
#endif //CRIPPLED

	}
	catch( Wobbly& e )
	{
		// uhoh...
		log_errorf("ERROR: %s\n", e.what() );
	}
	catch( Scene::QuitNotification& )
	{
		// if we end up here, it means player has asked the game to close
		// (via alt-f4, window decoration, whatever).
		// Slight abuse of exceptions perhaps, but greatly simplifies the
		// main loop!
	}

	SoundMgr::Destroy();
    delete g_HighScores;
    delete g_ControllerMgr;

	// clean up global textures
	if( g_Display )
        FreeTextures();

	delete g_Display;

	SDL_Quit();
	Agent_Shutdown();

#ifdef CRIPPLED
	if( s_LaunchWeb )
		CrippleClock::LaunchWebSite();
#endif	// CRIPPLED
    log_close();
	return 0;
}

// create/load global textures
void InitTextures()
{
    g_Font = new FileTexture( Resources::Map( "font.png" ).c_str() );
    g_Textures[TX_FONT] = g_Font;
    g_Textures[TX_INVADER] = new FileTexture( Resources::Map( "invaders.png").c_str() );
    g_Textures[TX_BLUEGLOW] = new BlueGlow( 64,64 );
    g_Textures[TX_NARROWBEAMGRADIENT] = new NarrowBeamGradient( 64,64 );
    g_Textures[TX_WIDEBEAMGRADIENT] = new WideBeamGradient( 64,64 );
    int i;
    for(i=0; i<TX_NUMTEXTURES; ++i)
    {
        assert(g_Textures[i]);
        g_Display->AddTexture(g_Textures[i]);
    }
}


// clean up global textures
void FreeTextures()
{
    int i;
    for(i=0; i<TX_NUMTEXTURES; ++i)
    {
        if(!g_Textures[i])
            continue;
        g_Display->RemoveTexture(g_Textures[i]);
        delete g_Textures[i];
        g_Textures[i] = 0;
    }
    g_Font = 0;
}


static void PlayGame()
{
    assert( g_GameState == 0 );
    g_GameState = new GameState();

	Player player;
	int wrapcount = 0;	// how many times all levels completed
	int levelindex=0;
	bool gameover=false;

	assert( g_LevelDefs.size() > 0 );

	while( !gameover )
	{
		switch( wrapcount )
		{
		case 0:
			g_GameState->BigHeadMode = false;
			g_GameState->NoExtraLives = false;
			break;
		case 1:
			g_GameState->BigHeadMode = true;
			g_GameState->NoExtraLives = false;
			break;
		case 2:
			g_GameState->BigHeadMode = false;
			g_GameState->NoExtraLives = true;
			break;
		}

		LevelDef* def;
		if( levelindex < (int)g_LevelDefs.size() )
			def = &g_LevelDefs[levelindex];
		else
			def = &g_LevelDefs.back();

		const int perceivedlevel =
			g_LevelDefs.size()*wrapcount + levelindex + 1;


        SceneResult result = NONE;
        {
    		Level l( *def, perceivedlevel );
	    	l.Run();
            result = l.Result();
        }

		if( result == DONE )
		{
			++levelindex;
			if( levelindex >= (int)g_LevelDefs.size() )
			{
				++wrapcount;
				levelindex = 0;
				CompletionScreen c( wrapcount );
				c.Run();
			}
		}
		else
		{
			gameover = true;
			GameOver g( player.Score(), perceivedlevel );
			g.Run();
			if(result != CANCEL)
			{

				int scoreidx = g_HighScores->Submit( player.Score() );
				HighScoreScreen scorescreen;
				if( scoreidx != -1 )
					scorescreen.EntryMode( scoreidx );

				// This makes sure that keypresses are correctly
				// mapped to characters (needed only for name entry).
//				SDL_EnableUNICODE(1);
				scorescreen.Run();
//				SDL_EnableUNICODE(0);
			}
		}
	}
    delete g_GameState;
    g_GameState = 0;
}




static void InitZigUserDir()
{
#ifdef _WIN32
	s_ZigUserDir = JoinPath( PerUserDir(), "Zig" );
#elif defined( __APPLE__ ) && defined( __MACH__ )
	s_ZigUserDir = JoinPath( PerUserDir(), "Zig" );
#else
	s_ZigUserDir = JoinPath( PerUserDir(), ".zig" );
#endif
	MakeDir( s_ZigUserDir );
}

std::string ZigUserDir()
{
	return s_ZigUserDir;
}

