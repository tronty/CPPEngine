#include "TransformGroup.h"

//include "../Renderer/Renderer.h"

#include "../Tools/Factory3DS.h"

#ifdef _INCLUDE_SCENEFRAME_
#include "../SceneFrame.h"
#endif


#if 1

#define  NV_SHADOW_PASS  0x00001

#define  JOE_SHADOW_PASS 0x00002

#define  ATI_SHADOW_PASS 0x00004





DistanceBufferTGP TransformGroup::groupsObjectList;

D3DXFROMWINEVECTOR3	      corners[4];

Planef		  lightFrustum[5];


std::vector<D3DXFROMWINEMATRIX> TransformGroup::s_worldTransformV;

TransformGroup::TransformGroup() : IOXMLObject("Group")

{
	worldTransform=identity4();

	applyWorldTransform = false;

  visibleOccluder     = true;

  occluder	      = false;

  visible	      = true;

  parent	      = NULL;

}



TransformGroup::TransformGroup(const Transform3D &t)  : IOXMLObject("Group")

{

  setTransform(t);

}



int TransformGroup::render(int renderingFormat, Frustum *frustum, const uint flags)

{

  size_t index = 0;

  int	 order = (renderingFormat & FRONT_TO_BACK) ? FRONT_TO_BACK :

		 (renderingFormat & BACK_TO_FRONT) ? BACK_TO_FRONT : 0,

	 tc    = 0;



  if(!visible || (renderingFormat & CULL_OCCLUDERS && !visibleOccluder))

    return tc;



  if(order)

  {

    TransformGroup *currentGroup   = NULL;

	D3DXFROMWINEVECTOR3	newEyePosition = IRenderer::GetRendererInstance()->GetEyePt(),

		    groupCenter;



    size_t	    shapesCount   = 0,

		    i		  = 0;



    float	    coeff	  = (order == FRONT_TO_BACK) ? 1.0f : -1.0f;



    groupsObjectList.clear();

    collectTransformGroups(this, frustum);



    for(i = 0; i < groupsObjectList.size(); i++)

    {

      currentGroup = groupsObjectList[i].getObject();

      currentGroup->updateBoundsDescriptor();



      groupCenter = currentGroup->boundsDescriptor.getCenterOBB();

      groupsObjectList[i].setDistance(getDistance(coeff*groupCenter,newEyePosition));

    }


	#if 0//ndef __APPLE__
    sort(groupsObjectList.begin(), groupsObjectList.end());
	#endif


    for(i = 0; i < groupsObjectList.size(); i++)

    {

      currentGroup  = groupsObjectList[i].getObject();



      if(!currentGroup->visible)

	continue;



      shapesCount = currentGroup->shapes.size();

      currentGroup->doTransform();



      for(size_t index = 0; index <shapesCount; index++)

	tc += currentGroup->shapes[index]->draw(renderingFormat,flags);



      currentGroup->undoTransform();

    }

  }

  else

  {

    updateBoundsDescriptor();



    if(frustum)

      if(!frustum->AABBInFrustum(boundsDescriptor))

	return 0;



     doTransform();



     for(index = 0; index < shapes.size(); index++)

       tc +=shapes[index]->draw(renderingFormat);



     undoTransform();



     for(index = 0; index < groups.size(); index++)

       tc += groups[index]->render(renderingFormat, frustum, RESET_ALL);

  }

  return tc;

}



void TransformGroup::renderSilhouettes(D3DXFROMWINEVECTOR3 &viewPosition,

				       Frustum *frustum)

{

  D3DXFROMWINEVECTOR3 viewPos(viewPosition);

  size_t  index 	= 0;



  transformToObjectSpace(viewPos);



  if(frustum)

    if(!frustum->AABBInFrustum(updateAndGetBoundsDescriptor()))

       return;



  doTransform();



  for(index = 0; index < shapes.size(); index++)
  {
    D3DXFROMWINEVECTOR4 v(viewPos.x, viewPos.y, viewPos.z, 0.0f);
    shapes[index]->renderSilhouette(v);
  }



  undoTransform();



  for(index = 0; index < groups.size(); index++)

    groups[index]->renderSilhouettes(viewPosition, frustum);

}

void TransformGroup::renderAABB()

