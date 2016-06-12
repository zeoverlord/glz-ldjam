// Copyright 2016 Peter Wallström
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

// glz example 2d class
// visit http://www.flashbang.se or contact me at overlord@flashbang.se
// the entire toolkit should exist in it's entirety at github
// https://github.com/zeoverlord/glz.git

#pragma once


#include "..\..\glz-core\utilities\type.h"
#include "..\..\glz-core\utilities\glz.h"
#include "..\..\glz-core\utilities\sprite.h"
#include "..\..\glz-core\utilities\resourcemanager.h"
#include "..\..\glz-core\utilities\vectormath.h"
#include "..\..\glz-core\utilities\framebuffer.h"
#include "..\..\glz-core\layout\viewport.h"
#include "..\..\glz-core\state\baseState.h"
#include "..\..\glz-core\effects\particle.h"
#include "..\..\glz-core\2d\2d-graph.h"
#include <vector>
#include <memory>

// how it works
// at a start the viewport its inside it's parent window
// if width is wider than what the origin coordinate allows then the window will be resized

class MainGameState : public GLZ::glzBaseState
{

public:

	MainGameState();
	~MainGameState(){ Deinitialize(); }
	bool Initialize(int width, int height) override;
	void Deinitialize(void) override;
	void Update(float seconds) override;
	void DisplayUpdate(int width, int height) override;
	void Draw(void) override;

	bool icanswitch(void);

	void reset_game(void);
	void trigger_win(void);

private:
		
	int testanim;
	int playeranim;
	int flashanimate;
	float testanimTimer;
	float playeranimTimer;
	float lightswitchtimer;
	float flashttimer;
	float deathcounter;

	int tiles_width;
	int tiles_height;
	int player_movedir;

	bool player_is_moving;
	float titlefade;
	GLZ::vert3 playerpos;
	bool colup;
	bool coldown;
	bool colleft;
	bool colright;

	GLZ::glztiles level_1_a;
	GLZ::glztiles level_1_b;
	GLZ::glztiles level_1_d;
	bool uselight;

	GLZ::glzMatrix m;

	enum class gamestates { INTROSCREEN, PLAY, WIN, CREDITS };

	gamestates gamestate;
	float spriteframetimer;
	int spriteframe;

	float camctrl_x = 0.0f;
	float camctrl_jump = 0.0f;
	int camctrl_level = 1;

	GLZ::glzCamera2D cam;
	GLZ::Object2DGraph mainPlayGraph;
	GLZ::Object2DGraph player;


	typedef struct
	{
		GLZ::node3 pos;
		float x;
		float y;
		bool heading_left;
		bool walking;
		bool playing;
		bool punching;
	} playerstruct;

	playerstruct pl;

	GLZ::node3 n;
	GLhandleARB  ProgramObject;
	GLhandleARB  ProgramObjectFSQ;
	GLhandleARB  ProgramObjectAtlas;
	GLZ::glzSimpleParticleSystem ps;

private:
	void loadMainRendergraph(void);
	

};