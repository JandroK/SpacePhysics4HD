#include "Player.h"
#include "Audio.h"
#include "Render.h"
#include "Input.h"
#include "App.h"
#include "Physics.h"
#include "SceneManager.h"
#include "Scene.h"

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
	active = true;
	// Create new ship
	ship = new Body;

	// Create new animations
	idleAnim = new Animation();
	flyAnim = new Animation();
	turboAnim = new Animation();
	turboVelocityAnim = new Animation();
	damageAnim = new Animation();
	deadAnim = new Animation();

	// Load textures and FX
	playerData.texture = app->tex->Load("Assets/Textures/space_ship.png");
	playerData.texLaserFly = app->tex->Load("Assets/Textures/laser_fly.png");
	playerData.texLaserTurbo = app->tex->Load("Assets/Textures/laser_turbo.png");
	playerData.texTurboVelocity = app->tex->Load("Assets/Textures/particle_velocity.png");
	playerData.texHitDead = app->tex->Load("Assets/Textures/space_ship_asset.png");
	turboBigFx = app->audio->LoadFx("Assets/Audio/Fx/turbo_big.wav");
	turboSmallFx = app->audio->LoadFx("Assets/Audio/Fx/turbo_small.wav");
	damageFx = app->audio->LoadFx("Assets/Audio/Fx/hit_metal_rock.wav");
	deadFx = app->audio->LoadFx("Assets/Audio/Fx/player_dead.wav");
	danger = app->audio->LoadFx("Assets/Audio/Fx/player_in_danger.wav");

	// Get dimensions of texture
	SDL_QueryTexture(playerData.texture,NULL ,NULL, &playerData.rectPlayer.w, &playerData.rectPlayer.h);

	float posX = (WINDOW_W / 2) - (playerData.rectPlayer.w / 2);
	float posY = 10282;

	// Set properties of the ship
	ship->SetMass(100);
	ship->SetLives(10);
	ship->SetClassType(BodyClass::PLAYER);
	ship->SetAxisCM({ posX + (playerData.rectPlayer.w >> 1), posY + (playerData.rectPlayer.h >> 1) });
	ship->SetDimension(PIXEL_TO_METERS(172), PIXEL_TO_METERS(148));
	ship->SetCoeficientDrag(0.82);
	ship->SetSurface(8);

	// Create the points matrix
	playerData.pointsCollision = new fPoint[playerData.numPoints];
	playerData.pointsCollisionWorld = new fPoint[playerData.numPoints];

	playerData.pointsCollision[0] =  { 0, -74 };
	playerData.pointsCollision[1] =  { 86, 42 };
	playerData.pointsCollision[2] =  { -86, 42 };
	
	for (int i = 0; i < playerData.numPoints; i++)
	{
		playerData.pointsCollisionWorld[i] = playerData.pointsCollision[i];
	}

	for (int i = 0; i < playerData.numPoints; i++)
	{
		playerData.pointsCollisionWorld[i].x += ship->GetAxis().x;
		playerData.pointsCollisionWorld[i].y += ship->GetAxis().y;
	}
	ship->SetCollisions(playerData.pointsCollision, playerData.pointsCollisionWorld, playerData.numPoints);

	// Set properties of the ship
	playerData.vecDir = { playerData.pointsCollisionWorld[0].x - ship->GetAxis().x , playerData.pointsCollisionWorld[0].y - ship->GetAxis().y };
	float radio = app->physics->CalculateModule(playerData.vecDir); 
	playerData.vecDir = app->physics->NormalizeVector(playerData.vecDir);
	ship->SetRadio(PIXEL_TO_METERS(radio));

	positionInitial = { PIXEL_TO_METERS(posX), PIXEL_TO_METERS(posY) };
	ship->SetPosition(positionInitial);
	ship->SetAxisCM({ PIXEL_TO_METERS(ship->GetAxis().x), PIXEL_TO_METERS(ship->GetAxis().y) });
	ship->SetSleep(true);

	// Add body to the engine
	app->physics->CreateBody(ship);
	
	playerData.state = IDLE;

	playerData.respawns = 3;
	playerData.fuel = 1500;
	playerData.lives = 3;

	// Load animations
	idleAnim->loop = true;
	flyAnim->loop = true;
	turboAnim->loop = true;
	turboVelocityAnim->loop = true;
	damageAnim->loop = false;
	deadAnim->loop = false;

	idleAnim->PushBack({ 0 ,0, playerData.rectPlayer.w, playerData.rectPlayer.h });
	
	for (int i = 0; i < 8; i++)
		flyAnim->PushBack({ 0 + (26 * i),0, 26, 52 });

	for (int i = 0; i < 8; i++)
		turboAnim->PushBack({ 0 + (26 * i),0, 26, 86 });

	for (int i = 0; i < 2; i++)
		turboVelocityAnim->PushBack({ 0 ,0 + (237 * i), 207, 237 });

	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 2; i++)
			damageAnim->PushBack({ 0, 0 + (168 * i) , 195, 168 });
	}
	
	for (int i = 0; i < 7; i++)
	{
		deadAnim->PushBack({ 0, 1344 + (-168 * i), 195, 168 });
	}
	
	playerData.currentAnimation = idleAnim;

	return true;
}

