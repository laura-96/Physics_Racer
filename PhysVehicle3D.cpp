#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet\src\btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder wheel;

	wheel.color = Blue;

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[0].radius;
		wheel.height = info.wheels[0].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.Render();
	}


	// LLUM  ESQUERRE
	Sphere light;
	light.radius = 0.3f;
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&light.transform);
	btQuaternion qLight = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset_light(info.light_offset.x, info.light_offset.y, info.light_offset.z);
	offset_light = offset_light.rotate(qLight.getAxis(), qLight.getAngle());

	light.transform.M[12] += offset_light.getX();
	light.transform.M[13] += offset_light.getY();
	light.transform.M[14] += offset_light.getZ();


	//CHASSIS COLOR 
	light.color = Green;

	light.Render();

	// LLUM  DRET
	Sphere light2;
	light2.radius = 0.3f;
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&light2.transform);
	btQuaternion qLight2 = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset_light2(info.light_offset2.x, info.light_offset2.y, info.light_offset2.z);
	offset_light2 = offset_light2.rotate(qLight2.getAxis(), qLight2.getAngle());

	light2.transform.M[12] += offset_light2.getX();
	light2.transform.M[13] += offset_light2.getY();
	light2.transform.M[14] += offset_light2.getZ();

	//CHASSIS COLOR 
	light2.color = Green;

	light2.Render();


	//CHASSIS ROJO
	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();

	//CHASSIS COLOR 
	chassis.color = Red;

	chassis.Render();


	// CHASSIS VERDE
	Cube chassis2(info.chassis2_size.x, info.chassis2_size.y, info.chassis2_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis2.transform);
	btQuaternion q2 = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset2(info.chassis2_offset.x, info.chassis2_offset.y, info.chassis2_offset.z);
	offset2 = offset2.rotate(q2.getAxis(), q2.getAngle());

	chassis2.transform.M[12] += offset2.getX();
	chassis2.transform.M[13] += offset2.getY();
	chassis2.transform.M[14] += offset2.getZ();


	//CHASSIS COLOR 
	chassis2.color = Green;

	chassis2.Render();

	// GUARDABARRO
	Cube fender(info.fender_size.x + 2.5f, info.fender_size.y + 0.5f, info.fender_size.z + 0.5f);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&fender.transform);
	btQuaternion q3 = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset3(info.fender_offset.x, info.fender_offset.y, info.fender_offset.z);
	offset3 = offset3.rotate(q3.getAxis(), q3.getAngle());

	fender.transform.M[12] += offset3.getX();
	fender.transform.M[13] += offset3.getY();
	fender.transform.M[14] += offset3.getZ();


	//CHASSIS COLOR 
	fender.color = White;

	fender.Render();

	// GUARDABARRO TRASERO
	Cube fender2(info.fender2_size.x + 2.5f, info.fender2_size.y + 0.5f, info.fender2_size.z + 0.5f);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&fender2.transform);
	btQuaternion q4 = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset4(info.fender2_offset.x, info.fender2_offset.y, info.fender2_offset.z);
	offset4 = offset4.rotate(q4.getAxis(), q4.getAngle());

	fender2.transform.M[12] += offset4.getX();
	fender2.transform.M[13] += offset4.getY();
	fender2.transform.M[14] += offset4.getZ();


	//CHASSIS COLOR 
	fender2.color = White;

	fender2.Render();

	//TECHO
	Cube roof(info.roof_size.x + 1.6, info.roof_size.y + 0.2, info.roof_size.z + 2.2);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&roof.transform);
	btQuaternion qR = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offsetR(info.roof_offset.x, info.roof_offset.y, info.roof_offset.z);
	offsetR = offsetR.rotate(qR.getAxis(), qR.getAngle());

	roof.transform.M[12] += offsetR.getX();
	roof.transform.M[13] += offsetR.getY();
	roof.transform.M[14] += offsetR.getZ();


	//CHASSIS COLOR 
	roof.color = Red;
	
	roof.Render();


	chassis.Render();



}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}
