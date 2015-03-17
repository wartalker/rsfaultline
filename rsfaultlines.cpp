#include <qfile.h>
#include <qdatastream.h> 
#include <qstring.h>
#include <math.h>
#include "rsfaultlines.h"

using namespace std;


/***********************************************************************
*
*/
RSFaultLines::RSFaultLines( )
           :m_linesList( ),m_IntersFace( ),m_LineID( 0 )
{  }

/***********************************************************************
*
*/
RSFaultLines::~RSFaultLines( )
{  }

/***********************************************************************
*[ ]
*/
RSLine& RSFaultLines::operator[ ]( int n )
{
	LinesList::iterator lineIt = m_linesList.begin( );
	advance( lineIt, n );
	if ( lineIt != m_linesList.end( ) )
		return *lineIt;
	else
		return m_linesList.back( );		
}

/***********************************************************************
*
*/
void RSFaultLines::clear( )
{
	m_linesList.clear( );
	m_IntersFace.clear( );
	m_LineID = 0;
}
/***********************************************************************
*
*/
RSLine* RSFaultLines::ptrLineID( unsigned int lineID )
{
	LinesList::iterator lineIt = findID( lineID );
	if ( lineIt != m_linesList.end( ) )
	{
		return &(*lineIt);
	}

	return NULL;
}


/***********************************************************************
*
*/
//void RSFaultLines::erase( unsigned int lineID )
//{
//	  this->m_linesList.erase( findID( lineID ) );
//};

/************************************************************************
*
*/
void RSFaultLines::push( const RSLine& line )
{
   line.setID( m_LineID );
   m_linesList.push_back( line );
   ++m_LineID;
}

/****************************************************************************
*
*/
LinesList::iterator  RSFaultLines::findID( unsigned int lineID )
{	
	return find_if( m_linesList.begin( ), m_linesList.end( ),RSLine_ID_Equal(lineID) );
}

/************************************************************************
*
*/
unsigned int  RSFaultLines::nearestLine( const RSTdPoint& point,float& distance )
{
	float p2LinesDis = 0;
	float minDis = 0;

	LinesList::iterator it = m_linesList.begin( );
	LinesList::iterator minDisIt = m_linesList.begin( );
	p2LinesDis = minDis = minDisIt->disP2Lines( point );

	while( it != m_linesList.end( ) )
	{
		p2LinesDis = it->disP2Lines( point );
		if( minDis > p2LinesDis )
		{
			minDis = p2LinesDis;
			minDisIt = it;
		}
		++it;
	}

	distance = minDis;
	return minDisIt->getID( );
}
/************************************************************************
*
*/
RSLine& RSFaultLines::refNearestLine( const RSTdPoint& point, float& distance )
{
	float p2LinesDis = 0;
	float minDis = 0;

	LinesList::iterator it = m_linesList.begin( );
	LinesList::iterator minDisIt = m_linesList.begin( );
	p2LinesDis = minDis = minDisIt->disP2Lines( point );

	while( it != m_linesList.end( ) )
	{
		p2LinesDis = it->disP2Lines( point );
		if( minDis > p2LinesDis )
		{
			minDis = p2LinesDis;
			minDisIt = it;
		}
		++it;
	}

	distance = minDis;
	return *minDisIt;
}
/***********************************************************************
*
*/
int RSFaultLines::readFile( const QString& strName )
{
	Q_UINT32 linesCount = 0;
	Q_UINT32 lineID = 0;
	Q_UINT32 nodesCount = 0;
	Q_UINT32 iClose = 0;
	QString sFileBegin;
	QString sLineBegin;
	bool close = false;
	double x = 0;
	double y = 0;
	double z = 0;
	int i = 0;
	int j = 0;
	
	QFile file( strName );

	m_linesList.clear( );

	if( file.open( IO_ReadOnly ) )
	{
	
		QDataStream stream( &file );

		stream >> sFileBegin >> linesCount >> m_LineID;

		if ( sFileBegin != "RSLines Total Count:" )
		{
			file.close( );
			return 1;
		}

		for( i = 0; i < linesCount; ++i )
		{
			stream >> sLineBegin >> lineID >> nodesCount >> iClose;
			
			if ( sLineBegin != "RSLine Nodes Count:" )
			{
				m_linesList.clear( );
				file.close( );
				return 2;
			}

			close = false;
			if( iClose == 1 )
				close = true;

			RSLine line( nodesCount );
			line.setID( lineID );
			line.SetClose( close );

			for(  j = 0; j < nodesCount; ++j )
			{				
				stream >> x >> y >> z;	

				RSTdPoint tdp( x, y, z);
				RSLineNode vn;
				vn.p = tdp;
				line.GetLine( ).push_back( vn );
			}

			m_linesList.push_back( line );

		}
	}
	else
	{
		return 3;
	}

   file.close( );

   return 0;
}

