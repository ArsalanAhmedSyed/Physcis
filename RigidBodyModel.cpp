#include "RigidBodyModel.h"
#include "Debug.h"
RigidBodyModel::RigidBodyModel(Transform* transform) : PhysicsModel(transform)
{
	XMStoreFloat3x3(&_inertiaTensor, XMMatrixIdentity());

	//Calculate the Intertia Tensor of the object according to its mass and radius
	//2 / 5 * mass * radius (square)
	_inertiaTensor._11 = 2.5f / 5.0f * 1.f * (3.0f * 3.0f);
	_inertiaTensor._22 = 2.5f / 5.0f * 1.f * (3.0f * 3.0f);
	_inertiaTensor._33 = 2.5f / 5.0f * 1.f * (3.0f * 3.0f);
}

void RigidBodyModel::Update(float deltaTime)
{
	CalculateAngularVelocity(deltaTime);
	PhysicsModel::Update(deltaTime);
}

void RigidBodyModel::AddRelativeForce(Vector3 force, Vector3 point)
{
	//Direction the force needs to be applied
	 _torque = CalculateCrossProduct(force, point);
}

const Vector3 RigidBodyModel::CalculateCrossProduct(Vector3 a, Vector3 b)
{
	Vector3 crossProduct;
	crossProduct.x = (a.y * b.z) - (a.z * b.y);
	crossProduct.y = (a.z * b.x) - (a.x * b.z);
	crossProduct.z = (a.x * b.y) - (a.y * b.x);

	return crossProduct;
}

void RigidBodyModel::CalculateAngularVelocity(float deltaTime)
{
	if (_mass == 0) { return; }

	//Store IntertiaTensor in to XMMatrix
	XMMATRIX intertiaTensorMatrix = XMLoadFloat3x3(&_inertiaTensor);
	//Invert the IntertiaTensor XMMatrix
	XMMATRIX inverseIntertiaTensor = XMMatrixInverse(nullptr, intertiaTensorMatrix);
	//Store the Vector3 Toruqe in to XMVECTOR
	XMVECTOR torquetoXMVector = XMVectorSet(_torque.x, _torque.y, _torque.z, 1);
	//Calculate the Angular velocity
	XMVECTOR angualrAccel = XMVector3Transform(torquetoXMVector, inverseIntertiaTensor);

	//Pass Angualr Acceleration to the velocity
	_angularVelocity.x +=  XMVectorGetX(angualrAccel) * deltaTime;
	_angularVelocity.y +=  XMVectorGetY(angualrAccel) * deltaTime;
	_angularVelocity.z +=  XMVectorGetZ(angualrAccel) * deltaTime;

	//Get Orientation as reference
	Quaternion orientation = _transform->GetOrientation();
	//Calculate new orientation after the angular velocity is applied
	//orientation += orientation * _angularVelocity * 0.5 * deltaTime;
	orientation += orientation + deltaTime * _angularVelocity * orientation * 0.5f;

	if (orientation.Magnitude() != 0)
	{
		//Normalise the orientation
		//To avoid drifting above or below 1 over time
		orientation = orientation / orientation.Magnitude(); 
	}

	//Pass the value back to the objects transform
	_transform->SetOrientation(orientation);

	//Damp the angular velocity to reduce the angular velocity;
	_angularVelocity *= powf(_angularDamping, deltaTime);

	_torque = Vector3();
}
