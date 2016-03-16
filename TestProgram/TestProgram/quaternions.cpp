#include "stdafx.h"
#include <E:\Repos\TestProgram\TestProgram\TestProgram\quaternions.h>
#include <cmath>
#include <iostream>

Quaternion::Quaternion(const Vector& n, float a) {
	// a er theta
	// Må konvertere fra grader til radianer
	// 360 grader = 2pi rad

	a = (a / 180)*(float)3.14;
	w = cos(a / 2);
	v = n*sin(a / 2);

}