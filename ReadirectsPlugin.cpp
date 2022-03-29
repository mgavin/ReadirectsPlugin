#include "ReadirectsPlugin.h"

BAKKESMOD_PLUGIN(ReadirectsPlugin,
								 "Readirects Plugin",
								 plugin_version,
								 PLUGINTYPE_FREEPLAY)

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
	// using this for a Timer function
	// assume game physics engine ticks at 120x per second
	// according to
	// https://discord.com/channels/862068148328857700/862081441080410143/934679289167761428
	// I assume so because I heard once that the physics engine ticks at 120/s ...
	//                                                  :\
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
