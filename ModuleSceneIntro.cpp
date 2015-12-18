#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	coins_to_collect = 3;

	s.size = vec3(5, 3, 1);
	s.SetPos(0, 2.5f, 20);
	s.color = Red;


	coin1.height = coin2.height = coin3.height = 0.5f;
	coin1.radius = coin2.radius = coin3.radius = 1.0f;
	coin1.color = coin2.color = coin3.color = White;

	coin1.SetPos(0, 18, 180);

	coin2.SetRotation(90, vec3{0, 1, 0});
	coin2.SetPos(15, 13, 135);

	coin3.SetRotation(90, vec3{ 0, 1, 0 });
	coin3.SetPos(88, 3, 100);

	sensor = App->physics->AddBody(s, 0.0f);
	sensor->SetAsSensor(true, NOT_SENSOR);
	

	coin1_sensor = App->physics->AddBody(coin1, 0.0f);
	coin1_sensor->SetAsSensor(true, COIN);
	coin1_sensor->collision_listeners.add(this);

	coin2_sensor = App->physics->AddBody(coin2, 0.0f);
	coin2_sensor->SetAsSensor(true, COIN);
	coin2_sensor->collision_listeners.add(this);

	coin3_sensor = App->physics->AddBody(coin3, 0.0f);
	coin3_sensor->SetAsSensor(true, COIN);
	coin3_sensor->collision_listeners.add(this);

	p1.size = {500, 0.1f, 500};
	p1.SetPos(0, 0, 0);
	fall_car = App->physics->AddBody(p1, 0.0f);
	fall_car->SetAsSensor(true, FALLEN_CAR);
	fall_car->collision_listeners.add(this);

	CreateScenary();


	coin_fx = App->audio->LoadFx("Sound/coin.wav");
	victory_fx = App->audio->LoadFx("Sound/Victory_Sound_Effect.ogg");
	defeat_fx = App->audio->LoadFx("Sound/Defeat_Sound_Effect.ogg");
	music = App->audio->LoadFx("Sound/Biohazard_Gerard.ogg");
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	App->audio->CleanUp();
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Plane p(0, 1, 0, 0);
	//p.axis = true;
	//p.Render();
	curve.Render();

	p2List_item<Cube>* item;
	for (item = path.getFirst(); item != NULL; item = item->next){
		item->data.Render();
	}

	if (coins_achieved == true){

		srand(0);
		int random = rand() % 1;
		if (random == 0){
			sensor->SetPos(43, 5, 57.5f);
		}

		else if (random == 1){
			sensor->SetPos(43, 5, 0);
		}

		sensor->SetAsSensor(true, FINISH_LINE);
		sensor->collision_listeners.add(this);
		sensor->GetTransform(&s.transform);
		coins_achieved = false;
	}

	if (coins_to_collect == 0){
		s.Render();
	}

	if (coin1_sensor->can_change_colour == true){
		coin1.color = Red;
		coin1_sensor->can_change_colour = false;
		App->audio->PlayFx(coin_fx);
	}
	coin1_sensor->GetTransform(&coin1.transform);
	coin1.Render();

	if (coin2_sensor->can_change_colour == true){
		coin2.color = Red;
		coin2_sensor->can_change_colour = false;
		App->audio->PlayFx(coin_fx);
	}
	coin2_sensor->GetTransform(&coin2.transform);
	coin2.Render();

	if (coin3_sensor->can_change_colour == true){
		coin3.color = Red;
		coin3_sensor->can_change_colour = false;
		App->audio->PlayFx(coin_fx);
	}
	coin3_sensor->GetTransform(&coin3.transform);
	coin3.Render();
	
	if (App->player->duration_game_timer->Read() > GAME_TIME){

		App->player->lose = true;
		App->player->win = false;

		App->player->points = 0.0f;
		char title[80];
		sprintf_s(title, "YOU LOSE :(    SCORE: %.2f", App->player->points && App->player->win == false);
		App->window->SetTitle(title);
		App->player->duration_game_timer->Stop();
		App->player->enable_control = false;
		App->audio->PlayFx(defeat_fx);
	}

	else if (App->player->duration_game_timer->Read() < GAME_TIME && App->player->win == true){

		App->player->lose = false;
		App->player->duration_game_timer->Stop();
		char title[80];
		sprintf_s(title, "YOU WIN :D    SCORE: %.2f", App->player->points);
		App->window->SetTitle(title);
		App->audio->PlayFx(victory_fx);
	}
	//if (App->player->win == false && App->player->lose == false){
		//App->audio->PlayFx(music);
	//}

	

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1->IsSensor() && body1->GetSensorType() == FINISH_LINE){
		LOG("wiiiiiiiiin");
		App->player->win = true;
		App->player->enable_control = false;
	}

	if (body1->IsSensor() && body1->GetSensorType() == COIN){
		App->player->points = App->player->points + 1000;
		if (coins_to_collect > 0){
			coins_to_collect--;
		}

		if (coins_to_collect == 0){
			coins_achieved = true;
		}
		body1->can_change_colour = true;
		body1->SetAsSensor(true, NOT_SENSOR);		
		
	}
	if (body1->IsSensor() && body1->GetSensorType() == FALLEN_CAR){
		App->player->points - 50;
		App->player->SetInitPos();
	}

	if (body1->IsSensor() && body1->GetSensorType() == NOT_SENSOR){
		LOG("Coin already taken or not the finish line yet");
	}

	LOG("Hit!");
}

