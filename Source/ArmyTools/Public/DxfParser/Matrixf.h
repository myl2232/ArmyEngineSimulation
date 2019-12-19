#pragma once
#define SET_ROW(row, v1, v2, v3, v4 )    \
    _mat[(row)][0] = (v1); \
    _mat[(row)][1] = (v2); \
    _mat[(row)][2] = (v3); \
    _mat[(row)][3] = (v4);

#define INNER_PRODUCT(a,b,r,c) \
     ((a)._mat[r][0] * (b)._mat[0][c]) \
    +((a)._mat[r][1] * (b)._mat[1][c]) \
    +((a)._mat[r][2] * (b)._mat[2][c]) \
    +((a)._mat[r][3] * (b)._mat[3][c])
class Matrix44
{

public:
	Matrix44() { makeIdentity(); }
	Matrix44(float a00, float a01, float a02, float a03,
		float a10, float a11, float a12, float a13,
		float a20, float a21, float a22, float a23,
		float a30, float a31, float a32, float a33)
	{
		SET_ROW(0, a00, a01, a02, a03)
			SET_ROW(1, a10, a11, a12, a13)
			SET_ROW(2, a20, a21, a22, a23)
			SET_ROW(3, a30, a31, a32, a33)
	}
	inline float operator()(int row, int col) {
		return _mat[row][col];
	}
	inline float operator()(int row, int col)const { return _mat[row][col]; }
	static  Matrix44 translate(float tx, float ty, float tz)
	{
		Matrix44 m;
		m.makeTranslate(tx, ty, tz);
		return m;

	}
	static Matrix44 translate(FVector pos)
	{
		Matrix44 m;
		m.makeTranslate(pos.X, pos.Y, pos.Z);
		return m;
	}
	inline  Matrix44 operator * (const Matrix44& m)const {
		Matrix44 r;
		r.mult(*this, m);
		return r;

	}
	void makeTranslate(float x, float y, float z)
	{
		SET_ROW(0, 1, 0, 0, 0)
		SET_ROW(1, 0, 1, 0, 0)
		SET_ROW(2, 0, 0, 1, 0)
		SET_ROW(3, x, y, z, 1)
	}
	void mult(const Matrix44&lhs, const Matrix44&rhs)
	{
		if (&lhs == this)
		{
			postMult(rhs);
			return;
		}
		if (&rhs == this)
		{
			preMult(lhs);
		}
		_mat[0][0] = INNER_PRODUCT(lhs, rhs, 0, 0);
		_mat[0][1] = INNER_PRODUCT(lhs, rhs, 0, 1);
		_mat[0][2] = INNER_PRODUCT(lhs, rhs, 0, 2);
		_mat[0][3] = INNER_PRODUCT(lhs, rhs, 0, 3);
		_mat[1][0] = INNER_PRODUCT(lhs, rhs, 1, 0);
		_mat[1][1] = INNER_PRODUCT(lhs, rhs, 1, 1);
		_mat[1][2] = INNER_PRODUCT(lhs, rhs, 1, 2);
		_mat[1][3] = INNER_PRODUCT(lhs, rhs, 1, 3);
		_mat[2][0] = INNER_PRODUCT(lhs, rhs, 2, 0);
		_mat[2][1] = INNER_PRODUCT(lhs, rhs, 2, 1);
		_mat[2][2] = INNER_PRODUCT(lhs, rhs, 2, 2);
		_mat[2][3] = INNER_PRODUCT(lhs, rhs, 2, 3);
		_mat[3][0] = INNER_PRODUCT(lhs, rhs, 3, 0);
		_mat[3][1] = INNER_PRODUCT(lhs, rhs, 3, 1);
		_mat[3][2] = INNER_PRODUCT(lhs, rhs, 3, 2);
		_mat[3][3] = INNER_PRODUCT(lhs, rhs, 3, 3);
	}
	inline static Matrix44 rotate(float angle, float x, float y, float z)
	{
		Matrix44 m;
		const float epsilon = 0.0000001;
		float v0, v1, v2, v3;
		float length = sqrt(x*x + y*y + z*z);
		if (length < epsilon)
		{
			v0 = v1 = v2 = 0;
			v3 = 1;
		}

		float inversenorm = 1.0 / length;
		float coshalfangle = cos(0.5*angle);
		float sinhalfangle = sin(0.5*angle);

		v0 = x * sinhalfangle * inversenorm;
		v1 = y * sinhalfangle * inversenorm;
		v2 = z * sinhalfangle * inversenorm;
		v3 = coshalfangle;
		float length2 = v0*v0 + v1*v1 + v2*v2 + v3*v3;
		if (FMath::Abs<float>(length2) <= MIN_flt)
		{
			m.makeIdentity();
			return m;
		}
		else
		{
			float rlength2;
			if (length2 != 1.0)
			{
				rlength2 = 2.0 / length2;
			}
			else
			{
				rlength2 = 2.0;
			}
			double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
			x2 = rlength2*v0;
			y2 = rlength2*v1;
			z2 = rlength2*v2;

			xx = v0 * x2;
			xy = v0 * y2;
			xz = v0 * z2;

			yy = v1 * y2;
			yz = v1 * z2;
			zz = v2 * z2;

			wx = v3 * x2;
			wy = v3 * y2;
			wz = v3 * z2;


			float a00 = 1.0 - (yy + zz);
			float a10 = xy - wz;
			float a20 = xz + wy;


			float a01 = xy + wz;
			float a11 = 1.0 - (xx + zz);
			float a21 = yz - wx;

			float a02 = xz - wy;
			float a12 = yz + wx;
			float a22 = 1.0 - (xx + yy);
			Matrix44	temp(a00, a01, a02, 0,
				a10, a11, a12, 0,
				a20, a21, a22, 0,
				0, 0, 0, 1);
			return temp;
		}

	}
    static Matrix44 scale(float sx, float sy, float sz)
	{
		Matrix44 m;
		m.makeScale(sx, sy, sz);
		return m;
	}
	void makeScale(float x, float y, float z)
	{
		SET_ROW(0, x, 0, 0, 0)
		SET_ROW(1, 0, y, 0, 0)
		SET_ROW(2, 0, 0, z, 0)
		SET_ROW(3, 0, 0, 0, 1)
	}
	void postMult(const Matrix44& other)
	{
		float t[4];
		for (int row = 0; row < 4; ++row)
		{
			t[0] = INNER_PRODUCT(*this, other, row, 0);
			t[1] = INNER_PRODUCT(*this, other, row, 1);
			t[2] = INNER_PRODUCT(*this, other, row, 2);
			t[3] = INNER_PRODUCT(*this, other, row, 3);
			SET_ROW(row, t[0], t[1], t[2], t[3])
		}
	}
	void preMult(const Matrix44& other)
	{
		float t[4];
		for (int col = 0; col < 4; ++col) {
			t[0] = INNER_PRODUCT(other, *this, 0, col);
			t[1] = INNER_PRODUCT(other, *this, 1, col);
			t[2] = INNER_PRODUCT(other, *this, 2, col);
			t[3] = INNER_PRODUCT(other, *this, 3, col);
			_mat[0][col] = t[0];
			_mat[1][col] = t[1];
			_mat[2][col] = t[2];
			_mat[3][col] = t[3];
		}
	}
	void makeIdentity()
	{
		SET_ROW(0, 1, 0, 0, 0)
			SET_ROW(1, 0, 1, 0, 0)
			SET_ROW(2, 0, 0, 1, 0)
			SET_ROW(3, 0, 0, 0, 1)
	}
protected:
	float _mat[4][4];
};