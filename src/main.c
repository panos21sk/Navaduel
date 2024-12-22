// Include needed 3rd party libs
#include "raylib.h"
#include "raymath.h"
// Include first party headers
#include "ship.h"
#include "settings.h"

int main() {
	//! Initialize window
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, "NavalDuel");
	// Set Framerate
	SetTargetFPS(60);

	struct accel_settings accel_settings = {MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL};

	//! Initialize Game State
	// First player
	Ship ship1;
	Camera camera1 = {0};
	ship1.camera = &camera1;
	ship1.camera->position = (Vector3){0.0f, 25.0f, 50.0f}; // Camera position
	ship1.camera->target = (Vector3){0.0f, 0.0f, 0.0f};	// Camera looking at point
	ship1.camera->up = (Vector3){0.0f, 0.0f, -1.0f};		// Camera up vector (rotation towards target)
	ship1.camera->fovy = 45.0f;									// Camera field-of-view Y
	ship1.camera->projection = CAMERA_PERSPECTIVE;
	ship1.model = LoadModel("resources/models/ship1.glb");
	struct movement_buttons btns1 = {KEY_D, KEY_A, KEY_W, KEY_S};
	ship1.movement_buttons = btns1;
	ship1.accel = accel_settings;
	ship1.position = (Vector3){0.0f, 0.0f, 0.0f};

	RenderTexture screenShip1 = LoadRenderTexture(WIDTH/2, HEIGHT);

	// Second player
	Ship ship2;
	Camera camera2 = {0};
	ship2.camera = &camera2;
	ship2.camera->position = (Vector3){25.0f, 25.0f, 0.0f};// Camera position
	ship2.camera->target = (Vector3){0.0f, 0.0f, 0.0f};	// Camera looking at point
	ship2.camera->up = (Vector3){0.0f, 0.0f, -1.0f};		// Camera up vector (rotation towards target)
	ship2.camera->fovy = 45.0f;									// Camera field-of-view Y
	ship2.camera->projection = CAMERA_PERSPECTIVE;
	ship2.model = LoadModel("resources/models/ship2.glb");
	struct movement_buttons btns2 = {KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN};
	ship2.movement_buttons = btns2;
	ship2.accel = accel_settings;
	ship2.position = (Vector3){0.0f, 0.0f, 0.0f};

	RenderTexture screenShip2 = LoadRenderTexture(WIDTH/2, HEIGHT);

	Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

	Vector3 camera_distance_vector = {0.0f, 25.0f, 50.f};

	//! Iniatializing Models for rendering
	Texture2D water_tex = LoadTexture("resources/sprites/water.png");
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
		switch (current_screen) {
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
					current_screen = GAME;
				}
				EndDrawing();
				break;
			}
			case GAME:
			{
				DisableCursor();
				//! Input Handling:
				// Ship Movement
				checkMovement(&ship1);
				checkMovement(&ship2);

				//Update Camera manually
				//TODO: Find a way to get the camera behind the ship regardless of where its facing
				updateCamera(&ship1, camera_distance_vector);
				updateCamera(&ship2, camera_distance_vector);

				BeginTextureMode(screenShip1);
				{
					ClearBackground(RAYWHITE);

					BeginMode3D(camera1);
					{
						DrawModel(water_model, (Vector3){-100, -1, -100}, 1, BLUE);
						DrawModel(ship1.model, ship1.camera->position, 1, RED);
						DrawModel(ship2.model, ship2.camera->position, 1, BLUE);
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
						DrawModel(water_model, (Vector3){-100, -1, -100}, 1.0f, BLUE);
						DrawModel(ship1.model, ship1.camera->position, 1.0f, RED);
						DrawModel(ship2.model, ship2.camera->position, 1.0f, BLUE);
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
				break;
			}
			default:
				break;
		}
	}
	UnloadRenderTexture(screenShip1);
	UnloadRenderTexture(screenShip2);
	//! destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}