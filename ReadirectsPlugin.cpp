#include "ReadirectsPlugin.h"
#include "bakkesmod/wrappers/GameEvent/TutorialWrapper.h"
#include "bakkesmod/wrappers/GameObject/BallWrapper.h"
#include "bakkesmod/wrappers/GameObject/CarWrapper.h"
#include "utils/parser.h"

BAKKESMOD_PLUGIN(ReadirectsPlugin, "Readirects Plugin", plugin_version, PLUGINTYPE_FREEPLAY)
bool readirectsEnabled = false;
bool gameHooked				 = false;

void ReadirectsPlugin::onLoad() {
	// Attach to an amount change to fill/drain the playlist
	auto addOnValueChangedPlaylistFill = [this](
																				 std::string cvarName, std::string cvarEnabler, std::string playlistValue) {
		CVarWrapper addvccvar = cvarManager->getCvar(cvarName);
		if (addvccvar) {
			addvccvar.addOnValueChanged([this, cvarEnabler, playlistValue](std::string oldValue, CVarWrapper cvar) {
				c_playlist.clear();
				CVarWrapper cvarEnable = cvarManager->getCvar(cvarEnabler);
				if (!cvarEnable)
					return;
				if (!cvarEnable.getBoolValue())
					return;

				int c			 = std::count(begin(playlist), end(playlist), playlistValue);
				int newVal = cvar.getIntValue();
				if (newVal < c) {
					auto it = std::find(rbegin(playlist), rend(playlist), playlistValue);
					// items exist
					for (int i = c - newVal; i > 0 && it != rend(playlist); --i) {
						if (it == rbegin(playlist)) {
							playlist.erase((it + 1).base());
						} else {
							playlist.erase(it.base());
						}
						it = std::find(rbegin(playlist), rend(playlist), playlistValue);
					}
				} else if (newVal > c) {
					for (int i = newVal - c; i > 0; --i) {
						playlist.push_back(playlistValue);
					}
				}
			});
		}
	};
	// Attach to an enabler to refill the playlist when enabled or clear it when disabled
	auto addOnValueChangedClearOrRefillPlaylist =
		[this](std::string cvarEnabler, std::string cvarAmount, std::string playlistValue) {
			CVarWrapper addvccvar = cvarManager->getCvar(cvarEnabler);
			if (addvccvar) {
				addvccvar.addOnValueChanged([this, cvarAmount, playlistValue](std::string oldValue, CVarWrapper cvar) {
					c_playlist.clear();
					bool				isEnabled = cvar.getBoolValue();
					CVarWrapper amount_cv = cvarManager->getCvar(cvarAmount);
					if (!amount_cv)
						return;
					if (isEnabled) {
						// refill the playlist with amount set in the cvar
						amount_cv.notify();
					} else {
						for (auto it = std::find(begin(playlist), end(playlist), playlistValue); it != end(playlist);
								 it			 = std::find(begin(playlist), end(playlist), playlistValue)) {
							playlist.erase(it);
						}
					}
				});
			}
		};

	// option to enable plugin
	cvarManager->registerCvar("readirects_enabled", "0", "Enable ReadirectsPlugin", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
			readirectsEnabled = cvar.getBoolValue();
			if (readirectsEnabled && !gameHooked) {
				HookGameEngine();
			} else {
				UnhookGameEngine();
			}
		});

	// options for where to redirect the ball
	cvarManager->registerCvar(
		"readirects_towards_goal", "0", "Enable ball redirecting towards goal", false, true, 0, true, 1);
	addOnValueChangedClearOrRefillPlaylist("readirects_towards_goal", "readirects_towards_goal_amount", "Towards Goal");
	cvarManager->registerCvar(
		"readirects_towards_wall", "0", "Enable ball redirecting towards wall", false, true, 0, true, 1);
	addOnValueChangedClearOrRefillPlaylist("readirects_towards_wall", "readirects_towards_wall_amount", "Towards Wall");
	cvarManager->registerCvar(
		"readirects_towards_corner", "0", "Enable ball redirecting towards corner", false, true, 0, true, 1);
	addOnValueChangedClearOrRefillPlaylist(
		"readirects_towards_corner", "readirects_towards_corner_amount", "Towards Corner");
	cvarManager->registerCvar(
		"readirects_towards_ceiling", "0", "Enable ball redirecting towards ceiling", false, true, 0, true, 1);
	addOnValueChangedClearOrRefillPlaylist(
		"readirects_towards_ceiling", "readirects_towards_ceiling_amount", "Towards Ceiling");
	cvarManager->registerCvar(
		"readirects_towards_car", "0", "Enable ball redirecting towards car", false, true, 0, true, 1);
	addOnValueChangedClearOrRefillPlaylist("readirects_towards_car", "readirects_towards_car_amount", "Towards Car");

	// towards goal settings
	cvarManager->registerCvar(
		"readirects_goal_shotspeed", "(0, 6000)", "Speed of shot directed towards goal", false, true, 0, true, 6000);
	cvarManager->registerCvar("readirects_goal_sideoffset",
														"(-2944, 2944)",
														"Side offset of shot directed towards goal",
														false,
														true,
														-2944,
														true,
														2944);
	cvarManager->registerCvar("readirects_goal_heightoffset",
														"(0, 2044)",
														"Height above goal of shot directed towards goal",
														false,
														true,
														0,
														true,
														2044);
	cvarManager->registerCvar(
		"readirects_goal_addedspin", "(-6, 6)", "Added spin of shot directed towards goal", false, true, -6, true, 6);
	cvarManager->registerCvar("readirects_towards_goal_amount",
														"1",
														"Amount of times ball is directed towards goal in the playlist",
														false,
														true,
														1,
														true,
														10);
	addOnValueChangedPlaylistFill("readirects_towards_goal_amount", "readirects_towards_goal", "Towards Goal");
	cvarManager->registerCvar(
		"readirects_goal_alternating", "0", "Target alternating goals each call", false, true, 0, true, 1);

	// towards wall settings
	cvarManager->registerCvar(
		"readirects_wall_shotspeed", "(0, 6000)", "Speed of shot directed towards wall", false, true, 0, true, 6000);
	cvarManager->registerCvar("readirects_wall_sideoffset",
														"(-3968, 3968)",
														"Side offset of shot directed towards wall",
														false,
														true,
														-2944,
														true,
														2944);
	cvarManager->registerCvar("readirects_wall_heightoffset",
														"(0, 2044)",
														"Height above wall of shot directed towards wall",
														false,
														true,
														0,
														true,
														2044);
	cvarManager->registerCvar(
		"readirects_wall_addedspin", "(-6, 6)", "Added spin of shot directed towards wall", false, true, -6, true, 6);
	cvarManager->registerCvar("readirects_towards_wall_amount",
														"1",
														"Amount of times ball is directed towards wall in the playlist",
														false,
														true,
														1,
														true,
														10);
	addOnValueChangedPlaylistFill("readirects_towards_wall_amount", "readirects_towards_wall", "Towards Wall");
	cvarManager->registerCvar(
		"readirects_wall_alternating", "0", "Target alternating walls each call", false, true, 0, true, 1);

	// towards corner settings
	cvarManager->registerCvar(
		"readirects_corner_shotspeed", "(0, 6000)", "Speed of shot directed towards corner", false, true, 0, true, 6000);
	cvarManager->registerCvar("readirects_corner_sideoffset",
														"(-814, 814)",
														"Side offset of shot directed towards corner",
														false,
														true,
														-2944,
														true,
														2944);
	cvarManager->registerCvar("readirects_corner_heightoffset",
														"(0, 2044)",
														"Height above corner of shot directed towards corner",
														false,
														true,
														0,
														true,
														2044);
	cvarManager->registerCvar(
		"readirects_corner_addedspin", "(-6, 6)", "Added spin of shot directed towards corner", false, true, -6, true, 6);
	cvarManager->registerCvar("readirects_towards_corner_amount",
														"1",
														"Amount of times ball is directed towards ceiling in the playlist",
														false,
														true,
														1,
														true,
														10);
	addOnValueChangedPlaylistFill("readirects_towards_corner_amount", "readirects_towards_corner", "Towards Corner");

	// towards ceiling settings
	cvarManager->registerCvar(
		"readirects_ceiling_shotspeed", "(0, 6000)", "Speed of shot directed towards ceiling", false, true, 0, true, 6000);
	cvarManager->registerCvar("readirects_ceiling_sideoffset",
														"(-2944, 2944)",
														"Side offset of shot directed towards ceiling",
														false,
														true,
														-2944,
														true,
														2944);
	cvarManager->registerCvar("readirects_ceiling_heightoffset",
														"(0, 2044)",
														"Height above ceiling of shot directed towards ceiling",
														false,
														true,
														0,
														true,
														2044);
	cvarManager->registerCvar(
		"readirects_ceiling_addedspin", "(-6, 6)", "Added spin of shot directed towards ceiling", false, true, -6, true, 6);
	cvarManager->registerCvar("readirects_towards_ceiling_amount",
														"1",
														"Amount of times ball is directed towards ceiling in the playlist",
														false,
														true,
														1,
														true,
														10);
	addOnValueChangedPlaylistFill("readirects_towards_ceiling_amount", "readirects_towards_ceiling", "Towards Ceiling");

	// towards car settings
	cvarManager->registerCvar(
		"readirects_car_pass_predict", "1", "Predict car position when redirecting ball", false, true, 0, true, 1);
	cvarManager->registerCvar(
		"readirects_car_pass_on_ground", "0", "Redirect the ball to car on the ground", false, true, 0, true, 1);
	cvarManager->registerCvar("readirects_car_predict_multiplier_x",
														"(2.0, 2.0)",
														"X offset to use when passing (playerVelocityX * value)",
														false,
														true,
														0.0f,
														true,
														5.0f);
	cvarManager->registerCvar("readirects_car_predict_multiplier_y",
														"(2.0, 2.0)",
														"Y offset to use when passing (playerVelocityY * value)",
														false,
														true,
														0.0f,
														true,
														5.0f);
	cvarManager->registerCvar(
		"readirects_car_shotspeed", "(850, 1000)", "Speed of shot directed towards car", false, true, 0, true, 1600);
	cvarManager->registerCvar(
		"readirects_car_boundoffset", "(0, 0)", "Side offset of shot directed towards car", false, true, -50, true, 50);
	cvarManager->registerCvar("readirects_car_heightoffset",
														"(0, 200)",
														"Height above car of shot directed towards car",
														false,
														true,
														0,
														true,
														2044);
	cvarManager->registerCvar(
		"readirects_car_addedspin", "(-6, 6)", "Added spin of shot directed towards car", false, true, -6, true, 6);
	cvarManager->registerCvar("readirects_towards_car_amount",
														"1",
														"Amount of times ball is directed towards car in the playlist",
														false,
														true,
														1,
														true,
														10);
	addOnValueChangedPlaylistFill("readirects_towards_car_amount", "readirects_towards_car", "Towards Car");

	cvarManager->registerCvar("readirects_enable_timer", "0", "Run playlist on a timer", false, true, 0, true, 1);

	cvarManager->registerCvar(
		"readirects_enable_afternumballhits", "0", "Run playlist after number of ball hits", false, true, 0, true, 1);
	cvarManager->registerCvar(
		"readirects_enable_afterballhitsground", "0", "Run playlist when ball hits ground", false, true, 0, true, 1);
	cvarManager->registerCvar(
		"readirects_timer_seconds", "1", "Seconds between redirecting the ball", false, true, 1, true, 60);
	cvarManager->registerCvar("readirects_numballhitscar",
														"0",
														"Number of ball hits by car before redirecting the ball",
														false,
														true,
														1,
														true,
														10);
	cvarManager->registerCvar("readirects_numballhitsground",
														"0",
														"Number of ball hits by wall before redirecting the ball",
														false,
														true,
														1,
														true,
														25);
	/* JUST CONCEPTUALLY ENABLING/DISABLING OTHER CHECKBOXES
	CVarWrapper ontimer = cvarManager->getCvar("readirects_enable_timer");
	CVarWrapper afternumballhits =
		cvarManager->getCvar("readirects_enable_afternumballhits");
	CVarWrapper afterballhitsground =
		cvarManager->getCvar("readirects_enable_afterballhitsground");
	ontimer.addOnValueChanged([&, this](std::string oldValue, CVarWrapper cvar) {
		CVarWrapper anbh =
			cvarManager->getCvar("readirects_enable_afternumballhits");
		if (!anbh)
			return;
		CVarWrapper abhg =
			cvarManager->getCvar("readirects_enable_afterballhitsground");
		if (!anbh)
			return;
		anbh.setValue(0);
		abhg.setValue(0);
	});
	*/
	cvarManager->registerCvar(
		"readirects_enable_randomizeplaylist", "0", "Randomly execute playlist?", false, true, 0, true, 1);

	cvarManager->registerCvar("readirects_shoot_key", "", "Key set to execute readirects next shot in playlist");
	cvarManager->registerNotifier("readirects_shoot",
																bind(&ReadirectsPlugin::LaunchBall, this, std::placeholders::_1),
																"Executes next type of redirect in the playlist",
																PERMISSION_FREEPLAY | PERMISSION_PAUSEMENU_CLOSED);

	gameWrapper->HookEvent("Function TAGame.Mutator_Freeplay_TA.Init", [this](std::string eventName) {
		if (readirectsEnabled && !gameHooked) {
			HookGameEngine();
		}
	});
	gameWrapper->HookEvent("Function TAGame.FreeplayCommands_TA.HandleSoccarGameDestroyed",
												 [this](std::string eventName) {
													 if (gameHooked) {
														 UnhookGameEngine();
													 }
												 });
	settings_ids.resize(5);
}

