#include <vector>
#include <map>
#include <iostream>
#include <raylib.h>
#include <fstream>
#include <string>

int WIDTH = 2560;
int HEIGHT = 1240;

int DEATH_ZONE = 5000;

float STARTING_X = -200;
float STARTING_Y = -200;

enum Platform_Type {
	norm,
	ice,
	sand,
	jumpy,
	walljump,
	hyperjumpy,
	water,
	windToRight,
	windToLeft,
	windToTop,
	windToBottom,
};

typedef struct Player {

	Rectangle posAndSize{ STARTING_X,STARTING_Y,50,100 }; 
	Rectangle old_pos{ posAndSize.x,posAndSize.y,posAndSize.width,posAndSize.height };

	Vector2 camera_coord = { STARTING_X,STARTING_Y };
	Vector2 camera_vel = { 0,0 };
	float camera_acc = 5000;

	float zoom = 2;
	float zoomMultiplier = 0;
	float cameraGrip = 6.7;

	float x_vel = 0;
	float y_vel = 0;

	float speed = 600;
	float acc = 1970;
	float air_acc = 1200;
	float air_decc = 700;
	float decc = 1900;

	float grav = 3500;
	float term_vel = 10000;

	float jump = -800;

	bool isJumping = false;
	bool onWall = false;
	bool isGrounded = false;
	bool inPhaseSpace = false;
	bool inWater = false;

} Player;


enum Asset_Type {
	decor,
	interactable,
	entry,
};


typedef struct Platforms_Data {
	float x;
	float y;
	float width;
	float height;
	Platform_Type type;
	bool phaseSpace;
};

typedef struct Platforms_Connecting {

	Platforms_Data platformY = {0,0,0,0, norm};
	Platforms_Data platformX = {0,0,0,0, norm};

	bool shouldStopTop = false;
	bool shouldStopBottom = false;
	bool shouldStopLeft = false;
	bool shouldStopRight = false;

} Platforms_Connecting;

typedef struct Assets_Data {
	float x;
	float y;
	float width;
	float height;
	Asset_Type type;
};

class Current_Level {
	private:

	public:
		std::vector<Platforms_Data> platforms = {
		};

		std::vector<Assets_Data> assets = {
		};

		//debug colors
		Color platform_color{ 255,255,255,255 };
		Color asset_color{ 30,30,30,255 };
};

