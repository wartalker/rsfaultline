#ifndef RSVERTICALFACE_H
#define RSVERTICALFACE_H

#include <vector>
#include "rshead.h"
#include "rsline.h"

using namespace std;

class RSVerticalFace
{
private:
	vector< RSVerticalFaceNode > m_vface;//
	bool close;
	float m_lowZ;//the Lower Z Coordinate of face
	float m_higZ;//the Higher Z Coordinate of face 
	unsigned int m_nodeIndex;//the ID of the node

public:
	RSVerticalFace( );
	RSVerticalFace( RSTdPoint node[ ], int n, float lowZ,float highZ );
	~RSVerticalFace( );


	void SetClose( bool clo );
	bool IsClose( );
	void SetLowZ( float lowZ  );
	void SetHighZ( float highZ );
	void push( const RSTdPoint& tdp );
	float GetLowZ( );
	float GetHighZ( );
	vector< RSVerticalFaceNode >& GetLine( );
	bool JudgeDire( vector<RSVerticalFaceNode>::iterator itBegin, vector< RSVerticalFaceNode>::iterator itEnd );

	void InterWithLines( const RSLine& line );//


};


#endif //