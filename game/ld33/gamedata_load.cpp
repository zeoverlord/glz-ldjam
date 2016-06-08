


#include "gamedata_load.h"
#include "..\..\glz-core\utilities\resourcemanager.h"



void gdl_textures(void)
{
	GLZ::glzResourcemanager rm;

	// default texture
//	rm.createTexture("default", "data\\derpy_phirana.tga", glzTexFilter::LINEAR);

	// fonts
	rm.createTexture("font.arial", "ld33\\data\\fonts\\arial.tga", GLZ::glzTexFilter::LINEAR, 2);
//	rm.createTexture("font.minya_m", "data\\fonts\\minya_m.tga", glzTexFilter::LINEAR);
//	rm.createTexture("font.ms_gothic", "data\\fonts\\ms_gothic.tga", glzTexFilter::LINEAR);
//	rm.createTexture("font.digitalstrip_l", "data\\fonts\\digitalstrip_l.tga", glzTexFilter::LINEAR);
//	rm.createTexture("font.morpheus_l", "data\\fonts\\morpheus_l.tga", glzTexFilter::LINEAR);

	// backgrounds
	rm.createTexture("background.back1", "ld33\\data\\bkg1.tga", GLZ::glzTexFilter::LINEAR);
	//rm.createTexture("background.back", "data\\back.tga", glzTexFilter::LINEAR);
	//rm.createTexture("background.cv90", "data\\cv90-1080p-04.tga", glzTexFilter::LINEAR, 1);

	// sprites
//	rm.createTexture("sprite.derpy_phirana", "data\\derpy_phirana.tga", glzTexFilter::LINEAR, 2);
//	rm.createTexture("sprite.explotion128a", "data\\explotion128a.tga", glzTexFilter::NEAREST, 3);
//	rm.createTexture("sprite.blob", "data\\blob.tga", glzTexFilter::NEAREST);
	rm.createTexture("sprite.player", "ld33\\data\\player.tga", GLZ::glzTexFilter::NEAREST);

	// tilemaps
//	rm.createTexture("atlas.tinytiles", "data\\tinytiles.tga", glzTexFilter::NEAREST);
	rm.createTexture("atlas.tileset", "ld33\\data\\tileset.tga", GLZ::glzTexFilter::NEAREST);


}