{

  if(updateAndGetBoundsDescriptor().isInitialized())

    boundsDescriptor.render(BoundsDescriptor::WIRE | BoundsDescriptor::AABB);



  for(size_t g = 0; g < groups.size(); g++)

    groups[g]->renderAABB();

}



void TransformGroup::renderOBB()

{

  if(updateAndGetBoundsDescriptor().isInitialized())

    boundsDescriptor.render(BoundsDescriptor::WIRE | BoundsDescriptor::OBB);



  for(size_t g = 0; g < groups.size(); g++)

    groups[g]->renderOBB();

}

extern D3DXFROMWINEMATRIX matProj;
Shape *TransformGroup::pickClosest(const Tuple2i &mouse,
                                   const Tuple2i &rect,
                                   const D3DXFROMWINEVECTOR4 &fov,
                                   Frustum *cull)
{
  Tuple4i viewport;
  uint  selectionBuffer[100],
          hits;

  ZeroMemory(selectionBuffer, 100);

  //glGetIntegerv(GL_VIEWPORT, viewport);
  viewport.x=IRenderer::GetRendererInstance()->GetViewportWidth();
  viewport.y=IRenderer::GetRendererInstance()->GetViewportHeight();
  viewport.z=IRenderer::GetRendererInstance()->GetViewportX();
  viewport.w=IRenderer::GetRendererInstance()->GetViewportY();

  //glSelectBuffer(100, selectionBuffer);
  //glRenderMode(GL_SELECT);
  //glInitNames();
  //glPushName(-1);

  //glMatrixMode(GL_PROJECTION);
  //glPushMatrix();
  //glLoadIdentity();
  s_worldTransformV.push_back(IRenderer::GetRendererInstance()->Getww());
  IRenderer::GetRendererInstance()->SetTransform(TS_WORLD, identity4());
    
  //???gluPickMatrix(mouse.x, mouse.y, rect.x, rect.y, viewport);
  //gluPerspective(fov.x, fov.y, fov.z, fov.w);
#ifdef _INCLUDE_SCENEFRAME_
  D3DXFROMWINEMatrixPerspectiveFovLH(&SceneFrame::matProj, fov.x, fov.y, fov.z, fov.w);
#endif
 
  //glMatrixMode(GL_MODELVIEW);

  //glColorMask(0,0,0,0);
  pickClosest(cull);
  //glColorMask(1,1,1,1);

  //glMatrixMode(GL_PROJECTION);
  //glPopMatrix();
  IRenderer::GetRendererInstance()->SetTransform(TS_WORLD, s_worldTransformV[s_worldTransformV.size()-1]);
  s_worldTransformV.pop_back();
  //glMatrixMode(GL_MODELVIEW);
  //glFlush();

  hits = 0;//???glRenderMode(GL_RENDER);
  if(hits)
  {

    uint dist    = 4294967295U,
           closest = 0;

    while (hits)
    {
      if(selectionBuffer[(hits - 1) * 4 + 1] < dist)
      {
        dist    = selectionBuffer[(hits - 1) * 4 + 1];
        closest = selectionBuffer[(hits - 1) * 4 + 3];
      }
      hits--;
    }
    return getShapeByPickingIndex(closest, cull);
  }
  return NULL;
}



void  TransformGroup::pickClosest(Frustum *frustum)

{

#ifdef PICKON

  size_t i = 0;



  if(frustum)

    if(!frustum->AABBInFrustum(boundsDescriptor))

       return;



  if(!visible)

    return;



  doTransform();

  for(i = 0; i < shapes.size(); i++)

  {

    glLoadName(shapes[i]->getPickingIndex());

    shapes[i]->draw(0);

  }

  undoTransform();



  for(i = 0; i < groups.size(); i++)

    groups[i]->pickClosest(frustum);

#endif

}



Shape *TransformGroup::getShapeByPickingIndex(int pickingIndex, Frustum *frustum)

{

  if(frustum)

    if(!frustum->AABBInFrustum(boundsDescriptor))

       return NULL;



  size_t i = 0;

  Shape *target = NULL;



  for(i = 0; i < shapes.size(); i++)

    if(shapes[i]->getPickingIndex() == pickingIndex)

      return shapes[i];



  for(i = 0; i < groups.size(); i++)

    if(target = groups[i]->getShapeByPickingIndex(pickingIndex, frustum))

      break;



  return target;

}



