#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Textures.h"

#include "Animation.h"
#include "Point.h"


enum State {

	IDLE,
	FLY,
	TURBO,
	ATTACK,
	DEADING,
	DEAD,
	HIT,
};

struct PlayerData
{
	iPoint position;
	State state;

	Animation* currentAnimation;
	uint lives = 0;
	uint respawns = 0;
	uint fuel = 0;
	uint *stateShoot = 0;
	iPoint* shootPosition;
	iPoint* shootPointsCollision;
	SDL_Rect rectPlayer = {0,0,0,0};

	SDL_Texture* texture;
	static const int numPoints = 6;
	iPoint pointsCollision[numPoints] = { { 0,0 },{45 , 0},{ 45,-26 },{45 ,-54 }, {0, -54},{0, -26} };

};


class Player : public Module
{
public:

	Player();
	Player( iPoint pPosition, float pVelocity, SDL_Texture* pTexture);
	virtual ~Player();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CheckGameOver(int level);


	void SetHit();
	void CoinPlus() { playerData.fuel++; };
	void LivePlus() { playerData.respawns++; };

	bool CleanUp();

private:

	void SpeedAnimationCheck(float dt);

	void CameraPlayer();

	void PlayerMoveAnimation();

	void MovePlayer( float dt);

	void PlayerControls(float dt);
	void GodModeControls(float dt);

	// Load state game
	bool LoadState(pugi::xml_node& data);
	// Save state game
	bool SaveState(pugi::xml_node& data)const;

public:

	PlayerData playerData;
	bool godMode = false;
	iPoint* positionInitial= new iPoint(0,0);	
	bool win= false;

private:
	
	int levelScene;

	float gravity = 0.3f;
	float velGodMode = 0;

	Animation* idleAnim= new Animation();
	Animation* flyAnim = new Animation();
	Animation* turboAnim = new Animation( );
	Animation* atakAnim = new Animation();
	Animation* damageAnim = new Animation();
	Animation* deadAnim = new Animation();

	pugi::xml_document playerFile;
	SString folder;
	iPoint tmp;

	uint fireFx;
	uint damageFx;

	State lastState;
};

#endif // _PLAYER_H_