bool Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player Parser");
	bool ret = true;
	
	return true;
}

bool Player::PreUpdate() 
{
	// Ckeck if player is in danger
	if (playerData.fuel < 225 || ship->GetLives() < 2)
	{
		app->audio->PlayFx(-1,danger);
	}	
	return true;
}

bool Player::Update(float dt) 
{
	// Active GodMode, this mode allows to move without physics
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		godMode = !godMode;

		if(ship->GetType() == BodyType::DYNAMIC_BODY)ship->SetBodyType(BodyType::STATIC_BODY);
		else ship->SetBodyType(BodyType::DYNAMIC_BODY);
		ship->SetVelocity({ 0,0 });
	}

	// Update animations
	playerData.currentAnimation->Update();
	PlayerMoveAnimation();
	SpeedAnimationCheck(dt);

	// If player is alive
	if (playerData.state != State::DEADING && playerData.state != State::DEAD && playerData.state != State::HIT)
	{
		if (!godMode) 
		{
			if (playerData.fuel >= 0)
			{
				PlayerControls(dt);
			}
			else
			{
				playerData.fuel = 0;
				playerData.state = State::IDLE;
			}
		}
		else GodModeControls(dt);

	}

	CameraPlayer();

	// Comprobe Win/Lose
	CheckWin();
	CheckGameOver();

	return true;
}

bool Player::PostUpdate()
{
	PlayerMoveAnimation();

	// Get positions spaceship
	fPoint positionPlayer = { METERS_TO_PIXELS(ship->GetAxis().x) - playerData.rectPlayer.w/2, 
							  METERS_TO_PIXELS(ship->GetAxis().y) - playerData.rectPlayer.h / 2 };

	SDL_Rect rectPlayer;
	rectPlayer = playerData.currentAnimation->GetCurrentFrame();

	fPoint posPropulsor;
	posPropulsor.x = METERS_TO_PIXELS(ship->GetAxis().x) - rectPlayer.w/2;
	posPropulsor.y = METERS_TO_PIXELS(ship->GetAxis().y) + playerData.rectPlayer.h / 2;

	float posX = positionPlayer.x + playerData.rectPlayer.w / 2 - turboVelocityAnim->GetCurrentFrame().w / 2;
	float posY = positionPlayer.y - 17;
	float angle = ship->GetRotation();

	// Draw player 
	if (playerData.state != HIT && playerData.state != DEADING && playerData.state != DEAD)
	{
		app->render->DrawTexture(playerData.texture, positionPlayer.x, positionPlayer.y,
			&playerData.rectPlayer, 1, ship->GetRotation());
	}

	switch (playerData.state)
	{
	case IDLE:
		break;

	case FLY:
		app->render->DrawTexture(playerData.texLaserFly, posPropulsor.x, posPropulsor.y - 13, &rectPlayer, 1, angle, rectPlayer.w / 2, -rectPlayer.h - 8);
		app->render->DrawTexture(playerData.texLaserFly, posPropulsor.x, posPropulsor.y - 30, &rectPlayer, 1, angle, rectPlayer.w / 2, -rectPlayer.h + 9);
		app->render->DrawTexture(playerData.texLaserFly, posPropulsor.x - 23, posPropulsor.y - 21, &rectPlayer, 1, angle, rectPlayer.w / 2 + 23, -rectPlayer.h);
		app->render->DrawTexture(playerData.texLaserFly, posPropulsor.x + 23, posPropulsor.y - 21, &rectPlayer, 1, angle, rectPlayer.w / 2 - 23, -rectPlayer.h);
		app->audio->PlayFx(-1,turboSmallFx, 100);
		break;

	case TURBO:
		app->render->DrawTexture(playerData.texTurboVelocity, posX, posY, &turboVelocityAnim->GetCurrentFrame(), 1, angle);
		app->render->DrawTexture(playerData.texLaserTurbo, posPropulsor.x, posPropulsor.y - 13, &rectPlayer, 1, angle, rectPlayer.w / 2, -rectPlayer.h - 8 +34);
		app->render->DrawTexture(playerData.texLaserTurbo, posPropulsor.x, posPropulsor.y - 30, &rectPlayer, 1, angle, rectPlayer.w / 2, -rectPlayer.h + 9 + 34);
		app->render->DrawTexture(playerData.texLaserTurbo, posPropulsor.x - 23, posPropulsor.y - 21, &rectPlayer, 1, angle, rectPlayer.w / 2 + 23, -rectPlayer.h + 34);
		app->render->DrawTexture(playerData.texLaserTurbo, posPropulsor.x + 23, posPropulsor.y - 21, &rectPlayer, 1, angle, rectPlayer.w / 2 - 23, -rectPlayer.h + 34);
		app->audio->PlayFx(-1,turboBigFx, 60);
		break;

	case HIT:
		app->render->DrawTexture(playerData.texHitDead, positionPlayer.x - 12, positionPlayer.y - 10, &rectPlayer, 1, ship->GetRotation());
		if (playerData.currentAnimation->HasFinished())
		{
			playerData.state = State::IDLE; 
			playerData.currentAnimation->Reset();
		}
		app->audio->PlayFx(-1,damageFx);
		break;

	case DEADING:
		app->render->DrawTexture(playerData.texHitDead, positionPlayer.x, positionPlayer.y, &rectPlayer, 1, ship->GetRotation());
		app->audio->PlayFx(-1,deadFx);
		ship->SetBodyType(BodyType::STATIC_BODY);
		ship->SetAcceleration({ 0, 0 });
		ship->SetAccelerationAngular(0);
		ship->SetSleep(true);
		if (playerData.currentAnimation->HasFinished())
		{
			playerData.state = DEAD;
			ship->SetBodyState(BodyState::DEAD);	
		}
		break;
	default:
		break;
	}

	// Draw collisions
	if (app->sceneManager->GetViewCollisions())
	{
		int x1 = ship->GetPointsCollisionWorld()[0].x;
		int y1 = ship->GetPointsCollisionWorld()[0].y;
		int x2 = ship->GetPointsCollisionWorld()[1].x;
		int y2 = ship->GetPointsCollisionWorld()[1].y;
		int x3 = ship->GetPointsCollisionWorld()[2].x;
		int y3 = ship->GetPointsCollisionWorld()[2].y;

		app->render->DrawLine(x1, y1, x2, y2, 255, 0, 0);
		app->render->DrawLine(x2, y2, x3, y3, 255, 0, 0);
		app->render->DrawLine(x3, y3, x1, y1, 255, 0, 0);

		int x4 = METERS_TO_PIXELS(ship->GetAxis().x);
		int y4 = METERS_TO_PIXELS(ship->GetAxis().y);

		app->render->DrawLine(x4, y4, x1, y1, 255, 0, 0);
	}

	return true;
}

