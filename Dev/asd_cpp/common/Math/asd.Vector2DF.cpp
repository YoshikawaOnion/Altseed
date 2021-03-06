﻿//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "asd.Vector2DF.h"
#include "asd.Vector2DI.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd
{
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF::Vector2DF()
		: X(0.0f)
		, Y(0.0f)
	{

	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF::Vector2DF(float x, float y)
		: X(x)
		, Y(y)
	{

	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Vector2DF::operator==(const Vector2DF& right)
	{
		return X == right.X && Y == right.Y;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Vector2DF::operator!=(const Vector2DF& right)
	{
		return X != right.X || Y != right.Y;
	}

	bool Vector2DF::operator > (const Vector2DF& o) const
	{
		if (X != o.X) return X > o.X;
		if (Y != o.Y) return Y > o.Y;
		return false;
	}

	bool Vector2DF::operator < (const Vector2DF& o) const
	{
		if (X != o.X) return X < o.X;
		if (Y != o.Y) return Y < o.Y;
		return false;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF Vector2DF::operator-()
	{
		return Vector2DF(-X, -Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF Vector2DF::operator+(const Vector2DF& right) const
	{
		return Vector2DF(X + right.X, Y + right.Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF Vector2DF::operator-(const Vector2DF& right) const
	{
		return Vector2DF(X - right.X, Y - right.Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF Vector2DF::operator*(const Vector2DF& right) const
	{
		return Vector2DF(X * right.X, Y * right.Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF Vector2DF::operator/(const Vector2DF& right) const
	{
		return Vector2DF(X / right.X, Y / right.Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF Vector2DF::operator*(float right) const
	{
		return Vector2DF(X * right, Y * right);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF Vector2DF::operator/(float right) const
	{
		return Vector2DF(X / right, Y / right);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF& Vector2DF::operator+=(const Vector2DF& right)
	{
		X += right.X;
		Y += right.Y;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF& Vector2DF::operator-=(const Vector2DF& right)
	{
		X -= right.X;
		Y -= right.Y;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF& Vector2DF::operator*=(const Vector2DF& right)
	{
		X *= right.X;
		Y *= right.Y;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF& Vector2DF::operator/=(const Vector2DF& right)
	{
		X /= right.X;
		Y /= right.Y;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF& Vector2DF::operator*=(float right)
	{
		X *= right;
		Y *= right;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DF& Vector2DF::operator/=(float right)
	{
		X /= right;
		Y /= right;
		return *this;
	}


	Vector2DI Vector2DF::To2DI() const
	{
		return Vector2DI((int32_t) X, (int32_t) Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------