void ReadirectsPlugin::onUnload() {
	settings_storage = 0;
	settings_ids.clear();
	playlist.clear();
	UnhookGameEngine();
}

void ReadirectsPlugin::HookGameEngine() {
	// to keep a count of how many times the ball's hit by car
	gameWrapper->HookEventPost("Function TAGame.Car_TA.EventHitBall",
														 std::bind(&ReadirectsPlugin::OnCarHitsBall, this, std::placeholders::_1));

	// when the ball hits some part of the map
	gameWrapper->HookEventPost("Function TAGame.Ball_TA.EventHitWorld",
														 std::bind(&ReadirectsPlugin::OnBallHitsWorld, this, std::placeholders::_1));

	// when the ball hits the ground
	gameWrapper->HookEventPost("Function TAGame.Ball_TA.EventHitGround",
														 std::bind(&ReadirectsPlugin::OnBallHitsGround, this, std::placeholders::_1));

	// follows the physics engine ticking to emulate a "timer"
	gameWrapper->HookEventPost("Function TAGame.Car_TA.SetVehicleInput",
														 std::bind(&ReadirectsPlugin::OnGameTick, this, std::placeholders::_1));

	// a check on if the game is hooked
	_launchBallTimer = 119;
	_lastPoint			 = std::chrono::system_clock::now();
	cvarManager->log("engine hooked");
	gameHooked = true;
}