/*************************************************************************
*
*/
int RSFaultLines::saveFile( const QString& strName )
{	

	Q_UINT32 linesCount = 0;
	Q_UINT32 lineID = 0;
	Q_UINT32 nodesCount = 0;
	Q_UINT32 iClose = 0;
	QString sFileBegin( "RSLines Total Count:" );
	QString sLineBegin( "RSLine Nodes Count:" );

	QFile file( strName );


	if ( file.open( IO_WriteOnly ) ) 
	{
		QDataStream stream( &file );

		linesCount = m_linesList.size( );

		stream << sFileBegin;//String of The File
		stream <<  linesCount;//Count of the Line
		stream << m_LineID;
		
		for( LinesList::iterator  lineIt = m_linesList.begin( ); lineIt != m_linesList.end( ); ++lineIt)//±éÀú
		{
			
			nodesCount = lineIt->GetLine( ).size( );
			lineID = lineIt->getID( );
			bool close = lineIt->IsClose( );

			if( close )
				iClose = 1;
			else
				iClose = 0;

			stream << sLineBegin;//String of Line 
			stream << lineID; //Line ID
			stream <<  nodesCount << iClose;//Number of LineNode

			for( vector< RSLineNode >::const_iterator nodeIt = lineIt->GetLine( ).begin( ); nodeIt != lineIt->GetLine( ).end( ); ++nodeIt )//±éÀú
			{
				stream << (nodeIt->p).x  << (nodeIt->p).y << (nodeIt->p).z;//Coordinate of Point
			}
		}
	}	
	else
	{
        return 2;
	}

	file.close( );

	return 0;
}


/*******************************************************************************
*Intersection with GridFace
*/
void RSFaultLines::intersectFace( RSGridFace& face )
{	
	m_IntersFace.clear( );

	for( LinesList::iterator  lineIt = m_linesList.begin( ); lineIt != m_linesList.end( ); ++lineIt )
	{
		face.InterWithLines( *lineIt );
		m_IntersFace.push_back( lineIt->GetInterWithFace( ) );
		lineIt->GetInterWithFace( ).clear( );
	}

}

/********************************************************************************
*Intersection with VerticalFace 
*/
void RSFaultLines::intersectFace( RSVerticalFace& face )
{
	m_IntersFace.clear( );

	for( LinesList::iterator  lineIt = m_linesList.begin( ); lineIt != m_linesList.end( ); ++lineIt )
	{

		face.InterWithLines( * lineIt );
		m_IntersFace.push_back( lineIt->GetInterWithFace( ) );
		lineIt->GetInterWithFace( ).clear( );
	}
}













/*********************************************************************************
*Class [RSInterNodes]:: nearest InterNode
*/


RSInterNode& RSInterNodes::nearestInterNode( const RSTdPoint& tdp, double& distance, int& index  )
{
	double dis = 0;
	double minDis = 0;
	vector< RSInterNode >::iterator nodeIt = m_interNodes.begin( );
	vector< RSInterNode >::iterator minDisIt = m_interNodes.begin( );
	minDis = Distance( tdp,nodeIt->p );
	dis = minDis;
	++nodeIt;
	while ( nodeIt != m_interNodes.end( ) )
	{
		dis = Distance( tdp,nodeIt->p );
		if ( minDis > dis )
		{
			minDis = dis;
			minDisIt = nodeIt;
		}
		++nodeIt;
	}
	distance = minDis;
	index = std::distance( m_interNodes.begin( ), minDisIt ) - 1;
	return *minDisIt;
}



/*********************************************************************************
*
*/
void RSInterNodes::eraseAutoGenerateInter( )
{
	vector<RSInterNode>::iterator it;
	it = find( m_interNodes.begin( ) );

	while ( it != m_interNodes.end( ) )
	{
		it->isContact = isContact( it );
		it = find( it + 1 );
	}

	m_interNodes.erase( remove_if( m_interNodes.begin( ), m_interNodes.end( ),RSInterNode_AutoGenerateInter( ) ),m_interNodes.end( ) );

}

/*********************************************************************************
*
*/
vector<RSInterNode>::iterator RSInterNodes::find( vector<RSInterNode>::iterator current )
{
	while ( (current+1) != m_interNodes.end( ) )
	{
		if ( (current->type != POINT_AUTO_GENERATE) && (current->isContact) && ((current+1)->type == POINT_AUTO_GENERATE) )
			return current;
		++current;
	}
	return m_interNodes.end( );
}

/*********************************************************************************
*
*/
bool RSInterNodes::isContact( vector<RSInterNode>::iterator current )
{
	vector<RSInterNode>::iterator it = current + 1;
	while ( it != m_interNodes.end( ) )
	{
		if ( !it->isContact )
			return false;
		if ( it->type != POINT_AUTO_GENERATE )
			return true;
		++it;
	}
	return true;
}














