// Copyright 2013 Peter Wallström
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.

// while it is not required i like to request a few things
// 1. please do share any meaningfull/usefull changes/additions/fixes you make with me so that i could include it in any future version
// 2. likewise do share any ideas for improvements
// 3. If you make something comersiol or at least something you release publicly that relies on this code then i would like to know and maybe use in my CV
// 4. Please do include me in your credits

// glz 2D example app - i wouldn't say it produces any meningfull results, but it's a start.
// visit http://www.flashbang.se or contact me at overlord@flashbang.se
// the entire toolkit should exist in it's entirety at github
// https://github.com/zeoverlord/glz.git

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


static PFNGLUSEPROGRAMPROC						glUseProgram;
static PFNGLUNIFORM1IPROC                       glUniform1i;
static PFNGLUNIFORMMATRIX4FVPROC                glUniformMatrix4fv;
static PFNGLUNIFORM4FARBPROC                    glUniform4f;
static PFNGLGETUNIFORMLOCATIONPROC              glGetUniformLocation;
static PFNGLBLENDCOLORPROC						glBlendColor;
static PFNGLACTIVETEXTUREPROC					glActiveTexture;



static PFNGLISRENDERBUFFEREXTPROC    glIsRenderbufferEXT = NULL;
static PFNGLBINDRENDERBUFFEREXTPROC    glBindRenderbufferEXT = NULL;
static PFNGLDELETERENDERBUFFERSEXTPROC    glDeleteRenderbuffersEXT = NULL;
static PFNGLGENRENDERBUFFERSEXTPROC    glGenRenderbuffersEXT = NULL;
static PFNGLRENDERBUFFERSTORAGEEXTPROC    glRenderbufferStorageEXT = NULL;
static PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC    glGetRenderbufferParameterivEXT = NULL;
static PFNGLISFRAMEBUFFEREXTPROC    glIsFramebufferEXT = NULL;
static PFNGLBINDFRAMEBUFFEREXTPROC   glBindFramebufferEXT = NULL;
static PFNGLDELETEFRAMEBUFFERSEXTPROC    glDeleteFramebuffersEXT = NULL;
static PFNGLGENFRAMEBUFFERSEXTPROC    glGenFramebuffersEXT = NULL;
static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC    glCheckFramebufferStatusEXT = NULL;
static PFNGLFRAMEBUFFERTEXTURE1DEXTPROC    glFramebufferTexture1DEXT = NULL;
static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    glFramebufferTexture2DEXT = NULL;
static PFNGLFRAMEBUFFERTEXTURE3DEXTPROC    glFramebufferTexture3DEXT = NULL;
static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC    glFramebufferRenderbufferEXT = NULL;
static PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC    glGetFramebufferAttachmentParameterivEXT = NULL;
static PFNGLGENERATEMIPMAPEXTPROC   glGenerateMipmapEXT = NULL;


MainGameState::MainGameState() :
m(),
spriteframetimer(0.0f),
spriteframe(0),
gamestate(gamestates::PLAY),
camctrl_x(0.0f),
camctrl_jump(0.0f),
camctrl_level(1),
fb(),
cam(),
mainPlayGraph(&cam),
player(&cam)
{
}