void TransformGroup::setupLightFrustum(const D3DXFROMWINEVECTOR3 &lightPosition)
{
  D3DXFROMWINEMATRIX  matrixBuffer;
  Tuple4i   viewport;
  D3DXFROMWINEVECTOR3   nearPlaneNormal,
            cameraPosition,
            rightVector,
            upVector;

  float     distance = 0.0f;

  //glGetIntegerv(GL_VIEWPORT, viewport);
  viewport.x=IRenderer::GetRendererInstance()->GetViewportWidth();
  viewport.y=IRenderer::GetRendererInstance()->GetViewportHeight();
  viewport.z=IRenderer::GetRendererInstance()->GetViewportX();
  viewport.w=IRenderer::GetRendererInstance()->GetViewportY();
  //glGetFloatv(GL_MODELVIEW_MATRIX , matrixBuffer);

#ifdef _INCLUDE_SCENEFRAME_
  matrixBuffer=g_SceneFrame->GetCamera().getModelViewMatrix();
#else
	D3DXFROMWINEMatrixIdentity(&matrixBuffer);
#endif

  rightVector.set(matrixBuffer[0],matrixBuffer[4], matrixBuffer[8]);
  upVector.set   (matrixBuffer[1],matrixBuffer[5], matrixBuffer[9]),

  //nearPlaneNormal.crossProduct(upVector, rightVector),
  //nearPlaneNormal.normalize();
      D3DXFROMWINEVec3Cross(&nearPlaneNormal, &upVector, &rightVector);
      D3DXFROMWINEVec3Normalize(&nearPlaneNormal, &nearPlaneNormal);

  //matrixBuffer.setInverse();
  FLOAT determinant=0.0f;
  D3DXFROMWINEMatrixInverse(&matrixBuffer, &determinant, &matrixBuffer);

  cameraPosition.set(matrixBuffer[12], matrixBuffer[13], matrixBuffer[14]);
  cameraPosition += nearPlaneNormal;

  //distance = nearPlaneNormal.getDotProduct(lightPosition) -
  //           nearPlaneNormal.getDotProduct(cameraPosition);
  distance = D3DXFROMWINEVec3Dot(&nearPlaneNormal,&lightPosition)-D3DXFROMWINEVec3Dot(&nearPlaneNormal,&cameraPosition);

  rightVector *= float(viewport.z)/viewport.w;

  corners[0]  =
  corners[1]  =
  corners[2]  =
  corners[3]  = cameraPosition;

  corners[0] -= upVector;
  corners[0] -= rightVector;

  corners[1] -= upVector;
  corners[1] += rightVector;

  corners[2] += rightVector;
  corners[2] += upVector;

  corners[3] += upVector;
  corners[3] -= rightVector;

  if(distance > EPSILON)
  {
    lightFrustum[0].set(lightPosition, corners[0], corners[1]);
    lightFrustum[1].set(lightPosition, corners[1], corners[2]);
    lightFrustum[2].set(lightPosition, corners[2], corners[3]);
    lightFrustum[3].set(lightPosition, corners[3], corners[0]);
    lightFrustum[4].set(corners[0], corners[2], corners[1]);
  }
  else
  {
    lightFrustum[0].set(lightPosition, corners[1], corners[0]);
    lightFrustum[1].set(lightPosition, corners[2], corners[1]);
    lightFrustum[2].set(lightPosition, corners[3], corners[2]);
    lightFrustum[3].set(lightPosition, corners[0], corners[3]);
    lightFrustum[4].set(corners[0], corners[1], corners[2]);
  }
}


void TransformGroup::renderLightFrustum(D3DXFROMWINEVECTOR3 &lightPosition)

{

	line_vertex line[2];

	line[0].colour=D3DXFROMWINEVECTOR4(0x00,0x00,0xFF,0xFF);

	line[1].colour=D3DXFROMWINEVECTOR4(0x00,0x00,0xFF,0xFF);

	IRenderer::GetRendererInstance()->InitPlain3(IRenderer::GetRendererInstance()->GetplainShader3(),IRenderer::GetRendererInstance()->GetplainVF3());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GetplainShader3());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GetplainVF3());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(0x00,0x00,0xFF,0xFF));

	line[0].pos=lightPosition;



 for(int i = 0; i < 4; i++)

  {

		line[1].pos=D3DXFROMWINEVECTOR3(corners[i].x, corners[i].y, corners[i].z);

	(IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_LINES, 1,line,line, sizeof(line_vertex)));

 }



}

