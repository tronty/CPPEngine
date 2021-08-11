#ifndef TRANSFORMGROUP_H

#define TRANSFORMGROUP_H



#include <Geometry/DistanceObject.h>

#include <Viewer/Frustum.h>

#include <Nodes/Transform3D.h>

#include "Shape.h"

//include "../Parsers/IOXMLObject.h"

//include "../Tools/NamedObject.h"



typedef DistanceObject<TransformGroup*> DistanceObjectTGP;

typedef vector<DistanceObjectTGP>       DistanceBufferTGP;



  class TransformGroup : public NamedObject, public IOXMLObject

{

  public:

    TransformGroup();

   ~TransformGroup(){destroy();};

    TransformGroup(const Transform3D &);



    virtual bool loadXMLSettings(XMLElement *element);

    virtual bool exportXMLSettings(ofstream &xmlFile);



    bool loadXMLShape(XMLElement *node);



    void renderSilhouettes(D3DXFROMWINEVECTOR3 &viewPosition, Frustum *frustum = NULL);

    void transformToWorldSpace(BoundsDescriptor &descriptor);

    void transformToObjectSpace(D3DXFROMWINEVECTOR3 &tuple);

    void transformToWorldSpace(D3DXFROMWINEVECTOR3 &tuple);

    void updateBoundsDescriptor();

    void renderAABB();

    void renderOBB();



    void renderLightFrustum(D3DXFROMWINEVECTOR3 &lightPosition);

    void setTransform(const Transform3D &t);

    void setOccluder(bool);

    void undoTransform();

    void doTransform();



    void renderShadowVolumes(const D3DXFROMWINEVECTOR3& lightPosition,

                             float          lightRange,

                             int            renderingFormat);



    void setParent(TransformGroup *);

    bool addChild(TransformGroup *child);



    TransformGroup *removeGroup(const char *name);

    TransformGroup *removeGroup(int        index);



    bool addChild(Shape *child);

    void setVisible(bool);

    void compile();



    float getEstimatedVolumeSize();

    int   render(int renderingFormat, Frustum *frustum = NULL, const uint flags=RESET_ALL);

    int   getShapesCount();

    int   getGroupsCount();



    BoundsDescriptor        &updateAndGetBoundsDescriptor();

    BoundsDescriptor        &getBoundsDescriptor();



    vector<TransformGroup*> &getGroups();

    vector<Shape*>          &getShapes();

    TransformGroup          *getParent();

    TransformGroup          *getGroup(const char* groupName);

    TransformGroup          *getGroup(int index);

    Transform3D             &getTransform();

    Shape                   *getShapeByPickingIndex(int pickingIndex, Frustum *culling = NULL);

    Shape                   *pickClosest(const Tuple2i& mouse,

                                         const Tuple2i& pickingAreaSize,

                                         const D3DXFROMWINEVECTOR4 &fovAspectNear,

                                         Frustum *cull = NULL);

    Shape                   *getShape(int index);



    const D3DXFROMWINEMATRIX &getWorldTransform() const;

    bool isOccluder(){ return occluder; }

    bool isVisible() { return visible;  }

    void destroy();



  private:

    static DistanceBufferTGP groupsObjectList;



    vector<TransformGroup*>  groups;

    vector<Shape*>           shapes;



    BoundsDescriptor         boundsDescriptor;

    TransformGroup          *parent;

    Transform3D              transform;
	static std::vector<D3DXFROMWINEMATRIX> s_worldTransformV;

    D3DXFROMWINEMATRIX                 worldTransform;

    bool                     applyWorldTransform,

                             visibleOccluder,

                             occluder,

                             visible;



    void  renderShadowVolumesPrivate(const D3DXFROMWINEVECTOR3& lightPosition,

                                     float lightRange,

                                     int   renderingFormat);

    void  collectTransformGroups(TransformGroup *group,

                                 Frustum         *frustum = NULL);

    void  updateWorldTransform();

    void  setupLightFrustum(const D3DXFROMWINEVECTOR3 &lightPosition);

    void  pickClosest(Frustum *cullingFrustum);



    int   renderOrdered(const    D3DXFROMWINEVECTOR3 &eyePosition,

                        int      renderingFormat,

                        int      order,

                        Frustum *frustum = NULL);



};



#endif
