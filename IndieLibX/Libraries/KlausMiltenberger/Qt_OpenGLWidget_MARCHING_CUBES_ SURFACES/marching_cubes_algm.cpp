/*
 * mc_alg.cpp
 *  Last Changes: 06.05.2018
 *  Author: manitoo
 *  Class MC_ALG encapsulates MARCHING_CUBES_ALGORITHM
 */

#include "marching_cubes_algm.h"

//Constructor
MC_ALG::MC_ALG(GLint Resolution)
{
    qDebug() << "      [ MC-ALG  constructed        | Main-TID: " << QThread::currentThreadId() << " ]";

    //Alg. Resolution , Stepsize
    iDataSetSize    = Resolution;
    fStepSize       = 1.0/iDataSetSize;
}

//Destructor
MC_ALG::~MC_ALG()
{
    qDebug() << "\n      [ MC-ALG   destructed    | Main-TID: " << QThread::currentThreadId() << " ]";
}

/* ----------------------------- Sample Functions ----------------------------- */
//fSample1 shows a Potential Surface
GLfloat MC_ALG::fSample1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fHeight = 0.0, dX = 0.5-fX, dY = 0.5-fY, dZ = 0.5-fZ;
    //Each Dodecahedron contributes to Potential
    for(size_t i = 0; i < sizeof(D)/sizeof(D[0]); i++)
    {
        fHeight += 1.0/sqrt((dX+D[i][0])*(dX+D[i][0])+(dY+D[i][1])*(dY+D[i][1])+(dZ+D[i][2])*(dZ+D[i][2]));
    }
    return 0.0397*fHeight;
}

//fSample2 shows a Toroidal Surface
GLfloat MC_ALG::fSample2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat a = 2.5, fscale = 7.0, dX = fscale*(0.5-fX),  dY = fscale*(0.5-fY),  dZ = fscale*(0.5-fZ);
    return(pow(sqrt(pow(dX,2)+pow(dY,2))-a,2)+pow(dZ,2))*(pow(sqrt(pow(dY,2)+pow(dZ,2))-a,2)+pow(dX,2))*(pow(sqrt(pow(dZ,2)+pow(dX,2))-a,2)+pow(dY,2));
}

//fSample3 shows a Double Torus
GLfloat MC_ALG::fSample3(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 2.25, dX = fscale*(0.5-fX),  dY = fscale*(0.5-fY),  dZ = fscale*(0.5-fZ);
    return pow(dX,8)+pow(dX,4)-2*pow(dX,6)-2*pow(dX,2)*pow(dY,2)+2*pow(dX,4)*pow(dY,2)+pow(dY,4)+pow(dZ,2);
}

//fSample4 shows a Chmutov Surface-12 (http://mathworld.wolfram.com)
GLfloat MC_ALG::fSample4(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 2.75, dX = fscale*(0.5-fX),  dY = fscale*(0.5-fY),  dZ = fscale*(0.5-fZ);
    return 8*(pow(dX,2)+pow(dY,2)+pow(dZ,2))-8*(pow(dX,4)+pow(dY,4)+pow(dZ,4));
}

//fSample5 shows a Chmutov Surface-2 (http://mathworld.wolfram.com)
GLfloat MC_ALG::fSample5(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 2.75, dX = fscale*(0.5-fX),  dY = fscale*(0.5-fY),  dZ = fscale*(0.5-fZ);
    return 2*(pow(dX,2)*pow(3-4*pow(dX,2),2)+pow(dY,2)*pow(3-4*pow(dY,2),2)+pow(dZ,2)*pow(3-4*pow(dZ,2),2));
}

//fGetOffset finds the approximate point of intersection of the surface
// between two points with the values fValue1 and fValue2
GLfloat MC_ALG::fGetOffset(const GLfloat &fValue1, const GLfloat &fValue2, const GLfloat &fValueDesired)
{
        double fDelta = fValue2 - fValue1;

        if(fDelta == 0.0)
        {
                return 0.5;
        }
        return (fValueDesired - fValue1)/fDelta;
}

