#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	countdown_fx = App->audio->LoadFx("Sound/Racer_Countdown.ogg");
	
	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2, 1, 4);
	car.chassis_offset.Set(0, 1.5f, 0);

	car.chassis2_size.Set(1.6, 1, 2.2f);
	car.chassis2_offset.Set(0, 2.5f, 0.7);

	car.fender2_offset.Set(0, 1.2, 1.9);
	car.fender_offset.Set(0, 1.2, -1.9);

	car.roof_offset.Set(0, 3.1f, 0.7f);
	car.light_offset.Set(-1, 1, 2);
	car.light_offset2.Set(1, 1, 2);

	
	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;


	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 12, 10);
	
	//Timers
	start_game_timer = new Timer();
	duration_game_timer = new Timer();


	start_game_timer->Start();

	second_timer_started = false;
	enable_control = false;
	lose = win = false;
	points = 0.0f;

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	delete start_game_timer;
	delete duration_game_timer;

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{

	//if(lose == false){
	if (enable_control == true){
		turn = acceleration = brake = 0.0f;

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			acceleration = MAX_ACCELERATION;
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (turn < TURN_DEGREES)
				turn += TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (turn > -TURN_DEGREES)
				turn -= TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			brake = BRAKE_POWER;
		}

		vehicle->ApplyEngineForce(acceleration);
		vehicle->Turn(turn);
		vehicle->Brake(brake);
	}

	vehicle->Render();
	
	//Three seconds, introduction time, while the player can't move and doesn't start
	if (second_timer_started == false){

		vec3 cam_position = { vehicle->GetPos().x, vehicle->GetPos().y + 10, vehicle->GetPos().z - 10};
		App->camera->Look(cam_position, vehicle->GetPos(), false);
		last_vehicle_position = { vehicle->GetPos().x, vehicle->GetPos().y, vehicle->GetPos().z };
		

		char title[80];
		double time = start_game_timer->Read() / 1000;
		App->audio->PlayFx(countdown_fx);
		if (start_game_timer->Read() <= 1000){
			sprintf_s(title, "RACER!     Ready");
		}

		if (start_game_timer->Read() > 1000 && start_game_timer->Read() <= 2000){
			sprintf_s(title, "RACER!     Steady");
		}

		if (start_game_timer->Read() > 2000 && start_game_timer->Read() <= 3000){
			sprintf_s(title, "RACER!     GO!");
		}
		App->window->SetTitle(title);
	}

	if (start_game_timer->Read() > 3000 && second_timer_started == false){
		enable_control = true;
		second_timer_started = true;
		duration_game_timer->Start();
		start_game_timer->Stop();
	}

	else if (lose == false && win == false && second_timer_started == true){
		
		//Camera follows the vehicle
		vec3 vector_direction = vehicle->GetPos() - last_vehicle_position;
		float vx = vector_direction.x;
		float vy = vector_direction.y;
		float vz = vector_direction.z;
		float vec_mod = sqrtf(vx*vx + vy*vy + vz*vz);
		vector_direction = vector_direction / vec_mod;

		vec3 look_at_point =  1.5*vector_direction;
		//App->camera->Look(App->camera->Position + vector_direction, vehicle->GetPos());
		
		float speed_cam = 0.7;
		vec3 position = vehicle->GetPos();

		vec3 dist_to_car = { -10.0f, 20.0f, -20.0f };
		vec3 camera_new_position = { position.x + (look_at_point.x * dist_to_car.x), position.y + look_at_point.y + dist_to_car.y, position.z + (look_at_point.z * dist_to_car.z) };
		vec3 direction = camera_new_position - App->camera->Position;

		App->camera->Look(App->camera->Position + (speed_cam * direction), position);

		last_vehicle_position = last_vehicle_position *vector_direction;

		char title[80];
		double time = duration_game_timer->Read() / 1000;
		sprintf_s(title, "RACER!     %.1f Km/h     TIME SPENT: %.0f s", vehicle->GetKmh(), time);
		App->window->SetTitle(title);
	}


	return UPDATE_CONTINUE;
}


void ModulePlayer::SetInitPos(){
	vehicle->Brake(2000);
	//Lacking orientation of the car
	vehicle->SetPos(0, 12, 10);
}
