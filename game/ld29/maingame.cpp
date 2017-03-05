// Copyright 2014 Peter Wallström
//




#include "maingame.h"
#include "gamedata_load.h"

#include <windows.h>											// Header File For Windows
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include <gl\glext.h>
#include "..\..\glz-core\app\appbase.h"
#include <fstream>
#include <math.h>
#include <string>
#include "..\..\glz-core\3d\geo.h"
#include "..\..\glz-core\shader\shader.h"
#include "..\..\glz-core\utilities\glz.h"
#include "..\..\glz-core\utilities\vectormath.h"
#include "..\..\glz-core\utilities\color.h"
#include "..\..\glz-core\image\tex.h"
#include "..\..\glz-core\2d\geo-2d.h"
#include "..\..\glz-core\3d\geo-generate.h"
#include "..\..\glz-core\2d\2d-utilities.h"
#include "..\..\glz-core\utilities\resourcemanager.h"
#include "..\..\glz-core\input\input.h"


using namespace std;
using namespace GLZ;

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking
#pragma comment( lib, "bass.lib" )	

#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif															// We Can Avoid Errors



// User Defined Variables

static PFNGLUSEPROGRAMPROC						glUseProgram;
static PFNGLUNIFORM1IPROC                       glUniform1i;
static PFNGLUNIFORMMATRIX4FVPROC                glUniformMatrix4fv;
static PFNGLUNIFORM4FARBPROC                    glUniform4f;
static PFNGLGETUNIFORMLOCATIONPROC              glGetUniformLocation;
static PFNGLBLENDCOLORPROC						glBlendColor;
static PFNGLACTIVETEXTUREPROC					glActiveTexture;


MainGameState::MainGameState() :
m(),
spriteframetimer(0.0f),
spriteframe(0),
testanim(0),
playeranim(0),
flashanimate(0),
testanimTimer(0.0f),
playeranimTimer(0.0f),
walktimer(0.0f),
bobing(0.0f),
lightswitchtimer(0.0f),
titlefade(0.0f),
flashttimer(0.0f),
deathcounter(0.0f),
player_movedir(0),
gamestate(gamestates::INTROSCREEN),
camctrl_x(0.0f),
camctrl_jump(0.0f),
camctrl_level(1),
playerpos(2, 6, 0),
player_is_moving(false),
colup(false),
coldown(false),
colleft(false),
colright(false),
coljump(false),
tiles_width(16),
tiles_height(16),
leveltex_filename("data\\l1a.tga"),
cpos(),
playerspeed(),
playerfacing_right(true),
player_moving(false),
player_score(0),
gem_sound_switch(false),
walk_sound_switch(false),
playbump(0.0f)
{
}


void MainGameState::Error(const char *es)
{

	GLZ::glzAppinitialization app;
	char mes[200];
	sprintf_s(mes,200, "%s\n(error code: %d)", es, BASS_ErrorGetCode());
	// replace with internal error messaging
	MessageBoxA(app.data.hWnd, mes, 0, 0);
}


bool MainGameState::getTileExtra(int x, int y, int layer)
{
	

/*	if (x<0) { x = 0;  }
	if (x>=arm_width - 1) { x = arm_width - 1; }

	if (y<0) { y = 0; }
	if (y>arm_height - 1) { y = arm_height - 1;  }

	if (img_1_data[glz2dTo1dImageRemap(x, y, 1 + (layer * 2), 4, arm_width, arm_height, true)] > 127) return true;*/

	if(levelColision.getTilecolision(x, y, 1 + (layer * 2), false)) return true;
	//if(level.get_pixel(x, y, 1)  > 127) return true;
	else return false;

}


