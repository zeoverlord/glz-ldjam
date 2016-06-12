


#include "gamedata_load.h"
#include "..\..\glz-core\utilities\resourcemanager.h"



void gdl_textures(void)
{
	GLZ::glzResourcemanager rm;
	
	// fonts
	rm.createTexture("font.arial", "data\\fonts\\arial.tga", GLZ::glzTexFilter::LINEAR, 2);

	// backgrounds

	// sprites
	rm.createTexture("sprite.player", "data\\player.tga", GLZ::glzTexFilter::NEAREST);
	rm.createTexture("sprite.flash", "data\\flash.tga", GLZ::glzTexFilter::LINEAR);
	rm.createTexture("sprite.water", "data\\water.tga", GLZ::glzTexFilter::NEAREST);
	rm.createTexture("sprite.water2", "data\\water2.tga", GLZ::glzTexFilter::NEAREST);

	// tilemaps
	rm.createTexture("atlas.tileset", "data\\tileset.tga", GLZ::glzTexFilter::NEAREST);



	rm.createTexture("title.title", "data\\title.tga", GLZ::glzTexFilter::LINEAR);
	rm.createTexture("title.win", "data\\win.tga", GLZ::glzTexFilter::LINEAR);
	rm.createTexture("title.credits", "data\\credits.tga", GLZ::glzTexFilter::LINEAR);



}

