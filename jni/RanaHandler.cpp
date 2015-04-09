#include "RanaHandler.h"
#include <fstream>

using namespace std;
using namespace alvar;

RanaHandler::RanaHandler() {

}

void RanaHandler::parseFile(std::string file) 
{
    ifstream input(file.c_str());
    
    int markerId;
    while(input >> markerId)
    {
        float scale;
        input >> scale;
        
        int numberOfDrawables;
        input >> numberOfDrawables;
        
        MarkerDrawables marker;
        for(int i = 0; i < numberOfDrawables; i++)
        {
            int format;
            input >> numberOfDrawables;
            
            RanaDrawable * draw;
            
            switch(format)
            {
                case RANA_TEXTURE:
                    draw = parseTexture(input);
                    break;
                case RANA_VIDEO:
                    draw = parseVideo(input);
                    break;
                case RANA_LINK:
                    draw = parseLink(input);
                    break;
                case RANA_AUDIO:
                    draw = parseAudio(input);
                    break;
                case RANA_3D_MODEL:
                    draw = parse3dModel(input);
                    break;
                default:
                    break;
            }
            
            
            marker.addDrawable(draw);
        }
        
        mappedDrawable.insert(pair<int,MarkerDrawables>(markerId, marker));
    }
    
}

RanaDrawable* RanaHandler::parseTexture(std::ifstream& input) 
{
    TextureDrawable * draw = new TextureDrawable();

    string link;
    input >> link;
    draw->setLink(link);
    input >> (draw->address);
    input >> (draw->width);
    input >> (draw->height);
    
    for(int i = 0; i < 12; i++)
        input >> (draw->vertices[i]);
}

RanaDrawable* RanaHandler::parseVideo(std::ifstream& input)
{

}

RanaDrawable* RanaHandler::parseLink(std::ifstream& input) 
{

}


bool RanaHandler::draw(alvar::Marker* marker)
{
    int id = marker->GetId();
    Pose pose = marker->pose;
    
    map<int, MarkerDrawables>::iterator it = mappedDrawable.find(id);
    if(it == mappedDrawable.end())
        return false;
    
    if(!(it->second.isInit()))
        it->second.initialize();
    
    it->second.draw(&pose);
}
