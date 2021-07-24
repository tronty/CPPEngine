#ifndef SHAPE_H

#define SHAPE_H



#include <Appearance/Appearance.h>

#include <Geometry/ShadowInfo.h>

#include <Geometry/Geometry.h>



class TransformGroup;

class Shape

{

  public:

    Shape(Geometry* geom = 0, Appearance *app = 0);

    Shape(const Shape&);

   ~Shape();

    Shape &operator=(const Shape&);



    bool loadXMLSettings(XMLElement *element, const char* shapePath);



    bool exportXMLSettings(ofstream &xmlFile  );

    bool exportXMLSettings(const char* xmlPath);



    void            setParent(TransformGroup *);

    TransformGroup *getParent();



    void initialize();



    void        setAppearance(Appearance *app);

    Appearance *getAppearance();



    void       setGeometry(Geometry *geom);

    Geometry  *getGeometry();



    void setVisible(bool vi);

    bool isVisible();



    void doShadowPass(D3DXFROMWINEVECTOR4 &lightPos, bool, int renderingPath);

    void renderSilhouette(D3DXFROMWINEVECTOR4 &viewPosition);



    void compile(bool);

    int  draw(int format, const uint flags=RESET_ALL);



    int  getPickingIndex();

  private:

    void drawVBOs(int rendering, const uint flags=RESET_ALL);

    void drawRegular(int rendering, const uint flags=RESET_ALL);



    bool isTextureActive(int format, int index);

    void enableTextureUnits(int enabledTextures, ShaderID sha);

    void disableTexureUnits(int enabledTextures);



    TransformGroup *parent;

    Appearance*     appearance;

    Geometry*       geometry;

    unsigned int          pickingIndex;

    bool            visible;

};

#endif