void ReadirectsPlugin::UnhookGameEngine() {
	// cleanup
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.EventHitBall");
	gameWrapper->UnhookEvent("Function TAGame.Ball_TA.EventHitWorld");
	gameWrapper->UnhookEvent("Function TAGame.Ball_TA.EventHitGround");
	// gameWrapper->UnhookEvent("Function TAGame.GameObserver_TA.Tick");
	gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
	cvarManager->log("engine unhooked");
	gameHooked = false;
}

std::string ReadirectsPlugin::next_in_playlist() {
	if (playlist.empty())
		return "";
	if (c_playlist.empty())
		std::copy(begin(playlist), end(playlist), std::back_inserter(c_playlist));

	CVarWrapper random_playlist = cvarManager->getCvar("readirects_enable_randomizeplaylist");
	std::string ret;
	if (!random_playlist.getBoolValue()) {
		ret = c_playlist.front();
		c_playlist.pop_front();
	} else {
		std::size_t r	 = random(0, c_playlist.size());
		auto				it = begin(c_playlist) + r;
		ret						 = *it;
		c_playlist.erase(it);
	}

	return ret;
}

void ReadirectsPlugin::LaunchBall(std::vector<std::string> params) {
	//  main driver
	if (!readirectsEnabled && !gameWrapper->IsInFreeplay())
		return;

	ServerWrapper sw = gameWrapper->GetCurrentGameState();
	if (!sw)
		return;

	// in case it's a workshop map
	if (sw.GetGoals().Count() < 2 || sw.GetCars().Count() == 0)
		return;

	CarWrapper player = sw.GetCars().Get(0);
	if (player.IsNull() || sw.GetBall().IsNull())
		return;

	std::string next = next_in_playlist();
	TowardsCar(std::make_shared<ServerWrapper>(sw), std::make_shared<CarWrapper>(player));
}
void ReadirectsPlugin::OnCarHitsBall(std::string eventName) {
	// basically counting how many times the ball is hit by the player
	cvarManager->log("player hit the ball");
}

