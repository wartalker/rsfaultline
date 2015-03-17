#ifndef RSGRIDFACE_H
#define RSGRIDFACE_H

#include <vector>
#include <math.h>
#include <algorithm>
#include <qwmatrix.h>

#include "rsline.h"
#include "rshead.h"

using namespace std;


class RSGridFace
{

private:
	vector< vector<float> >  m_z;//Z Coordinate 

	float	m_leftX;					//the Left Coordinate of X
	float	m_bottomY;					//the Low Coordinate of Y
	int		m_numOfGridX;				//the number of Grid in X
    int		m_numOfGridY;				//the number of Grid in Y
	float   m_lengthOfGridX;			//the length of Grid in X
	float   m_lengthOfGridY;			//the length of Grid in Y

	QWMatrix m_InvMatrix;
	QWMatrix m_RevMatrix;

	float	GetZ( RSTdPoint a , RSTdPoint b , RSTdPoint c , float x , float y );
	int		GetDiagonal( RSTdPoint vertex[ ] );
	void	CalVerticalInter( const RSLineNode & startNode , const RSLineNode & endNode , int leftIndex , int rightIndex );
	void    CalHorizontalInter( const RSLineNode & startNode ,  const RSLineNode & endNode , int lowerIndex , int upperIndex );
	float GetAng( const RSGridFaceRectNode&, const RSGridFaceRectNode& );
	float GetLength( const RSGridFaceRectNode&, const RSGridFaceRectNode& );
	void Invert( RSLineNode& );
	void Reverse( RSTdPoint& ) const;
	void Reverse( const RSLine& l ) const;

	float	CalZ( const RSLineNode & endingNode );	
	void	CalFaceWithLineInter( const RSLineNode& startNode, const RSLineNode& endNode);

public:

	RSGridFace( RSGridFaceRect rect, const RSGridFaceZ& z );
	~RSGridFace(void);

	void	InterWithLines( const RSLine& l );//

	float	GetLeftX();					//
	float	GetBottomY();				//
	int		GetNumOfGridX();			//
	int		GetNumOfGridY();			//
	float   GetLengthOfGridX();			//
	float   GetLengthOfGridY();			//

	vector< vector<float> >& GetZ( );
	vector< pair< float, float > > m_segOnFace;

};

//Arrange the Point Increment X
struct IncreX
{
	bool operator( )(const pair<float,float>& a , const pair<float,float>& b)
	{
		if( a.second > b.second ) return true;
		return false;
	}
};

//Arrange the Point Decrement X
struct DecreX
{
	bool operator( )( const pair<float,float>&a, const pair<float,float>& b )
	{
		if ( a.second < b.second ) return true;
		return false;
	}
};


#endif //FACE_H