void MainGameState::playerColide(float x, float y)
{
	int colide = 0;
	colup = false,
	coldown = false;
	colleft = false;
	colright = false;
	coljump = false;
	

	if (!getTileExtra((int)(playerpos.x + 0.3), (int)(playerpos.y - 1.0), 0)) coldown = true;
	if (!getTileExtra((int)(playerpos.x), (int)(playerpos.y - 1.0), 0)) coldown = true;
	if (!getTileExtra((int)(playerpos.x - 0.3), (int)(playerpos.y - 1.0), 0)) coldown = true;


	if (!getTileExtra((int)(playerpos.x + 0.3), (int)(playerpos.y + 0.7), 0)) colup = true;
	if (!getTileExtra((int)(playerpos.x), (int)(playerpos.y + 0.7), 0)) colup = true;
	if (!getTileExtra((int)(playerpos.x - 0.3), (int)(playerpos.y + 0.7), 0)) colup = true;


	if (!getTileExtra((int)(playerpos.x - 0.97), (int)(playerpos.y + 0.3), 0)) colright = true;
	if (!getTileExtra((int)(playerpos.x - 0.97), (int)(playerpos.y), 0)) colright = true;
	if (!getTileExtra((int)(playerpos.x - 0.97), (int)(playerpos.y - 0.3), 0)) colright = true;


	if (!getTileExtra((int)(playerpos.x + 0.97), (int)(playerpos.y + 0.3), 0)) colleft = true;
	if (!getTileExtra((int)(playerpos.x + 0.97), (int)(playerpos.y), 0)) colleft = true;
	if (!getTileExtra((int)(playerpos.x + 0.97), (int)(playerpos.y - 0.3), 0)) colleft = true;



	if (!getTileExtra((int)(playerpos.x + 0.3), (int)(playerpos.y - 1.2), 0)) coljump = true;
	if (!getTileExtra((int)(playerpos.x), (int)(playerpos.y - 1.2), 0)) coljump = true;
	if (!getTileExtra((int)(playerpos.x - 0.3), (int)(playerpos.y - 1.2), 0)) coljump = true;

	if (!getTileExtra((int)(playerpos.x + 0.3), (int)(playerpos.y + 1.2), 0)) coljump = false;
	if (!getTileExtra((int)(playerpos.x), (int)(playerpos.y + 1.2), 0)) coljump = false;
	if (!getTileExtra((int)(playerpos.x - 0.3), (int)(playerpos.y + 1.2), 0)) coljump = false;

	

		
	return;

}

