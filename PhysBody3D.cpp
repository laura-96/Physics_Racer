#include "PhysBody3D.h"
#include "glmath.h"
#include "Bullet\src\btBulletDynamicsCommon.h"

// =================================================
PhysBody3D::PhysBody3D(btRigidBody* body) : body(body)
{
	sens_type = NOT_SENSOR;
	body->setUserPointer(this);
}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{
	delete body;
}

// ---------------------------------------------------------
void PhysBody3D::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// ---------------------------------------------------------
void PhysBody3D::GetTransform(float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}

const vec3 PhysBody3D::GetPos() const
{
	vec3 ret;
	btTransform t = body->getWorldTransform();
	btVector3 v = t.getOrigin();
	ret.Set(v.getX(), v.getY(), v.getZ());
	return ret;
}

// ---------------------------------------------------------
void PhysBody3D::SetAsSensor(bool is_sensor, sensor_type s_type)
{
	sens_type = s_type;
	if(this->is_sensor != is_sensor)
	{
		this->is_sensor = is_sensor;
		if(is_sensor == true)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		else
			body->setCollisionFlags(body->getCollisionFlags() &~ btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}
sensor_type PhysBody3D::GetSensorType() const{
	return sens_type;
}

// ---------------------------------------------------------
bool PhysBody3D::IsSensor() const
{
	return is_sensor;
}
