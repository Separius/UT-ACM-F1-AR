#ifndef __RANA_DRAWABLE_H
#define __RANA_DRAWABLE_H


#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <vector>

#include "Pose.h"

enum RanaDrawableType
{
    RANA_TEXTURE,
    RANA_LINK,
    RANA_VIDEO,
    RANA_3D_MODEL,
    RANA_AUDIO
};

class RanaDrawable 
{
protected:
        std::string link;
        bool available;
public:
    
    RanaDrawable() : available (true), link("")
    {
        
    }
    
    virtual void draw() = 0;


    void setLink(std::string _link)
    {
        link = _link;
    }

    std::string getLink()
    {
        return link;
    }

    bool isAvailable() 
    {
        return available;
    }

    void setAvailable(bool _available) 
    {
        available = _available;
    }
    
    virtual void init() = 0;
};


class TextureDrawable : public RanaDrawable
{
public:	
	TextureDrawable();
	

	void draw();
	GLuint textureId;
	GLfloat vertices[12];
	GLfloat texture[8];
        
        
        std::string address;
        int width;
        int height;
	
	
	
	void initTexture();
	void initSurface();


	void loadFromMat(cv::Mat image, int width, int height);
	void loadFromFile();
        
        
        void init()
        {
            loadFromFile();
        }
};



class VideoDrawable : public TextureDrawable 
{
public: 
};

class LinkDrawable : public TextureDrawable
{
public:
};

class MarkerDrawables 
{
private:
	float matrix[16];
        float scale;
        bool available;
        
        bool init;
public:
    MarkerDrawables();
    

    std::vector < RanaDrawable * > drawables;
	
    void draw(alvar::Pose * p);    
    
    void setScale(float _scale) 
    {
        scale = _scale;
    }
    
    bool isAvailable() 
    {
        return available;
    }

    void setAvailable(bool _available) 
    {
        available = _available;
    }


    void setInit(bool _init)
    {
        init = _init;
    }
    
    bool isInit()
    {
        return init;
    }
    
    void addDrawable(RanaDrawable * ranaDrawable)
    {
        drawables.push_back(ranaDrawable);
    }
    
    void initialize();
};


#endif //__RANA_DRAWABLE_H
