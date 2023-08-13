#pragma once

#include <math.h>

class Vector3f
{
public:
	float x;
	float y;
	float z;

	Vector3f();
	Vector3f(float x, float y, float z);
	Vector3f(const Vector3f& other); // ���� ������.
	~Vector3f();

	float Length();								// ���� ����.
	Vector3f Add(const Vector3f& other);		// ���ϱ�.
	Vector3f Substract(const Vector3f& other);	// ����.
	//float Dot(const Vector3f& other);			// ����.
	//float Normalized();						// ���� ����. (����ȭ.)

	Vector3f& operator=(const Vector3f& other);
	Vector3f operator+(const Vector3f& other);
	Vector3f operator-(const Vector3f& other);
};