#ifndef __MODULE_H__
#define __MODULE_H__

#include "SString.h"

#include "PugiXml/src/pugixml.hpp"
#include "SDL_image/include/SDL_image.h"

class App;

class Module
{
public:

	Module() : active(false)
	{}

	void Init()
	{
		active = true;
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	// Virtual methods to Load state
	virtual bool LoadState(pugi::xml_node&)
	{
		return true;
	}
	// Virtual methods to Save state
	virtual bool SaveState(pugi::xml_node&) const
	{
		return true;
	}
	 

public:

	SString name;
	bool active;
	float oldFPS=-1;

};

#endif // __MODULE_H__