void do_physics(Player* player) {

	float deltaTime = GetFrameTime();

	player->old_pos = player->posAndSize;

	if (player->posAndSize.y > DEATH_ZONE) {
		player->posAndSize.x = STARTING_X;
		player->posAndSize.y = STARTING_Y;
		player->y_vel = 0;
		player->x_vel = 0;
	}

	if (player->isGrounded) {
		if (IsKeyPressed(KEY_SPACE)) {
			player->y_vel = player->jump;
		}
		if (IsKeyDown(KEY_A) && player->x_vel > -player->speed) {
			player->x_vel -= player->acc * deltaTime;
		}
		if (IsKeyDown(KEY_D) && player->x_vel < player->speed) {
			player->x_vel += player->acc * deltaTime;
		}

		if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
			if (player->x_vel > 0) {
				if (player->x_vel - (player->decc * deltaTime) < 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel -= player->decc * deltaTime;
				}
			}
			if (player->x_vel < 0) {
				if (player->x_vel + (player->decc * deltaTime) > 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel += player->decc * deltaTime;
				}
			}
		}

		// if I'm going over the allowed speed when transitioning to different blocks
		if (abs(player->x_vel) > player->speed) {
			if (player->x_vel > 0) {
				if (player->x_vel - (player->decc * deltaTime) < 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel -= player->decc * deltaTime;
				}
			}
			if (player->x_vel < 0) {
				if (player->x_vel + (player->decc * deltaTime) > 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel += player->decc * deltaTime;
				}
			}
		}

		if (abs(player->y_vel) > player->term_vel) {
			if (player->y_vel > 0) {
				if (player->y_vel - (player->decc * deltaTime) < 0) {
					player->y_vel = 0;
				}
				else {
					player->x_vel -= player->decc * deltaTime;
				}
			}
			if (player->y_vel < 0) {
				if (player->y_vel + (player->decc * deltaTime) > 0) {
					player->y_vel = 0;
				}
				else {
					player->y_vel += player->decc * deltaTime;
				}
			}
		}
	}
	else {
		if (IsKeyDown(KEY_A) && player->x_vel > -player->speed) {
			player->x_vel -= player->air_acc * deltaTime;
		}
		if (IsKeyDown(KEY_D) && player->x_vel < player->speed) {
			player->x_vel += player->air_acc * deltaTime;
		}

		if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
			if (player->x_vel > 0) {
				if (player->x_vel - (player->air_decc * deltaTime) < 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel -= player->air_decc * deltaTime;
				}
			}
			if (player->x_vel < 0) {
				if (player->x_vel + (player->air_decc * deltaTime) > 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel += player->air_decc * deltaTime;
				}
			}
		}
	}
	if (player->onWall == true) {
		if (IsKeyPressed(KEY_SPACE)) {
			if (IsKeyDown(KEY_A)) {
				player->y_vel = player->jump;
				player->x_vel = 130;
			}
			if (IsKeyDown(KEY_D)) {
				player->y_vel = player->jump;
				player->x_vel = -130;
			}
		}
	}
	if (player->inWater == true) {
		if (IsKeyDown(KEY_SPACE)) {
			player->y_vel += -5000 * deltaTime;
		}
	}

	player->posAndSize.x += player->x_vel * deltaTime;

	if (abs(player->y_vel) < player->term_vel) {
		player->y_vel += player->grav * deltaTime;
	}
	else {
		if (player->y_vel > 0) {
			player->y_vel -= player->air_decc * deltaTime;
		}
		if (player->y_vel < 0) {
			player->y_vel += player->air_decc * deltaTime;
		}
	}

	player->posAndSize.y += player->y_vel * deltaTime;
}

void platform_responsesY(Player * player, Platforms_Data platform, float newPos) {
	
	float y_recoil;

	if (!platform.phaseSpace) {
		y_recoil = -player->y_vel;
		player->y_vel = 0;
		player->isGrounded = true;
		player->posAndSize.y = newPos;
	}

	if (platform.type == norm || platform.type == walljump) {
		player->speed = 600;
		player->acc = 1970;
		player->decc = 1900;
		player->air_acc = 1200;
		player->air_decc = 700;

		player->grav = 3500;
		player->term_vel = 10000;

		player->jump = -800;

	}
	else if (platform.type == ice) {
		player->speed = 870;
		player->acc = 1970;
		player->decc = 590;
		player->air_acc = 1200;
		player->air_decc = 700;

		player->grav = 3500;
		player->term_vel = 10000;

		player->jump = -800;

	}
	else if (platform.type == sand) {
		player->speed = 200;
		player->acc = 600;
		player->decc = 2000;
		player->air_acc = 1200;
		player->air_decc = 700;

		player->grav = 3500;
		player->term_vel = 10000;

		player->jump = -600;

	}
	else if (platform.type == jumpy) {
		player->y_vel = y_recoil * 0.85;

		player->speed = 600;
		player->acc = 1970;
		player->decc = 1900;
		player->air_acc = 1200;
		player->air_decc = 700;

		player->grav = 3500;
		player->term_vel = 10000;

		player->jump = -800;

	}
	else if (platform.type == hyperjumpy) {
		player->y_vel = y_recoil;

		player->speed = 600;
		player->acc = 1970;
		player->decc = 1900;
		player->air_acc = 1200;
		player->air_decc = 700;

		player->grav = 3500;
		player->term_vel = 10000;

		player->jump = -800;
	}
}