void ModuleSceneIntro::CreateScenary() {
		

	Cube start_rec;
	start_rec.size = vec3(10, 2, 118.5);
	start_rec.color = Green;
	start_rec.SetPos(0, 7, 52);
	App->physics->AddBody(start_rec, 0.0f);

	path.add(start_rec);

	Cube path_1;
	path_1.size = vec3(10, 2, 15);
	path_1.color = Blue;
	path_1.SetPos(0, 5, 115);

	App->physics->AddBody(path_1, 0.0f);

	path.add(path_1);

	Cube path_2;
	path_2.size = vec3(10, 2, 40);
	path_2.color = Blue;
	path_2.SetPos(0, 10, 140);
	path_2.SetRotation(-20, vec3{1, 0, 0});
	App->physics->AddBody(path_2, 0.0f);

	path.add(path_2);

	Cube first_coin;
	first_coin.size = vec3(20, 2, 40);
	first_coin.color = Red;
	first_coin.SetPos(0, 15, 180);
	App->physics->AddBody(first_coin, 0.0f);

	path.add(first_coin);

	Cube second_coin;
	second_coin.size = vec3(30, 2, 30);
	second_coin.color = Blue;
	second_coin.SetPos(15, 10, 135);
	App->physics->AddBody(second_coin, 0.0f);

	path.add(second_coin);

	Cube path_3;
	path_3.size = vec3(20, 2, 40);
	path_3.color = Blue;
	path_3.SetPos(18.5, 11, 180);
	path_3.SetRotation(-25, vec3{0,0,1});
	App->physics->AddBody(path_3, 0.0f);

	path.add(path_3);

	Cube path_4;
	path_4.size = vec3(50, 2, 15);
	path_4.color = Green;
	path_4.SetPos(40, 8, 180);
	App->physics->AddBody(path_4, 0.0f);

	path.add(path_4);

	Cube path_5;
	path_5.size = vec3(8, 2, 15);
	path_5.color = Green;
	path_5.SetPos(66, 8, 179);
	path_5.SetRotation(20,vec3{ 0, 1, 0 });
	App->physics->AddBody(path_5, 0.0f);

	path.add(path_5);

	Cube path_6;
	path_6.size = vec3(8, 2, 15);
	path_6.color = Green;
	path_6.SetPos(71, 8, 176);
	path_6.SetRotation(35, vec3{ 0, 1, 0 });
	App->physics->AddBody(path_6, 0.0f);

	path.add(path_6);

	Cube path_7;
	path_7.size = vec3(8, 2, 15);
	path_7.color = Green;
	path_7.SetPos(76, 8, 172);
	path_7.SetRotation(40, vec3{ 0, 1, 0 });
	App->physics->AddBody(path_7, 0.0f);

	path.add(path_7);

	Cube path_8;
	path_8.size = vec3(15, 2, 15);
	path_8.color = Green;
	path_8.SetPos(85, 5, 165);
	path_8.SetRotation(45, vec3{ 0, 1, 0 });
	App->physics->AddBody(path_8, 0.0f);

	path.add(path_8);

	Cube path_9;
	path_9.size = vec3(15, 2, 15);
	path_9.color = Green;
	path_9.SetPos(88, 2, 157);
	path_9.SetRotation(90, vec3{ 0, 1, 0 });
	App->physics->AddBody(path_9, 0.0f);

	path.add(path_9);

	Cube path_10;
	path_10.size = vec3(15, 2, 170);
	path_10.color = Red;
	path_10.SetPos(88, 0, 77.5);
	App->physics->AddBody(path_10, 0.0f);

	path.add(path_10);

	Cube path_11;
	path_11.size = vec3(76, 2, 15);
	path_11.color = Red;
	path_11.SetPos(43, 3.6f, 57.5f);
	path_11.SetRotation(-5.65f, vec3{0, 0, 1});
	App->physics->AddBody(path_11, 0.0f);

	path.add(path_11);

	Cube path_12;
	path_12.size = vec3(76, 2, 15);
	path_12.color = Red;
	path_12.SetPos(43, 3.6f, 0);
	path_12.SetRotation(-5.65f, vec3{ 0, 0, 1 });
	App->physics->AddBody(path_12, 0.0f);

	path.add(path_12);
}