void ReadirectsPlugin::OnBallHitsWorld(std::string eventName) {
	// basically counting how many times the ball
	// hits the map (wall, ceiling)
	ServerWrapper sw = gameWrapper->GetCurrentGameState();
	if (!sw)
		return;
	BallWrapper bw = sw.GetBall();
	if (!bw)
		return;
	Vector balloc = bw.GetLocation();
	if (balloc.Z > 100) {
		// far enough up wall;
	}
}

void ReadirectsPlugin::OnBallHitsGround(std::string eventName) {
	// keeping a count of how many times the ball hits the ground
}

void ReadirectsPlugin::OnGameTick(std::string eventName) {
	// using this for a Timer function
	// assume game physics engine ticks at 120x per second
	// according to
	// https://discord.com/channels/862068148328857700/862081441080410143/934679289167761428
	// I assume so because I heard once that the physics engine ticks at 120/s ...
	//  :\
  // so ((number of seconds) x 120) == each tick;
	if (_launchBallTimer <= 0) {
		cvarManager->log("Timer ticked");
		cvarManager->log(
			"Calculated time in second: " +
			std::to_string(
				std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _lastPoint).count()));
		_launchBallTimer = 119;
		_lastPoint			 = std::chrono::system_clock::now();

		ServerWrapper sw = gameWrapper->GetCurrentGameState();
		if (!sw)
			return;
		BallWrapper bw = sw.GetBall();
		if (!bw)
			return;
		Vector balloc = bw.GetLocation();
		cvarManager->log("Ball location (X:" + std::to_string(balloc.X) + ",Y:" + std::to_string(balloc.Y) +
										 ",Z:" + std::to_string(balloc.Z) + ")");
	} else {
		_launchBallTimer--;
	}
}

