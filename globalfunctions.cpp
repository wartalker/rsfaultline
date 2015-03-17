#include <cmath>
#include <math.h>
#include "rshead.h"

using namespace std;

/******************************************************
*The nearest Distance Point to Line
*/
double PointLineDistanceSquared3D( const RSTdPoint& q, const RSSegment& s,double& t )
{
	RSTdPoint l = s.endPoint - s.startPoint;
	t = Dot( l, q - l ) / Dot( l,l );

	RSTdPoint qPrime = s.startPoint + l * t;
	RSTdPoint vec = q - qPrime;

	return Dot( vec,vec );
}


/******************************************************
*The nearest Distance Point to Segment
*/
double PointLineSegDistanceSquared3D( const RSTdPoint& q, const RSSegment& s )
{
	double t = 0;
	double distanceSquared =  PointLineDistanceSquared3D( q,s, t);

	if ( t < 0 )
	{
		RSTdPoint vec = q - s.startPoint;
		return Dot( vec, vec );
	}
	else if ( t > 1 )
	{
		RSTdPoint vec = q - s.endPoint;
		return Dot( vec, vec );
	}

	return distanceSquared;
}



/******************************************************
*Intersection of the same Segment
*/
int FindIntersection( float u0, float u1, float v0, float v1, float w[2] )
{
	if ( ( u1 < v0 ) || ( u0 > v1 ) )
	{
		return 0;
	}

	if ( u1 > v0 )
	{
		if ( u0 < v1 )
		{
			if ( u0 < v0 )
			{
				w[0] = v0;
			}
			else 
			{
				w[0] = u0;
			}
			if ( u1 > v1 )
			{
				w[1] = v1;
			}
			else
			{
				w[1] = u1;
			}
			return 2; 
		}
		else
		{
			w[0] = u0;
			return 1;
		}
	}
	else
	{
		w[0] = u1;
		return 1;
	}
}




/************************************************************
*Intersection of Segments
*/
int FindIntersection( RSSegment& S1, RSSegment& S2, RSTdPoint I[2] )
{
	double kross = 0;
	double sqrKross = 0;
	double sqrLen0 = 0;
	double sqrLen1 = 0;
	double s = 0;
	double t = 0;

	RSTdPoint P0 = S1.startPoint;
	RSTdPoint D0 = S1.endPoint - S1.startPoint;
	RSTdPoint P1 = S2.startPoint;
	RSTdPoint D1 = S2.endPoint - S2.startPoint;

	//if ( abs( D1.x ) + abs( D1.y ) < G_EPSILON )
	//	return 0;

	RSTdPoint E = P1 - P0;
	kross = D0.x * D1.y - D0.y * D1.x;
	sqrKross = kross * kross;
	sqrLen0 = D0.x * D0.x + D0.y * D0.y;
	sqrLen1 = D1.x * D1.x + D1.y * D1.y;

	if ( sqrKross > G_SQREPSILON * sqrLen0 * sqrLen1 )
	{
		s = ( E.x * D1.y - E.y * D1.x ) / kross;
		if ( ( s < 0 ) || ( s > 1 ) )
			return 0;

	    t = ( E.x * D0.y - E.y * D0.x ) / kross;
		if ( (t < 0) || (t > 1) )
			return 0;

		I[0] = P0 + D0 * s;
		return 1;
	}

	//lines of the segments are parallel
	double sqrLenE = E.x * E.x + E.y * E.y;
	kross = E.x * D0.y - E.y * D0.x;
	sqrKross = kross * kross;
	if ( sqrKross > G_SQREPSILON * sqrLen0 * sqrLenE )
		return 0;

	//lines of the segments are the same
	double s0 = ( D0.x * E.x + D0.y * E.y ) / sqrLen0;
	double s1 = s0 + ( D0.x * D1.x + D0.y * D1.y ) / sqrLen0;
	float w[2];
	float smin = static_cast<float>(min( s0,s1 ));
	float smax = static_cast<float>(max( s0,s1 ));
	int imax = FindIntersection( 0.0, 1.0, smin, smax, w );
	int i;

	for ( i = 0; i < imax; ++i )
		I[i] = P0 + D0 * w[i];

	return imax;
}
