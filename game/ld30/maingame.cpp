// Copyright 2014 Peter Wallström

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

#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif															// We Can Avoid Errors

enum class gamestates { INTROSCREEN, PLAY, WIN, CREDITS };


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
lightswitchtimer(0.0f),
flashttimer(0.0f),
deathcounter(0.0f),
tiles_width(16),
tiles_height(16),
player_movedir(0),
gamestate(gamestates::INTROSCREEN),
camctrl_x(0.0f),
camctrl_jump(0.0f),
camctrl_level(1),
playerpos(2, 2, 0),
titlefade(0.0f),
player_is_moving(false),
colup(false),
coldown(false),
colleft(false),
colright(false),
cam(),
mainPlayGraph(&cam),
player(&cam)
{
}


bool MainGameState::Initialize(int width, int height)
{
	glzResourcemanager rm;

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
	glEnable(GL_CULL_FACE);

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

	unsigned int ad[64]={2,2,2,1,2,2,2,2,
						 2,2,4,1,4,2,2,2,
						 3,3,0,1,0,3,3,3,
						 3,3,0,1,0,3,3,3,
						 2,2,4,1,4,2,2,2,
						 2,2,2,1,2,2,2,2,
						 2,2,2,1,1,1,1,1,
						 2,2,2,2,2,2,2,2};





	mt.LoadIdentity();
	mt2.LoadIdentity();
	mt3.LoadIdentity();

	mo.LoadIdentity();
	mg.LoadIdentity();
	mh.LoadIdentity();


	
	
	//glzTranslatef(mt,-3.9f,1.9f,0);
	mg.translate(0,0,0);	

	mt.scale(0.17f,0.17f,0.17f);
	mt2.scale(0.3f, 0.3f, 0.3f);
	mt3.scale(0.13f, 0.13f, 0.13f);
	mg.scale(32.0f, 32.0f, 32.0f);
	mh.scale(0.4f, 0.4f, 0.4f);


	mh.rotate(90,1.0f,0.0f,0.0f);

	mh.translate(-8.0,8.0,0.0);

	gdl_textures();

	rm.load_all();

	ProgramObject = glzShaderLoad("data\\glsl.vert", "data\\glsl.frag", glzVAOType::AUTO);
	ProgramObjectFSQ = glzShaderLoad("data\\fsq.vert", "data\\fsq.frag", glzVAOType::AUTO);
	ProgramObjectAtlas = glzShaderLoad("data\\atlastexture.vert", "data\\atlastexture.frag", glzVAOType::AUTO);
	glzShaderLink(ProgramObject);
	glzShaderLink(ProgramObjectFSQ);
	glzShaderLink(ProgramObjectAtlas);
	// load the textures

	level_1_a.load("data\\l1a.tga", glzTileType::DOUBLE_LAYER);
	level_1_b.load("data\\l1b.tga", glzTileType::DOUBLE_LAYER);
	level_1_d.load("data\\l1d.tga", glzTileType::QUAD_LAYER);
	/*
	level_1_light_a.load("data\\light1a.tga");
	level_1_light_a.type = glzTileType::DOUBLE_LAYER;
	level_1_light_b.load("data\\light1b.tga");
	level_1_light_b.type = glzTileType::DOUBLE_LAYER;
	level_1_light_dynamic.load("data\\light1_dynamic.tga");
	level_1_light_dynamic.type = glzTileType::QUAD_LAYER;*/




	return true;												// Return TRUE (Initialization Successful)
}



bool MainGameState::icanswitch(void)
{

	if (!level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5, 1, true)) return false;
	if (!level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5 - 0.1, 1, true)) return false;
	if (!level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5 + 0.1, 1, true)) return false;
	if (!level_1_d.getTilecolision(playerpos.x - 0.1, level_1_d.height - playerpos.y + 0.5, 1, true)) return false;
	if (!level_1_d.getTilecolision(playerpos.x + 0., level_1_d.height - playerpos.y + 0.51, 1, true)) return false;

	if (!level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5, 2, true)) return false;
	if (!level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5 - 0.1, 2, true)) return false;
	if (!level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5 + 0.1, 2, true)) return false;
	if (!level_1_d.getTilecolision(playerpos.x - 0.1, level_1_d.height - playerpos.y + 0.5, 2, true)) return false;
	if (!level_1_d.getTilecolision(playerpos.x + 0., level_1_d.height - playerpos.y + 0.51, 2, true)) return false;

	return true;

}

