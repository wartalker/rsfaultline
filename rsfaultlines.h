#ifndef RSFAULTLINES_H
#define RSFAULTLINES_H

#include <list>
#include <vector>
#include <algorithm>
#include <qstring.h>
#include <iterator>

#include "rsgridface.h"
#include "rsverticalface.h"


using namespace std;


typedef list< RSLine > LinesList;
typedef vector<RSLineNode>::iterator LineNodeIterator;
 
class RSFaultLines
{

private:
	LinesList m_linesList;//Broken Line
	RSInterNodes m_IntersFace;// Point of intersection
	unsigned int m_LineID;//Broken Line ID

	LinesList::iterator findID( unsigned int lineID );//The specify Line of ID

	RSFaultLines( const RSFaultLines& );//No Copy
	RSFaultLines& operator = ( const RSFaultLines& );//No assign operator

public:

	RSFaultLines( );
	~RSFaultLines( );

	int readFile( const QString& strName );// Read data from file
	int saveFile( const QString& strName );//Save data to file
	void intersectFace(  RSGridFace& face );//Intersect with GridFace
	void intersectFace(  RSVerticalFace& face );//Intersect with VerticalFace
	void clear( );
	
	unsigned int  nearestLine( const RSTdPoint& point,float& distance );//The nearest segment
	RSLine& refNearestLine( const RSTdPoint& point, float& distance );//The nearest segment
	RSLine* ptrLineID( unsigned int lineID );//The pointer to IDLine

	inline void erase( unsigned int lineID ) { this->m_linesList.erase( findID( lineID ) ); } //Erase the specify Line
	void push(  const RSLine& line );//Push the specify RSLine

	inline int size( ) { return this->m_linesList.size( ); }//The num of Line
	RSLine& operator[ ]( int n );//[ ]No boundary check

	inline RSInterNodes& getInters( ){ return m_IntersFace; }//The vector for Points of intersect 
	

	/* RSLine Interface

	RSLine( int size = 0 );//size is the number of nodes

	void SetClose(bool clo);//
	bool IsClose( );//
	unsigned int getID( );// 

	LineNodeIterator nearestNode( const RSTdPoint& tdp, float& distance );//

	void push( const RSTdPoint& tdp, RSPoint_Type type = POINT_USER_DEFINE );//
	void erase( LineNodeIterator where );//
	void insert( LineNodeIterator where, const RSTdPoint& point, RSPoint_Type type = POINT_USER_DEFINE );//
	LineNodeIterator getPoint( float x, float y, float z );//
	LineNodeIterator getPoint( const RSTdPoint& tdp );//
	void setPoint(LineNodeIterator where, float x, float y, float z );//
	void setPoint( LineNodeIterator where, const RSTdPoint& tdp );//
	void setType( LineNodeIterator where,RSPoint_Type type = POINT_USER_DEFINE );//
	const getType( LineNodeIterator where );//

	const RSSegment& getNearestSegment(  );//

	RSLine( const vector< pair< RSTdPoint,RSPoint_Type > >& vec );//
	*/



	/* RSVerticalFace Interface

	RSVerticalFace( );
	void SetLowZ( float lowZ  );
	void SetHighZ( float highZ );
	void push( const RSTdPoint& tdp );

    RSVerticalFace( RSTdPoint node[], int n, float lowZ,float highZ );
	
	float GetLowZ( );
	float GetHighZ( );
	*/


	/* RSGridFace Interface
    RSGridFace( RSGridFaceRect rect, const RSGridFaceZ& z );
	*/

};






//Predicate of RSLine ID
struct RSLine_ID_Equal : public unary_function< RSLine,bool >
{
	RSLine_ID_Equal( unsigned int lineID ):id(lineID){ }
	bool operator( )( const RSLine& rline ) { return ( rline.getID( ) == id ); }

private:
	unsigned int id;
};


#endif //RSFAULTLINES_H