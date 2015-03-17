#include "rsgridface.h"
#include <math.h>

using namespace std;


/******************************************************
*GridFace Construct
*/
RSGridFace::RSGridFace( RSGridFaceRect rect, const RSGridFaceZ& z )
:m_z( z.nodeZ )
{
	RSGridFaceRectNode leftBottom,leftTop, rightBottom, rightTop;
	float lengthX;
	float lengthY;

	leftBottom = rect[0];
	leftTop = rect[1];
	rightBottom = rect[2];
	rightTop = rect[3];

	lengthX = GetLength( leftBottom, rightBottom );
	lengthY = GetLength( leftBottom, leftTop );

	m_numOfGridX = rect.getLine( ) - 1;
	m_numOfGridY = rect.getCdp( ) - 1;
	m_lengthOfGridX = lengthX/m_numOfGridX;
	m_lengthOfGridY = lengthY/m_numOfGridX;
	m_leftX = 0;
	m_bottomY = 0;

	m_InvMatrix.rotate( -GetAng( leftBottom, rightBottom ) );
	m_InvMatrix.translate( -leftBottom.x, -leftBottom.y );
	m_RevMatrix = m_InvMatrix.invert( );

	m_segOnFace.reserve( rect.getCdp( ) + rect.getLine( ) + 4 );
	m_segOnFace.resize( 0 );
}


/******************************************************
*
*/
RSGridFace::~RSGridFace(void)
{
}
/***************************************************************
 * The Z value in Face a-b-c of Point x-y
 */
float RSGridFace::GetZ(RSTdPoint a , RSTdPoint b , RSTdPoint c , float x , float y)
{
	RSTdPoint normal;
	normal = (c - a) * (b - a);
	return ((normal.x*(a.x-x)) + (normal.y*(a.y-y)) + (normal.z*a.z))/normal.z; 
}


/*******************************************************************************************
 *The convex Diagonal of Cell Grid
 */
int RSGridFace::GetDiagonal(RSTdPoint vertex[])
{
     RSSegment s;
	 float midZ,endZ;
	 midZ = (vertex[0].z + vertex[2].z)/2;
	 endZ = vertex[1].z + 2 * (midZ - vertex[1].z);
	 if(endZ > vertex[3].z) 
	 {
		 return 0;
	 }
	 else 
	 {
		 return 1;
	 }
}


/*****************************************************************
 *The Z Value of EndingNode in Face
 *precondiction:endingPoint must be within the face f
 */
float RSGridFace::CalZ( const RSLineNode & endingNode )
{

	float x = endingNode.p.x;
	float y = endingNode.p.y;
	int leftXIndex = static_cast< int >( (x - m_leftX) / m_lengthOfGridX );
	int lowerYIndex = static_cast< int >( (y - m_bottomY) / m_lengthOfGridY );

	if ( leftXIndex == m_numOfGridX )
	{
		--leftXIndex;
	}

	if ( lowerYIndex == m_numOfGridY )
	{
		--lowerYIndex;
	}

	RSTdPoint fourPoints[4];

	fourPoints[0].x = m_leftX + leftXIndex*m_lengthOfGridX;
	fourPoints[0].y = m_bottomY + lowerYIndex*m_lengthOfGridY;
	fourPoints[0].z = (*((*(m_z.begin()+leftXIndex)).begin() + lowerYIndex));

	fourPoints[1].x = m_leftX + (leftXIndex+1)*m_lengthOfGridX;
	fourPoints[1].y = m_bottomY + lowerYIndex*m_lengthOfGridY;
	fourPoints[1].z = (*((*(m_z.begin()+leftXIndex+1)).begin() + lowerYIndex));

	fourPoints[2].x = m_leftX + (leftXIndex+1)*m_lengthOfGridX;
	fourPoints[2].y = m_bottomY + (lowerYIndex+1)*m_lengthOfGridY;
	fourPoints[2].z = (*((*(m_z.begin()+leftXIndex+1)).begin() + lowerYIndex+1));

	fourPoints[3].x = m_leftX + (leftXIndex)*m_lengthOfGridX;
	fourPoints[3].y = m_bottomY + (lowerYIndex+1)*m_lengthOfGridY;
	fourPoints[3].z = (*((*(m_z.begin()+leftXIndex)).begin() + lowerYIndex+1));

	if( GetDiagonal(fourPoints) == 0 )
	{
		if(sign( )(endingNode.p , fourPoints[0] ,  fourPoints[2]) == 1)
			return GetZ(fourPoints[0] , fourPoints[2] , fourPoints[3] , x , y);
		else 
			return GetZ(fourPoints[0] , fourPoints[1] , fourPoints[2] , x , y);
	}
	else
	{
		if(sign( )(endingNode.p , fourPoints[1] ,  fourPoints[3]) == 1)
			return GetZ(fourPoints[0] , fourPoints[1] , fourPoints[3] , x , y);
		else 
			return GetZ(fourPoints[1] , fourPoints[2] , fourPoints[3] , x , y);
	}
}



