#include "ReadirectsPlugin.h"
#include "bakkesmod/wrappers/GameObject/BallWrapper.h"
#include "bakkesmod/wrappers/GameObject/CarWrapper.h"
#include "utils/parser.h"

BAKKESMOD_PLUGIN(ReadirectsPlugin,
								 "Readirects Plugin",
								 plugin_version,
								 PLUGINTYPE_FREEPLAY)
bool readirectsEnabled = false;

void ReadirectsPlugin::onLoad() {
	// set options
	// redirect towards goal options
	/*cvarManager->registerCvar("readirects_ballspeed",
														"700",
														"How fast the ball will redirect when launched",
														true,
														true,
														0,
														true,
														6000,
														true);
	cvarManager->registerCvar(
		"readirects_goal_added_height",
		"(0, 1400)",
		"When redirected towards goal, the added height above",
		true,
		true,
		-600,
		true,
		1402,
		true);
	cvarManager->registerCvar("readirects_goal_side_offset",
														"(-2944, 2944)",
														"Added side distance above goal",
														true,
														true,
														-2944,
														true,
														2944,
														true);*/
	// cvarManager->registerCvar("readirects_goal_cycle_option");

	// clang-format off
  cvarManager->registerCvar("readirects_enabled", "0", "Enable ReadirectsPlugin", true, true, 0, true, 1)
    .addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
      readirectsEnabled = cvar.getBoolValue();
    });
  cvarManager->registerCvar("readirects_towards_goal",   "0", "Enable ball redirecting towards goal",    false, true, 0, true, 1);
  cvarManager->registerCvar("readirects_towards_wall",   "0", "Enable ball redirecting towards wall",    false, true, 0, true, 1);
  cvarManager->registerCvar("readirects_towards_corner", "0", "Enable ball redirecting towards corner",  false, true, 0, true, 1);
  cvarManager->registerCvar("readirects_towards_ceiling","0", "Enable ball redirecting towards ceiling", false, true, 0, true, 1);
  cvarManager->registerCvar("readirects_towards_car",    "0", "Enable ball redirecting towards car",     false, true, 0, true, 1);

  // towards goal settings
  cvarManager->registerCvar("readirects_goal_shotspeed_min",    "0", "Minimum speed of shot directed towards goal", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_goal_shotspeed_max",    "0", "Maximum speed of shot directed towards goal", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_goal_sideoffset_min",   "0", "Side offset min of shot directed towards goal", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_goal_sideoffset_max",   "0", "Side offset max of shot directed towards goal", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_goal_heightoffset_min", "0", "Height above goal min of shot directed towards goal", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_goal_heightoffset_max", "0", "Height above goal max of shot directed towards goal", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_goal_addedspin_min",    "0", "Added spin min of shot directed towards goal", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_goal_addedspin_max",    "0", "Added spin max of shot directed towards goal", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_goal_alternating",      "0", "Target alternating goals each call",     false, true, 0, true, 1);

  // towards wall settings
  cvarManager->registerCvar("readirects_wall_shotspeed_min",    "0", "Minimum speed of shot directed towards wall", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_wall_shotspeed_max",    "0", "Maximum speed of shot directed towards wall", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_wall_sideoffset_min",   "0", "Side offset min of shot directed towards wall", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_wall_sideoffset_max",   "0", "Side offset max of shot directed towards wall", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_wall_heightoffset_min", "0", "Height above wall min of shot directed towards wall", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_wall_heightoffset_max", "0", "Height above wall max of shot directed towards wall", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_wall_addedspin_min",    "0", "Added spin min of shot directed towards wall", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_wall_addedspin_max",    "0", "Added spin max of shot directed towards wall", false, true, 0, true, 50);

  // towards corner settings
  cvarManager->registerCvar("readirects_corner_shotspeed_min",    "0", "Minimum speed of shot directed towards corner", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_corner_shotspeed_max",    "0", "Maximum speed of shot directed towards corner", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_corner_sideoffset_min",   "0", "Side offset min of shot directed towards corner", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_corner_sideoffset_max",   "0", "Side offset max of shot directed towards corner", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_corner_heightoffset_min", "0", "Height above corner min of shot directed towards corner", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_corner_heightoffset_max", "0", "Height above corner max of shot directed towards corner", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_corner_addedspin_min",    "0", "Added spin min of shot directed towards corner", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_corner_addedspin_max",    "0", "Added spin max of shot directed towards corner", false, true, 0, true, 50);

  // towards ceiling settings
  cvarManager->registerCvar("readirects_ceiling_shotspeed_min",    "0", "Minimum speed of shot directed towards ceiling", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_ceiling_shotspeed_max",    "0", "Maximum speed of shot directed towards ceiling", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_ceiling_sideoffset_min",   "0", "Side offset min of shot directed towards ceiling", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_ceiling_sideoffset_max",   "0", "Side offset max of shot directed towards ceiling", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_ceiling_heightoffset_min", "0", "Height above ceiling min of shot directed towards ceiling", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_ceiling_heightoffset_max", "0", "Height above ceiling max of shot directed towards ceiling", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_ceiling_addedspin_min",    "0", "Added spin min of shot directed towards ceiling", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_ceiling_addedspin_max",    "0", "Added spin max of shot directed towards ceiling", false, true, 0, true, 50);

  // towards car settings
  cvarManager->registerCvar("readirects_car_shotspeed_min",    "0", "Minimum speed of shot directed towards car", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_car_shotspeed_max",    "0", "Maximum speed of shot directed towards car", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_car_sideoffset_min",   "0", "Side offset min of shot directed towards car", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_car_sideoffset_max",   "0", "Side offset max of shot directed towards car", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_car_heightoffset_min", "0", "Height above car min of shot directed towards car", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_car_heightoffset_max", "0", "Height above car max of shot directed towards car", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_car_addedspin_min",    "0", "Added spin min of shot directed towards car", false, true, 0, true, 50);
  cvarManager->registerCvar("readirects_car_addedspin_max",    "0", "Added spin max of shot directed towards car", false, true, 0, true, 50);

	// clang-format on
	// to keep a count of how many times the ball's hit by car
	gameWrapper->HookEventPost(
		"Function TAGame.Car_TA.EventHitBall",
		std::bind(&ReadirectsPlugin::onCarHitBall, this, std::placeholders::_1));

	// when the ball hits some part of the map
	gameWrapper->HookEventPost(
		"Function TAGame.Ball_TA.EventHitWorld",
		std::bind(&ReadirectsPlugin::onWorldHitBall, this, std::placeholders::_1));

	// if there's a timer, then hook into game ticks ---- THIS function IS FPS,
	// lol
	// gameWrapper->HookEventPost(
	//  "Function TAGame.GameObserver_TA.Tick",
	//  std::bind(&ReadirectsPlugin::onGameTick, this, std::placeholders::_1));

	// follows the physics engine ticking to emulate a "timer"
	gameWrapper->HookEventPost(
		"Function TAGame.Car_TA.SetVehicleInput",
		std::bind(&ReadirectsPlugin::onGameTick, this, std::placeholders::_1));
	_launchBallTimer = 119;
	_lastPoint			 = std::chrono::system_clock::now();
}

void ReadirectsPlugin::onUnload() {
	// cleanup
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.EventHitBall");
	gameWrapper->UnhookEvent("Function TAGame.Ball_TA.EventHitWorld");
	// gameWrapper->UnhookEvent("Function TAGame.GameObserver_TA.Tick");
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
}

void ReadirectsPlugin::launchBall() {
	// not in freeplay
	if (!gameWrapper->IsInFreeplay()) {
		return;
	}

	ServerWrapper sw = gameWrapper->GetCurrentGameState();
	// check for null
	if (!sw) {
		return;
	}

	// in case it's a workshop map
	if (sw.GetGoals().Count() < 2 || sw.GetCars().Count() == 0) {
		return;
	}

	CarWrapper player = sw.GetCars().Get(0);
	// check for null
	if (player.IsNull() || sw.GetBall().IsNull()) {
		return;
	}
}
void ReadirectsPlugin::onCarHitBall(std::string eventName) {
	// basically counting how many times the ball is hit by the player
}

void ReadirectsPlugin::onWorldHitBall(std::string eventName) {
	// basically counting how many times the ball
	// hits the map (ground, wall, ceiling)
}

void ReadirectsPlugin::onGameTick(std::string eventName) {
	if (!readirectsEnabled) {
		return;
	}
	// using this for a Timer function
	// assume game physics engine ticks at 120x per second
	// according to
	// https://discord.com/channels/862068148328857700/862081441080410143/934679289167761428
	// I assume so because I heard once that the physics engine ticks at 120/s ...
	//                                                                                            :\
  //so ((number of seconds) x 120)-- each tick;
	if (_launchBallTimer <= 0) {
		cvarManager->log("Timer ticked");
		cvarManager->log(
			"Calculated time in second: " +
			std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
											 std::chrono::system_clock::now() - _lastPoint)
											 .count()));
		_launchBallTimer = 119;
		_lastPoint			 = std::chrono::system_clock::now();
	} else {
		_launchBallTimer--;
	}
}