void platform_responsesX(Player* player, Platforms_Data platform, float newPos) {

	float x_recoil;
	if (platform.type != water) {
		x_recoil = -player->x_vel * 0.85;
		player->x_vel = 0;
		player->posAndSize.x = newPos;
	}

	if (platform.type == jumpy) {
		player->x_vel = x_recoil;
	}
	else if (platform.type == walljump) {
		player->grav = 3500;
		player->term_vel = 100;
		player->air_decc = 4000;
		player->jump = -800;
		player->onWall = true;
	}
	else if (platform.type == ice) {
		player->y_vel *= 1 + GetFrameTime();
	}
}

void phase_space_responses(Player* player, Platforms_Data platform) {

	player->inPhaseSpace = true;

	if (platform.type == water && player->posAndSize.y + (player->posAndSize.height / 2) > platform.y) {
		player->inWater = true;
		player->speed = 100;
		player->acc = 200;
		player->decc = 2300;
		player->air_acc = 1200;
		player->air_decc = 2300;

		player->grav = 300;
		player->term_vel = 100;

		player->jump = -20;
	}
	if (platform.type == windToRight) {
		player->x_vel += 5000 * GetFrameTime();
	}
	if (platform.type == windToLeft) {
		player->x_vel -= 5000 * GetFrameTime();
	}
	if (platform.type == windToTop) {
		player->y_vel -= 5000 * GetFrameTime();
	}
	if (platform.type == windToBottom) {
		player->y_vel += 5000 * GetFrameTime();
	}
}

void collision_detection(Player* player, std::vector<Platforms_Data> platforms) {

	//std::printf("%.2f %.2f\n", player->posAndSize.x, player->posAndSize.y);
	//std::printf("%.2f\n", player->y_vel);

	int cntr = 0;
	for (int i = 0; i < platforms.size(); i++) {
		cntr += 1;
		if (CheckCollisionRecs(player->posAndSize, Rectangle{ platforms[i].x, platforms[i].y, platforms[i].width, platforms[i].height })) {
			//If my old position is above the platform
			if (player->old_pos.y + player->old_pos.height <= platforms[i].y) {
				if (!platforms[i].phaseSpace) {
					platform_responsesY(player, platforms[i], platforms[i].y - player->posAndSize.height);
					break;
				}
			}
			//If my old position is below the platform
			if (player->old_pos.y >= platforms[i].y + platforms[i].height) {
				if (!platforms[i].phaseSpace) {
					platform_responsesY(player, platforms[i], platforms[i].y + platforms[i].height);
					break;
				}
			}
		}
		else {
			player->isGrounded = false;
		}
	}
	for (int i = 0; i < platforms.size(); i++) {
		if (CheckCollisionRecs(player->posAndSize, Rectangle{ platforms[i].x, platforms[i].y, platforms[i].width, platforms[i].height })) {
			//If my old position is to the left of the platform
			if (player->old_pos.x + player->old_pos.width <= platforms[i].x) {
				if (!platforms[i].phaseSpace) {
					platform_responsesX(player, platforms[i], platforms[i].x - player->posAndSize.width);
					break;
				}
			}
			//If my old position is to the right of the platform
			if (player->old_pos.x >= platforms[i].x + platforms[i].width) {
				if (!platforms[i].phaseSpace) {
					platform_responsesX(player, platforms[i], platforms[i].x + platforms[i].width);
					break;
				}
			}
		}
		else {
			player->onWall = false;
		}
	}
	for (int i = 0; i < platforms.size(); i++) {
		if (CheckCollisionRecs(player->posAndSize, Rectangle{ platforms[i].x, platforms[i].y, platforms[i].width, platforms[i].height })) {
			phase_space_responses(player, platforms[i]);
			break;
		}
		else {
			player->inPhaseSpace = false;
			player->inWater = false;
		}
	}

	if (player->inWater == false && player->onWall == false && player->isGrounded == false) {
		player->grav = 3500;
		player->term_vel = 10000;

		player->speed = 600;
		player->acc = 1970;
		player->decc = 1900;
		player->air_acc = 1200;
		player->air_decc = 700;
	}
}