/**************************************************************
*The Point of Intersection with startNode-endNode from leftIndex to rightIndex
*Condition£º0 <= leftIndex < rightIndex <= m_numOfGridX
*/
void RSGridFace::CalVerticalInter( const RSLineNode & startNode , const RSLineNode & endNode , int leftIndex , int rightIndex)
{   
	if ( abs( startNode.p.x - endNode.p.x ) <  G_EPSILON )
	{
		return;
	}

	float k = (endNode.p.y-startNode.p.y)/(endNode.p.x-startNode.p.x);
	float upperY = m_bottomY + m_numOfGridY*m_lengthOfGridY;

	for ( int i = leftIndex;i <= rightIndex; ++i )
	{
		float X = m_leftX+i*m_lengthOfGridX;
		float y = k * (X - startNode.p.x) + startNode.p.y;
		if( y <= upperY && y >= m_bottomY ) 
		{
			int yIndex = static_cast< int >( (y - m_bottomY)/m_lengthOfGridY );

			float zl = (* ( ( *(m_z.begin( ) + i)).begin( ) + yIndex ) );
			float zu = (*((*(m_z.begin() + i)).begin() + yIndex+1));
			float z  = (y - (m_bottomY+yIndex*m_lengthOfGridY))/m_lengthOfGridY * (zu - zl) + zl;

            float len = sqrt(1+k*k) * abs(X-startNode.p.x);//the Length to StartNode 

			m_segOnFace.push_back( make_pair<float,float>( z, len ) );
		}
	}
}



/**************************************************************
*the Point of Intersection with startNode-endNode from lowerIndex to upperIndex
*Condition£º0 <= lowerIndex < upperIndex <= m_numOfGridY
*/
void RSGridFace::CalHorizontalInter( const RSLineNode& startNode , const RSLineNode& endNode, int lowerIndex , int upperIndex )
{
	if ( abs( startNode.p.y - endNode.p.y ) < G_EPSILON )
		return;


	float k = (endNode.p.x-startNode.p.x)/(endNode.p.y-startNode.p.y);
	float rightX = m_leftX + m_numOfGridX*m_lengthOfGridX;

	for(int i=lowerIndex;i<=upperIndex;i++)
	{
		float Y = m_bottomY+i*m_lengthOfGridY;
		float x = k * (Y - startNode.p.y) + startNode.p.x;
		if(x >= m_leftX && x <= rightX) 
		{
			int xIndex = static_cast< int >( (x - m_leftX)/m_lengthOfGridX );
			if ( ( ( x - m_leftX - xIndex * m_lengthOfGridX ) < G_EPSILON ) || (  ( m_leftX + (xIndex + 1) * m_lengthOfGridX - x ) < G_EPSILON ) )
				continue;

			float zl = (*((*(m_z.begin() + xIndex)).begin() + i));
			float zr = (*((*(m_z.begin() + xIndex+1)).begin() + i));
			float z  = (x - (m_leftX+xIndex*m_lengthOfGridX))/m_lengthOfGridX * (zr - zl) + zl;
            float len = sqrt(1+k*k) * abs(Y-startNode.p.y);

			m_segOnFace.push_back( make_pair<float,float>( z,len ) );
		}
	}
}



