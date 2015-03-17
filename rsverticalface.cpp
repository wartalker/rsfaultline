#include "rsverticalface.h"


/******************************************************
*
*/
RSVerticalFace::RSVerticalFace( )
:m_vface( ),close(false),m_lowZ( 0 ),m_higZ( 0 ),m_nodeIndex(0)
{
}

/******************************************************
*
*/
RSVerticalFace::RSVerticalFace( RSTdPoint node[ ], int n, float lowZ,float highZ )
{ 
	m_vface.resize( 0 );
	m_vface.reserve( n );

	for ( int i = 0; i < n; ++i )
	{
		RSVerticalFaceNode vfnode;
		vfnode.p = node[i];
		m_vface.push_back( vfnode );
	}
	m_lowZ = lowZ;
	m_higZ = highZ;
};

RSVerticalFace::~RSVerticalFace( )
{ };

/******************************************************
*
*/
vector< RSVerticalFaceNode >& RSVerticalFace::GetLine( )
{
	return m_vface;
};

/******************************************************
*
*/
bool RSVerticalFace::JudgeDire(vector<RSVerticalFaceNode>::iterator itBegin, vector< RSVerticalFaceNode>::iterator itEnd )
{
	if((*(itEnd)).p.x > (*itBegin).p.x) return false;
	if((*(itEnd)).p.x < (*itBegin).p.x) return true;
	if((*(itEnd)).p.y > (*itBegin).p.y) return false;
	return true;
}

/******************************************************
*
*/
void RSVerticalFace::SetClose( bool clo )
{
	close = clo;
}
/******************************************************
*
*/
bool RSVerticalFace::IsClose( )
{
	return close;
}
/******************************************************
*
*/
void RSVerticalFace::SetLowZ( float lowZ )
{
	m_lowZ = lowZ;
}
/******************************************************
*
*/
void RSVerticalFace::SetHighZ( float highZ )
{
	m_higZ = highZ;
}
/******************************************************
*
*/
void RSVerticalFace::push( const RSTdPoint& tdp )
{
	RSVerticalFaceNode node;
	node.p = tdp;
	m_vface.push_back( node );
}
/******************************************************
*
*/
float RSVerticalFace::GetHighZ( )
{
	return m_higZ;
}
/******************************************************
*
*/
float RSVerticalFace::GetLowZ( )
{
	return m_lowZ;
}
/******************************************************
*
*/
void RSVerticalFace::InterWithLines( const RSLine& line )
{
	m_nodeIndex = 0;
	vector<RSVerticalFaceNode>::iterator vfStartIt = m_vface.begin( );
	vector<RSVerticalFaceNode>::iterator vfEndIt = vfStartIt + 1;

	while( vfEndIt != m_vface.end( ) )
	{
        JudgeDire( vfStartIt,vfEndIt );
		vfStartIt->nodeIndex = m_nodeIndex;
		line.intersect( *vfStartIt,*vfEndIt,m_lowZ,m_higZ );

		vfStartIt = vfEndIt;
		++vfEndIt;
		++m_nodeIndex;
	}

	line.eraseExtreme( );
}
/******************************************************
*
*/


































