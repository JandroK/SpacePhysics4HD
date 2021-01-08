#include "Player.h"
#include "Audio.h"
#include "Render.h"
#include "Input.h"
#include "App.h"


#include "Log.h"
#include "Defs.h"


Player::Player() : Module()
{
	name.Create("player");
}

Player::Player(iPoint pPosition, float pVelocity, SDL_Texture* pTexture): Module()
{
	playerData.state = IDLE;
	name.Create("player");

}

Player::~Player()
{}

bool Player::Start() 
{

	idleAnim = new Animation();
	flyAnim = new Animation();
	atakAnim = new Animation();
	damageAnim = new Animation();
	turboAnim = new Animation();

	godMode = true;

	playerData.texture = app->tex->Load("Assets/Textures/space_ship.png");

	SDL_QueryTexture(playerData.texture,NULL ,NULL, &playerData.rectPlayer.w, &playerData.rectPlayer.h);
	playerData.position = { WINDOW_W/2- (playerData.rectPlayer.w>>1), 10538 };
	playerData.state = IDLE;


	fireFx = app->audio->LoadFx("Assets/Audio/Fx/hello_man.wav");
	damageFx = app->audio->LoadFx("Assets/Audio/Fx/hello_man.wav");

	playerData.respawns = 3;
	playerData.fuel = 0;
	playerData.lives = 3;

	win = false;

	idleAnim->loop = true;
	idleAnim->speed = 0.05f;

	flyAnim->loop = true;
	flyAnim->speed = 0.08f;

	damageAnim->loop = false;
	damageAnim->speed = 0.005f;

	turboAnim->loop = true;
	turboAnim->speed = 0.10f;
	
	atakAnim->loop = false;
	atakAnim->speed = 0.10f;


	idleAnim->PushBack({ 0 ,0, playerData.rectPlayer.w, playerData.rectPlayer.h });
	
	for (int i = 0; i < 6; i++)
		flyAnim->PushBack({ 312 + (78 * i),0, 78, 78 });

	for (int i = 0; i < 4; i++)
		damageAnim->PushBack({ 1008 + (78 * i),0, 78, 78 });

	for (int i = 0; i < 1; i++)
		damageAnim->PushBack({ 1008 + (78 * i),0, 78, 78 });

	deadAnim->PushBack({ 1008 + (78 * 1),0, 78, 78 });

	for (int i = 0; i < 4; i++)
		turboAnim->PushBack({ 1319 + (78 * i),0, 78, 78 });

	playerData.currentAnimation = idleAnim;

	return true;
}

bool Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player Parser");
	bool ret = true;
	
	return true;
}


bool Player::LoadState(pugi::xml_node& player) 
{
	bool ret=true;
		playerData.position.x = player.child("position").attribute("x").as_int(playerData.position.x);
		playerData.position.y = player.child("position").attribute("y").as_int(playerData.position.y);
		playerData.respawns = player.child("lives").attribute("num_respawns").as_int(playerData.respawns);
		playerData.fuel = player.child("fuel").attribute("count").as_int(playerData.fuel);
	return ret;
}

bool Player::SaveState(pugi::xml_node& player) const
{
	pugi::xml_node positionPlayer = player.child("position");
	pugi::xml_node coinsPlayer = player.child("coins");
	pugi::xml_node respawnsPlayer = player.child("lives");
	
	positionPlayer.attribute("x").set_value(playerData.position.x);
	positionPlayer.attribute("y").set_value(playerData.position.y);
	coinsPlayer.attribute("count").set_value(playerData.fuel);
	respawnsPlayer.attribute("num_respawns").set_value(playerData.respawns);
	
	return true;
}

bool Player::PreUpdate() 
{
	return true;
}

bool Player::Update(float dt) 
{
	
	PlayerMoveAnimation();
	SpeedAnimationCheck(dt);

	if (!godMode)PlayerControls(dt);
	else GodModeControls(dt);
	CameraPlayer();


	if (playerData.state == ATTACK && playerData.currentAnimation->HasFinished())
	{
		playerData.state = IDLE;
		atakAnim->Reset();
	}
	
	return true;
}

void Player::SpeedAnimationCheck(float dt)
{
	idleAnim->speed = (dt * 5) ;
	flyAnim->speed = (dt * 9) ;
	atakAnim->speed = (dt * 5) ;
	damageAnim->speed = (dt * 10) ;
	turboAnim->speed = (dt * 9) ;
	
}

