#include "rsline.h"
#include <math.h>


/*
*
*/
RSLine::RSLine( int size )
    :close(false),m_lineID( 0 ),m_nodeIndex(0),m_line( size,RSLineNode( ) ),m_IntersWithFace( )
{
	m_line.resize( 0 );
}

/*
*
*/
RSLine::RSLine( const RSLine& rLine )
:close( rLine.close),m_lineID( rLine.m_lineID),m_nodeIndex( rLine.m_nodeIndex),m_line( rLine.m_line),m_IntersWithFace( )
{

}

/*
*
*/
RSLine::RSLine( const vector< pair< RSTdPoint,RSPoint_Type > >& vec )
{
	m_line.resize( 0 );
	m_line.reserve( vec.size( ) );

	for ( vector< pair< RSTdPoint, RSPoint_Type > >::const_iterator it = vec.begin( ); it != vec.end( ); ++it )
	{
		RSLineNode vn;
		vn.p = it->first;
		vn.type = it->second;

		m_line.push_back( vn );
	}
	m_nodeIndex = 0;
}

/*
*
*/
RSLine::~RSLine( void )
{
}

/*
*
*/
 const vector<RSLineNode>& RSLine::GetLine( ) const
{
	return m_line;
}

vector<RSLineNode>& RSLine::GetLine( )
{ 
	return m_line;
}

/*
*
*/
vector<RSInterNode>& RSLine::GetInterWithFace( ) const
{
	return m_IntersWithFace;
}

/*
*
*/
void RSLine::SetClose(bool clo)
{
	close = clo;
}

/*
*
*/
bool RSLine::IsClose( ) const
{
	return close;
}
/*
*
*/
void RSLine::setType( vector< RSLineNode >::iterator where,RSPoint_Type type  )
{
	where->type = type;
}
/*
*
*/
RSPoint_Type RSLine::getType( vector< RSLineNode >::iterator where )
{
	return where->type;
}
/*
*
*/
void RSLine::setID( unsigned int lineID ) const
{
	m_lineID = lineID;
}
/*
*
*/
unsigned int RSLine::getID( )
{
	return m_lineID;
}
unsigned int RSLine::getID( ) const
{
	return m_lineID;
}
/*
*
*/
void RSLine::push( const RSTdPoint& tdp, RSPoint_Type type )
{
	RSLineNode vn;
	vn.p = tdp;
	vn.type = type;
	m_line.push_back( vn );
}

/*
*
*/
double RSLine::CalZ( const RSSegment& S, const RSTdPoint& p ) const
{
	double sLen = 0;
	double bLen = 0;
	bLen = sqrt( pow( p.x - S.startPoint.x,2 ) + pow( p.y - S.startPoint.y,2 ) );
	sLen = sqrt( pow( S.startPoint.x - S.endPoint.x,2 ) + pow( S.startPoint.y - S.endPoint.y,2 ) );
	return (S.startPoint.z + ( S.endPoint.z - S.startPoint.z ) * bLen / sLen);
}
/******************************************************
*
*/
double RSLine::disP2Lines( const RSTdPoint& point )
{
	vector<RSLineNode>::iterator itNearest = m_line.begin( );
	vector<RSLineNode>::iterator itStart = m_line.begin( );
	vector<RSLineNode>::iterator itEnd = itStart + 1;

	double minDis = 0.0;
	double p2LineDis = 0.0;
	RSSegment s( itStart->p, itEnd->p );
	p2LineDis = PointLineSegDistanceSquared3D( point, s );
	minDis = p2LineDis;
	++itStart;
	++itEnd;

	while( itEnd != m_line.end( ) )
	{
		s.startPoint = itStart->p;
		s.endPoint = itEnd->p;

		p2LineDis = PointLineSegDistanceSquared3D( point, s );
		if( minDis > p2LineDis )
		{
			minDis = p2LineDis;
			itNearest = itStart;	
		}
		++itStart;
		++itEnd;
	}

	m_itNearestLine = itNearest;
	nearestSeg.startPoint = itNearest->p;
	nearestSeg.endPoint = (itNearest+1)->p;

	return minDis;
}



