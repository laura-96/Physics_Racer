#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void CreateScenary();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	Cube s;
	Cube p1;

	Cylinder curve;

	Cylinder coin1;
	Cylinder coin2;
	Cylinder coin3;

	p2List<Cube> path;

	PhysBody3D* sensor;
	PhysBody3D* fall_car;
	PhysBody3D* coin1_sensor;
	PhysBody3D* coin2_sensor;
	PhysBody3D* coin3_sensor;

	int coins_to_collect;
	bool coins_achieved;

	uint coin_fx;
	uint victory_fx;
	uint defeat_fx;
	uint music;

};
