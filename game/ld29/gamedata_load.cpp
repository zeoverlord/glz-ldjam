


#include "gamedata_load.h"
#include "..\..\glz-core\utilities\resourcemanager.h"



void gdl_textures(void)
{
	GLZ::glzResourcemanager rm;
	
	// fonts
	rm.createTexture("font.vandyke", "data\\fonts\\vandyke.tga", GLZ::glzTexFilter::LINEAR, 2);

	// backgrounds

	rm.createTexture("background.overlay", "data\\overlay.tga", GLZ::glzTexFilter::LINEAR);

	// sprites
	rm.createTexture("sprite.entity-tiles", "data\\entity-tiles.tga", GLZ::glzTexFilter::NEAREST);

	// tilemaps
	rm.createTexture("atlas.tileset", "data\\tileset.tga", GLZ::glzTexFilter::NEAREST);



	rm.createTexture("title.title", "data\\title.tga", GLZ::glzTexFilter::LINEAR);
	rm.createTexture("title.win", "data\\win.tga", GLZ::glzTexFilter::LINEAR);
	rm.createTexture("title.credits", "data\\credits.tga", GLZ::glzTexFilter::LINEAR);
	


}