void ReadirectsPlugin::TowardsGoal(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player) {
	Vector ballLoc = sw->GetBall().GetLocation();

	// Calculate nearest goal
	Vector playerLocLater = player->GetLocation() + (player->GetVelocity() * 200);	// Calculate where player is facing
	if (abs(player->GetVelocity().X) < 1 && abs(player->GetVelocity().X) < 1) {
		playerLocLater = ballLoc;	 // if player is not moving, set rebound to the goal the ball is closest to
	}
	Vector goal1Diff	= sw->GetGoals().Get(0).GetLocation() - playerLocLater;
	Vector goal2Diff	= sw->GetGoals().Get(1).GetLocation() - playerLocLater;
	float	 goal1DiffF = abs(goal1Diff.X) + abs(goal1Diff.Y) + abs(goal1Diff.Z);
	float	 goal2DiffF = abs(goal2Diff.X) + abs(goal2Diff.Y) + abs(goal2Diff.Z);

	int target = 0;
	if (goal1DiffF > goal2DiffF) {
		target = 1;
	}

	float sideOffset = cvarManager->getCvar("rebound_side_offset").getFloatValue();

	Vector goalTarget = sw->GenerateGoalAimLocation(target, ballLoc);

	goalTarget.Z = sw->GetGoalExtent(target).Z + cvarManager->getCvar("rebound_addedheight").getFloatValue();
	goalTarget.X += sideOffset;

	float reboundSpeed = cvarManager->getCvar("rebound_shotspeed").getFloatValue();

	Vector shot = sw->GenerateShot(ballLoc, goalTarget, reboundSpeed);
	if (cvarManager->getCvar("rebound_resetspin").getBoolValue()) {
		sw->GetBall().Stop();
	}
	Vector addedSpin = {cvarManager->getCvar("rebound_addedspin").getFloatValue(),
											cvarManager->getCvar("rebound_addedspin").getFloatValue(),
											cvarManager->getCvar("rebound_addedspin").getFloatValue()};

	sw->GetBall().SetVelocity(shot);
	sw->GetBall().SetAngularVelocity(addedSpin, true);
}
void ReadirectsPlugin::TowardsWall(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player) {
	sw->GetFieldCenter();
}
void ReadirectsPlugin::TowardsCorner(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player) {
}
void ReadirectsPlugin::TowardsCeiling(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player) {
}