/*
*set the flip
*/
bool RSLine::JudgeDire( const RSLineNode& nodeBegin,const RSLineNode& nodeEnd ) const
{
	if ( nodeEnd.p.x > nodeBegin.p.x ) return false;
	if ( nodeEnd.p.x < nodeBegin.p.x ) return true;
	if ( nodeEnd.p.y > nodeBegin.p.y ) return false;
	return true;
}

/*
*
*/
void RSLine::erase( vector< RSLineNode >::iterator where )
{
	m_line.erase( where );
}

/*
*
*/
void RSLine::insert( vector< RSLineNode >::iterator  where, const RSTdPoint& point,RSPoint_Type type )
{
	RSLineNode vn;
	vn.p = point;
	vn.type = type;
	m_line.insert( where, vn );
}

/*
*
*/
vector< RSLineNode >::iterator RSLine::getPoint( float x, float y, float z )
{
	RSTdPoint tdp( x, y, z );
	RSLineNode vn;
	vn.p = tdp;
	return find( m_line.begin( ), m_line.end( ), vn );
}
/*
*
*/
vector< RSLineNode >::iterator RSLine::getPoint( const RSTdPoint& tdp )
{
	RSLineNode vn;
	vn.p = tdp;
    return find( m_line.begin( ), m_line.end( ), vn );
}

/*
*
*/
vector< RSLineNode >::iterator RSLine::nearestNode( const RSTdPoint& tdp, double& dis )
{
	double distance = 0;
	double min = 0;
	vector< RSLineNode >::iterator nodeIt;
	vector< RSLineNode >::iterator minDisIt;
	distance = Distance( tdp, nodeIt->p );
	minDisIt = nodeIt;
	min = distance;
	++nodeIt;

	while( nodeIt != m_line.end( ) )
	{
		distance = Distance( tdp, nodeIt->p );
		if ( min > distance )
		{
			min = distance;
			minDisIt = nodeIt;
		}
	}
	dis = min;
	return minDisIt;
}
/*
*
*/
void RSLine::setPoint( vector< RSLineNode >::iterator where,  float x, float y, float z )
{
	if( where != m_line.end( ) )
	{
       where->p.x = x;
       where->p.y = y;
       where->p.z = z;
	}
}
/*
*
*/
void RSLine::setPoint( vector< RSLineNode >::iterator where, const RSTdPoint& tdp )
{
	if ( where != m_line.end( ) )
	{
		where->p = tdp;
	}
}
/*
*
*/
const RSSegment& RSLine::getNearestSegment(  )
{
	return nearestSeg;
}

/*
*
*/
vector<RSLineNode>::iterator RSLine::getNearestPoint( )
{
	return m_itNearestLine;
}


/******************************************************
*
*/
void RSLine::eraseRepeat( ) const
{
	m_nodeIndex = 0;

	sort( m_IntersWithFace.begin( ), m_IntersWithFace.end( ),InterNode_GFace_Inc( ) );
	m_IntersWithFace.erase( unique( m_IntersWithFace.begin( ), m_IntersWithFace.end( ) ), m_IntersWithFace.end( ) );

	if ( this->close &&  ( this->m_IntersWithFace.size( ) > 1) )
	{
		if ( m_IntersWithFace.back( ).p == m_IntersWithFace.front( ).p && m_IntersWithFace.back( ).isSeg && !m_IntersWithFace.front( ).isSeg )
			m_IntersWithFace.erase( m_IntersWithFace.begin( ) );
	}
}



/******************************************************
*
*/
void RSLine::eraseExtreme( ) const
{
	m_nodeIndex = 0;

	sort( m_IntersWithFace.begin( ), m_IntersWithFace.end( ), InterNode_VFace_Inc( ) );

	if ( close )
		GetLineInside( );

	m_IntersWithFace.erase( remove_if( m_IntersWithFace.begin( ), m_IntersWithFace.end( ), RSInterNode_NotInter( ) ), m_IntersWithFace.end( ) );

}