void Player::CameraPlayer()
{
	//// Camera follow the player
	//int followPositionPalyerX = (WINDOW_W / 2) + (playerData.position.x * -1);
	//int followPositionPalyerY = (WINDOW_H / 2) + (playerData.position.y * -1) + 125;

	//if (playerData.position.x < (WINDOW_W/2))
	//	if (app->render->camera.x < 48) followPositionPalyerX = 0;

	//// Camera delimitation x
	//if (app->render->camera.x <= (playerData.position.x * -1)&& app->render->camera.x >= -((app->map->data.width * app->map->data.tileWidth) - WINDOW_W))
	//	app->render->camera.x = followPositionPalyerX;
	//else if (followPositionPalyerX<-1&& followPositionPalyerX>-((app->map->data.width * app->map->data.tileWidth) - WINDOW_W+32))
	//	app->render->camera.x = followPositionPalyerX;

	//// Reposition right in limit camera in X
	//if ((playerData.position.x *-1)< -((app->map->data.width * app->map->data.tileWidth) - (WINDOW_W/2) + 32))
	//	app->render->camera.x = -((app->map->data.width * app->map->data.tileWidth) - WINDOW_W + 32);

	//if ((playerData.position.x *-1)> -(WINDOW_W/2) + 32)
	//	app->render->camera.x = 0;

	//// Camera delimitation y
	//if ((app->render->camera.y <= -48) && (app->render->camera.y >= -((app->map->data.height * app->map->data.tileHeight) - (WINDOW_H + (4 * app->map->data.tileHeight)))))
	//	app->render->camera.y = followPositionPalyerY;
	//else if ((followPositionPalyerY < -48) && (followPositionPalyerY > -((app->map->data.height * app->map->data.tileHeight) - (WINDOW_H + (4 * app->map->data.tileHeight)))))
	//	app->render->camera.y = followPositionPalyerY;
	//if (app->render->camera.y >= -39)
	//	app->render->camera.y = -39;
}

void Player::PlayerMoveAnimation()
{
	switch (playerData.state)
	{
	case IDLE:
		playerData.currentAnimation = idleAnim;
		break;

	case FLY:
		playerData.currentAnimation = flyAnim;
		break;

	case TURBO:
		playerData.currentAnimation = turboAnim;
		break;
	
	case HIT:
		playerData.currentAnimation = damageAnim;
		//if (playerData.currentAnimation->HasFinished())playerData.state = IDLE;
		break;	
	
	case ATTACK:
		playerData.currentAnimation = atakAnim;
		break;

	default:
		break;
	}
}

void Player::PlayerControls(float dt)
{
	// Player turbo
	if ((app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		&& (playerData.state == State::FLY || playerData.state == State::TURBO))
	{
		playerData.state = State::TURBO;
	}
	else if(app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		playerData.state = State::FLY;
	}
	else playerData.state = State::IDLE;

	
		// Comprobamos si las tecas est�n pulsadas al mismo tiempo
	if (!(app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		&& (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT))
	{
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT); // Add torque;

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT); // Add torque;
	}	

	MovePlayer(dt);
}

void Player::GodModeControls(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)velGodMode = 20;
	else velGodMode = 10;
		if (godMode == true)
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)playerData.position.y -= velGodMode;
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)playerData.position.y += velGodMode;
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)playerData.position.x -= velGodMode;
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)playerData.position.x += velGodMode;
	}
}

void Player::MovePlayer(float dt)
{
	switch (playerData.state)
	{
	case IDLE:
		// Future conditions in state IDLE...
		break;	

	case FLY:
		// Move in state WALK 
		// Add force ;
		// Future conditions in state FLY...
		break;

	case TURBO:
		// Move in state TURBO 
		// Add force ;
		// Future conditions in state TURBO...
		break;

	default:
		break;
	}
}


bool Player::PostUpdate() 
{
	SDL_Rect rectPlayer;
	rectPlayer = playerData.currentAnimation->GetCurrentFrame();
	// Draw player 
	app->render->DrawTexture(playerData.texture, playerData.position.x , playerData.position.y , &rectPlayer);

	return true;
}


bool Player::CleanUp() 
{
	if (!active)
		return true;

	app->audio->Unload1Fx(fireFx);
//	app->audio->Unload1Fx(damageFx);
	
	app->tex->UnLoad(playerData.texture);
	active = false;

	delete idleAnim;
	delete flyAnim;
	delete turboAnim;
	delete atakAnim;
	delete damageAnim;

	return true;
}

bool Player::CheckGameOver(int level)
{
	if (playerData.state==DEADING)
	{
		playerData.currentAnimation = deadAnim;
		return true;
	}
	if (playerData.currentAnimation == deadAnim && playerData.currentAnimation->HasFinished())
	{
		playerData.state = DEAD;
		playerData.position = *positionInitial;
	}
		
	return false;
}

void Player::SetHit()
{
	if (playerData.respawns > 0 && playerData.state != HIT && !godMode) 
	{
		playerData.respawns--;
		playerData.state = HIT;
		app->audio->PlayFx(damageFx);
	}
	
}
