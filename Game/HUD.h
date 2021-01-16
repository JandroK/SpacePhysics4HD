#ifndef __HUD_H__
#define __HUD_H__

#include "Textures.h"
#include "p2Point.h"

class Font;
class HUD: public Module
{
public:
	HUD();
	~HUD();

	bool Awake(pugi::xml_node&);

	bool Strart();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

private:

	Font* font;

};


#endif // !__HUD_H__
