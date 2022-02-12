#include "collision.h"

using namespace PhysX;

collisionBox::collisionBox(std::string channel,glm::vec2 location, glm::vec2 scale, uint16_t rotation){
    this->location = location;
    this->rotation = rotation;
    this->scale = scale;

    this->channel = channel;
}

float getPointOnLineAtAngle(glm::vec2 point, uint16_t angle){
    return (point.x+(point.y*tan(angle*PI/180)))/(1+pow(tan(angle*PI/180),2));
}

glm::vec2 getRotatedRectPoint_RB(glm::vec2 center,glm::vec2 size,uint16_t angle){
    return glm::vec2(center.x + ((size.x / 2) * cos(angle*PI/180)) + ((size.y / 2) * sin(angle*PI/180)),center.y + ((size.x / 2) * sin(angle*PI/180)) - ((size.y / 2) * cos(angle*PI/180)));
}

glm::vec2 getRotatedRectPoint_LB(glm::vec2 center,glm::vec2 size,uint16_t angle){
    return glm::vec2(center.x - ((size.x / 2) * cos(angle*PI/180)) + ((size.y / 2) * sin(angle*PI/180)),center.y - ((size.x / 2) * sin(angle*PI/180)) - ((size.y / 2) * cos(angle*PI/180)));
}

glm::vec2 getRotatedRectPoint_LT(glm::vec2 center,glm::vec2 size,uint16_t angle){
    return glm::vec2(center.x - ((size.x / 2) * cos(angle*PI/180)) - ((size.y / 2) * sin(angle*PI/180)),center.y - ((size.x / 2) * sin(angle*PI/180)) + ((size.y / 2) * cos(angle*PI/180)));
}

glm::vec2 getRotatedRectPoint_RT(glm::vec2 center,glm::vec2 size,uint16_t angle){
    return glm::vec2(center.x + ((size.x / 2) * cos(angle*PI/180)) - ((size.y / 2) * sin(angle*PI/180)),center.y + ((size.x / 2) * sin(angle*PI/180)) + ((size.y / 2) * cos(angle*PI/180)));
}

bool oneDimensionalCheck(collisionBox A,collisionBox B, uint16_t lineAngle){
    float thisPointA = 0;
    float thisPointB = 0;
    float thisPointC = 0;
    float thisPointD = 0;

    float thisMin = 0;
    float thisMax = 0;


    float otherPointA = 0;
    float otherPointB = 0;
    float otherPointC = 0;
    float otherPointD = 0;

    float otherMin = 0;
    float otherMax = 0;

    // LINE A
    thisPointB = getPointOnLineAtAngle(getRotatedRectPoint_LT(A.getLocation(),A.getScale(),A.getRotation()),lineAngle);
    thisPointC = getPointOnLineAtAngle(getRotatedRectPoint_RB(A.getLocation(),A.getScale(),A.getRotation()),lineAngle);
    thisPointD = getPointOnLineAtAngle(getRotatedRectPoint_LB(A.getLocation(),A.getScale(),A.getRotation()),lineAngle);
    thisPointA = getPointOnLineAtAngle(getRotatedRectPoint_RT(A.getLocation(),A.getScale(),A.getRotation()),lineAngle);

    otherPointA = getPointOnLineAtAngle(getRotatedRectPoint_RT(B.getLocation(),B.getScale(),B.getRotation()),lineAngle);
    otherPointB = getPointOnLineAtAngle(getRotatedRectPoint_LT(B.getLocation(),B.getScale(),B.getRotation()),lineAngle);
    otherPointC = getPointOnLineAtAngle(getRotatedRectPoint_RB(B.getLocation(),B.getScale(),B.getRotation()),lineAngle);
    otherPointD = getPointOnLineAtAngle(getRotatedRectPoint_LB(B.getLocation(),B.getScale(),B.getRotation()),lineAngle);

    thisMin = std::min(std::min(thisPointA,thisPointB),std::min(thisPointC,thisPointD));
    thisMax = std::max(std::max(thisPointA,thisPointB),std::max(thisPointC,thisPointD));

    otherMin = std::min(std::min(otherPointA,otherPointB),std::min(otherPointC,otherPointD));
    otherMax = std::max(std::max(otherPointA,otherPointB),std::max(otherPointC,otherPointD));

    return (otherMin < thisMax && otherMax > thisMin);
}

bool collisionBox::check(collisionBox other){
    glm::vec2 otherLocation = other.getLocation();
    glm::vec2 otherScale = other.getScale();
    uint16_t otherRotation = other.getRotation();

    // std::cout << "L: " << other.getLocation().x << "\n";

    if(!oneDimensionalCheck(*this,other,(*this).getRotation()+90)) // A
        return false;
    if(!oneDimensionalCheck(*this,other,(*this).getRotation())) // B
        return false;
    if(!oneDimensionalCheck(*this,other,other.getRotation()+90)) // C
        return false;
    if(!oneDimensionalCheck(*this,other,other.getRotation())) // D
        return false;

    return true;
}