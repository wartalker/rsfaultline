#ifndef RSHEAD
#define RSHEAD

#include <cmath>
#include <math.h>
#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

const double G_EPSILON = 0.0000001;
const double G_SQREPSILON = 0.00000000000001;
const double G_PI = 3.1415926;


/***********************************************************************************************************************************
*Point Type
*/
enum RSPoint_Type { POINT_NOT_EXTREME = 0, POINT_USER_DEFINE, POINT_AUTO_GENERATE };

//POINT_NOT_EXTREME: Point of Intersection is not The RSLineNode;
//POINT_USER_DEFINE: Point of Intersection is RSLineNode[ USER DEFINE ]
//POINT_AUTO_GENERATE:Point of Intersection is RSLineNode[ AUTO GENERATE]



/***********************************************************************************************************************************
* Three Dimensional Point struct
*/
struct RSTdPoint
{
	double x;
	double y;
	double z;

	RSTdPoint( ) :x(0),y(0),z(0){  }
	RSTdPoint( double _x , double _y, double _z ):x(_x),y(_y),z(_z){ }
	RSTdPoint( const RSTdPoint& tdp ):x(tdp.x),y(tdp.y),z(tdp.z){ }


	RSTdPoint friend operator - (const RSTdPoint& a , const RSTdPoint& b)
	{
		return RSTdPoint( a.x-b.x, a.y-b.y, a.z-b.z);
	}

