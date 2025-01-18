#ifndef SHIP_H
#define SHIP_H

/* Import the required game headers (first party libraries) */
#include "anim.h"
#include "obstacles.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"

/* Import the required tool headers (third party libraries) */
#include "cJSON.h"

/* Defines the maximum acceleration factor, used for each ship type's special statistics */
#define MAX_ACCEL 1
/* Defines the maximum turn factor for every cannon */
#define MAX_TURN 3.1415/9.0f
/* Defines the maximum turn up factor for every cannon */
#define MAX_TURN_UP 3.1415/2.25f
/* Defines the acceleration factor, used for each ship type's special statistics */
#define ACCEL_STEP 0.005f
/* Defines the deacceleration factor, used for each ship type's special statistics */
#define DEACCEL_STEP 0.01f
/* Defines the minimum acceleration factor, used for each ship type's special statistics */
#define MIN_ACCEL 0.01f
/* Defines the velocity factor, used for each ship type's special statistics */
#define MOVEMENT_STEP 1.0f

/* Movement buttons structure */
typedef struct {
    int right;
    int left;
    int forward;
    int backwards;
    int turn_cannon_left;
    int turn_cannon_right;
    int fire;
} movement_buttons;

/* Acceleration settings structure */
typedef struct {
    float r_coefficient; //steer right accel
    float l_coefficient; //steer left accel
    float f_coefficient; //forward accel
    float b_coefficient; //backward accel
    float turn_l_coefficient; //turn cannon left accel
    float turn_r_coefficient; //turn cannon right accel
    float fire_coefficient; //fire accel
} accel_settings;

/* Cannonball structure */
typedef struct {
    Vector3 position; //cannonball position in world  space
    Vector3 velocity; //cannonball velocity vector
    Vector3 accel; //cannonball accel vector
    bool has_splashed; //bool to store whether the cannonball has splashed (y coordinate low enough to trigger animation and sound once)
    bool has_hit_enemy; //bool to store whether the cannonball has hit an enemy (triggers explosion animation and sound once)
} Cannonball;

/* Cannon structure */
typedef struct {
    Vector3 relative_position; //cannon position vector relative to ship center
    Vector3 rotation; //cannon rotation vector (pitch and yaw are only used)
    Model stand_model; //model for the cannon stand
    Model rail_model; //model for cannon rail. seperation to ensure independant rotation
} Cannon;

/* Ship structure */
typedef struct {
    int id; //player id, also representing index in ship list
    int team; //integer to represent team.
    float yaw; //yaw float to determine rotation around y axis
    movement_buttons movement_buttons; //movement buttons
    Vector3 position; //position in world space
    Vector3 prev_position; //previous position to fallback to in case of collision with bounds
    Vector3 prev_position_turn; //position in previous turn to be displayed in current turn
    float prev_shot_release; //yaw of cannon at time of release of last shot
    float time_to_reload_since_last_shot; //time to reload until cannonball splashes
    Camera *camera; //camera associated with ship
    Model model; //ship model
    Cannon* cannon; //cannon struct associated with ship
    Cannonball cannonball; //cannonball associated with ship
    BoundingBox boundary; //ship cube hitbox
    bool can_fire; //bool saving whether the ship can fire currently
    bool can_move; //bool saving whether the ship can move currently
    bool is_spawn_valid; //bool to determine whether spawn is valid to continue with init process
    int current_health; //current ship health
    accel_settings default_accel; //default acceleration of ship
    accel_settings accel; //current acceleration
    bool is_destroyed; //bool determining if ship is destroyed
    //ship specific
    float cannonball_power_coefficient; //coefficient to change power of shot based on ship type
    int initial_health; //integer to save each ship type's initial health
    float max_accel; //ship's max accel
    float min_accel; //ship's min accel
    float accel_step; //ship/s acceleration change step
    Vector3 camera_distance_vector_fp; //camera distance vector relative to ship according to type for first person perspective
    Vector3 camera_distance_vector_tp; //camera distance vector relative to ship according to type for third person perspective
    float sphere_hitbox_radius; //sphere hitbox
} Ship;

/* Ships' data structure */
typedef struct {
    Ship* ship_list; //ship of all lists in current game
    int* type_list; //array of types at each index corresponding to different ship ids
    int* team_list; //array of teams at each index corresponding to different ship ids
    int player_count; //initial amount of players
} Ship_data;

/* Variable declarations */
extern const accel_settings default_accel; //universal default accel to base each differing default accel off of.
extern Ship_data ship_data; //global external ship data variable
extern bool game_ended; //bool to save whether the game ended
extern Cannonball initcannonball; //placeholder/initialization cannonball to ensure proper behaviour.

/* Function declarations */
Ship* SetupShips(int player_count, const int* type_list, const int* team_list, Obstacles obs, Model* ship_models);
Ship_data CreateShipData(int player_count, int* type_list, int* team_list, Obstacles obs, Model* ship_models);
Ship LoadShip(int type, const cJSON *shipState, int playercount);
void *EndGame();
void CheckWin(Ship_data ship_data);
int FindNextAliveShipIndex(Ship_data ship_data, int start_index);
void CheckMovement(Ship *ship, Sound fire);
void InitializeCannonball(Ship* ship);
void UpdateCannonballState(Cannonball* cannonball, Sound splash, Animation* splash_anim);
void UpdateShipCamera(const Ship *ship, bool first_person);
void CheckHit(Ship* player_ship, Ship* enemy_ship, Sound explosion, Obstacles obstacles, Animation* explosion_anim);
void CheckCollisionWithBounds(Ship *ship, BoundingBox bound);

#endif // SHIP_H