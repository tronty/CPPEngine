#ifndef BLEND_ATTRIBUTES_H

#define BLEND_ATTRIBUTES_H



#include <Parsers/IOXMLObject.h>



class BlendAttributes : public IOXMLObject

{

  public:

    BlendAttributes(int source       = -1,

                    int destination  = -1);



    BlendAttributes(const BlendAttributes &copy);

    BlendAttributes &operator = (const BlendAttributes &);



    virtual bool loadXMLSettings(XMLElement *element);

    virtual bool exportXMLSettings(ofstream &xmlFile);



    void  setAttributes(int source, int destination);



    void  setSource(int source);

    int   getSource();



    void  setDestination(int destination);

    int   getDestination();



    void  apply();

    void  stop();



  private:

    int   getXMLDestinationFactor(const NSString &value);

    int   getXMLSourceFactor(const NSString &value);



    int   destination,

          source;

};

#endif