/*******************************************************************************
*the Point of Intersection with startNode-endNode in X-Y 
*/
void RSGridFace::CalFaceWithLineInter( const RSLineNode & startNode ,const RSLineNode & endNode)
{

	float leftNodeX		=	startNode.p.x<endNode.p.x ? startNode.p.x:endNode.p.x;
	float rightNodeX	=	startNode.p.x>endNode.p.x ? startNode.p.x:endNode.p.x;
	float lowerNodeY	=	startNode.p.y<endNode.p.y ? startNode.p.y:endNode.p.y;
	float upperNodeY	=	startNode.p.y>endNode.p.y ? startNode.p.y:endNode.p.y;

/*******************************************************************************/
	float fLeftIndex  = (leftNodeX - m_leftX) / m_lengthOfGridX;
	float fRightIndex = (rightNodeX  - m_leftX) / m_lengthOfGridX;

	if(  (fRightIndex  < 0) || (fLeftIndex > m_numOfGridX) )
		return;


	int leftIndex,rightIndex;
	if(fLeftIndex<0) leftIndex = 0;
	else 
	{
		leftIndex = static_cast< int >( fLeftIndex )  + 1;
	}

	if(fRightIndex>m_numOfGridX)
	{
		rightIndex = m_numOfGridX;
	}
	else
	{
		rightIndex = static_cast< int >( fRightIndex );

		if( ( rightNodeX - ( rightIndex*m_lengthOfGridX + m_leftX ) ) < G_EPSILON )
		{
			rightIndex--;
		}
	}
/*******************************************************************************/
	float fLowerIndex  = (lowerNodeY - m_bottomY) / m_lengthOfGridY;
	float fUpperIndex  = (upperNodeY  - m_bottomY) / m_lengthOfGridY;

	if( ( fUpperIndex< 0) || (fLowerIndex > m_numOfGridY) ) 
	{
		return;
	}

	int lowerIndex,upperIndex;

	if(fLowerIndex < 0 )
	{
		lowerIndex = 0;
	}
	else 
	{
		lowerIndex = static_cast< int >(fLowerIndex) + 1;
	}

	if( fUpperIndex > m_numOfGridY )
	{
		upperIndex = m_numOfGridY;
	}
	else
	{
		upperIndex = static_cast< int >(fUpperIndex);

		if( ( upperNodeY - (upperIndex*m_lengthOfGridY+m_bottomY) ) < G_EPSILON )
			upperIndex--;
	}
/*******************************************************************************/
	m_segOnFace.resize( 0 );
/*******************************************************************************/
    CalVerticalInter(startNode,endNode,leftIndex,rightIndex);
    CalHorizontalInter(startNode,endNode,lowerIndex,upperIndex);
/*******************************************************************************/
	//cal the corresponding z value of ending point(s)
	float rightX = m_leftX + m_numOfGridX*m_lengthOfGridX;
	float upperY = m_bottomY + m_numOfGridY*m_lengthOfGridY;
	if(m_leftX<=startNode.p.x&&startNode.p.x<=rightX && m_bottomY<=startNode.p.y&&startNode.p.y<=upperY)
	{
		m_segOnFace.push_back(make_pair<float,float>(CalZ(startNode),0.0));
	}
	if(m_leftX<=endNode.p.x&&endNode.p.x<=rightX && m_bottomY<=endNode.p.y&&endNode.p.y<=upperY)
	{
		float len1 = (endNode.p.y-startNode.p.y)*(endNode.p.y-startNode.p.y);
		float len2 =(endNode.p.x-startNode.p.x)*(endNode.p.x-startNode.p.x);
		float len = sqrt(len1+len2);
		m_segOnFace.push_back(make_pair<float,float>(CalZ(endNode) , len));
	}
/*******************************************************************************/
	if ( startNode.flip )
		sort( m_segOnFace.begin( ), m_segOnFace.end( ), DecreX( ) );
	else
	    sort( m_segOnFace.begin( ), m_segOnFace.end( ), IncreX( ) );
}


