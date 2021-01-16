#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "List.h"
#include "PerfTimer.h"
#include "Timer.h"

#include "PugiXml/src/pugixml.hpp"

#define CONFIG_FILENAME		"config.xml"
#define SAVE_STATE_FILENAME "save_game.xml"
	
// Modules
class Window;
class Input;
class Render;
class Textures;
class Physics;
class Audio;
class SceneManager;
class Player;
class HUD;
class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	// Methods to request Load / Save
	void LoadGameRequest();
	void SaveGameRequest() const;
	void LoadConfigRequested();
	void SaveConfigRequested() const;

	void SetLastScene(Module* scene) { lastScene = scene; }
	float GetCapMs();

	int GetFramerate();

	int GetFramesOnLastSecond() { return framesOnLastSecond; };
	

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGame(const SString _filename);
	bool SaveGame(const SString _filename) const;



public:

	// Modules
	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	Physics* physics;
	Audio* audio;
	SceneManager* sceneManager;
	Player* player;
	HUD* hud;
	//ModuleFonts* fonts;

private:

	int argc;
	char** args;
	SString title;
	SString organization;
	int framerate;
	List<Module *> modules;

	// TODO 2: Create new variables from pugui namespace:
	// a xml_document to store the config file and
	// two xml_node to read specific branches of the xml
	pugi::xml_document configFile;
	pugi::xml_node config;
	pugi::xml_node configApp;

	uint frames;
	PerfTimer ptimer;
	uint64 frameCount = 0;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;
	uint32 lastSecFrameCount = 0;
	uint32 prevLastSecFrameCount = 0;
	uint32 framesOnLastSecond = 0;
	uint32 lastFrameMs = 0;
	float dt = 0.0f;
	float perfTime;
	float oldLastFrame = 0.0f;
	float timeFramesSecond = 0.0f;
	float	cappedMs = -1;

	mutable bool saveGameRequested;
	bool loadGameRequested;

	pugi::xml_document stateFile;
	pugi::xml_node rootStateFile;

	mutable bool saveConfigRequested;
	bool loadConfigRequested;
	SString filenameGame = "save_game.xml";
	SString filenameConfig = "config.xml";

	float fPS = 0;
	bool changeFPS=false;

	Module* lastScene;
};

extern App* app;

#endif	// __APP_H__