void ReadirectsPlugin::TowardsCar(std::shared_ptr<ServerWrapper> sw, std::shared_ptr<CarWrapper> player) {
	// taken from the redirect plugin that comes with bakkesmod by default
	Vector playerPosition = player->GetLocation();
	Vector ballPosition		= sw->GetBall().GetLocation();
	Vector playerVelocity = player->GetVelocity();

	float ballSpeed = cvarManager->getCvar("readirects_car_shotspeed").getIntValue();
	float offset_z	= cvarManager->getCvar("readirects_car_heightoffset").getIntValue();

	bool predict = cvarManager->getCvar("readirects_car_pass_predict").getBoolValue();

	float predictMultiplierX = cvarManager->getCvar("readirects_car_predict_multiplier_x").getFloatValue();
	float predictMultiplierY = cvarManager->getCvar("readirects_car_predict_multiplier_y").getFloatValue();

	bool onGround = cvarManager->getCvar("readirects_car_pass_on_ground").getBoolValue();

	int		 offsetX	 = cvarManager->getCvar("readirects_car_boundoffset").getIntValue();
	int		 offsetY	 = cvarManager->getCvar("readirects_car_boundoffset").getIntValue();
	int		 offsetZ	 = random(offset_z / 3, offset_z);
	Vector offsetVec = Vector(offsetX, offsetY, offsetZ);

	Vector velMultiplied;
	if (predict)
		velMultiplied = playerVelocity * Vector(predictMultiplierX, predictMultiplierY, 1);
	offsetVec				= offsetVec + velMultiplied;
	Vector shotData = sw->GenerateShot(ballPosition, playerPosition + offsetVec, ballSpeed);
	if (onGround)
		shotData.Z = 0;
	Vector addedSpin = {cvarManager->getCvar("readirects_car_addedspin").getFloatValue(),
											cvarManager->getCvar("readirects_car_addedspin").getFloatValue(),
											cvarManager->getCvar("readirects_car_addedspin").getFloatValue()};

	sw->GetBall().SetVelocity(shotData);
	sw->GetBall().SetAngularVelocity(addedSpin, true);
}