void Player::SpeedAnimationCheck(float dt)
{
	idleAnim->speed = (dt * 5) ;
	flyAnim->speed = (dt * 9) ;
	turboAnim->speed = (dt * 9) ;
	turboVelocityAnim->speed = (dt * 15) ;
	damageAnim->speed = (dt * 12) ;
	deadAnim->speed = (dt * 8) ;
}

void Player::CameraPlayer()
{
	app->render->camera.y = -(METERS_TO_PIXELS(ship->GetAxis().y) - WINDOW_H / 2);
	if (app->render->camera.y > 0) app->render->camera.y = 0;
	if (app->render->camera.y < -11081+ WINDOW_H) app->render->camera.y = -11081 + WINDOW_H;
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
		turboVelocityAnim->Update();
		break;
	
	case HIT:
		playerData.currentAnimation = damageAnim;
		break;	
	
	case DEADING:
		playerData.currentAnimation = deadAnim;
		break;

	default:
		break;
	}

	switch (ship->GetBodyState())
	{
	case BodyState::IDLE:
		break;
	case BodyState::HIT:
		playerData.state = State::HIT;
		ship->SetBodyState(BodyState::IDLE);
		break;
	case BodyState::DEADING:
		playerData.state = State::DEADING;
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
		// The direction of the force is equal to normalized director vector
		playerData.vecDir = { ship->GetPointsCollisionWorld()[0].x - METERS_TO_PIXELS(ship->GetAxis().x), ship->GetPointsCollisionWorld()[0].y - METERS_TO_PIXELS(ship->GetAxis().y) };
		playerData.vecDir = app->physics->NormalizeVector(playerData.vecDir);
		playerData.state = State::TURBO;

		// Add forces
		if(playerData.fuel>1)ship->AddForces({ playerData.vecDir.x * 4000, playerData.vecDir.y * 4000 });
		else ship->AddForces({ playerData.vecDir.x * 1000, playerData.vecDir.y * 1000 });

		// Fuel--
		playerData.fuel -= 1.5;
	}
	else if(app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		playerData.vecDir = { ship->GetPointsCollisionWorld()[0].x - METERS_TO_PIXELS(ship->GetAxis().x), ship->GetPointsCollisionWorld()[0].y - METERS_TO_PIXELS(ship->GetAxis().y) };
		playerData.vecDir = app->physics->NormalizeVector(playerData.vecDir);
		playerData.state = State::FLY;

		// Add forces
		if (playerData.fuel > 1)ship->AddForces({ playerData.vecDir.x * 2000, playerData.vecDir.y * 2000 });
		else ship->AddForces({ playerData.vecDir.x * 500, playerData.vecDir.y * 500 });

		// Fuel--
		playerData.fuel -= 1;
	}
	else playerData.state = State::IDLE;

	// Add torque for rotate the ship
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		ship->AddTorque(400);
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		ship->AddTorque(-400);
	}

	// If the player donesn't apply torque and there is wind, the velocity angular is reduced
	if (ship->GetGravity() != 0 && !app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN 
		&& !app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN )
	{
		// Add Torque in wrong way 
		if(ship->GetVelocityAngular() < 0)
			ship->AddTorque(200);
		if(ship->GetVelocityAngular() > 0)
			ship->AddTorque(-200);		
	}

	// Add Torque for stabilize the ship, this system help player to land
	if (app->input->GetKey(SDL_SCANCODE_S) != KEY_DOWN )
	{
		int gravityDirection = 0;
		if (ship->GetGravity() > 0)gravityDirection = 1;
		else if (ship->GetGravity() < 0)gravityDirection = -1;
		
		if (ship->GetPointsCollisionWorld()[2].y - ship->GetPointsCollisionWorld()[1].y > 0.1f)
			ship->AddTorque(300 * gravityDirection);
		else if (ship->GetPointsCollisionWorld()[2].y - ship->GetPointsCollisionWorld()[1].y < -0.1f)
			ship->AddTorque(-300 * gravityDirection);
	}
}

