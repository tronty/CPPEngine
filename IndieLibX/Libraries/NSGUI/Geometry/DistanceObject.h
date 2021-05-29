#ifndef DISTANCE_OBJECT_H

#define DISTANCE_OBJECT_H







template <class T>

class DistanceObject

{

  public:

    DistanceObject()

    {

      distance = 0.0f;

    }



    DistanceObject(T objectArg)

    {

      distance = 0.0f;

      object   = objectArg;

    }



    DistanceObject(const DistanceObject &copy)

    {

      operator=(copy);

    }



    DistanceObject &operator =(const DistanceObject &copy)

    {

      if(this != &copy)

      {

        distance = copy.distance;

        object   = copy.object;

      }

      return *this;

    }



    bool operator ==(const DistanceObject &copy)

    {

       return (distance == copy.distance);

    }



    bool operator >=(const DistanceObject &copy)

    {

       return (distance >= copy.distance);

    }



    bool operator <=(const DistanceObject &copy)

    {

       return (distance <= copy.distance);

    }



    bool operator >(const DistanceObject &copy)

    {

       return (distance > copy.distance);

    }

 

    bool operator < (const DistanceObject &copy)

    {

       return (distance < copy.distance);

    }

   

    void set(float dist, T obj)

    {

      distance = dist;

      object   = obj;

    }



    void setDistance(float dist)

    {

      distance = dist;

    }



    float getDistance() const

    {

     return distance;

    }



    void setObject(T objectArg)

    {

      object =objectArg;

    }



    T &getObject()

    {

      return object;

    }

   private:

     float distance;

     T     object;



};



#endif