void TransformGroup::doTransform()
{
  if(applyWorldTransform)
  {
    //glPushMatrix();
    //glMultMatrixf(worldTransform);
    s_worldTransformV.push_back(IRenderer::GetRendererInstance()->Getww());
    IRenderer::GetRendererInstance()->SetTransform(TS_WORLD, worldTransform);
  }
}

void TransformGroup::undoTransform()
{
  if(applyWorldTransform)
  {
    //glPopMatrix();
    IRenderer::GetRendererInstance()->SetTransform(TS_WORLD, s_worldTransformV[s_worldTransformV.size()-1]);
    s_worldTransformV.pop_back();
  }
    
}

bool TransformGroup::addChild(Shape *child)

{



  if(!child)

    {LOG_PRINT("Couldn't add shape to TransformGroup: NULL\n");return false;}



  if(!child->getGeometry() || !child->getAppearance())

    {LOG_PRINT("Couldn't add shape to TransformGroup: NULL Geometry or Appearance\n");return false;}



  if(child->getParent())

    {LOG_PRINT("Couldn't add shape to TransformGroup: Already has a parent\n");return false;}



#ifdef INFOSHADOW

  if(occluder)

    child->getGeometry()->activeInfoShadow();

#endif



  LOG_PRINT("Adding shape to TransformGroup\n");



  child->setParent(this);

  shapes.push_back(child);

  return true;

}



bool TransformGroup::addChild(TransformGroup *child)

{



  if(!child){LOG_PRINT("Couldn't add Group to TransformGroup: NULL\n");return false;}



  if(this == child){LOG_PRINT("Couldn't add shape to TransformGroup: parent is same as child\n");return false;}



  if(child->getParent()){LOG_PRINT("Couldn't add shape to TransformGroup: Already has a parent\n");return false;}



  LOG_PRINT("Adding Group to TransformGroup\n");



  groups.push_back(child);

  child->setParent(this);

  transform.overrideUpdatedFlag(true);

  updateWorldTransform();

  return true;

}





void TransformGroup::setParent(TransformGroup *prt)

{

  parent = prt;

}



void TransformGroup::transformToWorldSpace(BoundsDescriptor &descriptor)

{

  updateWorldTransform();

  descriptor *= worldTransform;

}



void TransformGroup::transformToObjectSpace(D3DXFROMWINEVECTOR3 &tuple)

{

  updateWorldTransform();

  float det=0.0f;
  D3DXFROMWINEMATRIX inverse;
  D3DXFROMWINEMatrixInverse(&inverse, &det, &worldTransform);
  //tuple = tuple*inverse;
  D3DXFROMWINEVec3TransformCoord(&tuple, &tuple, &inverse);

}



void TransformGroup::transformToWorldSpace(D3DXFROMWINEVECTOR3 &tuple)

{
  LOG_FNLN;
  updateWorldTransform();
  //tuple = tuple*worldTransform;
  D3DXFROMWINEVec3TransformCoord(&tuple, &tuple, &worldTransform);

}



void TransformGroup::compile()

{



  for(size_t s = 0; s < shapes.size(); s++)

  {

	  if(occluder)

      shapes[s]->compile(true);

    else

      shapes[s]->compile(false);

  }





  for(size_t g = 0; g < groups.size(); g++)

  {

    groups[g]->compile();

  }





}



void TransformGroup::setVisible(bool vis)

{

  visible = vis;

}



int TransformGroup::getShapesCount()

{

  return int(shapes.size());

}



int TransformGroup::getGroupsCount()

{

  return int(groups.size());

}



Shape *TransformGroup::getShape(int index)

{

  return  shapes.at(index);

}



TransformGroup *TransformGroup::getParent()

{

  return parent;

}



TransformGroup *TransformGroup::getGroup(int index)

{

  return  groups.at(index);

}



vector<Shape*> &TransformGroup::getShapes()

{

  return shapes;

}



vector<TransformGroup*> &TransformGroup::getGroups()

{

  return groups;

}



TransformGroup *TransformGroup::getGroup(const char* groupName)

{

  TransformGroup *target = NULL;



  if(!groupName)

    return NULL;



  if(name == groupName)

    return this;



  for(size_t i = 0; i < groups.size(); i++)

    if(target = groups[i]->getGroup(groupName))

      break;



  return target;

}