GLvoid MC_ALG::vNormalizeVector(GLvector &rfVectorResult, GLvector &rfVectorSource)
{
        GLfloat fOldLength;
        GLfloat fScale;

        fOldLength = sqrtf( (rfVectorSource.fX * rfVectorSource.fX) +
                            (rfVectorSource.fY * rfVectorSource.fY) +
                            (rfVectorSource.fZ * rfVectorSource.fZ) );

        if(fOldLength == 0.0)
        {
                rfVectorResult.fX = rfVectorSource.fX;
                rfVectorResult.fY = rfVectorSource.fY;
                rfVectorResult.fZ = rfVectorSource.fZ;
        }
        else
        {
                fScale = 1.0/fOldLength;
                rfVectorResult.fX = rfVectorSource.fX*fScale;
                rfVectorResult.fY = rfVectorSource.fY*fScale;
                rfVectorResult.fZ = rfVectorSource.fZ*fScale;
        }
}

//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
GLvoid MC_ALG::vGetNormal(GLvector &rfNormal, const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
        rfNormal.fX = (this->*fSample)(fX-0.01, fY, fZ) - (this->*fSample)(fX+0.01, fY, fZ);
        rfNormal.fY = (this->*fSample)(fX, fY-0.01, fZ) - (this->*fSample)(fX, fY+0.01, fZ);
        rfNormal.fZ = (this->*fSample)(fX, fY, fZ-0.01) - (this->*fSample)(fX, fY, fZ+0.01);
        vNormalizeVector(rfNormal, rfNormal);
}

/* --------------------------- Space Scanning --------------------------- */
GLvoid MC_ALG::vMarchingCubes(GLint level_set, GLint partition)
{
    GLfloat fTv;    //Targetvalue
    switch ( level_set )
    {
        case 1: { fSample = &MC_ALG::fSample1; fTv = 2.465; break;}
        case 2: { fSample = &MC_ALG::fSample2; fTv = 3.595; break;}
        case 3: { fSample = &MC_ALG::fSample3; fTv = 0.015; break;}
        case 4: { fSample = &MC_ALG::fSample4; fTv = 3.000; break;}
        case 5: { fSample = &MC_ALG::fSample5; fTv = 3.000; break;}
        default:    break;
    }

     switch ( partition )
    {
        case 1: { vMarchCube = &MC_ALG::vMarchCube1; break;}
        case 2: { vMarchCube = &MC_ALG::vMarchCube2; break;}
        default:    break;
    }


    for(GLint iX = 0; iX < iDataSetSize; iX++)
        for(GLint iY = 0; iY < iDataSetSize; iY++)
            for(GLint iZ = 0; iZ < iDataSetSize; iZ++)
            {
                (this->*vMarchCube)(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize, fTv);
            }
}


//vMarchCube1 performs the Marching Cubes algorithm on a single cube
GLvoid MC_ALG::vMarchCube1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv)
{
        GLint iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        GLfloat fOffset;
        GLfloat afCubeValue[8];
        GLvector asEdgeVertex[12];
        GLvector asEdgeNorm[12];

        //Make a local copy of the values at the cube's corners
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
            afCubeValue[iVertex] = (this->*fSample)(fX + a2fVertexOffset[iVertex][0]*fScale,fY + a2fVertexOffset[iVertex][1]*fScale,fZ + a2fVertexOffset[iVertex][2]*fScale);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(afCubeValue[iVertexTest] <= fTv)     iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                return;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 12; iEdge++)
        {
            //if there is an intersection on this edge
            if(iEdgeFlags & (1<<iEdge))
            {
                fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ],afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTv);

                asEdgeVertex[iEdge].fX = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
                asEdgeVertex[iEdge].fY = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
                asEdgeVertex[iEdge].fZ = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

                vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
            }
}

        //Draw the triangles that were found.  There can be up to five per cube
        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
            if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0) break;

            for(iCorner = 0; iCorner < 3; iCorner++)
            {
                iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

                glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
            }
        }
}

//vMarchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to vMarchTetrahedron
GLvoid MC_ALG::vMarchCube2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv)
{
        GLint iVertex, iTetrahedron, iVertexInACube;
        GLvector asCubePosition[8];
        GLfloat  afCubeValue[8];
        GLvector asTetrahedronPosition[4];
        GLfloat  afTetrahedronValue[4];

        //Make a local copy of the cube's corner positions
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                asCubePosition[iVertex].fX = fX + a2fVertexOffset[iVertex][0]*fScale;
                asCubePosition[iVertex].fY = fY + a2fVertexOffset[iVertex][1]*fScale;
                asCubePosition[iVertex].fZ = fZ + a2fVertexOffset[iVertex][2]*fScale;
        }

        //Make a local copy of the cube's corner values
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
            afCubeValue[iVertex] = (this->*fSample)(asCubePosition[iVertex].fX,asCubePosition[iVertex].fY,asCubePosition[iVertex].fZ);
        }

        for(iTetrahedron = 0; iTetrahedron < 6; iTetrahedron++)
        {
            for(iVertex = 0; iVertex < 4; iVertex++)
            {
                iVertexInACube = a2iTetrahedronsInACube[iTetrahedron][iVertex];
                asTetrahedronPosition[iVertex].fX = asCubePosition[iVertexInACube].fX;
                asTetrahedronPosition[iVertex].fY = asCubePosition[iVertexInACube].fY;
                asTetrahedronPosition[iVertex].fZ = asCubePosition[iVertexInACube].fZ;
                afTetrahedronValue[iVertex] = afCubeValue[iVertexInACube];
            }
            vMarchTetrahedron(asTetrahedronPosition, afTetrahedronValue, fTv);
        }
}


//vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
GLvoid MC_ALG::vMarchTetrahedron(GLvector *pasTetrahedronPosition, GLfloat *pafTetrahedronValue, GLfloat fTv)
{
        GLint iEdge, iVert0, iVert1, iEdgeFlags, iTriangle, iCorner, iVertex, iFlagIndex = 0;
        GLfloat fOffset, fInvOffset;
        GLvector asEdgeVertex[6];
        GLvector asEdgeNorm[6];

        //Find which vertices are inside of the surface and which are outside
        for(iVertex = 0; iVertex < 4; iVertex++)
        {
                if(pafTetrahedronValue[iVertex] <= fTv)     iFlagIndex |= 1<<iVertex;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiTetrahedronEdgeFlags[iFlagIndex];

        //If the tetrahedron is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                return;
        }
        //Find the point of intersection of the surface with each edge
        // Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 6; iEdge++)
        {
            //if there is an intersection on this edge
            if(iEdgeFlags & (1<<iEdge))
            {
                iVert0 = a2iTetrahedronEdgeConnection[iEdge][0];
                iVert1 = a2iTetrahedronEdgeConnection[iEdge][1];
                fOffset = fGetOffset(pafTetrahedronValue[iVert0], pafTetrahedronValue[iVert1], fTv);
                fInvOffset = 1.0 - fOffset;

                asEdgeVertex[iEdge].fX = fInvOffset*pasTetrahedronPosition[iVert0].fX  +  fOffset*pasTetrahedronPosition[iVert1].fX;
                asEdgeVertex[iEdge].fY = fInvOffset*pasTetrahedronPosition[iVert0].fY  +  fOffset*pasTetrahedronPosition[iVert1].fY;
                asEdgeVertex[iEdge].fZ = fInvOffset*pasTetrahedronPosition[iVert0].fZ  +  fOffset*pasTetrahedronPosition[iVert1].fZ;

                vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
            }
        }
        //Draw the triangles that were found.  There can be up to 2 per tetrahedron
        for(iTriangle = 0; iTriangle < 2; iTriangle++)
        {
            if(a2iTetrahedronTriangles[iFlagIndex][3*iTriangle] < 0)    break;

            for(iCorner = 0; iCorner < 3; iCorner++)
            {
                iVertex = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+iCorner];

                glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
            }
        }
}
