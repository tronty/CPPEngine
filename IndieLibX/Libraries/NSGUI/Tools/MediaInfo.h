#ifndef MEDIAINFO_H

#define MEDIAINFO_H





#include "../Managers/MediaPathManager.h"

#include <algorithm>

#include <cstring>



template <class MediaInfoType>

class MediaInfo

{

  public:

    MediaInfo(std::string mediaPath_= 0, MediaInfoType mediaArg = 0)

    {

      userCount = 1;

      mediaPath = mediaPath_;

      media     = mediaArg;

    }



    MediaInfo(const MediaInfo &copy)

    {

      this->operator=(copy);

    }



    MediaInfo &operator =(const MediaInfo &copy)

    {

      if(this != &copy)

      {

        userCount = copy.userCount;

        mediaPath = copy.mediaPath;

        media     = copy.media;

      }

      return *this;

    }



    void setMediaPath(std::string mediaPath_)

    {

      mediaPath = mediaPath_;

    }



    const std::string &getMediaPath() const

    {

      return mediaPath;

    }



    void increaseUserCount()

    {

      userCount++;

    }



    void decreaseUserCount()

    {

      userCount--;

    }



    unsigned int getUserCount()

    {

      return userCount;

    }



    MediaInfoType &getMedia()

    {

      return media;

    }



    void  setMedia(MediaInfoType &mediaArg)

    {

      media = mediaArg;

    }



    bool operator > (const MediaInfo &info)

    {

      return mediaPath > info.mediaPath;

    }



    bool operator < (const MediaInfo &info)

    {

      return  mediaPath < info.mediaPath;

    }



    bool operator == (const MediaInfo &info)

    {

      return (mediaPath == info.mediaPath) &&

             (userCount == info.userCount) &&

             (media     == info.media    );

    }



    bool operator != (const MediaInfo &info)

    {

      return !this->operator ==(info);

    }



  protected:

    unsigned int   userCount;

    MediaInfoType  media;

    std::string         mediaPath;



};



#endif