void ReadirectsPlugin::towardsPlayer() {
	// taken from the redirect plugin that comes with bakkesmod by default
	ServerWrapper training = gameWrapper->GetGameEventAsServer();

	if (training.GetGameCar().IsNull() || training.GetBall().IsNull())
		return;

	Vector playerPosition = training.GetGameCar().GetLocation();
	Vector ballPosition		= training.GetBall().GetLocation();
	Vector playerVelocity = training.GetGameCar().GetVelocity();

	float ballSpeed = cvarManager->getCvar("redirect_shot_speed").getIntValue();
	float offset_z = cvarManager->getCvar("redirect_pass_offset_z").getIntValue();

	bool predict = cvarManager->getCvar("redirect_pass_predict").getBoolValue();

	float predictMultiplierX =
		cvarManager->getCvar("redirect_predict_multiplier_x").getFloatValue();
	float predictMultiplierY =
		cvarManager->getCvar("redirect_predict_multiplier_y").getFloatValue();

	bool onGround = cvarManager->getCvar("redirect_on_ground").getBoolValue();

	int		 offsetX	 = cvarManager->getCvar("redirect_pass_offset").getIntValue();
	int		 offsetY	 = cvarManager->getCvar("redirect_pass_offset").getIntValue();
	int		 offsetZ	 = random(offset_z / 3, offset_z);
	Vector offsetVec = Vector(offsetX, offsetY, offsetZ);

	Vector velMultiplied;
	if (predict)
		velMultiplied =
			playerVelocity * Vector(predictMultiplierX, predictMultiplierY, 1);
	offsetVec = offsetVec + velMultiplied;
	Vector shotData =
		training.GenerateShot(ballPosition, playerPosition + offsetVec, ballSpeed);
	if (onGround)
		shotData.Z = 0;
	training.GetBall().SetVelocity(shotData);
}