/******************************************************
*
*/
void RSGridFace::InterWithLines( const RSLine& l )
{
    vector<RSLineNode>::const_iterator  startIt , endIt;
	startIt = l.GetLine().begin();
	endIt   = l.GetLine().begin()+1;

	RSLineNode startNode = *startIt;
	RSLineNode endNode = *endIt;
	Invert( startNode );

	while( endIt != l.GetLine().end( ) )
	{
		Invert( endNode );
		CalFaceWithLineInter(startNode,endNode);
		l.intersect(startNode,endNode,m_segOnFace );

		startIt = endIt;
		++endIt;
		startNode = endNode;
		endNode = *endIt;
	}

	if ( l.IsClose( ) )
	{
		endIt = l.GetLine( ).begin( );
		endNode = *endIt;
		Invert( endNode );
		CalFaceWithLineInter(startNode,endNode);
		l.intersect(startNode,endNode,m_segOnFace );
	}

	l.eraseRepeat( );
	Reverse( l );
}

/******************************************************
*
*/
float RSGridFace::GetBottomY()
{
	return m_bottomY;
}


/******************************************************
*
*/
float RSGridFace::GetLeftX()
{
	return m_leftX;
}


/******************************************************
*
*/
float RSGridFace::GetLengthOfGridX()
{
	return m_lengthOfGridX;
}


/******************************************************
*
*/
float RSGridFace::GetLengthOfGridY()
{
	return m_lengthOfGridY;
}


/******************************************************
*
*/
int RSGridFace::GetNumOfGridX()
{
	return m_numOfGridX;
}

/******************************************************
*
*/
int RSGridFace::GetNumOfGridY()
{
	return m_numOfGridY;
}


/******************************************************
*
*/
vector<vector<float> >& RSGridFace::GetZ()
{
	return m_z;
}




/******************************************************
*
*/
float RSGridFace::GetAng( const RSGridFaceRectNode& start, const RSGridFaceRectNode& end )
{
	double dx;
	double dy;
	double ang;

	dy = end.y - start.y;
	dx = end.x - start.x;
	ang = asin( dy / sqrt( dx*dx + dy*dy ) );
	if ( dx >= 0 )
	{
		return static_cast< float >( ang/G_PI*180 );
	}
	else
	{
		return static_cast< float >(( 1 - ang/G_PI )*180);
	}

}

/******************************************************
*
*/
float RSGridFace::GetLength( const RSGridFaceRectNode& start, const RSGridFaceRectNode& end )
{
	double dx;
	double dy;

	dx = end.x - start.x;
	dy = end.y - start.y;

	return static_cast< float >( sqrt( dx*dx + dy*dy ) );
}

/******************************************************
*
*/
void RSGridFace::Invert( RSLineNode& preNode )
{
	double x = 0;
	double y = 0;

	m_InvMatrix.map( static_cast< double >( preNode.p.x ), static_cast< double >( preNode.p.y), &x, &y );

	if ( abs( x ) < G_EPSILON )
	{
		x = 0;
	}
	if ( abs( y ) < G_EPSILON )
	{
		y = 0;
	}

	preNode.p.x = x;
	preNode.p.y = y;
}

/******************************************************
*
*/
void RSGridFace::Reverse( RSTdPoint& tdp ) const 
{
	double x = 0;
	double y = 0;

	m_RevMatrix.map( static_cast< double >( tdp.x ), static_cast< double >( tdp.y), &x, &y );
	tdp.x = x;
	tdp.y = y;
}

/******************************************************
*
*/
void RSGridFace::Reverse( const RSLine& l ) const
{
	for ( vector< RSInterNode >::iterator it = l.GetInterWithFace( ).begin( ); it != l.GetInterWithFace( ).end( ); ++it )
	{
		Reverse( it->p );
	}
}



