void MainGameState::loadMainRendergraph(void)
{
	glzResourcemanager rm;
	n.rs.rotate(20, 0.0, 0.0, 1.0);
	n.pos = vert3(200.0, -130.0, 0.0);



	mainPlayGraph.sort_z = true;
	mainPlayGraph.add(obj2d_Clear());
	mainPlayGraph.add(obj2d_Fullscreen(-1, rm.gettexture("background.back1")));

	// load the tilemap
	tilemap.load("ld33\\data\\map1_a.tga", glzTileType::DOUBLE_LAYER);
	tilemap2.load("ld33\\data\\map1_b.tga", glzTileType::DOUBLE_LAYER);
	tilemap3.load("ld33\\data\\map1_d.tga", glzTileType::QUAD_LAYER);

	mainPlayGraph.add(obj2d_Tiles(42, &tilemap, 0, 16, 16, 1.0, nullptr, node3(vert3(-240, -150.0, 0.0)), rm.gettexture("atlas.tileset"), 128, 1.0f));
	mainPlayGraph.add(obj2d_Tiles(42, &tilemap, 1, 16, 16, 1.0, nullptr, node3(vert3(-240, -150.0, 0.0)), rm.gettexture("atlas.tileset"), 128, 1.0f));
	mainPlayGraph.add(obj2d_Tiles(42, &tilemap2, 0, 16, 16, 1.0, nullptr, node3(vert3(-240, -150.0, 0.0)), rm.gettexture("atlas.tileset"), 128, 1.0f));
	mainPlayGraph.add(obj2d_Tiles(42, &tilemap2, 1, 16, 16, 1.0, nullptr, node3(vert3(-240, -150.0, 0.0)), rm.gettexture("atlas.tileset"), 128, 1.0f));

	//tempgraph.set(42, glzOBject2DSetvar::SPRITE, glzSprite(2, 2, 2, 0.0));




//	mainPlayGraph.add(obj2d_Sprite(343, glzSprite(8, 4, 16, 0.0), nullptr, node3(), rm.gettexture("sprite.explotion128a"), 1.0));


	//tempgraph.add(obj2d_Sprite(62, glzSprite(), &n, node3(), texture[1], 1.0));
	//mainPlayGraph.add(obj2d_Sprite(62, glzSprite(), &n, node3(), rm.gettexture("default"), 1.0));


	glzSpriteList expl_spritelist;
	glzSpriteanimationList expl_spritelist_vector;


	expl_spritelist_vector.map.push_back(glzSpriteList(8, 4));


//	int v[] = { 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
//	expl_spritelist_vector.map.push_back(glzSpriteList(8, 4, v, 32));




	//mainPlayGraph.add(obj2d_Sprite(111, expl_spritelist_vector, nullptr, node3(vert3(100.0, 0.0, 0.0)), rm.gettexture("sprite.explotion128a"), 1.0, 30.0f));

	mainPlayGraph.set(343, glzOBject2DSetvar::SCALE, 2.0f);
	mainPlayGraph.set(111, glzOBject2DSetvar::BLEND, glzBlendingMode::ADDITIVE);
	mainPlayGraph.set(111, glzOBject2DSetvar::CURRENT_ANIMATION, 0);
	mainPlayGraph.set(111, glzOBject2DSetvar::ANIMATIONSTOP);
	//mainPlayGraph.set(343, glzOBject2DSetvar::Z_LEVEL, 2.0f);
	

	mainPlayGraph.set(62, glzOBject2DSetvar::ORIGIN, glzOrigin::CENTERED);

	mainPlayGraph.add(obj2d_Object2DGraph(32, &player));




	glzSpriteanimationList player_spritelist_vector;

	
	int v[] = { 0,1,2,1 };
	player_spritelist_vector.map.push_back(glzSpriteList(5, 5, v, 4));
	int v2[] = { 5, 6, 7, 6 };
	player_spritelist_vector.map.push_back(glzSpriteList(5, 5, v2, 4));




	player.add(obj2d_Sprite(1, player_spritelist_vector, nullptr, pl.pos, rm.gettexture("sprite.player"), 1.0, 4.0f));
	player.set(1, glzOBject2DSetvar::BLEND, glzBlendingMode::ALPHA);
	player.set(1, glzOBject2DSetvar::CURRENT_ANIMATION, 0);
	player.set(1, glzOBject2DSetvar::WIDTH, 48.0f);
	player.set(1, glzOBject2DSetvar::HEIGHT, 48.0f);
	player.set(1, glzOBject2DSetvar::ANIMATIONPLAY);
	player.set(1, glzOBject2DSetvar::NODE_LOCAL, pl.pos);

}


