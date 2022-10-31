// Game1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <iostream>
#include <raylib.h>

int WIDTH = 2560;
int HEIGHT = 1440;

typedef struct Player {

	Rectangle pos_size{ 1000,-1000,50,100 };
	Rectangle old_pos{ pos_size.x,pos_size.y,pos_size.width,pos_size.height };

	Vector2 camera_coord = { 948,200 };
	Vector2 camera_vel = { 0,0 };
	float camera_acc = 14;

	float x_vel = 0;
	float y_vel = 0;

	float speed = 600;
	float acc = 24;
	float air_acc = 10;
	float air_decc = 8;
	float decc = 28;

	float grav = 24;
	float term_vel = 800;

	float jump = -1000;

	bool isJumping = false;
	bool isGrounded = false;

} Player;

//typedef struct Platform {
//	int x;
//	int y;
//	int width;
//	int height;
//	Color color;
//} Platform;

Rectangle platforms[] = {
	{100, 700, 1900, 50},
	{400, 400, 50,50},
	{900, 600, 50, 100},
	{650, 500, 50, 50},
	{650, 300, 50, 50},
	{400, 200, 50, 50},
	{650, 100, 50, 50},
	{900, 0, 100, 50},
	{2000, 800, 50, 50},
	{1500, 900, 1400, 100},
};

Color platform_color{ 255,255,255,255 };

void do_physics(Player* player) {

	float deltaTime = GetFrameTime();

	player->old_pos = player->pos_size;

	if (player->isGrounded) {
		if (IsKeyPressed(KEY_SPACE) && player->isGrounded) {
			player->y_vel = player->jump;
		}
		if (IsKeyDown(KEY_A) && player->x_vel > -player->speed) {
			player->x_vel -= player->acc;
		}
		if (IsKeyDown(KEY_D) && player->x_vel < player->speed) {
			player->x_vel += player->acc;
		}

		if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
			if (player->x_vel > 0) {
				if (player->x_vel - (player->decc) < 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel -= player->decc;
				}
			}
			if (player->x_vel < 0) {
				if (player->x_vel + (player->decc) > 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel += player->decc;
				}
			}
		}
	}
	else {
		if (IsKeyDown(KEY_A) && player->x_vel > -player->speed) {
			player->x_vel -= player->air_acc;
		}
		if (IsKeyDown(KEY_D) && player->x_vel < player->speed) {
			player->x_vel += player->air_acc;
		}

		if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
			if (player->x_vel > 0) {
				if (player->x_vel - (player->air_decc) < 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel -= player->air_decc;
				}
			}
			if (player->x_vel < 0) {
				if (player->x_vel + (player->air_decc) > 0) {
					player->x_vel = 0;
				}
				else {
					player->x_vel += player->air_decc;
				}
			}
		}
	}
	player->pos_size.x += player->x_vel * deltaTime;

	player->y_vel += player->grav;

	player->pos_size.y += player->y_vel * deltaTime;
}

void collision_detection(Player* player, Rectangle[]) {

	//std::printf("%.2f %.2f\n", player->pos_size.x, player->pos_size.y);
	//std::printf("%.2f\n", player->y_vel);

	float deltaTime = GetFrameTime();

	for (int i = 0; i < sizeof(platforms) / sizeof(Rectangle); i++) {
		if (CheckCollisionRecs(player->pos_size, platforms[i])) {
			//If my old position is above the platform
			if (player->old_pos.y + player->old_pos.height < platforms[i].y) {
				player->y_vel = 0;
				player->isGrounded = true;
				player->pos_size.y = platforms[i].y - player->pos_size.height - deltaTime;
				break;
			}
			//If my old position is below the platform
			if (player->old_pos.y > platforms[i].y + platforms[i].height) {
				player->y_vel = 0;
				player->pos_size.y = platforms[i].y + platforms[i].height + deltaTime;
			}
			//If my old position is to the left of the platform
			if (player->old_pos.x + player->old_pos.width < platforms[i].x) {
				player->x_vel = 0;
				player->pos_size.x = platforms[i].x - player->pos_size.width - deltaTime;
			}
			//If my old position is to the right of the platform
			if (player->old_pos.x > platforms[i].x + platforms[i].width) {
				player->x_vel = 0;
				player->pos_size.x = platforms[i].x + platforms[i].width + deltaTime;
			}
		}
		else {
			player->isGrounded = false;
		}
	}
	for (int i = 0; i < sizeof(platforms) / sizeof(Rectangle); i++) {
		if (CheckCollisionRecs(player->pos_size, platforms[i])) {
			//If my old position is to the left of the platform
			if (player->old_pos.x + player->old_pos.width < platforms[i].x) {
				player->x_vel = 0;
				player->pos_size.x = platforms[i].x - player->pos_size.width - deltaTime;
			}
			//If my old position is to the right of the platform
			if (player->old_pos.x > platforms[i].x + platforms[i].width) {
				player->x_vel = 0;
				player->pos_size.x = platforms[i].x + platforms[i].width + deltaTime;
			}
		}
	}
}

void camera_movement(Player * player) {
	player->camera_vel.x = (0.05 * (player->pos_size.x - player->camera_coord.x));
	player->camera_vel.y = (0.05 * (player->pos_size.y - player->camera_coord.y));
	player->camera_coord.x += player->camera_vel.x;
	player->camera_coord.y += player->camera_vel.y;
}

void draw_screen(Rectangle[], Player* player, Camera2D camera) {
	BeginDrawing();

		ClearBackground(BLACK);

		BeginMode2D(camera);



		//debug
		for (int i = 0; i < sizeof(platforms) / sizeof(Rectangle); i++) {
			DrawRectangle(platforms[i].x, platforms[i].y, platforms[i].width, platforms[i].height, platform_color);
		}
		DrawRectangleLines(player->pos_size.x, player->pos_size.y, player->pos_size.width, player->pos_size.height, { 255, 0, 0, 255 });
		DrawPixel(player->pos_size.x, player->pos_size.y, Color{ 0,255,0,255 });
		DrawPixel(player->camera_coord.x, player->camera_coord.y, Color{ 255,0,0,255 });
		//

		EndMode2D();

		DrawFPS(GetScreenWidth() - 200, GetScreenHeight() - 80);

	EndDrawing();
}

int main(void) 
{	
	InitWindow(WIDTH, HEIGHT, "Game");

	ToggleFullscreen();

	Player player;

	Camera2D camera;
	camera.zoom = 3;
	camera.offset = { (float) (WIDTH / 2) - (player.pos_size.width / 2), (float) (HEIGHT / 2) - (player.pos_size.height / 2)};
	camera.rotation = 0;

	SetTargetFPS(165);


	while (!WindowShouldClose()) {

		do_physics(&player);

		collision_detection(&player, platforms);

		camera_movement(&player);
		camera.target = { player.camera_coord.x, player.camera_coord.y };

		draw_screen(platforms, &player, camera);
	}

	CloseWindow();
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
