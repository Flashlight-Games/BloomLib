#include "collision.h"

using namespace PhysX;

collisionBox::collisionBox(std::string channel,glm::vec2 location, glm::vec2 scale, uint16_t rotation, glm::vec2 origin){
    this->location = location;
    this->rotation = rotation;
    this->scale = scale;
    this->origin = origin;

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

glm::mat4 collisionBox::makeTransformationMatrix(){
    glm::mat4 result = glm::mat4(1);
    result           = glm::translate(result,glm::vec3(location[0],location[1],0.0f));
    result           = glm::rotate(result,(float)glm::radians((float)this->rotation),glm::vec3(0.0f,0.0f,1.0f));
    return result;
}

bool oneDimensionalCheck(collisionBox A,collisionBox B, uint16_t lineAngle,glm::mat4 transformationMatrix,glm::mat4 otherTransformationMatrix){
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
    glm::vec2 originA = A.getOrigin(); glm::vec2 scaleA = A.getScale();
    thisPointB = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((originA[0]-1)*scaleA[0],(originA[1])*scaleA[1]),0,1),lineAngle);
    thisPointC = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((originA[0])*scaleA[0],(originA[1]-1)*scaleA[1]),0,1),lineAngle);
    thisPointD = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((originA[0]-1)*scaleA[0],(originA[1]-1)*scaleA[1]),0,1),lineAngle);
    thisPointA = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((originA[0])*scaleA[0],(originA[1])*scaleA[1]),0,1),lineAngle);

    glm::vec2 originB = B.getOrigin(); glm::vec2 scaleB = B.getScale();
    otherPointA = getPointOnLineAtAngle(otherTransformationMatrix * glm::vec4(glm::vec2((originB[0]-1)*scaleB[0],(originB[1])*scaleB[1]),0,1),lineAngle);
    otherPointB = getPointOnLineAtAngle(otherTransformationMatrix * glm::vec4(glm::vec2((originB[0])*scaleB[0],(originB[1]-1)*scaleB[1]),0,1),lineAngle);
    otherPointC = getPointOnLineAtAngle(otherTransformationMatrix * glm::vec4(glm::vec2((originB[0]-1)*scaleB[0],(originB[1]-1)*scaleB[1]),0,1),lineAngle);
    otherPointD = getPointOnLineAtAngle(otherTransformationMatrix * glm::vec4(glm::vec2((originB[0])*scaleB[0],(originB[1])*scaleB[1]),0,1),lineAngle);

    thisMin = std::min(std::min(thisPointA,thisPointB),std::min(thisPointC,thisPointD));
    thisMax = std::max(std::max(thisPointA,thisPointB),std::max(thisPointC,thisPointD));

    otherMin = std::min(std::min(otherPointA,otherPointB),std::min(otherPointC,otherPointD));
    otherMax = std::max(std::max(otherPointA,otherPointB),std::max(otherPointC,otherPointD));

    return (otherMin < thisMax && otherMax > thisMin);
}

bool collisionBox::check(collisionBox other,glm::mat4 transformationMatrix,glm::mat4 otherTransformationMatrix){
    glm::vec2 otherLocation = other.getLocation();
    glm::vec2 otherScale = other.getScale();
    uint16_t otherRotation = other.getRotation();

    if(!oneDimensionalCheck(*this,other,(*this).getRotation()+90,transformationMatrix,otherTransformationMatrix)) // A
        return false;
    if(!oneDimensionalCheck(*this,other,(*this).getRotation(),transformationMatrix,otherTransformationMatrix)) // B
        return false;
    if(!oneDimensionalCheck(*this,other,other.getRotation()+90,transformationMatrix,otherTransformationMatrix)) // C
        return false;
    if(!oneDimensionalCheck(*this,other,other.getRotation(),transformationMatrix,otherTransformationMatrix)) // D
        return false;

    return true;
}

bool collisionBox::check(collisionBox other){
    glm::vec2 otherLocation = other.getLocation();
    glm::vec2 otherScale = other.getScale();
    uint16_t otherRotation = other.getRotation();

    glm::mat4 transMatrix = makeTransformationMatrix();
    glm::mat4 otherTransMatrix = other.makeTransformationMatrix();

    if(!oneDimensionalCheck(*this,other,(*this).getRotation()+90,transMatrix,otherTransMatrix)) // A
        return false;
    if(!oneDimensionalCheck(*this,other,(*this).getRotation(),transMatrix,otherTransMatrix)) // B
        return false;
    if(!oneDimensionalCheck(*this,other,other.getRotation()+90,transMatrix,otherTransMatrix)) // C
        return false;
    if(!oneDimensionalCheck(*this,other,other.getRotation(),transMatrix,otherTransMatrix)) // D
        return false;

    return true;
}