#ifndef ALPHA_ATTRIBUTES_H

#define ALPHA_ATTRIBUTES_H







class AlphaAttributes : public IOXMLObject

{

  public:

    AlphaAttributes(int   function  = -1,

                    float reference = -1.0f);

    AlphaAttributes(const AlphaAttributes &copy);

    AlphaAttributes &operator = (const AlphaAttributes &);



    virtual bool  loadXMLSettings(XMLElement *element);

    virtual bool  exportXMLSettings(ofstream &xmlFile);



    void  setAttributes(int function, float reference);



    void  setFunction(int function);

    int   getFunction();



    void  setReference(float reference);

    float getReference();



    void  apply();

    void  stop();



  private:

    int   getXMLAlphaFunc(const NSString &value);

    float reference;

    int   function;

};

#endif