void MainGameState::add_gems(void)
{

	items.push_back({ vert3(64.0, 6.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(63.0, 6.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(62.0, 6.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(55.0, 11.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(54.0, 11.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(53.0, 11.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(52.0, 11.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(51.0, 11.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(50.0, 11.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(49.0, 11.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(48.0, 11.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
		

	items.push_back({ vert3(13.0, 5.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(12.0, 5.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(11.0, 5.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(10.0, 5.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(9.0, 5.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(8.0, 5.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(29.0, 10.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(25.0, 31.0, 0.0), itemtypes::GREEN_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(12.0, 21.0, 0.0), itemtypes::GREEN_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(11.0, 31.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(15.0, 30.0, 0.0), itemtypes::RED_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(11.0, 48.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(11.0, 42.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(18.0, 46.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(17.0, 39.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(13.0, 36.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(16.0, 52.0, 0.0), itemtypes::RED_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(10.0, 46.0, 0.0), itemtypes::GREEN_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(10.0, 41.0, 0.0), itemtypes::DULL_GEM, false, 50, glzRandfs() *  PI });


	items.push_back({ vert3(30.0, 55.0, 0.0), itemtypes::DULL_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(25.0, 60.0, 0.0), itemtypes::DULL_GEM, false, 50, glzRandfs() *  PI });


	items.push_back({ vert3(22.0, 63.0, 0.0), itemtypes::RED_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(26.0, 55.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(23.0, 54.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(23.0, 48.0, 0.0), itemtypes::RED_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(30.0, 57.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(35.0, 60.0, 0.0), itemtypes::GREEN_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(47.0, 67.0, 0.0), itemtypes::GREEN_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(54.0, 50.0, 0.0), itemtypes::GREEN_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(43.0, 60.0, 0.0), itemtypes::GREEN_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(39.0, 61.0, 0.0), itemtypes::RED_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(38.0, 52.0, 0.0), itemtypes::RED_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(54.0, 62.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(61.0, 62.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(54.0, 62.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(75.0, 51.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(76.0, 51.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(77.0, 51.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(61.0, 47.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(62.0, 47.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(63.0, 47.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(44.0, 40.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(45.0, 40.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(46.0, 40.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(47.0, 40.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(48.0, 40.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(49.0, 40.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });

	items.push_back({ vert3(90.0, 72.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(83.0, 75.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(88.0, 79.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(83.0, 82.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(87.0, 85.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });


	
	items.push_back({ vert3(111.0, 99.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(112.0, 99.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(113.0, 99.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(114.0, 99.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(115.0, 99.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(116.0, 99.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });

	
	items.push_back({ vert3(118.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(119.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(120.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(121.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });

	
	items.push_back({ vert3(111.0, 107.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(112.0, 107.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(113.0, 107.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(114.0, 107.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(115.0, 107.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(116.0, 107.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });

	
	items.push_back({ vert3(103.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(104.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(105.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(106.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(107.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });
	items.push_back({ vert3(108.0, 103.0, 0.0), itemtypes::BLUE_GEM, false, 50, glzRandfs() *  PI });


	items.push_back({ vert3(15.5, 94.0, 0.0), itemtypes::EXIT, false, 0, 0 });


}

void MainGameState::reset_game(void)
{
	playerfacing_right = true;
	player_moving = false;

	player_score = 0;
	items.clear();
	add_gems();

	playerpos.x = 74;
	cpos.x = -playerpos.x / (20);

	playerpos.y = 8;

}


void MainGameState::trigger_win(void)
{
	gamestate = gamestates::WIN;
	
}

void MainGameState::play_gem(void)
{

	if (gem_sound_switch)
	{
		BASS_ChannelPlay(gemch1, true);
		gem_sound_switch = false;
	}
	else
	{
		BASS_ChannelPlay(gemch2, true);
		gem_sound_switch = true;
	}


}

void MainGameState::play_walk(void)
{

	if (walk_sound_switch)
	{
		BASS_ChannelPlay(walkch1, true);
		walk_sound_switch = false;
	}
	else
	{
		BASS_ChannelPlay(walkch2, true);
		walk_sound_switch = true;
	}


}



// manages entity pickups
void MainGameState::entity_colide(void)
{

	float dist = 0,xd=0,yd=0;

	auto i = items.begin();
	int i2 = 0; // o yes this is way uggly, but i think it woks
	while (i < items.end()) {

		xd = abs(items[i2].pos.x - playerpos.x); 
		yd = abs(items[i2].pos.y - playerpos.y);

		dist = sqrt(xd*xd+yd*yd);

		if (dist<1.0)
			{
			if (items[i2].type == itemtypes::EXIT) { trigger_win(); return; }
			
			if (items[i2].type == itemtypes::BLUE_GEM) player_score += 25;
			if (items[i2].type == itemtypes::RED_GEM) player_score += 50;
			if (items[i2].type == itemtypes::GREEN_GEM) player_score += 75;
			if (items[i2].type == itemtypes::DULL_GEM) player_score -= 75;
			if (items[i2].type == itemtypes::DULL_GEM) BASS_ChannelPlay(d_gemch, true);
				else play_gem();
				
				i = items.erase(i);
			}
		else 
			{
				++i;
				i2++;
			}


	}

}





bool MainGameState::Initialize(int width, int height)				// Any GL Init Code & User Initialiazation Goes Here
{
	
	glzResourcemanager rm;
	glzAppinitialization app;
	if (!BASS_Init(-1, 44100, 0, app.data.hWnd, NULL))
		Error("Can't initialize device");
	
	
	reset_game();

	GetFocus();
	GetAsyncKeyState(WM_KEYUP);

	// Start Of User Initialization

	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						// Black Background
	glClearDepth (1.0f);										// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
	glEnable (GL_DEPTH_TEST);									// Enable Depth Testing
	glShadeModel (GL_SMOOTH);									// Select Smooth Shading
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	glFrontFace(GL_CCW);
	

	glUseProgram				= (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
	glGetUniformLocation		= (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");                                                                     
	glUniform1i= (PFNGLUNIFORM1IPROC) wglGetProcAddress("glUniform1i");
	glUniform4f= (PFNGLUNIFORM4FARBPROC) wglGetProcAddress("glUniform4fARB");
	glUniformMatrix4fv= (PFNGLUNIFORMMATRIX4FVPROC) wglGetProcAddress("glUniformMatrix4fv");

	glBlendColor = (PFNGLBLENDCOLORPROC)wglGetProcAddress("glBlendColor");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");

	




	glzMatrix mt;
	glzMatrix mt2;
	glzMatrix mt3;
	glzMatrix mo;
	glzMatrix mg;
	glzMatrix mh;






	mt.LoadIdentity();
	mt2.LoadIdentity();
	mt3.LoadIdentity();

	mo.LoadIdentity();
	mg.LoadIdentity();
	mh.LoadIdentity();


	
	
	mg.translate(0,0,0);	

	mt.scale(0.17f,0.17f,0.17f);
	mt2.scale(0.3f, 0.3f, 0.3f);
	mt3.scale(0.13f, 0.13f, 0.13f);
	mg.scale(32.0f, 32.0f, 32.0f);
	mh.scale(0.4f, 0.4f, 0.4f);	
	mh.rotate(90,1.0f,0.0f,0.0f);
	mh.translate(-8.0,8.0,0.0);
	

	ProgramObject = glzShaderLoad("data\\glsl.vert", "data\\glsl.frag", glzVAOType::AUTO);;
	glzShaderLink(ProgramObject);


	level.load(leveltex_filename, glzTileType::DOUBLE_LAYER);
	levelColision.load("data\\l1d.tga", glzTileType::QUAD_LAYER);
	
	
	playerpos.x = 74;
	cpos.x = -playerpos.x / (20);

	playerpos.y =6;

	gdl_textures();

	rm.load_all();

	gem = BASS_SampleLoad(FALSE, "data\\gem.wav", 0, 0, 3, BASS_SAMPLE_OVER_POS);
	walk = BASS_SampleLoad(FALSE, "data\\walk.wav", 0, 0, 3, BASS_SAMPLE_OVER_POS); 
	d_gem = BASS_SampleLoad(FALSE, "data\\dull_gem.wav", 0, 0, 3, BASS_SAMPLE_OVER_POS);

	gemch1 = BASS_SampleGetChannel(gem, FALSE);
	gemch2 = BASS_SampleGetChannel(gem, FALSE);
	walkch1 = BASS_SampleGetChannel(walk, FALSE);
	walkch2 = BASS_SampleGetChannel(walk, FALSE);
	d_gemch = BASS_SampleGetChannel(d_gem, FALSE);

	return TRUE;												// Return TRUE (Initialization Successful)
}


void MainGameState::Deinitialize(void)										// Any User DeInitialization Goes Here
{

	// this shouldn't normally be nessecary, but it's better to make it a habit to delete data for when you start to load and unload resources mid game.


	glzKillAllVAO();		// deletes all vaos in existance


}

void MainGameState::Update(float seconds)								// Perform Motion Updates Here
{

	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);


		glzMatrix mt;
		mt.LoadIdentity();
		mt.scale(0.17f,0.17f,0.17f);

	glzInput input;

	if (input.getKeyState(VK_ESCAPE) == TRUE)					// Is ESC Being Pressed?
		{
			mMessageQuit = true;						// Terminate The Program
		}

	if (input.getKeyState(VK_F1) == TRUE)						// Is F1 Being Pressed?
		{
			mMessageFullscreen = true;							// Toggle Fullscreen Mode
		}

	keyTimer += seconds;




	if (gamestate == gamestates::INTROSCREEN)
	{

		if (input.getKeyState(VK_RETURN) == TRUE)
		{
			gamestate = gamestates::PLAY;						// Terminate The Program
			reset_game();
		}

		titlefade += seconds*1.0;
		if (titlefade >= 1.0f) { titlefade = 1.0f; }


		playeranimTimer += seconds;
		if (playeranimTimer > 0.15) { playeranim++; playeranimTimer -= 0.15; }
		if (playeranim >= 4) playeranim = 0;

		bobing += seconds * 2;
		if (bobing > 2.0*PI) { bobing -= 2.0*PI; }

	}







	if (gamestate == gamestates::PLAY)
	{

	testanimTimer += seconds;
	if (testanimTimer > 0.45) { testanim++; testanimTimer -= 0.45; }
	if (testanim >= 4) testanim = 0;

	playeranimTimer += seconds;
	if (playeranimTimer > 0.07) { playeranim++; playeranimTimer -= 0.07; }
	if (playeranim >= 4) playeranim = 0;

	

	bobing += seconds*2;
	if (bobing > 2.0*PI) { bobing -= 2.0*PI; }
	


	walktimer += seconds;







	player_moving = false;

	
		//if (g_keys->keyDown[VK_UP] == TRUE) { cpos.y -= seconds*2.0; }
		//if (g_keys->keyDown[VK_DOWN] == TRUE) { cpos.y += seconds*2.0; }

		//if (g_keys->keyDown[VK_LEFT] == TRUE) { cpos.x += seconds*2.0; }
		//if (g_keys->keyDown[VK_RIGHT] == TRUE) { cpos.x -= seconds*2.0; }

/*	
		if(input.getKeyState('W')) { playerspeed.y += seconds*300.0; }
		if(input.getKeyState('S')) { playerspeed.y -= seconds*300.0; }

		if(input.getKeyState('A'))  { playerspeed.x -= seconds*300.0; playerfacing_right = false; }
		if(input.getKeyState('D')) { playerspeed.x += seconds*300.0; playerfacing_right = true; }*/

	if (keyTimer > 0.15)		
		if ((input.getKeyState('W')) && (coljump)) {
			playerspeed.y += 20.0; keyTimer = 0;
		}
	

		//if ((g_keys->keyDown['S'] == TRUE) && (!coldown)) { playerspeed.y -= seconds*50.0; }

	if ((input.getKeyState('A')) && (!colright)) { playerspeed.x -= seconds*200.0; playerfacing_right = false; }
	if ((input.getKeyState('D')) && (!colleft)) { playerspeed.x += seconds*200.0; playerfacing_right = true; }

	
	
	float xdist = (-playerpos.x / 20 - cpos.x) * 5;
	float ydist = (-playerpos.y / 20 - cpos.y) * 5;

	float camspeedx = seconds*(xdist*xdist);
	float camspeedy= seconds*(ydist*ydist);

	if (camspeedx < 0.003) camspeedx = 0.003;
	if (camspeedy < 0.003) camspeedy = 0.003;
	
	if (abs(xdist) < 0.01) camspeedx = 0;
	if (abs(ydist) < 0.01) camspeedy = 0;

	if (cpos.x < -playerpos.x / (20)) cpos.x += camspeedx;
	if (cpos.x > -playerpos.x / (20)) cpos.x -= camspeedx;

	if (cpos.y < -playerpos.y / (20)) cpos.y += camspeedy;
	if (cpos.y > -playerpos.y / (20)) cpos.y -= camspeedy;

	//if (xdist<0.01) cpos.x = -playerpos.x /(20);


	//physics




	playerColide(playerpos.x, playerpos.y);
	entity_colide();

//	playerspeed -= playerspeed/seconds;
	playerspeed.x *= 1.0 - 13.8*seconds;
//	playerspeed.y *= 1.0 - 13.8*seconds;

	playerpos.x += playerspeed.x*seconds;
	playerpos.y += playerspeed.y*seconds;

	if (playerspeed.x>0.1)player_moving = true;
	if (playerspeed.x<-0.1)player_moving = true;

	if (!coldown) playerspeed.y -= 40.0f*seconds;

	if (playerspeed.y < -30.0) playerspeed.y = -30.0;

	if (coldown)
	{		
		
		if (playerspeed.y<0) playerspeed.y *= -0.0;
		
		playerpos.y = quantize(playerpos.y+0.9, 1.0);

		
	}


	if (colup)
	{
		
		if (playerspeed.y>0) playerspeed.y *= -0.4;
		
		playerpos.y = quantize(playerpos.y-0.1, 1.0);
	}

	if (colleft)
	{
		
		if (playerspeed.x>0) playerspeed.x *= -0.1;
		playerpos.x = quantize(playerpos.x, 1.0);
		
	}

	if (colright)
	{
		if (playerspeed.x<0) playerspeed.x *= -0.1;
		playerpos.x = quantize(playerpos.x+0.9, 1.0);
	}


	

	if ((coldown) && (playbump>0.25)) { playbump = 0.0; play_walk(); }


	if (coldown) playbump = 0.0;
	else playbump += seconds;

	if ((player_moving) && (coldown) && (walktimer>0.1)) { play_walk(); walktimer = 0.0; }
	



	}


if (gamestate==gamestates::CREDITS)
	{

	deathcounter += seconds;
	if (deathcounter > 2.0)
	{
		mMessageQuit = true;
		deathcounter = 0.0f;
	}
	
	}


if (gamestate == gamestates::WIN)
{
	playeranimTimer += seconds;
	if (playeranimTimer > 0.15) { playeranim++; playeranimTimer -= 0.15; }
	if (playeranim >= 4) playeranim = 0;

	bobing += seconds * 2;
	if (bobing > 2.0*PI) { bobing -= 2.0*PI; }


	if (input.getKeyState(VK_RETURN))
	{
		gamestate = gamestates::PLAY;						// Terminate The Program
		reset_game();
	}

}

if (input.getKeyState('1') == TRUE)  gamestate = gamestates::INTROSCREEN;
if (input.getKeyState('2') == TRUE)  gamestate = gamestates::PLAY;
if (input.getKeyState('3') == TRUE)  gamestate = gamestates::WIN;
if (input.getKeyState('4') == TRUE)  gamestate = gamestates::CREDITS;
}



void MainGameState::DisplayUpdate(int width, int height)
{
	mView.setDisplay(0, 0, width, height);
}

void MainGameState::Draw(void)
{
	glzResourcemanager rm;


	unsigned int atlas_program = glzShaderReurnTilemap();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer

	float mtemp[16];
	glEnable(GL_TEXTURE_2D);

	unsigned int loc1 = glGetUniformLocation(ProgramObject, "projMat");
	unsigned int loc2 = glGetUniformLocation(ProgramObject, "texunit0");
	unsigned int loc3 = glGetUniformLocation(ProgramObject, "tint");	

	glUseProgram(ProgramObject);
	glUniform1i(loc2, 0);
	glUniform4f(loc3, 1.0f, 1.0f, 1.0f, 1.0f);
	glUseProgram(atlas_program);
	glzUniformMatrix4fv(atlas_program, "projMat", m);
	glzUniform1i(atlas_program, "texunit0", 0);
	glzUniform1i(atlas_program, "texunit1", 1);
	glzColor blendcolor(COL_WHITE);
	glzUniform4f(atlas_program, "color", blendcolor.r, blendcolor.g, blendcolor.b, blendcolor.a);
	glzUniform1i(atlas_program, "layer", 0);
	glzUniform1i(atlas_program, "anim", testanim);
	glzUniform1i(atlas_program, "width", level.width);
	glzUniform1i(atlas_program, "height", level.height);
	glzUniform1i(atlas_program, "a_width", tiles_width);
	glzUniform1i(atlas_program, "a_height", tiles_height);
	glzUniform1i(atlas_program, "extr", 0);

	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);

	// i have used these gamestates in a few games for LD48 now and they are really quick and dirty, but effective.
	// they allow you to quickly make a title screen and end screen at the end of the project without changing that much code, instead you just encapsulate it in a gamestate

	if (gamestate == gamestates::INTROSCREEN)
	{

		glzBackdrop(rm.gettextureHandle("title.title"));
		glDisable(GL_DEPTH_TEST);
		m.LoadIdentity();

		m.ortho(-1, 1, -1 / mView.getAspect(), 1 / mView.getAspect(), -100, 100);
		m.scale(0.1, 0.1, 0.0);

		glUseProgram(ProgramObject);

		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);


		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("sprite.entity-tiles"));

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
		glEnable(GL_BLEND);


		// render player


		glzDirectSpriteRenderAtlas(-7, 0.5, 1, 2.0, -2.0, 8, 8, playeranim, glzOrigin::CENTERED);

		//items


		glzDirectSpriteRenderAtlas(-2, -3.4 + (sin(bobing)*0.07), 1, 1.0, -1.2, 8, 8, 56, glzOrigin::CENTERED);
		glzDirectSpriteRenderAtlas(0, -3.4 + (sin(bobing + 1)*0.07), 1, 1.0, -1.2, 8, 8, 57, glzOrigin::CENTERED);
		glzDirectSpriteRenderAtlas(2, -3.4 + (sin(bobing + 2)*0.07), 1, 1.0, -1.2, 8, 8, 58, glzOrigin::CENTERED);





		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
		glBlendColor(1.0 - titlefade, 1.0 - titlefade, 1.0 - titlefade, 1.0f);
		glEnable(GL_BLEND);

		// add a fade here
		glzBackdrop(rm.gettextureHandle("title.title"));
	//	glzDrawTexture(rm.gettextureHandle("title.title"), 0, 0, 0, viewport[2], viewport[3], 0, 0, 0, 0, 0);

		glDisable(GL_BLEND);

	}
	

	if(gamestate == gamestates::PLAY)
	{
		glDisable(GL_DEPTH_TEST);
		m.LoadIdentity();
		
		m.ortho(-1, 1, -1 / mView.getAspect(), 1 / mView.getAspect(), -100, 100);
		m.translate(quantize(cpos.x, 0.007), quantize(cpos.y, 0.007), 0.0);
		m.scale(0.05, 0.05, 0.0);

		glUseProgram(atlas_program);

		glzUniformMatrix4fv(atlas_program, "projMat", m);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, level.tex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("atlas.tileset"));
		glActiveTexture(GL_TEXTURE0);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glzUniform1i(atlas_program, "layer", 0);
		glzDirectSpriteRender(0.0, 0.0, 2, level.width, level.height, 0, 0, 1.0, 1.0, glzOrigin::BOTTOM_LEFT);
		
		glzUniform1i(atlas_program, "layer", 1);
		glzDirectSpriteRender(0.0, 0.0, 2, level.width, level.height, 0, 0, 1.0, 1.0, glzOrigin::BOTTOM_LEFT);
		glDisable(GL_BLEND);



		// render enteties

		glUseProgram(ProgramObject);

		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);


		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("sprite.entity-tiles"));

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);



		//items
		int spr = 63;

		for(auto v : items)
		{
			if(v.type == itemtypes::BLUE_GEM) spr = 56;
			if(v.type == itemtypes::RED_GEM) spr = 57;
			if(v.type == itemtypes::GREEN_GEM) spr = 58;
			if(v.type == itemtypes::DULL_GEM) spr = 59;
			if(v.type != itemtypes::EXIT) glzDirectSpriteRenderAtlas(v.pos.x, v.pos.y + (sin(bobing + v.bob_offset)*0.07), 1, 1.0, -1.0, 8, 8, spr, glzOrigin::CENTERED);
		}


		// render player
				
		if(player_moving)
		{
			if(playerfacing_right) glzDirectSpriteRenderAtlas(playerpos.x, playerpos.y, 1, 2.0, -2.0, 8, 8, playeranim, glzOrigin::CENTERED);
			else  glzDirectSpriteRenderAtlas(playerpos.x, playerpos.y, 1, -2.0, -2.0, 8, 8, playeranim, glzOrigin::CENTERED);

		}
		else
		{
			if(playerfacing_right) glzDirectSpriteRenderAtlas(playerpos.x, playerpos.y, 1, 2.0, -2.0, 8, 8, 0, glzOrigin::CENTERED);
			else  glzDirectSpriteRenderAtlas(playerpos.x, playerpos.y, 1, -2.0, -2.0, 8, 8, 0, glzOrigin::CENTERED);
		}

		//glBindTexture(GL_TEXTURE_2D, texture[3]);

		//if (getTileExtra((int)(playerpos.x), (int)(playerpos.y), 0))
		//if ((colup)  || (colleft) || (colright))

		//	glzDirectSpriteRenderAtlas(64, 6, 1, 1.5, -1.5, 8, 8, 56, glzOrigin::CENTERED, glzOrigin::CENTERED);

		glDisable(GL_BLEND);

				
		// draw overlay

		glDisable(GL_BLEND);

		glzBackdrop(rm.gettextureHandle("background.overlay"), glzBlendingMode::MULTIPLY);
		//glzDrawTexture(rm.gettextureHandle("background.overlay"), 0, 0, 0, viewport[2], viewport[3], 0, 0, 0, 1, 1);

		char tbuffer[160];

		sprintf_s(tbuffer, 160, "SCORE: %i", (int)player_score);

		glzDrawText(tbuffer, -0.45f, -0.45f, 0.050f, 1.1f, mView.getAspect(), rm.gettexture("font.vandyke"), COL_WHITE, glzOrigin::BOTTOM_LEFT);
	//	draw_text2(tbuffer, 1.0f, 40.0f, 24.0f, 1.1f, 2, ProgramObject, COL_WHITE, glzOrigin::BOTTOM_LEFT);


		
		
		glEnable(GL_DEPTH_TEST);

	}

	if (gamestate == gamestates::CREDITS)
	{
		
		glzBackdrop(rm.gettextureHandle("title.credits"));
		
	}


	if (gamestate == gamestates::WIN)
	{
		glzBackdrop(rm.gettextureHandle("title.win"));


		glDisable(GL_DEPTH_TEST);
		m.LoadIdentity();



		m.ortho(-1, 1, -1 / mView.getAspect(), 1 / mView.getAspect(), -100, 100);
		//m.translate(0, quantize(cpos.y, 0.007), 0.0);
		m.scale(0.1, 0.1, 0.0);

		glUseProgram(ProgramObject);

		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);


		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("sprite.entity-tiles"));

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		
		// render player

		glzDirectSpriteRenderAtlas(-7, 0.5, 1, 2.0, -2.0, 8, 8, playeranim, glzOrigin::CENTERED);

		 //items


		 glzDirectSpriteRenderAtlas(-2, -3.4 + (sin(bobing)*0.07), 1, 1.0, -1.2, 8, 8, 56, glzOrigin::CENTERED);
		 glzDirectSpriteRenderAtlas(0, -3.4 + (sin(bobing+1)*0.07), 1, 1.0, -1.2, 8, 8, 57, glzOrigin::CENTERED);
		 glzDirectSpriteRenderAtlas(2, -3.4 + (sin(bobing+2)*0.07), 1, 1.0, -1.2, 8, 8, 58, glzOrigin::CENTERED);
		

		 char tbuffer2[160];

		 sprintf_s(tbuffer2, 160, "SCORE: %i", (int)player_score);

		 glzDrawText(tbuffer2, (mView.getDisplayWidth() / 10.0)*2.5, (mView.getDisplayHeigth() / 10.0)*5.0, 24.0f, 1.1f, mView.getAspect(), rm.gettexture("font.vandyke"), COL_WHITE, glzOrigin::CENTERED);
	

		glDisable(GL_BLEND);



	}


	glUseProgram(0);
	glFlush ();													// Flush The GL Rendering Pipeline
}