void camera_movement(Player * player, Camera2D * camera) {
	float deltaTime = GetFrameTime();

	float vel;
	if (abs(player->x_vel) > abs(player->y_vel)) {
		vel = abs(player->x_vel);
	}
	else {
		vel = abs(player->y_vel);
	}

	float distance = player->zoomMultiplier - vel;
	player->zoomMultiplier += 3.7 * -distance * deltaTime;

	camera->zoom = ((6 * abs(player->zoomMultiplier)) / (abs(player->zoomMultiplier) + 20000)) + 2;
	player->cameraGrip = ((30 * abs(player->zoomMultiplier)) / (abs(player->zoomMultiplier) + 2000)) + 6.7;

	player->camera_vel.x = (player->cameraGrip * (player->posAndSize.x - player->camera_coord.x));
	player->camera_vel.y = (player->cameraGrip * (player->posAndSize.y - player->camera_coord.y));
	player->camera_coord.x += player->camera_vel.x * deltaTime;
	player->camera_coord.y += player->camera_vel.y * deltaTime;


}

void draw_screen(Current_Level level, Player* player, Camera2D camera) {
	BeginDrawing();

		ClearBackground(BLACK);

		BeginMode2D(camera);



	//debug

		//if (player->inWater == false) {
		//	std::cout << "not in water" << std::endl;
		//}
		//else {
		//	std::cout << "is in water" << std::endl;
		//}

		for (int i = 0; i < level.platforms.size(); i++) {
			if (level.platforms[i].type == norm) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, level.platform_color);
			}
			else if (level.platforms[i].type == ice) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, Color{ 130,190,255,255 });
			}
			else if (level.platforms[i].type == sand) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, Color{ 200,190,50,255 });
			}
			else if (level.platforms[i].type == jumpy) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, Color{ 255,70,90,255 });
			}
			else if (level.platforms[i].type == walljump) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, Color{ 170,170,170,255 });
			}
			else if (level.platforms[i].type == hyperjumpy) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, Color{ 180,255,180,255 });
			}
			else if (level.platforms[i].type == water) {
				DrawRectangle(level.platforms[i].x, level.platforms[i].y, level.platforms[i].width, level.platforms[i].height, Color{ 0,0,255,130 });
			}
		}
		for (int i = 0; i < level.assets.size(); i++) {
			DrawRectangle(level.assets[i].x, level.assets[i].y, level.assets[i].width, level.assets[i].height, level.asset_color);
		}
		DrawRectangleLines(player->posAndSize.x, player->posAndSize.y, player->posAndSize.width, player->posAndSize.height, { 255, 0, 0, 255 });
		//DrawPixel(player->posAndSize.x, player->posAndSize.y, Color{ 0,255,0,255 });
		//DrawPixel(player->camera_coord.x, player->camera_coord.y, Color{ 255,0,0,255 });
		//

		EndMode2D();

		DrawFPS(GetScreenWidth() - 200, GetScreenHeight() - 80);

	EndDrawing();
}

void load(Current_Level * level) {
	std::fstream file("Levels\\level1.lvl");

	std::string platform_data;

	float x, y, width, height, type;
	bool phaseSpace;

	while (file >> x >> y >> width >> height >> type >> phaseSpace) {
		level->platforms.insert(level->platforms.begin(), Platforms_Data{ x, y, width, height, (Platform_Type)type, phaseSpace });
	}

	file.close();
}

int main(void)
{	
	InitWindow(WIDTH, HEIGHT, "Game");

	//ToggleFullscreen();

	Player player;
	Current_Level level;

	Platforms_Connecting platforms_connecting;

	load(&level);

	Camera2D camera;
	camera.zoom = player.zoom;
	camera.offset = { (float) (WIDTH / 2) - (player.posAndSize.width + 25), (float) (HEIGHT / 2) - (player.posAndSize.height + 50)};
	camera.rotation = 0;

	//if fps is > 1000 or < 12, you fall through the platform
	SetTargetFPS(165);

	while (!WindowShouldClose()) {

		do_physics(&player);

		collision_detection(&player, level.platforms);

		camera_movement(&player, &camera);
		camera.target = { player.camera_coord.x, player.camera_coord.y };

		draw_screen(level, &player, camera);
	}

	CloseWindow();
}