void TransformGroup::setTransform(const Transform3D &t)

{

  transform = t;

  transform.overrideUpdatedFlag(true);

  updateWorldTransform();

}



Transform3D &TransformGroup::getTransform(){

  return transform;

}



void TransformGroup::setOccluder(bool occ){

  occluder = occ;

}



void TransformGroup::updateBoundsDescriptor()

{

  updateWorldTransform();

  boundsDescriptor.reset();



  for(size_t s = 0; s < shapes.size(); s++)

  {

    if(!boundsDescriptor.isInitialized())

      boundsDescriptor	= shapes[s]->getGeometry()->getBoundsDescriptor();

    else

      boundsDescriptor += shapes[s]->getGeometry()->getBoundsDescriptor();

  }



  if(boundsDescriptor.isInitialized())

    boundsDescriptor *= worldTransform;



  for(size_t g = 0; g < groups.size(); g++)

    if(!boundsDescriptor.isInitialized())

      boundsDescriptor	= groups[g]->updateAndGetBoundsDescriptor();

    else

      boundsDescriptor += groups[g]->updateAndGetBoundsDescriptor();

}



BoundsDescriptor &TransformGroup::updateAndGetBoundsDescriptor()

{

  updateBoundsDescriptor();

  return getBoundsDescriptor();

}



BoundsDescriptor &TransformGroup::getBoundsDescriptor()

{

  return boundsDescriptor;

}



float TransformGroup::getEstimatedVolumeSize()

{

  D3DXFROMWINEVECTOR3 diagonal	= boundsDescriptor.getMaxEndAABB();

	  diagonal -= boundsDescriptor.getMinEndAABB();

  float l=D3DXFROMWINEVec3Length(&diagonal);
  return l;

}





bool TransformGroup::loadXMLSettings(XMLElement *element)

{



  if(!isSuitable(element))

    return false;



  XMLElement  *child	= NULL;

  destroy();



  for(size_t i = 0; i < element->getChildrenCount(); i++)

  {

    if(!(child = element->getChild(i)))

      continue;

    const NSString &childName = child->getName();



    if(childName == "visible")

      setVisible(child->getValue() == "true");



    if(childName == "occluder")

      setOccluder(child->getValue() == "true");



    if(childName == "name")

      setName(child->getValue());



    if(childName == "Transform")

    {

      transform.loadXMLSettings(child);

      continue;

    }



    if(childName == "Group")

    {

      TransformGroup *group = new TransformGroup();

      if(!group->loadXMLSettings(child))

      {

	deleteObject(group);

      }

      else

	addChild(group);

      continue;

    }



    if(childName == "Shape")

    {



      loadXMLShape(child);



      continue;

    }

  }



  return true;

}



bool TransformGroup::exportXMLSettings(ofstream &xmlFile  )