	RSTdPoint friend operator + (const RSTdPoint& a, const RSTdPoint& b)
	{
		return RSTdPoint(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	RSTdPoint friend operator * (const RSTdPoint& a , const RSTdPoint& b)
	{
		return RSTdPoint(a.y*b.z - a.z*b.y , a.z*b.x - a.x*b.z , a.x*b.y - a.y*b.x);
	}

	RSTdPoint friend operator * ( const RSTdPoint& a, const double f )
	{
		return RSTdPoint( a.x * f, a.y * f, a.z * f );
	}

	RSTdPoint friend operator * ( const double f, const RSTdPoint& a )
	{
		return RSTdPoint( a.x * f, a.y * f, a.z * f ); 
	}

	RSTdPoint& operator = (const RSTdPoint& p)
	{
		x = p.x;
		y = p.y;
		z = p.z;

		return *this;
	}

	bool friend operator == (const RSTdPoint& p, const RSTdPoint& q )
	{
		return ( ( abs( q.x - p.x ) + abs( q.y - p.y ) + abs( q.z - p.z ) ) < G_EPSILON );
	}

	bool friend operator != ( const RSTdPoint& p, const RSTdPoint& q )
	{
		return ( ( abs( q.x - p.x ) + abs( q.y - p.y ) + abs( q.z - p.z ) ) > G_EPSILON );
	}


	double friend Dot( const RSTdPoint& a, const RSTdPoint& b )
	{
		return ( a.x * b.x + a.y * b.y + a.z * b.z );
	}

	double friend Distance( const RSTdPoint& a, const RSTdPoint& b )
	{
		return sqrt(pow(a.x-b.x , 2) + pow(a.y-b.y , 2) + pow(a.z-b.z , 2));
	}

	void setPoint(double x1 , double y1 , double z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}

	double getX( ) { return x; }
	double getY( ) { return y; }
	double getZ( ) { return z; }

}; 


/***********************************************************************************************************************************
*Point of Intersection struct
*/
struct RSInterNode
{
	RSTdPoint p;//Coordinate 
	unsigned int lineID;//ID of Line
	unsigned int nodeIndex;//RSLineNode index
	RSPoint_Type type;//Type of Point
	bool isContact;//The Connection with next Point
	bool isSeg;//Is the Extreme Point of Segment

	unsigned int VFaceNodeIndex;//VerticalFaceNode index
	mutable bool isFlip;//Direction of Line 
	bool isInter; //Is the Point of Intersection


	RSInterNode( ):p( ),lineID(0),nodeIndex(0),type( POINT_USER_DEFINE ),isContact(false),isSeg(false), VFaceNodeIndex(0),isFlip(false),isInter(false){ } 

	bool friend operator == ( const RSInterNode& a, const RSInterNode& b )
	{
		if ( a.nodeIndex != b.nodeIndex ) return false;
		if ( a.p != b.p ) return false;
		return true;
	};

	bool isSegmentNode( ) { return isSeg; }
	bool isContatcNode( ) { return isContact; }
	bool isInterNode( ) { return isInter; }
	RSPoint_Type Type( ) { return type; }
	unsigned int LineID( ) { return lineID; }
	unsigned int NodeIndex( ) { return nodeIndex; }
	RSTdPoint& getPoint( ) { return p; }
	double getX( ){ return p.x; }
	double getY( ){ return p.y; }
	double getZ( ){ return p.z; }
};





/***********************************************************************************************************************************
*Points of Intersection struct
*/

//Predicate POINT_AUTO_GENERATE
struct RSInterNode_AutoGenerateInter : public unary_function< RSInterNode, bool >
{
	bool operator ( ) ( const RSInterNode& node ) { return ( node.type == POINT_AUTO_GENERATE ); }
};


//Point of Intersection struct
class RSInterNodes
{
private:
	vector< RSInterNode > m_interNodes;
	vector< RSInterNode >::iterator find( vector<RSInterNode>::iterator current );
	bool isContact( vector<RSInterNode>::iterator current );


public:
	RSInterNodes( ){  }
	~RSInterNodes( ) {  }

	int size( ) { return m_interNodes.size( ); }
	RSInterNode& operator[ ]( int n ) { return m_interNodes[n]; }
	RSInterNode& nearestInterNode( const RSTdPoint& tdp, double& distance,int& index );

	void reserve( int size  ) { m_interNodes.reserve( size ); }

	void eraseAutoGenerateInter( );

	void push_back( const RSInterNode& node ) {  m_interNodes.push_back( node ); }
	void push_back( const vector< RSInterNode > & vec ) { copy( vec.begin( ),vec.end( ),back_insert_iterator< vector<RSInterNode> >( m_interNodes ) ); }
	void clear( ) { m_interNodes.clear( ); }
};



/***********************************************************************************************************************************
*Point of Broken Line struct
*/

struct RSLineNode
{
	RSTdPoint p;//Coordinate  
	RSPoint_Type  type;// Point Type

	double   length;		//The Length to the next Point
	bool    flip;		//Direction to the next Point for Arranging

	RSLineNode( ) :p( ),type( POINT_USER_DEFINE ), length(0), flip( false ) {  }
	bool friend operator ==( const  RSLineNode& a, const RSLineNode& b ) { return ( a.p == b.p ); }
	RSTdPoint& getPoint( ) { return p; }
	RSPoint_Type Type( ) { return type; }
	void setPoint( const RSTdPoint& tdp ){ p = tdp;}
	void setType( RSPoint_Type _type ){ type = _type; }

	double getX( ){ return p.x; }
	double getY( ){ return p.y; }
	double getZ( ){ return p.z; }
	void setX( double _x ){ p.x = _x;}
	void setY( double _y ){ p.y = _y;}
	void setZ( double _z ){ p.z = _z;}
};


/***********************************************************************************************************************************
*Point of VerticalFace struct
*/

struct RSVerticalFaceNode
{
	RSTdPoint p;
	unsigned int nodeIndex;

	float length;
	bool flip;

	RSVerticalFaceNode( ):p( ),nodeIndex(0),length(0),flip(false){ }
};



/***********************************************************************************************************************************
*Point For Rect of GridFace 
*/
struct RSGridFaceRectNode
{
	float x;//Coordinate X
	float y;//Coordinate Y
	float line;//ID of Line
	float cdp;//ID of Cdp

	RSGridFaceRectNode( ) :x(0),y(0),line(0),cdp(0) {  }
	RSGridFaceRectNode( float _x, float _y, float _line, float _cdp ) :x(_x),y(_y),line(_line),cdp(_cdp){  }
	void setX( float _x ) { x = _x;}
	void setY( float _y ) { y = _y; }
	void setLine( float _line ){ line = _line; }
	void setCdp( float _cdp ){ cdp = _cdp; }
};



/***********************************************************************************************************************************
*Horizontal Projection Rect of GridFace 
*/

//
struct FaePoint_Inc
{
	bool operator( )( const RSGridFaceRectNode& a, const RSGridFaceRectNode& b )
	{
		if ( a.line  < b.line )
		{
			return true;
		}
		if ( a.line > b.line )
		{
			return false;
		}
        if ( a.cdp  < b.cdp )
		{
			return true;
		}
		return false;
	}
};

//
struct RSGridFaceRect
{
	RSGridFaceRect( ) :numLine(0),numCdp(0),i(0) {  }
	RSGridFaceRect( RSGridFaceRectNode _rect[ ],int _numLine =  0, int _numCdp = 0 ) 
		:numLine(_numLine),numCdp(_numCdp),i(0)
	{
		copy( _rect,_rect + 4,rect );
		sort( rect, rect + 4, FaePoint_Inc( ) );
	}

	void push( const RSGridFaceRectNode& node ) 
	{
		if ( i < 4 )
			rect[i] = node;
		if ( i == 3 )
		{
			sort( rect, rect + 4, FaePoint_Inc( ) );
			i = 0;
		}

		++i;
	}

	void setLine( float n = 0 ) { numLine = static_cast<int>(n); }
	void setCdp( float n = 0 ) { numCdp = static_cast<int>(n); }
	int getLine( ) { return numLine; }
	int getCdp( ) { return numCdp; }

	RSGridFaceRectNode& operator[ ]( int n )
	{
		if ( n <  3 )
			return rect[n];
		return rect[3];
	}


private:
	int numLine;//Number of Line
	int numCdp;//Number of Cdp
	int i;//
	RSGridFaceRectNode rect[4];//Coordinate of Rect
};


/***********************************************************************************************************************************
*Coordinate Z of GridFace struct
*/
struct RSGridFaceZ
{
	vector< vector< float > > nodeZ;

	RSGridFaceZ( ) { };
	RSGridFaceZ( const vector< vector< float > >& z): nodeZ( z ){  }

	void push( float z[ ], int n )
	{
		nodeZ.resize( nodeZ.size( ) + 1 );
		nodeZ.back( ).reserve( n );
		copy( z, z + n, back_insert_iterator< vector< float > >( nodeZ.back( ) ) );
	}

	void push( vector<float>& z )
	{
		nodeZ.push_back( z );
	}
};



/***********************************************************************************************************************************
*Segment struct
*/
struct RSSegment
{
	RSTdPoint startPoint;
	RSTdPoint endPoint;

	RSSegment( ) :startPoint( ),endPoint( ) {  }
	RSSegment( const RSTdPoint& sPoint, const RSTdPoint& ePoint ):startPoint( sPoint ),endPoint( ePoint ){  }
};




/***********************************************************************************************************************************
*
*/
struct sign
{
	int operator( )(const RSTdPoint& a,const RSTdPoint& b,const RSTdPoint& c)
	{
		double matrix = (a.x-c.x)*(b.y-c.y)-(a.y-c.y)*(b.x-c.x);
		if(matrix<0.0005&&matrix>-0.0005) return 0;
		else if (matrix<0) return -1;
		return 1;
	}
};



extern double PointLineSegDistanceSquared3D( const RSTdPoint& q, const RSSegment& s );
extern int FindIntersection( RSSegment& S1, RSSegment& S2, RSTdPoint I[2] );

#endif