#include "RanaDrawable.h"


#include <android/log.h>

using namespace alvar;

#define LOG_TAG    "RANA_JNI"
#define LOG(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


TextureDrawable::TextureDrawable() 
{
}


void TextureDrawable::draw()
{
}


void TextureDrawable::initTexture()
{
	float verticesX[] = {
			-1.0f, -1.0f,  0.0f,		// V1 - bottom left
			-1.0f,  1.0f,  0.0f,		// V2 - top left
			 1.0f, -1.0f,  0.0f,		// V3 - bottom right
			 1.0f,  1.0f,  0.0f			// V4 - top right
	};
	for(int i = 0; i < 15; i++)
		vertices [ i ] = verticesX [ i ];
}
void TextureDrawable::initSurface()
{
	float textureX[] = {    		
			// Mapping coordinates for the vertices
			0.0f, 1.0f,		// top left		(V2)
			0.0f, 0.0f,		// bottom left	(V1)
			1.0f, 1.0f,		// top right	(V4)
			1.0f, 0.0f		// bottom right	(V3)
	};
	for(int i = 0; i < 15; i++)
		texture[ i ] = textureX [ i ];
}


void TextureDrawable::loadFromMat(cv::Mat image, int width, int height)
{
	// generate one texture pointer
	glGenTextures(1, &textureId);
	// ...and bind it to our array
	glBindTexture(GL_TEXTURE_2D, textureId);

	// create nearest filtered texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	
	// TODO TOFF 3264x1840
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image.ptr());	

}

void TextureDrawable::loadFromFile()
{
	cv::Mat mMat = cv::imread(address.c_str());
	if(!mMat.data)
			LOG("logo failed to load ");

	cv::cvtColor(mMat, mMat, CV_BGR2RGBA);

	loadFromMat (mMat, width, height);
}


void MarkerDrawables::draw(alvar::Pose * p)
{
    // TODO why double? :(
    double gl_mat[16];
    p->GetMatrixGL(gl_mat);

    GLfloat glMat[16];
    for(int q = 0; q < 16; q++)
    {
            if ( q % 4 == 0 )
                    glMat[q] = (float)(-1.f * gl_mat [ q ]);
            else
                glMat [ q ] = (float)(gl_mat [ q ]);

    }
    glPushMatrix();
    glMultMatrixf(glMat);
    glScalef(scale, scale, 1);

    for(unsigned int i = 0; i < drawables.size(); i++)
    {
        if(drawables[i]->isAvailable())
            drawables[i]->draw();
    }

    glPopMatrix();	
}

MarkerDrawables::MarkerDrawables() : scale(1.f), available(false), init(false) {
}

void MarkerDrawables::initialize()
{
    if(init)
        return;
    for(unsigned int i = 0; i < drawables.size(); i++)
    {
        drawables[i]->init();
    }
    
    init = true;
}

