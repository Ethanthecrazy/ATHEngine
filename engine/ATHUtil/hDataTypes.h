#ifndef HDATATYPES_H
#define HDATATYPES_H

#define IF(x) if(x)(x)

#include <Windows.h>
#include <math.h>

class float3;
class float2
{
public:

	union
	{
		float Array[2];

		struct
		{
			float vX;
			float vY;
		};
	};

	float2(){}
	float2( float _x, float _y )
	{
		vX = _x;
		vY = _y;
	}

	float2& float2::operator=(const float &rhs)	
	{
		vX = rhs;
		vY = rhs;
		return *this;
	}

	float2 float2::operator+(const float &rhs) 
	{
		float2 toReturn;
		toReturn.vX = vX + rhs;
		toReturn.vY = vY + rhs;
		return toReturn;
	}

	float2 float2::operator+(const float2 &rhs) 
	{
		float2 toReturn;
		toReturn.vX = vX + rhs.vX;
		toReturn.vY = vY + rhs.vY;
		return toReturn;
	}

	float2 float2::operator-(const float2 &rhs) 
	{
		float2 toReturn;
		toReturn.vX = vX - rhs.vX;
		toReturn.vY = vY - rhs.vY;
		return toReturn;
	}

	float2 float2::operator*(const float &rhs) 
	{
		return float2( vX * rhs, vY * rhs );
	}

	float2 float2::operator*(const float2 &rhs) 
	{
		return float2( vX * rhs.vX, vY * rhs.vY );
	}

	float2 float2::operator/(const float &rhs) 
	{
		return float2( vX / rhs, vY / rhs );
	}

	float2 float2::operator/(const float2 &rhs) 
	{
		return float2( vX / rhs.vX, vY / rhs.vY );
	}

	float2 & float2::operator+=(const float2 &rhs) 
	{
		vX += rhs.vX;
		vY += rhs.vY;
		return *this;
	}

	float2 & float2::operator-=(const float2 &rhs) 
	{
		vX -= rhs.vX;
		vY -= rhs.vY;
		return *this;
	}

	float2 & float2::operator*=(const float2 &rhs) 
	{
		vX *= rhs.vX;
		vY *= rhs.vY;
		return *this;
	}

	float2 & float2::operator/=(const float &rhs) 
	{
		vX /= rhs;
		vY /= rhs;
		return *this;
	}

	inline float magnitude() const
	{
		return ( sqrt( vX * vX + vY * vY ) ); 
	}
};

class float3
{
public:

	union
	{
		float Array[3];

		struct
		{
			float vX;
			float vY;
			float vZ;
		};

		struct
		{
			float cR;
			float cG;
			float cB;
		};

		struct
		{
			float2 xy;
		};
	};

	float3() : vX(0.0f), vY(0.0f), vZ(0.0f) {}

	float3( float _val )
	{
		vX = _val;
		vY = _val;
		vZ = _val;
	}

	float3( float _x, float _y, float _z )
	{
		vX = _x;
		vY = _y;
		vZ = _z;
	}

	float3( float2 _xy, float _z )
	{
		vX = _xy.vX;
		vY = _xy.vY;
		vZ = _z;
	}

	float3( float _x, float2 _yz )
	{
		vX = _x;
		vY = _yz.vX;
		vZ = _yz.vY;
	}

	float3 & float3::operator=(const float &rhs) 
	{
		vX = rhs;
		vY = rhs;
		vZ = rhs;
		return *this;
	}

	float3 & float3::operator=(const float2 &rhs) 
	{
		vX = rhs.vX;
		vY = rhs.vY;
		return *this;
	}

	float3 float3::operator+(const float3 &rhs) 
	{
		float3 toReturn;
		toReturn.vX = vX + rhs.vX;
		toReturn.vY = vY + rhs.vY;
		toReturn.vZ = vZ + rhs.vZ;
		return toReturn;
	}

	float3 float3::operator+(const float2 &rhs) 
	{
		float3 toReturn;
		toReturn.vX = vX + rhs.vX;
		toReturn.vY = vY + rhs.vY;
		toReturn.vZ = vZ;
		return toReturn;
	}

	float3 float3::operator-(const float3 &rhs) 
	{
		float3 toReturn;
		toReturn.vX = vX - rhs.vX;
		toReturn.vY = vY - rhs.vY;
		toReturn.vZ = vZ - rhs.vZ;
		return toReturn;
	}

	float3 & float3::operator+=(const float3 &rhs) 
	{
		vX += rhs.vX;
		vY += rhs.vY;
		vZ += rhs.vZ;
		return *this;
	}

