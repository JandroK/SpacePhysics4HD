#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Textures.h"

#include "Animation.h"
#include "p2Point.h"
#include "Body.h"

enum State {

	IDLE,
	FLY,
	TURBO,
	ATTACK,
	HIT,
	DEADING,
	DEAD
};

struct PlayerData
{
	State state;

	Animation* currentAnimation;
	uint lives = 5;
	uint respawns = 0;
	float fuel = 0;
	//uint *stateShoot = 0;
	//iPoint* shootPosition;
	//iPoint* shootPointsCollision;
	SDL_Rect rectPlayer = {0,0,0,0};

	SDL_Texture* texture;
	SDL_Texture* texLaserFly;
	SDL_Texture* texLaserTurbo;
	SDL_Texture* texTurboVelocity;
	SDL_Texture* texHitDead;

	static const int numPoints = 3;
	fPoint* pointsCollision;// [numPoints] ;// = { { 0, -74 },{86, 42},{ -86, 42 } };
	fPoint* pointsCollisionWorld;// [numPoints] ;// = { { 0, -74 },{86, 42},{ -86, 42 } };
	fPoint vecDir = { 0,0 };
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

	void CheckWin();
	void CheckGameOver();

	bool PostUpdate();

	//void SetHit();
	//void CoinPlus() { playerData.fuel++; };
	//void LivePlus() { playerData.respawns++; };

	bool CleanUp();

	Body* GetBody() { return ship; };

private:

	void SpeedAnimationCheck(float dt);

	void CameraPlayer();

	void PlayerMoveAnimation();

	//void MovePlayer( float dt);

	void PlayerControls(float dt);
	void GodModeControls(float dt);


	// Load state game
	//bool LoadState(pugi::xml_node& data);
	// Save state game
	//bool SaveState(pugi::xml_node& data)const;

public:

	PlayerData playerData;
	bool godMode = false;
	fPoint positionInitial;

private:
	
	Body *ship;
	int levelScene;

	float velGodMode = 0;

	Animation* idleAnim;
	Animation* flyAnim;
	Animation* turboAnim;
	Animation* turboVelocityAnim;
	Animation* damageAnim;
	Animation* deadAnim;

	pugi::xml_document playerFile;
	SString folder;
	iPoint tmp;

	uint turboBigFx;
	uint turboSmallFx;
	uint deadFx;
	uint damageFx;

	State lastState;
};

#endif // _PLAYER_H_