/******************************************************
*
*/
void RSLine::intersect( const RSVerticalFaceNode& startNode,const RSVerticalFaceNode& endNode,float lowZ,float higZ ) const
{
	int intersect;
	double interZ;
	double sem1Z;
	double sem2Z;
	RSTdPoint I[2];

	RSSegment S1;
	S1.startPoint = startNode.p;
	S1.endPoint = endNode.p;
	RSSegment S2;
	vector<RSLineNode>::const_iterator lineStartIt = m_line.begin( );
	vector<RSLineNode>::const_iterator lineEndIt = lineStartIt + 1;

	pushNode( startNode );
	while ( lineEndIt != m_line.end( ) )
	{

		S2.startPoint = lineStartIt->p;
		S2.endPoint = lineEndIt->p;
		intersect = FindIntersection( S1,S2,I );
		if ( intersect == 1 )
		{
			interZ = CalZ( S2,I[0] );
			if ( (interZ >= lowZ) && (interZ <= higZ) )
			{
				I[0].z = interZ;
				pushNode( startNode,*lineStartIt,*lineEndIt,I[0],false );
			}
		}
		else if ( intersect == 2 )
		{
			sem1Z = CalZ( S2,I[0] );
			sem2Z = CalZ( S2,I[1] );

			if ( (sem1Z >= lowZ) && (sem1Z <= higZ) && (sem2Z >= lowZ) && (sem2Z <= higZ) )
			{
				I[0].z = sem1Z;
				I[1].z = sem2Z;
				pushNode( startNode,*lineStartIt,*lineEndIt,I[0],true );
				pushNode( startNode,*lineStartIt,*lineEndIt,I[1],true );
			}
		}

		lineStartIt = lineEndIt;
		++lineEndIt;
		++m_nodeIndex;
	}

	if ( close )
	{
		lineEndIt = m_line.begin( );

		S2.startPoint = lineStartIt->p;
		S2.endPoint = lineEndIt->p;
		intersect = FindIntersection( S1,S2,I );
		if ( intersect == 1 )
		{
			interZ = CalZ( S2,I[0] );
			if ( (interZ >= lowZ) && (interZ <= higZ) )
			{
				I[0].z = interZ;
				pushNode( startNode,*lineStartIt,*lineEndIt,I[0],false );
			}
		}
		if ( intersect == 2 )
		{
			sem1Z = CalZ( S2,I[0] );
			sem2Z = CalZ( S2,I[1] );

			if ( sem1Z >= lowZ && sem1Z <= higZ && sem2Z >= lowZ && sem2Z <= higZ )
			{
				I[0].z = sem1Z;
				I[1].z = sem2Z;
				pushNode( startNode,*lineStartIt,*lineEndIt,I[0],true );
				pushNode( startNode,*lineStartIt,*lineEndIt,I[1],true );
			}
		}
	}
	
	m_nodeIndex = 0;
}

