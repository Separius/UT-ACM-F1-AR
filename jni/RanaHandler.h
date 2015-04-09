/* 
 * File:   RanaHandler.h
 * Author: hooman
 *
 * Created on August 7, 2014, 7:53 PM
 */

#ifndef RANAHANDLER_H
#define	RANAHANDLER_H

#include <string>
#include <map>
#include <fstream>

#include "RanaDrawable.h"
#include "Marker.h"



class RanaHandler 
{
private:
    std::map <int, MarkerDrawables> mappedDrawable;
    
    
    RanaDrawable * parseTexture(std::ifstream& input);
    RanaDrawable * parseVideo(std::ifstream& input);
    RanaDrawable * parseLink(std::ifstream& input);
    RanaDrawable * parse3dModel(std::ifstream& input) {}
    RanaDrawable * parseAudio(std::ifstream& input) {}
    
public:
    RanaHandler();
    
    void parseFile(std::string file);
    
    
    bool draw(alvar::Marker * marker);
};


#endif	/* RANAHANDLER_H */