	float3 & float3::operator+=(const float2 &rhs) 
	{
		vX += rhs.vX;
		vY += rhs.vY;
		return *this;
	}

	float3 & float3::operator-=(const float3 &rhs) 
	{
		vX -= rhs.vX;
		vY -= rhs.vY;
		vZ -= rhs.vZ;
		return *this;
	}

	float3 float3::operator*(const float &rhs) 
	{
		float3 toReturn;
		toReturn.vX = vX * rhs;
		toReturn.vY = vY * rhs;
		toReturn.vZ = vZ * rhs;
		return toReturn;
	}

	float3 float3::operator/(const float &rhs) 
	{
		float3 toReturn;
		toReturn.vX = vX / rhs;
		toReturn.vY = vY / rhs;
		toReturn.vZ = vZ / rhs;
		return toReturn;
	}

	float3 & float3::operator*=(const float3 &rhs) 
	{
		vX *= rhs.vX;
		vY *= rhs.vY;
		vZ *= rhs.vZ;
		return *this;
	}

	float3 & float3::operator*=(const float &rhs) 
	{
		vX *= rhs;
		vY *= rhs;
		vZ *= rhs;
		return *this;
	}

	float3 & float3::operator/=(const float &rhs) 
	{
		vX /= rhs;
		vY /= rhs;
		vZ /= rhs;
		return *this;
	}

	inline float dotProduct(const float3 &v) const
	{
		return vX * v.vX + vY * v.vY + vZ * v.vZ;
	}

	inline friend float DotProduct(const float3 &a, const float3 &b)
	{
		return a.dotProduct(b);
	}

	inline float3 float3::Cross( const float3 &b )
	{
		float3 out;
		out.vX = vY * b.vZ - vZ * b.vY;
		out.vY = vZ * b.vX - vX * b.vZ;
		out.vZ = vX * b.vY - vY * b.vX;
		return out;
	}

	inline float magnitude() const
	{
		return (float)sqrt(dotProduct(*this));
	}

	inline float3 &normalize()
	{
		float invMag = 1 / magnitude();
		*this *= invMag;
		return *this;
	}

	float& operator[] (unsigned i)
	{
		return Array[i];
	}
};

class float4
{
public:

	union
	{
		float Array[4];

		struct
		{
			float vX;
			float vY;
			float vZ;
			float vW;
		};

		struct 
		{
			float rLeft;
			float rTop;
			float rBottom;
			float rRight;

		};

		struct
		{
			float cR;
			float cG;
			float cB;
			float cA;
		};

		struct
		{
			float3 xyz;
			float w;
		};

		struct
		{
			float2 xy;
			float z;
			float w;
		};
	};

	float4()
	{
		Array[0] = 0.0f;
		Array[1] = 0.0f;
		Array[2] = 0.0f;
		Array[3] = 0.0f;
	}

	float4( float val ) 
	{
		Array[0] = val;
		Array[1] = val;
		Array[2] = val;
		Array[3] = val;
	}

	float4( float _x, float _y, float _z, float _w )
	{
		Array[0] = _x;
		Array[1] = _y;
		Array[2] = _z;
		Array[3] = _w;
	}

	float4( float3 _xyz, float _w )
	{
		Array[0] = _xyz.vX;
		Array[1] = _xyz.vY;
		Array[2] = _xyz.vZ;
		Array[3] = _w;
	}

	bool toBool()
	{
		if( Array[0] || Array[1] || Array[2] || Array[3] )
			return true;
		return false;
	}

	RECT toRECT()
	{
		RECT returnValue;
		returnValue.left = long( rLeft );
		returnValue.top = long( rTop );
		returnValue.right = long( rRight );
		returnValue.bottom = long( rBottom );	

		return returnValue;
	}

	float4 & float4::operator=(const float &rhs) 
	{
		vX = rhs;
		vY = rhs;
		vZ = rhs;
		vW = rhs;

		return *this;
	}
};

class float22
{
public:
	union
	{
		float Array[4];

		struct
		{
			float2 axis_x;
			float2 axis_y;
		};

		struct
		{
			float xx;
			float xy;
			float yx;
			float yy;
		};
	};
};

class float33
{
public:

	union
	{
		float Array[9];

		struct
		{
			float3 axis_x;
			float3 axis_y;
			float3 axis_z;
		};

		struct
		{
			float xx;
			float xy;
			float xz;
			float yx;
			float yy;
			float yz;
			float zx;
			float zy;
			float zz;
		};
	};

	float3& operator[] (unsigned i)
	{
		switch( i )
		{
		default:
			return axis_x;
		case 1:
			return axis_y;
		case 2:
			return axis_z;

		}
	}
};

#endif