bool MainGameState::Initialize(int width, int height)					// Any GL Init Code & User Initialiazation Goes Here
{
	glzResourcemanager rm;

	GetFocus();
	GetAsyncKeyState(WM_KEYUP);

	// Start Of User Initialization
	//angle		= 0.0f;											// Set Starting Angle To Zero

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

	glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
	glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
	glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
	glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
	glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
	glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
	glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
	glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
	glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
	glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");

	
	ProgramObject = glzShaderLoad("ld33\\data\\glsl.vert", "ld33\\data\\glsl.frag", glzVAOType::AUTO);
	
	glzShaderLink(ProgramObject);

	pl.pos = node3(vert3(-250.0, (float(camctrl_level - 1)*(8.0f*23.0f)) -150, 0.0));
	pl.x = 0.0f;
	pl.y = 0.0f;
	pl.heading_left = false;
	pl.punching = false;
	pl.walking = false;


	// load the textures


	gdl_textures();
	rm.load_all();

	cam.setsize(480, 300);
	cam.moveTo(vert3(0.0,0.0, 0.0));
	cam.moveSpeed(100);

	loadMainRendergraph();

	fb.init(480, 300);

	return true;												// Return TRUE (Initialization Successful)
}


void MainGameState::Deinitialize(void)										// Any User DeInitialization Goes Here
{

	// this shouldn't normally be nessecary, but it's better to make it a habit to delete data for when you start to load and unload resources mid game.

	glzKillAllVAO();		// deletes all vaos in existance


}

