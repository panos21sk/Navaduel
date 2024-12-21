// Include needed 3rd party libs
#include "raylib.h"
#include "raymath.h"
#define WIDTH 920
#define HEIGHT 600
// Include first party headers

// #Dont know if this is a worthy inclusion, keep in mind for structuring
//  typedef struct{
////for player 1
//     Camera camera1;
//     Vector3 ship1pos;
//     Vector3 ship1rot;
// } state;

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
	camera1.position = (Vector3){25.0f, 25.0f, 0.0f};// Camera position
	camera1.target = (Vector3){0.0f, 0.0f, 0.0f};	// Camera looking at point
	camera1.up = (Vector3){0.0f, 0.0f, -1.0f};		// Camera up vector (rotation towards target)
	camera1.fovy = 45.0f;									// Camera field-of-view Y
	camera1.projection = CAMERA_PERSPECTIVE;

	RenderTexture screenShip2 = LoadRenderTexture(WIDTH/2, HEIGHT);

	Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

	Vector3 camera_distance_vector = {0.0f, 25.0f, 50.f};
	Vector3 ship1forward_vec = {0.0f, 0.0f, 1.0f};
	Vector3 ship1pos = {0.0f, 0.0f, 0.0f}; // initialization
	Vector3 ship1rot = {0.0f, 0.0f, 0.0f};

	Vector3 ship2pos = {0.0f, 0.0f, 0.0f};

	//! Iniatializing Models for rendering
	Model ship = LoadModel("../../resources/models/ship2.glb");
	Texture2D water_tex = LoadTexture("../../resources/sprites/water.png");
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
			Vector3 ship1_mvmnt_vector;
			Vector3 ship2_mvmnt_vector;
			//ship1 axis movement
			if(IsKeyDown(KEY_W)){
				ship1_mvmnt_vector = (Vector3){0.0f, 0.0f, -2.0f};
				ship1pos = Vector3Add(ship1pos, ship1_mvmnt_vector);
			} else if(IsKeyDown(KEY_S)) {
				ship1_mvmnt_vector = (Vector3){0.0f, 0.0f, 2.0f};
				ship1pos = Vector3Add(ship1pos, ship1_mvmnt_vector);
			} else if(IsKeyDown(KEY_A)) {
				ship1_mvmnt_vector = (Vector3){-2.0f, 0.0f, 0.0f};
				ship1pos = Vector3Add(ship1pos, ship1_mvmnt_vector);
			} else if(IsKeyDown(KEY_D)) {
				ship1_mvmnt_vector = (Vector3){2.0f, 0.0f, 0.0f};
				ship1pos = Vector3Add(ship1pos, ship1_mvmnt_vector);
			}
			//apply rotation to model, only when a or d are pressed to not have any reduntant calls when rotation doesnt change
			//maybe trying rotation using mouse only???
			else if(IsKeyDown(KEY_Q)){
				ship1rot.y += 0.2f;
				ship.transform = MatrixRotateXYZ(ship1rot);
			} else if(IsKeyDown(KEY_E)){
				ship1rot.y -= 0.2f;
				ship.transform = MatrixRotateXYZ(ship1rot);
			}
			//ship2 axis movement
			if(IsKeyDown(KEY_UP)) {

			}

			//Update Camera manually
			//TODO: Find a way to get the camera behind the ship regardless of where its facing

			camera1.position = Vector3Add(ship1pos, camera_distance_vector);
			camera1.target = Vector3Add(ship1pos, (Vector3){0.0f, 10.0f, 0.0f});

			camera2.position = Vector3Add(ship2pos, camera_distance_vector);
			camera2.target = Vector3Add(ship2pos, (Vector3){0.0f, 10.0f, 0.0f});

			//UpdateCameraPro(&camera1, ship1_mvmnt_vector, (Vector3){0.0f,0.0f,0.0f}, 0);
			/*UpdateCameraPro(&camera1,
			(Vector3){
				(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))*2.0f -      // Move forward-backward
				(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))*2.0f,
				(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))*2.0f -   // Move right-left
				(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))*2.0f,
				0.0f                                                // Move up-down
			},
			(Vector3){
				GetMouseDelta().x,                            // Rotation: yaw
				GetMouseDelta().y,                            // Rotation: pitch
				0.0f                                                // Rotation: roll
			},
			GetMouseWheelMove()*2.0f);*/

			BeginTextureMode(screenShip1);
			{
				ClearBackground(RAYWHITE);

				BeginMode3D(camera1);
				{
					DrawModel(water_model, (Vector3){-100, -1, -100}, 1, BLUE);
					DrawCube(camera1.position, 1, 1, 1, RED);
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