/*****************************************************************************************
/* Desc: Parse collision files
/*****************************************************************************************/

#ifndef _COLLISIONPARSER_
#define _COLLISIONPARSER_

// ----- Includes -----

#include <list>


// --------------------------------------------------------------------------------
//									CollisionParser
// --------------------------------------------------------------------------------

class CollisionParser 
{
public:
	
	static CollisionParser* Instance();

	// ----- Methods -----

	bool ParseCollision			(list <BOUNDING_COLLISION*> *pBList, const char *pFile);
	void SetBoundingTriangle	(list <BOUNDING_COLLISION*> *pBList, const char *pId, int pAx, int pAy, int pBx, int pBy, int pCx, int pCy);
	void SetBoundingCircle		(list <BOUNDING_COLLISION*> *pBList, const char *pId, int pOffsetX, int pOffsetY, int pRadius);
	void SetBoundingRectangle	(list <BOUNDING_COLLISION*> *pBList, const char *pId, int pOffsetX, int pOffsetY, int pWidth, int pHeight);
	void DeleteBoundingAreas	(list <BOUNDING_COLLISION*> *pBList, const char *pId);

protected:

	CollisionParser()  {}
	CollisionParser(const CollisionParser&);
	CollisionParser& operator = (const CollisionParser&);

private:
	static CollisionParser *pinstance;
};


#endif // _COLLISIONPARSER_