void MainGameState::Update(float seconds)							// Perform Motion Updates Here
{

	glzAppinitialization app;
	glzInput input;
	glzMatrix mt;
	mt.LoadIdentity();
	mt.scale(0.17f,0.17f,0.17f);


	glzResourcemanager rm;
	rm.load_one();



	if (input.getKeyState(VK_ESCAPE) == TRUE)					// Is ESC Being Pressed?
	{
		mMessageQuit = true;						// Terminate The Program
	}

	if (input.getKeyState(VK_F1) == TRUE)						// Is F1 Being Pressed?
	{
		mMessageFullscreen = true;							// Toggle Fullscreen Mode
	}
	
	//angle += seconds*10;						// Update angle Based On The Clock
		
	//if (angle>360) angle=0;






if (gamestate == gamestates::PLAY)
{


//	if (g_keys->keyDown['A'] == TRUE) cam.moveToRel(vert3(seconds * -100, 0.0, 0.0));
//	if (g_keys->keyDown['D'] == TRUE) cam.moveToRel(vert3(seconds * 100, 0.0, 0.0));
//	if (g_keys->keyDown['W'] == TRUE) cam.moveToRel(vert3(0.0, seconds * 100, 0.0));
//	if (g_keys->keyDown['S'] == TRUE) cam.moveToRel(vert3(0.0, seconds * -100, 0.0));
	//if (g_keys->keyDown[VK_SPACE] == TRUE) { cam.zoomTo(5.0f); 	}
	//else cam.zoomTo(1.0f);

	pl.walking = false;


	if (input.getKeyState('W') == TRUE)
	{
		pl.y += seconds * 50;
		if (pl.y > 24.0f) pl.y = 24.0f;

		pl.pos.pos.y = (float(camctrl_level - 1)*(8.0f*23.0f)) + pl.y - 150;

		pl.walking = true;
		pl.pos.tick(seconds);
		player.set(1, glzOBject2DSetvar::NODE_LOCAL, pl.pos);
	}

	if (input.getKeyState('S') == TRUE)
	{
		pl.y -= seconds * 50; 
		if (pl.y <0.0f) pl.y = 0.0f;

		pl.pos.pos.y = (float(camctrl_level - 1)*(8.0f*23.0f)) + pl.y -150;

		pl.walking = true;
		pl.pos.tick(seconds);
		player.set(1, glzOBject2DSetvar::NODE_LOCAL, pl.pos);
	}


	if (input.getKeyState('A') == TRUE)
	{
		pl.heading_left = true;
		pl.x -= seconds * 50;

		if (pl.x < 0.0f) pl.x = 0.0f;
		pl.walking = true;		
		pl.pos.pos.x = pl.x - 250;
		pl.pos.tick(seconds);
		player.set(1, glzOBject2DSetvar::NODE_LOCAL, pl.pos);
		player.set(1, glzOBject2DSetvar::CURRENT_ANIMATION, 1);
	}


	if (input.getKeyState('D') == TRUE)
	{
		pl.heading_left = false;
		pl.x += seconds * 50;
		if (pl.x > 996.0f) pl.x = 996.0f;
		pl.walking = true;
		pl.pos.pos.x = pl.x - 250;
		pl.pos.tick(seconds);
		player.set(1, glzOBject2DSetvar::NODE_LOCAL, pl.pos);
		player.set(1, glzOBject2DSetvar::CURRENT_ANIMATION,0);
	}


	if (pl.walking) player.set(1, glzOBject2DSetvar::ANIMATIONPLAY);
	else player.set(1, glzOBject2DSetvar::ANIMATIONSTOP);


	if (input.getKeyState(VK_RIGHT) == TRUE) camctrl_x -= seconds * 100;
	if (input.getKeyState(VK_LEFT) == TRUE) camctrl_x += seconds * 100;


	if (camctrl_x < 0.0f) camctrl_x = 0.0f;
	if (camctrl_x > 736.0f) camctrl_x = 736.0f;

	if (camctrl_level > 8) camctrl_level = 8;
	if (camctrl_level < 0) camctrl_level = 0;

	cam.moveSnap(vert3(-camctrl_x, (float(camctrl_level - 1)*(-8.0f*23.0f)) - camctrl_jump, 0.0));
	

	cam.update(seconds);

	n.tick(seconds);
	mainPlayGraph.update(seconds);
	player.update(seconds);

	if (input.getKeyState('1') == TRUE) gamestate = gamestates::PLAY;
	

}

void MainGameState::DisplayUpdate(int width, int height)
{
	mView.setDisplay(0, 0, width, height);


}


void MainGameState::Draw(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	
	//glzResourcemanager rm;

	float mtemp[16];
	glEnable(GL_TEXTURE_2D);
	unsigned int loc1 = glGetUniformLocation(ProgramObject,"projMat");
	unsigned int loc2 = glGetUniformLocation(ProgramObject,"texunit0");
	unsigned int loc3 = glGetUniformLocation(ProgramObject,"tint");

	glUseProgram(ProgramObject);
	glUniform1i(loc2, 0);	
	glUniform4f(loc3, 1.0f,1.0f,1.0f,1.0f);

	// i have used these gamestates in a few games for LD48 now and they are really quick and dirty, but effective.
	// they allow you to quickly make a title screen and end screen at the end of the project without changing that much code, instead you just encapsulate it in a gamestate

	

	if (gamestate == gamestates::PLAY)
	{

		fb.startRendering();
		
		glzMatrix mt;
		mt.LoadIdentity();

		mt.transferMatrix(&mtemp[0]);
		glUniformMatrix4fv(loc1, 1, GL_FALSE, mtemp);


		glBlendFunc(GL_ONE, GL_ONE);
		glEnable(GL_BLEND);
		mainPlayGraph.draw();
		glDisable(GL_BLEND);

		fb.stopRendering();


		glzShaderUseBasic();


		m.LoadIdentity();
		mt.LoadIdentity();


		mt.translate(-1.0, -1.0, 0.0);

		mt.scale(2.0, 2.0, 1.0);
		unsigned int basic_program = glzShaderReurnBasic();

		glzUniformMatrix4fv(basic_program, "projMat", mt);
		glzUniform1i(basic_program, "texunit0", 0);
		glzUniform4f(basic_program, "color", 1.0f, 1.0f, 1.0f, 1.0f);
		glzDirectSpriteRender(m, fb.getColorBufferHandle(), glzSprite(), 1, 1, glzOrigin::BOTTOM_LEFT);

		glDisable(GL_BLEND);

		

	}


	glUseProgram(0);
	glFlush ();													// Flush The GL Rendering Pipeline
}