{
#if 0
  if(!xmlFile.is_open())

    {LOG_PRINT("Cannot export Group to XML: file not ready.\n");return false;}

  #if 0
  xmlFile << "<Group visible = \"" << (visible ? "true\"" : "false\"")

	  << (!name.getLength() ? ">\n" : (NSString("\n	     name    = \"") + name +  "\"> \n\n");
  #endif

  transform.exportXMLSettings(xmlFile);



  size_t c = 0;



  ofstream shapeFile;

  for(c = 0; c < shapes.size(); c++)

  {

    if(!shapeFile.is_open())

    {

      shapeFile.open(shapes[c]->getGeometry()->getName(), ios::app | ios::binary);

    }

    shapes[c]->exportXMLSettings(shapeFile);

  }



  if(shapeFile.is_open())

  {

    xmlFile << "  <Shape path = \"" << shapes[0]->getGeometry()->getName() << "\" />\n";

    shapeFile.close();

  }



  for(c = 0; c < groups.size(); c++)

    groups[c]->exportXMLSettings(xmlFile);



  xmlFile << "</Group>" << endl;
#endif
  return true;

}



bool TransformGroup::loadXMLShape(XMLElement *element)

{



  if(!element)

  {    LOG_PRINT("NULL Shape node\n");return false;}



  if(element->getName() != "Shape")

    {LOG_PRINT("Need a <Shape> tag in the XML Node\n");return false;}



  XMLElement *child;

  XMLStack    stack;



  if(!(child = element->getChildByName("path")))

    {LOG_PRINT("Need a path child in <Shape>\n");return false;}

	std::string buf;
	buf.append(MediaPathManager::pathStack[MediaPathManager::pathStack.size()-1].data);
	buf.append(child->getValuec());

  const char* verifiedPath = buf.c_str();//MediaPathManager::lookUpMediaPath(buf);

  bool	visible  = true;

  char stringBuffer[MAX_PATH];

  int	meshCount    = 0;



  if(!verifiedPath)

    {LOG_PRINT("Couldn't locate Shape file at -> %s\n", child->getValuec());return false;}


#if 0
  if(strstr(verifiedPath, ".3ds") || strstr(verifiedPath, ".3DS"))

    return load3DS(this, true, (char*)verifiedPath);

#endif

  stack.loadXMLFile(verifiedPath);



  if(child = element->getChildByName("visible"))

     visible = (child->getValue() == "true");



  for(size_t i = 0; i < stack.getChildrenCount(); i++)

  {

    if(!(child = stack.getChild(i)))

      continue;



    if(child->getName() == "Shape")

    {

      sprintf(stringBuffer, "%s_%d", verifiedPath, meshCount++);
      

      Shape *shape = new Shape();



      if(shape->loadXMLSettings(child, stringBuffer))

      {

	shape->setVisible(visible);

	addChild(shape);

      }

      else

      {

	deleteObject(shape);

	LOG_PRINT("Failed to load Shape -> %s\n", stringBuffer);

      }

    }

  }



  return true;

}



void  TransformGroup::updateWorldTransform()

{

  if(transform.updatedFlagOn())

  {

    if(parent)

    {

      worldTransform  = parent->getWorldTransform();

      worldTransform = worldTransform*transform.getMatrix4f();

    }

    else

      worldTransform = transform.getMatrix4f();



    for(size_t g = 0; g < groups.size(); g++)

    {

      groups[g]->transform.overrideUpdatedFlag(true);

      groups[g]->updateWorldTransform();

    }

    transform.overrideUpdatedFlag(false);

	D3DXFROMWINEMATRIX mI;

    mI=identity4();

    applyWorldTransform = (worldTransform != mI);

  }

}



const D3DXFROMWINEMATRIX &TransformGroup::getWorldTransform() const

{

  return worldTransform;

}



void TransformGroup::collectTransformGroups(TransformGroup *parent,

					    Frustum	   *frustum)

{

  const vector<TransformGroup*> &children = parent->groups;



  if(parent && !frustum)

  {

    for(size_t g = 0; g < children.size(); g++)

    {

      groupsObjectList.push_back(DistanceObjectTGP(children[g]));

      collectTransformGroups(children[g]);

    }

  }



  if(parent && frustum)

  {

    for(size_t g = 0; g < children.size(); g++)

    {

      if(frustum->AABBInFrustum(children[g]->getBoundsDescriptor()))

      {

	groupsObjectList.push_back(DistanceObjectTGP(children[g]));

	collectTransformGroups(children[g], frustum);

      }

    }

  }

}



TransformGroup *TransformGroup::removeGroup(const char *name)

{

  TransformGroup *target = NULL;



  if(name)

  {

    for(size_t i =0; i < groups.size(); i++)

      if((groups[i]->getName() == name) && !target)

      {

	target = removeGroup(int(i));

	break;

      }

      else

	target = groups[i]->removeGroup(name);

  }

  return target;

}



TransformGroup *TransformGroup::removeGroup(int index)

{

  if(index < 0 || index >= int(groups.size()))

    return NULL;



  vector<TransformGroup*>  temp;

  TransformGroup	  *target;



  for(size_t i =0; i < groups.size(); i++)

   if(index != int(i))

     temp.push_back(groups[i]);

   else

     target = groups[i];



  groups.clear();



  for(size_t i =0; i < temp.size(); i++)

    groups.push_back(temp[i]);



  temp.clear();



  target->setParent(NULL);

  target->getTransform().overrideUpdatedFlag(true);

  target->updateWorldTransform();



  return target;

}



void TransformGroup::destroy()

{

  for(size_t i =0; i < shapes.size(); i++)

    deleteObject(shapes[i]);



  for(size_t i =0; i < groups.size(); i++)

    deleteObject(groups[i]);



  parent   = NULL;

  visible  = true;

  occluder = false;



  groups.clear();

  shapes.clear();

}

#endif

