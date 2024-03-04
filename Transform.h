#pragma once

#include "Quaternion.h"
#include "Debug.h"
class Transform
{
public:
	Transform() {};
	~Transform() {};

	// Setters and Getters for position/rotation/scale
	void SetPosition(Vector3 position) { _position = position;}

	void SetPosition(float x, float y, float z) { _position.x = x; _position.y = y; _position.z = z; }

	Vector3 GetPosition() const { return _position; }

	void SetScale(Vector3 scale) { _scale = scale; }
	void SetScale(float x, float y, float z) { _scale.x = x; _scale.y = y; _scale.z = z; }

	Vector3 GetScale() const { return _scale; }

	//Eular setter and getter
	void SetRotation(Vector3 rotation) { _orientataion = MakeQFromEulerAngles(rotation.x, rotation.y, rotation.z); }
	void SetRotation(float x, float y, float z) { _orientataion = MakeQFromEulerAngles(x, y, z); }
	Vector3 GetRotation() const { return MakeEulerAnglesFromQ(_orientataion); }

	//Quaternion setter and getter
	void SetOrientation(Quaternion orientation) { _orientataion = orientation; }
	Quaternion GetOrientation() { return _orientataion; }
protected:

private:
	Vector3 _position{ Vector3()};
	Quaternion _orientataion{};
	Vector3 _scale{};
};

