#include <algorithm>

#include "Collider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "Debug.h"

//Sphere and Box Closest point of contact
inline Vector3 ClosestPointOnBox(Vector3& SpherePos, BoxCollider& box)
{
	Vector3 closestPoint;
	
	//Get the closest point on the x
	closestPoint.x = std::clamp(SpherePos.x, box.GetPosition().x - box.GetHalfExtend().x, box.GetPosition().x + box.GetHalfExtend().x);
	//Get the closest point on the y
	closestPoint.y = std::clamp(SpherePos.y, box.GetPosition().y - box.GetHalfExtend().y, box.GetPosition().y + box.GetHalfExtend().y);
	//Get the closest point on the z
	closestPoint.z = std::clamp(SpherePos.z, box.GetPosition().z - box.GetHalfExtend().z, box.GetPosition().z + box.GetHalfExtend().z);

	return closestPoint;
}

//Sphere Collision with the box collider check
inline bool AABBSphereOverlap(BoxCollider& box, SphereCollider& sphere)
{
	Vector3 pt = ClosestPointOnBox(sphere.GetPosition(), box);

	return (sphere.GetPosition() - pt).Magnitude() < sphere.GetRadius();
}

//Box collider and Plane collider contact point
inline Vector3 PlaneBoxCollide(Vector3& a, Vector3& b)
{
	Vector3 normalB = b;
	normalB.Normalize();

	float distance = (normalB * a) + b.Magnitude();
	Vector3 closestPoint = a - distance * normalB / (normalB * normalB);

	return closestPoint;
}


//Sphere Collision with the plane collider check
inline Vector3 PlaneSphereCollide(SphereCollider& sphere, PlaneCollider& plane)
{
	Vector3 normal = plane.GetNormal();
	normal.Normalize();
	float distance = (sphere.GetPosition() - plane.GetPosition()) * normal;

	if (distance <= sphere.GetRadius())
	{
		Vector3 contactPoint = sphere.GetPosition() - distance * normal;
		return contactPoint;
	}

	return Vector3();
}