void Player::GodModeControls(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)velGodMode = PIXEL_TO_METERS(20);
	else velGodMode = PIXEL_TO_METERS(10);

	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		ship->SetAxisCM({ ship->GetAxis().x, ship->GetAxis().y - velGodMode });
		ship->SetPosition({ ship->GetPosition().x, ship->GetPosition().y - velGodMode });
	}	
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		ship->SetAxisCM({ ship->GetAxis().x, ship->GetAxis().y + velGodMode });
		ship->SetPosition({ ship->GetPosition().x, ship->GetPosition().y + velGodMode });
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		ship->SetAxisCM({ ship->GetAxis().x - velGodMode, ship->GetAxis().y });
		ship->SetPosition({ ship->GetPosition().x - velGodMode, ship->GetPosition().y });
	}
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		ship->SetAxisCM({ ship->GetAxis().x + velGodMode, ship->GetAxis().y });
		ship->SetPosition({ ship->GetPosition().x + velGodMode, ship->GetPosition().y });
	}	
}

void Player::CheckGameOver()
{
	if (playerData.state==DEAD || playerData.fuel <= 0 )
	{
		app->sceneManager->scene->SetLose(true);
	}
}
void Player::CheckWin()
{
	// If player land on the platform moon and after in Earth player win
	if (abs(ship->GetPointsCollisionWorld()[2].y - ship->GetPointsCollisionWorld()[1].y) < 0.3)
	{
		if (METERS_TO_PIXELS(ship->GetPosition().y) > 560 && METERS_TO_PIXELS(ship->GetPosition().y) < 580
			&& app->sceneManager->scene->GetWin() != 1)
		{
			app->sceneManager->scene->SetWin(1);
		}
			
		if (METERS_TO_PIXELS(ship->GetPosition().y) < 10300 && METERS_TO_PIXELS(ship->GetPosition().y) > 10200
			&& app->sceneManager->scene->GetWin() != 0 && app->sceneManager->scene->GetWin() != 2)
		{
			app->sceneManager->scene->SetWin(2);
		}	
	}
}

bool Player::CleanUp()
{
	if (!active)
		return true;

	//app->audio->Unload1Fx(fireFx);
	//	app->audio->Unload1Fx(damageFx);

	app->tex->UnLoad(playerData.texture);
	app->tex->UnLoad(playerData.texLaserFly);
	app->tex->UnLoad(playerData.texLaserTurbo);
	app->tex->UnLoad(playerData.texTurboVelocity);
	app->audio->Unload1Fx(turboBigFx);
	app->audio->Unload1Fx(turboSmallFx);
	app->audio->Unload1Fx(damageFx);
	app->audio->Unload1Fx(deadFx);
	app->audio->Unload1Fx(danger);

	delete idleAnim;
	delete flyAnim;
	delete turboAnim;
	delete turboVelocityAnim;
	delete damageAnim;
	delete playerData.pointsCollision;
	delete playerData.pointsCollisionWorld;

	active = false;

	return true;
}