void MainGameState::reset_game(void)
{
	playerpos.x = 2;
	playerpos.y = 2;
	uselight = false;

}

void MainGameState::trigger_win(void)
{
	gamestate = gamestates::WIN;

}

void MainGameState::Deinitialize(void)											// Any User DeInitialization Goes Here
{

	// this shouldn't normally be nessecary, but it's better to make it a habit to delete data for when you start to load and unload resources mid game.

	glzKillAllVAO();		// deletes all vaos in existance


}

void MainGameState::Update(float seconds)								// Perform Motion Updates Here
{
	glzInput input;

	if (input.getKeyState(VK_ESCAPE) == TRUE)					// Is ESC Being Pressed?
	{
		mMessageQuit = true;						// Terminate The Program
	}

	if (input.getKeyState(VK_F1) == TRUE)						// Is F1 Being Pressed?
	{
		mMessageFullscreen = true;							// Toggle Fullscreen Mode
	}

	testanimTimer += seconds;
	if (testanimTimer > 0.45) { testanim++; testanimTimer -= 0.45; }
	if (testanim >= 4) testanim = 0;



	if (gamestate == gamestates::INTROSCREEN)
	{

		if (input.getKeyState(VK_RETURN) == TRUE)
		{
			gamestate = gamestates::PLAY;						// Terminate The Program
			reset_game();
		}

		titlefade += seconds*1.0;
		if (titlefade >= 1.0f) { titlefade = 1.0f; }

			

	}





	if (gamestate==gamestates::PLAY)
	{
	
		player_is_moving = false;
		colup = false,
		coldown = false;
		colleft = false;
		colright = false;


		if (uselight)
		{ 
		
		colup = level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5 - 0.1, 2, true);
		coldown = level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5 + 0.1, 2, true);
		colleft = level_1_d.getTilecolision(playerpos.x - 0.1, level_1_d.height - playerpos.y + 0.5, 2, true);
		colright = level_1_d.getTilecolision(playerpos.x + 0.1, level_1_d.height - playerpos.y + 0.5, 2, true);
		
		}

		else 	
		{

			colup = level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5 - 0.1, 1, true);
			coldown = level_1_d.getTilecolision(playerpos.x, level_1_d.height - playerpos.y + 0.5 + 0.1, 1, true);
			colleft = level_1_d.getTilecolision(playerpos.x - 0.1, level_1_d.height - playerpos.y + 0.5, 1, true);
			colright = level_1_d.getTilecolision(playerpos.x + 0.1, level_1_d.height - playerpos.y + 0.5, 1, true);

		}
		
		if (input.getKeyState('W') && colup) { playerpos.y += seconds*5.0; player_is_moving = true;	player_movedir = 3; }
		if (input.getKeyState('S') && coldown) { playerpos.y -= seconds*5.0; player_is_moving = true; player_movedir = 0; }

		if (input.getKeyState('A') && colleft) { playerpos.x -= seconds*5.0; player_is_moving = true; player_movedir = 2; }
		if (input.getKeyState('D') && colright) { playerpos.x += seconds*5.0; player_is_moving = true; player_movedir = 1; }

	
	if (player_is_moving) playeranimTimer += seconds;
	else playeranim = 0;
	if (playeranimTimer > 0.1) { playeranim++; playeranimTimer -= 0.1; }
	if (playeranim >= 4) playeranim = 0;

	lightswitchtimer += seconds;

	

	if ((input.getKeyState(VK_SPACE) == TRUE) && (lightswitchtimer>1.0) && (icanswitch()))						// Is F1 Being Pressed?
	{
	
		if (((int)playerpos.x == 27) && ((int)playerpos.y == 16) && (!uselight)) gamestate = gamestates::WIN;


		if (uselight) uselight = false;
		else uselight = true;
		lightswitchtimer = 0.0f;
		flashanimate = 0;


		

		
	}

	if (flashanimate<7)flashttimer += seconds;

	if ((flashttimer>0.07) && (flashanimate<7))						// Is F1 Being Pressed?
	{
		flashanimate++;
		if (flashanimate == 7) flashanimate =7;
		flashttimer = 0.0f;

	}


	}



	if (gamestate == gamestates::CREDITS)
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

		
		if (input.getKeyState(VK_RETURN) == TRUE)
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
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	
	float mtemp[16];
	glEnable(GL_TEXTURE_2D);
	unsigned int loc1 = glGetUniformLocation(ProgramObject,"projMat");
	unsigned int loc2 = glGetUniformLocation(ProgramObject,"texunit0");
	unsigned int loc3 = glGetUniformLocation(ProgramObject,"tint");


	unsigned int loc4 = glGetUniformLocation(ProgramObjectAtlas, "projMat");
	unsigned int loc5 = glGetUniformLocation(ProgramObjectAtlas, "texunit0");
	unsigned int loc6 = glGetUniformLocation(ProgramObjectAtlas, "texunit1");
	unsigned int loc7 = glGetUniformLocation(ProgramObjectAtlas, "layer");
	unsigned int loc8 = glGetUniformLocation(ProgramObjectAtlas, "anim");

	unsigned int loc9 = glGetUniformLocation(ProgramObjectAtlas, "width");
	unsigned int loc10 = glGetUniformLocation(ProgramObjectAtlas, "height");
	unsigned int loc11 = glGetUniformLocation(ProgramObjectAtlas, "a_width");
	unsigned int loc12 = glGetUniformLocation(ProgramObjectAtlas, "a_height");

	glUseProgram(ProgramObject);
	glUniform1i(loc2, 0);
	glUniform4f(loc3, 1.0f, 1.0f, 1.0f, 1.0f);

	glUseProgram(ProgramObjectAtlas);
	glUniform1i(loc5, 0);
	glUniform1i(loc6, 1);
	glUniform1i(loc7, 0); //layer
	glUniform1i(loc8, testanim);

	glUniform1i(loc9, level_1_a.width);
	glUniform1i(loc10, level_1_a.height);
	glUniform1i(loc11, tiles_width);
	glUniform1i(loc12, tiles_height);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	float aspect = level_1_a.width / level_1_a.height;

	// i have used these gamestates in a few games for LD48 now and they are really quick and dirty, but effective.
	// they allow you to quickly make a title screen and end screen at the end of the project without changing that much code, instead you just encapsulate it in a gamestate

	float zoom = 25.0f;

	if (gamestate == gamestates::INTROSCREEN)
	{


		
		glDisable(GL_DEPTH_TEST);

		glUseProgram(ProgramObject);
		
		glDisable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
		glBlendColor( titlefade, titlefade, titlefade, 1.0f);
		glEnable(GL_BLEND);

		m.LoadIdentity();
		m.ortho2D(-10, 10, -10, 10);
		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);

		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("title.title"));

		glzDirectSpriteRender(0, 0, 0, 20, 20, 0, 0, 1.0, 1.0, glzOrigin::CENTERED);

		glDisable(GL_BLEND);

	}



	if (gamestate == gamestates::PLAY)
	{
		glDisable(GL_DEPTH_TEST);

	//	glBlendFunc(GL_ONE, GL_ONE);
	//	glEnable(GL_BLEND);

		glUseProgram(ProgramObject);

		m.LoadIdentity();
		m.ortho2D(-5 , 5 , -5, 5);
		m.translate(-playerpos.x*0.5, -playerpos.y*0.5, 0.0);
		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);

		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("sprite.water"));
		

		int xi = (int)playerpos.x*0.5, yi = (int)playerpos.y*0.5;
		//float xf = playerpos.x - (float)xi, yf = playerpos.y - (float)yi;

		int xa = -5+xi, ya=-5+yi;

		// water layer
		while (xa<7 + xi)
		{
			while (ya < 7 + yi)
			{

				if (uselight) glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("sprite.water"));
				else glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("sprite.water2"));
				glzDirectSpriteRenderAtlas(xa, ya, 0, 1.11, 1.11, 4, 1, testanim, glzOrigin::CENTERED);
				//if (uselight) glzDirectSpriteRenderAtlas(xa,ya, 0, 1.11, 1.11, 4, 1, testanim, glzOrigin::CENTERED);
				//else glzDirectSpriteRenderAtlas(xa,ya, 0, 1.11, 1.11, 4, 1, testanim, glzOrigin::CENTERED);
				
				//if (uselight) glzDirectSpriteRenderAtlas(1.0 + xa*1.0 - (xf*1.0), 1.0 + ya - (yf*2.0), 0, 2.11, 2.11, 4, 2, testanim, glzOrigin::CENTERED);
				//else glzDirectSpriteRenderAtlas(1.0 + xa*0.5 - (xf*1.0), 1.0 + ya - (yf*2.0), 0, 2.11, 2.11, 4, 2, testanim, glzOrigin::CENTERED);

				ya+=1;
				
			}
			ya = -5 + yi;
			xa+=1;
		}

		

		glDisable(GL_BLEND);

		m.LoadIdentity();


		m.ortho(-100 * mView.getAspect(), 100 * mView.getAspect(), -100, 100, -100, 100);
		m.translate(-playerpos.x*zoom, -playerpos.y*zoom, 0.0);
		m.scale(zoom, zoom, 0.0);
		
		glUseProgram(ProgramObjectAtlas);
		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc4, 1, GL_FALSE, mtemp);


		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("atlas.tileset"));
		glActiveTexture(GL_TEXTURE0);


		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);


		if (uselight) glBindTexture(GL_TEXTURE_2D, level_1_b.tex);
		else glBindTexture(GL_TEXTURE_2D, level_1_a.tex);

		glUniform1i(loc7, 0);

		// bottom layer
		glzDirectSpriteRender(0.0, 0.0, 2, level_1_a.width, level_1_a.height, 0, 0, 1.0, 1.0, glzOrigin::BOTTOM_LEFT);
		glUniform1i(loc7, 1);
		glzDirectSpriteRender(0.0, 0.0, 2, level_1_a.width, level_1_a.height, 0, 0, 1.0, 1.0, glzOrigin::BOTTOM_LEFT);

	
		// character rendering goes in here
		glUseProgram(ProgramObject);
		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);

		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("sprite.player"));
		glzDirectSpriteRenderAtlas(playerpos.x, playerpos.y, 1, -1.5, -1.5, 4, 4, playeranim+(4*player_movedir), glzOrigin::CENTERED);


		
		
