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
#include "..\..\glz-core\utilities\tiletools.h"
#include "..\..\glz-core\layout\viewport.h"
#include "..\..\glz-core\state\baseState.h"
#include "..\..\glz-core\effects\particle.h"
#include "..\..\glz-core\2d\2d-graph.h"
#include "bass.h"
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

private:
		
	int testanim;
	int playeranim;
	int flashanimate;
	float testanimTimer;
	float playeranimTimer;
	float lightswitchtimer;
	float flashttimer;
	float deathcounter;
	float walktimer;
	float bobing;
	float keyTimer;
	float titlefade;

	int tiles_width;
	int tiles_height;
	int player_movedir;

	bool player_is_moving;
	bool colup;
	bool coldown;
	bool colleft;
	bool colright;
	bool coljump;

	GLZ::glzMatrix m;

	enum class gamestates { INTROSCREEN, PLAY, WIN, CREDITS };

	gamestates gamestate;
	float spriteframetimer;
	int spriteframe;

	float camctrl_x = 0.0f;
	float camctrl_jump = 0.0f;
	int camctrl_level = 1;

	enum class itemtypes { BLUE_GEM, RED_GEM, GREEN_GEM, DULL_GEM, EXIT };

	typedef struct{
		GLZ::vert3 pos;
		itemtypes type;
		bool animated;
		int points;
		float bob_offset;
	}itemstruct;

	std::vector<itemstruct> items;	

	GLhandleARB  ProgramObject;
	GLZ::glzSimpleParticleSystem ps;

	std::string leveltex_filename;

	GLZ::vert3 cpos;
	GLZ::vert3 playerpos;
	GLZ::vec3 playerspeed;
	bool playerfacing_right;
	bool player_moving;
	int player_score;

	// sound stuff

	HSAMPLE gem, walk, d_gem;
	HCHANNEL gemch1, gemch2, walkch1, walkch2, d_gemch;
	bool gem_sound_switch;
	bool walk_sound_switch;
	float playbump;

	GLZ::glztiles level;
	GLZ::glztiles levelColision;

private:
	void loadMainRendergraph(void);
	void Error(const char *es);
	bool getTileExtra(int x, int y, int layer);
	void playerColide(float x, float y);
	void add_gems(void);
	void reset_game(void);
	void trigger_win(void);
	void play_gem(void);
	void play_walk(void);
	void entity_colide(void);
	

};