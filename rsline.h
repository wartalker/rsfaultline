#ifndef RSLINE_H
#define RSLINE_H

#include <vector>
#include <algorithm>
#include <math.h>
#include "rshead.h"

using namespace std;

class RSLine
{

private:
	bool close;//Close
	mutable unsigned int m_lineID; //ID of Line 
	mutable unsigned int m_nodeIndex; // Index of Node

	vector<RSLineNode> m_line;//Break Line Point
	mutable vector< RSInterNode > m_IntersWithFace; //Points of Intersection

	vector<RSLineNode>::iterator m_itNearestLine;
	RSSegment nearestSeg;//Nearest Segment

	bool JudgeDire( const RSLineNode &nodeBegin, const RSLineNode &nodeEnd ) const;//Direction of Line	

	void pushNode( const RSVerticalFaceNode& fnode ) const;//push Point of Intersection
	void pushNode( const RSLineNode& startNode, const RSLineNode& endNode,RSTdPoint& tdp, bool isSeg ) const;
	void pushNode( const RSVerticalFaceNode& fnode, const RSLineNode& startNode, const RSLineNode& endNode,RSTdPoint& tdp, bool isSeg ) const;

	void invert( const RSLineNode &startNode,RSTdPoint& tdpSrc, RSTdPoint& tdpDes, double k ) const;//Coordinate Invert
	void invert( const RSLineNode &startNode,RSTdPoint& tdpSrc, RSTdPoint& tdpDes ) const;//Coordinate Invert

	double CalZ( const RSSegment& S, const RSTdPoint& p) const;//Z
	double _CalAngle(RSTdPoint p , const RSLineNode & startNode , const RSLineNode & endNode) const;
	double CalAngle(RSTdPoint p ) const;
	bool  IsContained( const RSSegment& s ) const;//Is the segment in Line
	bool HasNextInter( vector< RSInterNode > ::iterator current ) const;	
	void GetLineInside( ) const;//

	
	RSLine& operator = ( const RSLine& );

public:
	explicit RSLine( int size = 0 );//size is the number of nodes
	explicit RSLine( const vector< pair< RSTdPoint,RSPoint_Type > >& vec );
	RSLine( const RSLine& rLine );
	~RSLine( );

	int size( ) { return m_line.size( ); };
	RSLineNode& operator[ ]( int n ) { return m_line[n]; }//NO Boundary Check
	void				SetClose(bool clo);//Set Close
	bool				IsClose( ) const;//
	void setID( unsigned int lineID ) const;//
	unsigned int getID( );//
	unsigned int getID( ) const;

	void push( const RSTdPoint& tdp, RSPoint_Type type = POINT_USER_DEFINE );//push the RSLineNode
	void setType( vector< RSLineNode >::iterator where,RSPoint_Type type = POINT_USER_DEFINE );//Set the type of RSLineNode
	RSPoint_Type getType( vector< RSLineNode >::iterator where );//
	vector< RSLineNode >::iterator nearestNode( const RSTdPoint& tdp, double& dis );//The nearest RSLineNode
	void erase( vector< RSLineNode >::iterator where );//erase The RSLineNode
	void insert( vector< RSLineNode >::iterator  where, const RSTdPoint& point, RSPoint_Type type = POINT_USER_DEFINE );//insert the RSLineNode
	vector< RSLineNode >::iterator getPoint( float x, float y, float z );//
	vector< RSLineNode >::iterator getPoint( const RSTdPoint& tdp );//
	void setPoint( vector< RSLineNode >::iterator where, float x, float y, float z );//set the RSLineNode
	void setPoint( vector< RSLineNode >::iterator where, const RSTdPoint& tdp );//

	const RSSegment& getNearestSegment(  );
	vector< RSLineNode >::iterator getNearestPoint( );

	const vector<RSLineNode>&	GetLine( ) const; //
	vector<RSLineNode>& GetLine( );
	vector<RSInterNode>& GetInterWithFace( ) const;//

	double disP2Lines( const RSTdPoint& point );//The nearest distance of Point to RSLine
	void intersect( RSLineNode & startNode ,  RSLineNode& endNode, const vector< pair< float, float > >& segOnFace ) const;//Intersection 		
	void intersect( const RSVerticalFaceNode& startNode, const RSVerticalFaceNode& endNode,float lowZ,float higZ ) const;//Intersection
	void eraseRepeat(  ) const;//erase the repeated Point
	void eraseExtreme( ) const;//erase the extreme Point
};





// Predicate the Point of Intersection
struct RSInterNode_NotInter : public unary_function< RSInterNode, bool >
{
	bool operator ( )( const RSInterNode& node ) { return !node.isInter; }
};

//Arrange the Point of Intersection with VerticalFace
struct InterNode_VFace_Inc
{
	bool operator ( )( const RSInterNode& a, const RSInterNode& b )
	{
		if ( a.VFaceNodeIndex  < b.VFaceNodeIndex ) return true;
		if ( a.VFaceNodeIndex  > b.VFaceNodeIndex ) return false;
		if ( a.isFlip )
		{
			if ( a.p.x > b.p.x ) return true;
			if ( a.p.x < b.p.x ) return false;
			if ( a.p.y > b.p.y ) return true;
			if ( a.p.y < b.p.y ) return false;
			return true;
		}
		else
		{
			if ( a.p.x > b.p.x ) return false;
			if ( a.p.x < b.p.x ) return true;
			if ( a.p.y > b.p.y ) return false;
			if ( a.p.y < b.p.y ) return true;
			return true;
		}
	}
};

//Arrange the Point of Intersection with GridFace
struct InterNode_GFace_Inc
{
	bool operator ( )( const RSInterNode& a, const RSInterNode& b )
	{
		if ( a.nodeIndex < b.nodeIndex ) return true;
		if ( a.nodeIndex > b.nodeIndex ) return false;
		if ( a.isFlip )
		{
			if ( a.p.x > b.p.x ) return true;
			if ( a.p.x < b.p.x ) return false;
			if ( a.p.y > b.p.y ) return true;
			if ( a.p.y < b.p.y ) return false;
			return true;
		}
		else
		{
			if ( a.p.x > b.p.x ) return false;
			if ( a.p.x < b.p.x ) return true;
			if ( a.p.y > b.p.y ) return false;
			if ( a.p.y < b.p.y ) return true;
			return true;
		}
	}
};

#endif //RSLINE_H