/*		// top layer
		glUseProgram(ProgramObjectAtlas);
		
		if (uselight) glBindTexture(GL_TEXTURE_2D, level_1_light_b.tex);
		else glBindTexture(GL_TEXTURE_2D, level_1_dark_b.tex);
		
		glUniform1i(loc7, 0);
		glzDirectSpriteRender(0.0, 0.0, 2, level_1_dark_b.width, level_1_dark_b.height, 0, 0, 1.0, 1.0, glzOrigin::BOTTOM_LEFT);
		glUniform1i(loc7, 1);
		glzDirectSpriteRender(0.0, 0.0, 2, level_1_dark_b.width, level_1_dark_b.height, 0, 0, 1.0, 1.0, glzOrigin::BOTTOM_LEFT);

		glDisable(GL_BLEND);*/

		
		//render flash animation
		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_BLEND);
				
		glUseProgram(ProgramObject);

		m.LoadIdentity();		
		m.ortho2D(-10, 10, -10, 10);
		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);
		
		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("sprite.flash"));
		glzDirectSpriteRenderAtlas(0, 0, 0, 21.0, 21.0, 4, 2, flashanimate, glzOrigin::CENTERED);

		glDisable(GL_BLEND);

	}

	if (gamestate == gamestates::CREDITS)
	{
		//glzDrawTexture(screentexture[2], 0, 0, 0, viewport[2], viewport[3], 0, 0, 0, 1, 1);
		//draw_backdrop(screentexture[2]);
		glUseProgram(ProgramObject);
		m.LoadIdentity();
		m.ortho2D(-10, 10, -10, 10);
		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);

		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("title.credits"));

		glzDirectSpriteRender(0, 0, 0, 20, 20, 0, 0, 1.0, 1.0, glzOrigin::CENTERED);


	}


	if (gamestate == gamestates::WIN)
	{
		//glzDrawTexture(screentexture[1], 0, 0, 0, viewport[2], viewport[3], 0, 0, 0, 1, 1);
		glUseProgram(ProgramObject);
		m.LoadIdentity();
		m.ortho2D(-10, 10, -10, 10);
		m.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);

		glBindTexture(GL_TEXTURE_2D, rm.gettextureHandle("title.win"));

		glzDirectSpriteRender(0, 0, 0, 20, 20, 0, 0, 1.0, 1.0, glzOrigin::CENTERED);


	}


	glUseProgram(0);
	glFlush ();													// Flush The GL Rendering Pipeline
}
