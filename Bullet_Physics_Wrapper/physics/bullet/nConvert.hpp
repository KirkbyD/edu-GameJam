#pragma once
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "cMesh.hpp"

namespace nConvert {
	// glm::vec3 -> btVector3
	inline btVector3 ToBullet(const glm::vec3& vec) {
		return btVector3(vec.x, vec.y, vec.z);
	}
	inline glm::vec3 ToSimple(const btVector3 vec) {
		return glm::vec3(vec.x(), vec.y(), vec.z());
	}

	inline btQuaternion ToBullet(const glm::quat quat) {
		btScalar x = quat.x;
		btScalar y = quat.y;
		btScalar z = quat.z;
		btScalar w = quat.w;
		return btQuaternion(x, y, z, w);
	}

	inline void ToSimple(const btTransform& transformIn, glm::mat4& transformOut) {
		transformIn.getOpenGLMatrix(&transformOut[0][0]);
	}
	inline void ToBullet(const glm::mat4& transformIn, btTransform& transformOut) {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transformIn, scale, rotation, translation, skew, perspective);

		transformOut = btTransform(ToBullet(rotation), ToBullet(translation));
	}

	// Laurent Couvidou
	// at https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
	inline void rotate_vector_by_quaternion(const glm::vec3& v, const glm::quat& q, glm::vec3& vprime)	{
		// Extract the vector part of the quaternion
		glm::vec3 u(q.x, q.y, q.z);

		// Extract the scalar part of the quaternion
		float s = q.w;

		// Do the math
		vprime = 2.0f * dot(u, v) * u
			+ (s * s - dot(u, u)) * v
			+ 2.0f * s * cross(u, v);
	}

	inline void ToBullet(const cMesh* meshIn, const glm::vec3 scale, const glm::vec3 pos, const glm::quat rot, btTriangleMesh* meshOut) {
		for (auto tri : meshIn->vecTriangles) {
			//Get points
			glm::vec3 v1 = glm::vec3(meshIn->vecVertices[tri.vert_index_1].x,
									meshIn->vecVertices[tri.vert_index_1].y,
									meshIn->vecVertices[tri.vert_index_1].z);
			glm::vec3 v2 = glm::vec3(meshIn->vecVertices[tri.vert_index_2].x,
									meshIn->vecVertices[tri.vert_index_2].y,
									meshIn->vecVertices[tri.vert_index_2].z);
			glm::vec3 v3 = glm::vec3(meshIn->vecVertices[tri.vert_index_3].x,
									meshIn->vecVertices[tri.vert_index_3].y,
									meshIn->vecVertices[tri.vert_index_3].z);

			//Rotate points around origin via quaternion
			rotate_vector_by_quaternion(v1, rot, v1);
			rotate_vector_by_quaternion(v2, rot, v2);
			rotate_vector_by_quaternion(v3, rot, v3);

			//Unsure why these don't work exactly - templating/typing issue
			//v1 = glm::rotate(rot, v1);
			//v2 = glm::rotate(rot, v2);
			//v3 = glm::rotate(rot, v3);

			// Convert to bullet and transpose points.
			btVector3 btV1 = btVector3(v1.x * scale.x + pos.x,
				v1.y * scale.y + pos.y,
				v1.z * scale.z + pos.z);
			btVector3 btV2 = btVector3(v2.x * scale.x + pos.x,
				v2.y * scale.y + pos.y,
				v2.z * scale.z + pos.z);
			btVector3 btV3 = btVector3( v3.x * scale.x + pos.x,
				v3.y * scale.y + pos.y,
				v3.z * scale.z + pos.z);

			meshOut->addTriangle(btV1, btV2, btV3, false);
		}
	}
}