/******************************************************
*
*/
void RSLine::intersect( RSLineNode &startNode, RSLineNode &endNode,const vector< pair< float, float > >& segOnFace ) const
{
	if ( segOnFace.empty( ) )	//none of Point of Intersection
	{
		++m_nodeIndex;
		return;
	}

	startNode.flip = JudgeDire( startNode, endNode );

	if ( segOnFace.size( ) == 1 )	//one Point of Intersection
	{
		if ( (segOnFace.begin( )->first == startNode.p.z) && segOnFace.begin( )->second == 0 )
		{
			pushNode( startNode,endNode, startNode.p, false );
		}
		++m_nodeIndex;
		return;
	}

	int intersect = 0;	//number of Point of Intersection
	RSTdPoint pInters[2];//Points of Intersection
	RSTdPoint p;
	RSTdPoint p1Seg;
	RSTdPoint p2Seg;
	RSSegment s1, s2;

	endNode.length = sqrt(pow((endNode.p.y-startNode.p.y),2)+pow((endNode.p.x-startNode.p.x),2));
	s1.startPoint.y = startNode.p.z;
	s1.startPoint.x = 0;
	s1.endPoint.y = endNode.p.z;
	s1.endPoint.x = endNode.length;
	vector< pair<float,float> >::const_iterator itstartNode , itendNode;
	itstartNode = segOnFace.begin( );
	itendNode = segOnFace.begin( ) + 1;

	if ( abs( endNode.p.x - startNode.p.x ) < G_EPSILON )// Slop K is Infinity,Line parall Y axes
	{
		while( itendNode != segOnFace.end( ) )
		{
			s2.startPoint.x = (*itstartNode).second;
			s2.startPoint.y = (*itstartNode).first;
			s2.endPoint.x = (*itendNode).second;
			s2.endPoint.y = (*itendNode).first;

			intersect = FindIntersection( s1,s2,pInters );
			if(  intersect == 1 )
			{
				invert( startNode,pInters[0],p );
				pushNode( startNode,endNode,p,false );
			}
		    else if ( intersect == 2 )
			{
				invert( startNode,pInters[0], p1Seg );
				invert( startNode,pInters[1], p2Seg );
				pushNode( startNode,endNode,p1Seg,true );
				pushNode( startNode,endNode,p2Seg,true );
			}
			itstartNode = itendNode;
			++itendNode;
		}
	}
	else
	{		
		double k = (endNode.p.y-startNode.p.y)/(endNode.p.x-startNode.p.x);

		while( itendNode != segOnFace.end( ) )
		{
			s2.startPoint.x = (*itstartNode).second;
			s2.startPoint.y = (*itstartNode).first;
			s2.endPoint.x = (*itendNode).second;
			s2.endPoint.y = (*itendNode).first;

			intersect = FindIntersection( s1,s2,pInters );
			if ( intersect == 1 )
			{
				invert( startNode,pInters[0],p,k );
				pushNode( startNode,endNode,p,false );			
			}
		    else if ( intersect == 2 )
			{
				invert( startNode,pInters[0],p1Seg,k );
				invert( startNode,pInters[1],p2Seg,k );
				pushNode( startNode,endNode, p1Seg,true );
				pushNode( startNode,endNode, p2Seg,true );
			}
			itstartNode = itendNode;
			++itendNode;
		}
	}

	++m_nodeIndex;
}


/******************************************************
*
*/
void RSLine::pushNode( const RSVerticalFaceNode& fnode ) const
{
	RSInterNode node;
	node.p = fnode.p;
	node.VFaceNodeIndex = fnode.nodeIndex;
	node.isFlip = fnode.flip;
	node.nodeIndex = m_nodeIndex;
	node.lineID = m_lineID;
	node.isSeg = false;
	node.isInter = false;
	node.isContact = false;
	node.type = POINT_NOT_EXTREME;

	m_IntersWithFace.push_back( node );
}


/******************************************************
*
*/
void RSLine::pushNode( const RSVerticalFaceNode& fnode, const RSLineNode& startNode, const RSLineNode& endNode,RSTdPoint& tdp, bool isSeg ) const
{
	RSInterNode node;

	node.p = tdp;
	node.VFaceNodeIndex = fnode.nodeIndex;
	node.isFlip = fnode.flip;
	node.nodeIndex = m_nodeIndex;
	node.lineID = m_lineID;
	node.isSeg = isSeg;
	node.isInter = true;
	node.isContact = false;

	if ( startNode.p == tdp )
	{
		node.type = startNode.type;
		m_IntersWithFace.push_back( node );
	}
	else 
		if ( tdp != endNode.p )
		{
			node.type = POINT_NOT_EXTREME;
			m_IntersWithFace.push_back( node );
		}
		else
		{
			node.nodeIndex += 1;
			node.type = endNode.type;
			m_IntersWithFace.push_back( node );
		}
}
/******************************************************
*
*/
void RSLine::pushNode( const RSLineNode& startNode, const RSLineNode& endNode,RSTdPoint& tdp,  bool isSeg ) const
{
	RSInterNode node;

	node.p = tdp;
	node.lineID = m_lineID;
	node.nodeIndex = m_nodeIndex;
	node.isFlip = startNode.flip;
	node.isSeg = isSeg;
	node.isContact = false;

	if ( startNode.p == tdp )
	{
		node.type = startNode.type;
		m_IntersWithFace.push_back( node );
	}
	else 
		if ( tdp != endNode.p )
		{
			node.type = POINT_NOT_EXTREME;
			m_IntersWithFace.push_back( node );
		}
		else
		{
			node.nodeIndex += 1;
			node.type = endNode.type;
			m_IntersWithFace.push_back( node );
		}
}

