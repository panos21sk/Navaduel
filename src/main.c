// Include needed 3rd party libs
#include "raylib.h"
#include "raymath.h"
#define WIDTH 1280
#define HEIGHT 600
// Include first party headers

typedef enum
{
	MAIN, //entry screen
	OPTIONS, //options
	GAME, //main game screen
	GAME_OVER, //game over screen - when a ship gets destroyed
	ABOUT // credits and basic gameplay
} screen;

int main() {
	//! Initialize window
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, "NavalDuel");
	// Set Framerate
	SetTargetFPS(60);

	//! Initialize Game State
	// First player
	Camera camera1 = {0};
	camera1.position = (Vector3){0.0f, 25.0f, 50.0f};// Camera position
	camera1.target = (Vector3){0.0f, 0.0f, 0.0f};	// Camera looking at point
	camera1.up = (Vector3){0.0f, 0.0f, -1.0f};		// Camera up vector (rotation towards target)
	camera1.fovy = 45.0f;									// Camera field-of-view Y
	camera1.projection = CAMERA_PERSPECTIVE;

	RenderTexture screenShip1 = LoadRenderTexture(WIDTH/2, HEIGHT);

	// Second player
	Camera camera2 = {0};
	camera2.position = (Vector3){25.0f, 25.0f, 0.0f};// Camera position
	camera2.target = (Vector3){0.0f, 0.0f, 0.0f};	// Camera looking at point
	camera2.up = (Vector3){0.0f, 0.0f, -1.0f};		// Camera up vector (rotation towards target)
	camera2.fovy = 45.0f;									// Camera field-of-view Y
	camera2.projection = CAMERA_PERSPECTIVE;

	RenderTexture screenShip2 = LoadRenderTexture(WIDTH/2, HEIGHT);

	Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

	Vector3 camera_distance_vector = {0.0f, 25.0f, 50.f};
	Vector3 ship1pos = {0.0f, 0.0f, 0.0f}; // initialization
	Vector3 ship1rot = {0.0f, 0.0f, 0.0f};

	Vector3 ship2pos = {0.0f, 0.0f, 10.0f};

	// Acceleration coefficients for Ship1;
	float accel_x_coefficient_1_r = 0.01f; // right
	float accel_x_coefficient_1_l = 0.01f; // left
	float accel_z_coefficient_1_f = 0.01f; // front
	float accel_z_coefficient_1_b = 0.01f; // back

	float accel_x_coefficient_2 = 0.01f;
	float accel_z_coefficient_2 = 0.01f;

	//! Iniatializing Models for rendering
	Model ship = LoadModel("../../resources/models/ship2.glb");
	Texture2D water_tex = LoadTexture("../resources/sprites/water.png");
	Mesh water_cube = GenMeshCube(300, 1, 300);
	Model water_model = LoadModelFromMesh(water_cube);
	water_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water_tex;

	Rectangle play_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 - 20, 160, 40};
	Vector2 mouse_point = {0.0f, 0.0f};

	//! Starting condition
	screen current_screen = MAIN;

	//! Game loop
	while (!WindowShouldClose()) // run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		//rendering begin
		switch (current_screen)
		{
		case MAIN:
		{
			mouse_point = GetMousePosition();
			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText("NAVALDUEL", 20, 20, 30, BLUE);
			DrawRectangleRec(play_button, BLACK);

			//TODO: Check if button was clicked. Make button collision detection into a function
			if (CheckCollisionPointRec(mouse_point, play_button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				//Vector2 mousepos = GetMousePosition();
				// check horizontal alignment
				/*if (mousepos.x - play_button.x > 0 && mousepos.x - play_button.x - play_button.width < 0)
				{
					if (mousepos.y - play_button.y > 0 && mousepos.y - play_button.y - play_button.height < 0)
					{
						DrawRectangle(50, 50, 200, 200, RED);
						current_screen = GAME;
					}
					else
						DrawRectangle(50, 50, 200, 200, BLUE);
				}
				else
					DrawRectangle(50, 50, 200, 200, BLUE);*/
				current_screen = GAME;
			}
			EndDrawing();
		} break;
		case GAME:
		{
			DisableCursor();
			//! Input Handling:
			// Ship Movement
			{
				// Ship1 axis movement
				if(IsKeyDown(KEY_W)){
					ship1pos.z += -2.0f*accel_z_coefficient_1_f;
					accel_z_coefficient_1_f = (accel_z_coefficient_1_f < 1) ? (accel_z_coefficient_1_f+0.005f) : 1;
				}
				if(IsKeyDown(KEY_S)) {
					ship1pos.z += 2.0f*accel_z_coefficient_1_b;
					accel_z_coefficient_1_b = (accel_z_coefficient_1_b < 1) ? (accel_z_coefficient_1_b+0.005f) : 1;
				}
				if(IsKeyDown(KEY_A)) {
					ship1pos.x += -2.0f*accel_x_coefficient_1_l;
					accel_x_coefficient_1_l = (accel_x_coefficient_1_l < 1) ? (accel_x_coefficient_1_l+0.005f) : 1;
				}
				if(IsKeyDown(KEY_D)) {
					ship1pos.x += 2.0f*accel_x_coefficient_1_r;
					accel_x_coefficient_1_r = (accel_x_coefficient_1_r < 1) ? (accel_x_coefficient_1_r+0.005f) : 1;
				}
				//apply rotation to model, only when q or e are pressed to not have any reduntant calls when rotation doesnt change
				//only 1 rotation per action (if-else block)
				if(IsKeyDown(KEY_Q)){
					ship1rot.y += 0.2f;
					ship.transform = MatrixRotateXYZ(ship1rot);
				} else if(IsKeyDown(KEY_E)){
					ship1rot.y -= 0.2f;
					ship.transform = MatrixRotateXYZ(ship1rot);
				}

				if(IsKeyReleased(KEY_W)) accel_z_coefficient_1_f = 0.01f;
				if(IsKeyReleased(KEY_S)) accel_z_coefficient_1_b = 0.01f;
				if(IsKeyReleased(KEY_A)) accel_x_coefficient_1_l = 0.01f;
				if(IsKeyReleased(KEY_D)) accel_x_coefficient_1_r = 0.01f;

				//ship2 axis movement
				if(IsKeyDown(KEY_UP)) {
					ship2pos.z += -1.0f;
				}
				if(IsKeyDown(KEY_DOWN)) {
					ship2pos.z += 1.0f;
				}
				if(IsKeyDown(KEY_RIGHT)) {
					ship2pos.x += 1.0f;
				}
				if(IsKeyDown(KEY_LEFT)) {
					ship2pos.x += -1.0f;
				}
				//ship2 rotation
				if(IsKeyDown(KEY_SLASH)) {

				} else if(IsKeyDown(KEY_APOSTROPHE)) {

				}
			}
			//Update Camera manually
			//TODO: Find a way to get the camera behind the ship regardless of where its facing

			camera1.position = Vector3Add(ship1pos, camera_distance_vector);
			camera1.target = Vector3Add(ship1pos, (Vector3){0.0f, 10.0f, 0.0f});

			camera2.position = Vector3Add(ship2pos, camera_distance_vector);
			camera2.target = Vector3Add(ship2pos, (Vector3){0.0f, 10.0f, 0.0f});

			BeginTextureMode(screenShip1);
			{
				ClearBackground(RAYWHITE);

				BeginMode3D(camera1);
				{
					DrawModel(water_model, (Vector3){-100, -1, -100}, 1, BLUE);
					DrawCube(camera1.position, 1, 1, 1, RED);
					DrawCube(camera2.position, 1, 1, 1, BLUE);
				}
				EndMode3D();

				DrawRectangle(0, 0, GetScreenWidth()/2, 40, Fade(RAYWHITE, 0.8f));
				DrawText("W/S/A/D to move", 10, 10, 20, DARKBLUE);
			}
			EndTextureMode();

			BeginTextureMode(screenShip2);
			{
				ClearBackground(RAYWHITE);

				BeginMode3D(camera2);
				{
					DrawModel(water_model, (Vector3){-100, -1, -100}, 1, BLUE);
					DrawCube(camera1.position, 1, 1, 1, RED);
					DrawCube(camera2.position, 1, 1, 1, BLUE);
				}
				EndMode3D();

				DrawRectangle(0, 0, GetScreenWidth()/2, 40, Fade(RAYWHITE, 0.8f));
				DrawText("UP/DOWN/RIGHT/LEFT to move", 10, 10, 20, DARKBLUE);
			}
			EndTextureMode();
			//! Rendering:
			BeginDrawing();
			{
				ClearBackground(RAYWHITE);

				DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){ 0, 0 }, WHITE);
				DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){ WIDTH/2.0f, 0 }, WHITE);
				DrawLine(WIDTH/2, 0, WIDTH/2, HEIGHT, BLACK);
			}
			EndDrawing();
		} break;
		default: break;
		}
	}
	UnloadRenderTexture(screenShip1);
	UnloadRenderTexture(screenShip2);
	//! destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}