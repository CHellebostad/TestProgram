#include "stdafx.h"
#include "quaternions.h"
#include <cmath>
#include <iostream>

Quaternion::Quaternion(const Vector& n, float degree) {
	// degree er theta
	// Må konvertere fra grader til radianer
	// 360 grader = 2pi rad
	// 180 grader = pi rad

	degree = (degree / 180)*(float)3.14;
	w = cos(degree / 2);
	v = n*sin(degree / 2);

}

const Quaternion Quaternion::Inverted() const
{
	Quaternion q;
	q.w = w;
	q.v = -v;
	return q;
}

const Quaternion Quaternion::operator*(const Quaternion& q) const
{
	Quaternion r;

	r.w = w*q.w - v.Dot(q.v);
	r.v = v*q.w + q.v*w + v.Cross(q.v);

	return r;
}

const Quaternion Quaternion::operator^(float t) const
{
	// Convert the quaternion back into axis/angle
	float a;
	Vector n;

	ToAxisAngle(n, a);

	// Scale the angle by t
	float at = a*t;

	// Make a new quaternion out of the new value
	return Quaternion(n, at);
}

// Spherical linear interpolation of a quaternion
// http://youtu.be/x1aCcyD0hqE
const Quaternion Quaternion::Slerp(const Quaternion& other, float t) const
{
	const Quaternion& q = *this;
	Quaternion r = other;

	// This is the quaternion equation that I discuss in the video, and it works just fine!
	// return ((r * q.Inverted()) ^ t) * q;

	// But this one is faster. For the derivation, see "3D Primer For Graphics
	// and Game Development" by Dunn and Parberry, section 10.4.13. I may go
	// over this one in a future video, but the two formulas come out the same.
	// (Hint: I'm more likely to do it if I get a request!)
	float flCosOmega = w*r.w + r.v.Dot(v);
	if (flCosOmega < 0)
	{
		// Avoid going the long way around.
		r.w = -r.w;
		r.v = -r.v;
		flCosOmega = -flCosOmega;
	}

	float k0, k1;
	if (flCosOmega > 0.9999f)
	{
		// Very close, use a linear interpolation.
		k0 = 1 - t;
		k1 = t;
	}
	else
	{
		// Trig identity, sin^2 + cos^2 = 1
		float flSinOmega = sqrt(1 - flCosOmega*flCosOmega);

		// Compute the angle omega
		float flOmega = atan2(flSinOmega, flCosOmega);
		float flOneOverSinOmega = 1 / flSinOmega;

		k0 = sin((1 - t)*flOmega) * flOneOverSinOmega;
		k1 = sin(t*flOmega) * flOneOverSinOmega;
	}

	// Interpolate
	Quaternion result;
	result.w = q.w * k0 + r.w * k1;
	result.v = q.v * k0 + r.v * k1;

	return result;
}

void Quaternion::ToAxisAngle(Vector& vecAxis, float& flAngle) const
{
	// Convert the quaternion back into axis/angle
	if (v.LengthSqr() < 0.0001f)
		vecAxis = Vector(1, 0, 0);
	else
		vecAxis = v.Normalized();

	//TAssert(fabs(vecAxis.LengthSqr() - 1) < 0.000001f);

	// This is the opposite procedure as explained in
	// http://youtu.be/SCbpxiCN0U0 w = cos(a/2) and a = acos(w)*2
	flAngle = acos(w) * 2;

	// Convert to degrees
	flAngle *= 180 / ((float)3.14);
}