/******************************************************
*
*/
void RSLine::invert( const RSLineNode& startNode,RSTdPoint& tdpSrc, RSTdPoint& tdpDes, double k  ) const 
{
	float s = static_cast<float>( 1.0 / sqrt( 1 + k*k ) );
	if(startNode.flip)
	{
		tdpDes.x = startNode.p.x - tdpSrc.x * s;
		tdpDes.y = startNode.p.y - tdpSrc.x * k * s;
	}
	else
	{
		tdpDes.x = startNode.p.x + tdpSrc.x * s;
		tdpDes.y = startNode.p.y + tdpSrc.x * k * s;
	}
	tdpDes.z = tdpSrc.y;
}

/******************************************************
*
*/
void RSLine::invert( const RSLineNode& startNode,RSTdPoint& tdpSrc, RSTdPoint& tdpDes ) const
{
	if(startNode.flip)
	{
		tdpDes.x = startNode.p.x;
		tdpDes.y = startNode.p.y - tdpSrc.x;
	}
	else
	{
		tdpDes.x = startNode.p.x;
		tdpDes.y = startNode.p.y + tdpSrc.x;
	}
	tdpDes.z = tdpSrc.y;
}

/******************************************************
*
*/
double RSLine::_CalAngle(RSTdPoint p, const RSLineNode& startNode , const RSLineNode& endNode) const
{
	double a = sqrt((endNode.p.y-startNode.p.y)*(endNode.p.y-startNode.p.y) + (endNode.p.x-startNode.p.x)*(endNode.p.x-startNode.p.x));
	double b = sqrt((endNode.p.y-p.y)*(endNode.p.y-p.y) + (endNode.p.x-p.x)*(endNode.p.x-p.x));
	double c = sqrt((startNode.p.y-p.y)*(startNode.p.y-p.y) + (startNode.p.x-p.x)*(startNode.p.x-p.x));
	double angle = acos((b*b+c*c-a*a)/(2*b*c));
	if(sign()(p , startNode.p, endNode.p) == 1)
		return angle;
	else return -angle;
}

/******************************************************
*
*/
double RSLine::CalAngle( RSTdPoint p ) const
{
	double angle=0;
	vector<RSLineNode>::const_iterator startIt , endIt;
	startIt = m_line.begin( );
	endIt = startIt+1;
	while(endIt != m_line.end( ) )
	{
		angle+=_CalAngle(p , *startIt , *endIt);
		startIt = endIt;
		endIt++;
	}
	startIt = m_line.end( )-1;
	endIt = m_line.begin( );
	angle+=_CalAngle(p , *startIt , *endIt);
	return angle;
}


/******************************************************
*
*/

bool RSLine::IsContained( const RSSegment& s ) const
{
	RSTdPoint p;
	p.x = (s.startPoint.x+s.endPoint.x)/2;
	p.y = (s.startPoint.y+s.endPoint.y)/2;
	double angle = CalAngle( p ) / ( static_cast<float>(G_PI)*2 );

	if (angle>0)
	{
		angle+=0.5;
		int i = int(angle);
		if(i%2==0) return false;
	}
	else
	{
		angle-=0.5;
		int i = int(angle);
		if(i%2==0) return false;
	}
	return true;
}

/******************************************************
*
*/
bool RSLine::HasNextInter( vector<RSInterNode>::iterator current ) const
{
	vector<RSInterNode>::iterator next = current;

	while( next != m_IntersWithFace.end( ) )
	{
		if( (*next).isInter ) break;
		++next;
	}
	if ( next == m_IntersWithFace.end( ) ) return false;
	else return true;
}

/******************************************************
*
*/
void RSLine::GetLineInside( ) const 
{   
	RSSegment s;
	vector<RSInterNode>::iterator startIt , endIt;

	if ( m_IntersWithFace.size( ) < 2 ) return;

	startIt = m_IntersWithFace.begin( );
	endIt = startIt+1;

	while(endIt != m_IntersWithFace.end())
	{		
		if((*startIt).isInter)
		{
			s.startPoint = (*startIt).p;
			s.endPoint   = (*endIt).p;
			if(IsContained(s)&& HasNextInter(startIt)) 
			{
				(*startIt).isContact = true;
			}
		}
		startIt = endIt;
		endIt++;
	}
}



































