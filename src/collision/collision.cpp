#include "collision.h"

using namespace PhysX;

collisionBox::collisionBox(std::string channel,glm::vec2 location, glm::vec2 scale, uint16_t rotation, glm::vec2 origin){
    this->location = location;
    this->rotation = rotation;
    this->scale = scale;
    this->origin = origin;

    this->channel = channel;
}

collisionBox::collisionBox(std::string channel, Bloom::Sprite* sprite) {
    this->location = sprite->GetLocation();
    this->rotation = sprite->GetRotation();
    this->scale = sprite->GetSize();
    this->origin = sprite->GetOrigin();

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

float oneDimensionalCheck(collisionBox A,collisionBox B, uint16_t lineAngle,glm::mat4 transformationMatrix,glm::mat4 otherTransformationMatrix){
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

    float penetrationDepth = abs(otherMax - thisMin);
    penetrationDepth = (otherMin < thisMax&& otherMax > thisMin) ? penetrationDepth : -1;
    return penetrationDepth;
}

bool collisionBox::check_overlap(collisionBox other,glm::mat4 transformationMatrix,glm::mat4 otherTransformationMatrix){
    glm::vec2 otherLocation = other.getLocation();
    glm::vec2 otherScale = other.getScale();
    uint16_t otherRotation = other.getRotation();
    float minPenetration;
    float minAxis;

    float PA = oneDimensionalCheck(*this, other, (*this).getRotation() + 90, transformationMatrix, otherTransformationMatrix);
    if(PA == -1) // A
        return false;
    float PB = oneDimensionalCheck(*this, other, (*this).getRotation(), transformationMatrix, otherTransformationMatrix);
    if(PB == -1) // B
        return false;
    float PC = oneDimensionalCheck(*this, other, other.getRotation() + 90, transformationMatrix, otherTransformationMatrix);
    if(PC == -1) // C
        return false;
    float PD = oneDimensionalCheck(*this, other, other.getRotation(), transformationMatrix, otherTransformationMatrix);
    if(PD == -1) // D
        return false;

    minPenetration = std::min(std::min(PA, PB), std::min(PC, PD));
    if (minPenetration == PA)
        minAxis = (*this).getRotation() + 90;
    else if (minPenetration == PB)
        minAxis = (*this).getRotation();
    else if (minPenetration == PC)
        minAxis = other.getRotation() + 90;
    else if (minPenetration == PD)
        minAxis = other.getRotation();

    return true;
}

bool collisionBox::check_overlap(collisionBox other){
    glm::mat4 transMatrix = makeTransformationMatrix();
    glm::mat4 otherTransMatrix = other.makeTransformationMatrix();
    return check_overlap(other, transMatrix, otherTransMatrix);
}

bool PhysX::LineTrace(std::string channel, collisionBox* other, glm::vec2 start, glm::vec2 end){
    return LineTraceRadius(channel,other,start,end, 1);
}
bool PhysX::LineTraceRadius(std::string channel, collisionBox* other, glm::vec2 start, glm::vec2 end, float radius)
{
    float height = (std::sqrt(std::pow(start.x - end.x, 2) + std::pow(start.y - end.y, 2)));
    float angle = glm::degrees(atan2(start.y - end.y, start.x - end.x)) - 90;
    collisionBox* box = new collisionBox(channel, start, glm::vec2(radius, height), angle, glm::vec2(0.5, 0.0));
    return box->check_overlap(*other);
}
glm::vec2 GetLineInterceptionPoint(glm::vec2 startA, glm::vec2 endA, glm::vec2 startB, glm::vec2 endB) {
    // Y - Y1 = M(X-X1)
    
    float slopeA = (endA.y - startA.y) / ((endA.x - startA.x)+0.001);
    float slopeB = (endB.y - startB.y) / ((endB.x - startB.x) + 0.001);
    float heightA = slopeA*(-startA.x) + startA.y;
    float heightB = slopeB * (-startB.x) + startB.y;

    if (slopeA == slopeB)
        slopeA += 0.001;

    
    float interceptionX = (heightB - heightA) / ((slopeA - slopeB) + 0.001);
    float interceptionY = slopeA * (interceptionX - startA.x) + startA.y;

    return glm::vec2(interceptionX, interceptionY);
}

glm::vec2 PhysX::LineTracePoint(std::string channel, collisionBox* other, glm::vec2 start, glm::vec2 end)
{
    glm::vec2 originA = other->getOrigin(); glm::vec2 scaleA = other->getScale();
    if (originA.x == 0)
        originA.x = 0.001;
    if (originA.y == 0)
        originA.y = 0.001;
    glm::vec2 pointA = other->makeTransformationMatrix() * glm::vec4(glm::vec2((originA[0] - 1) * scaleA[0], (originA[1]) * scaleA[1]), 0, 1);
    glm::vec2 pointB = other->makeTransformationMatrix() * glm::vec4(glm::vec2((originA[0]) * scaleA[0], (originA[1] - 1) * scaleA[1]), 0, 1);
    glm::vec2 pointC = other->makeTransformationMatrix() * glm::vec4(glm::vec2((originA[0] - 1) * scaleA[0], (originA[1] - 1) * scaleA[1]), 0, 1);
    glm::vec2 pointD = other->makeTransformationMatrix() * glm::vec4(glm::vec2((originA[0]) * scaleA[0], (originA[1]) * scaleA[1]), 0, 1);

    auto BC = GetLineInterceptionPoint(start, end, pointB, pointC);
    auto CA = GetLineInterceptionPoint(start, end, pointC, pointA);
    auto AD = GetLineInterceptionPoint(start, end, pointA, pointD);
    auto DB = GetLineInterceptionPoint(start, end, pointD, pointB);

    float minDist = INFINITY;
    float tolerance = 0.5;
    float radius = std::sqrt(std::pow(other->getScale().x * 0.5, 2) + std::pow(other->getScale().y * 0.5, 2)) + tolerance;
    if(Utils::Distance2D(other->getCenter(), BC) <= radius && Utils::Distance2D(start, BC) <= minDist)
        minDist = Utils::Distance2D(start, BC);
    if (Utils::Distance2D(other->getCenter(), CA) <= radius && Utils::Distance2D(start, CA) <= minDist)
        minDist = Utils::Distance2D(start, CA);
    if (Utils::Distance2D(other->getCenter(), AD) <= radius && Utils::Distance2D(start, AD) <= minDist)
        minDist = Utils::Distance2D(start, AD);
    if (Utils::Distance2D(other->getCenter(), DB) <= radius && Utils::Distance2D(start, DB) <= minDist)
        minDist = Utils::Distance2D(start, DB);

    if (minDist == Utils::Distance2D(start, BC))
        return BC;
    if (minDist == Utils::Distance2D(start, CA))
        return CA;
    if (minDist == Utils::Distance2D(start, AD))
        return AD;
    if (minDist == Utils::Distance2D(start, DB))
        return DB;
    return glm::vec2(INFINITY);
    
}
bool PhysX::AreaCheck(std::string channel, collisionBox* other, glm::vec2 position, glm::vec2 size)
{
    collisionBox* box = new collisionBox(channel, position, size, 0, glm::vec2(0.5, 0.5));
    return box->check_overlap(*other);
}
bool PhysX::PointCheck(std::string chanel, collisionBox* other, glm::vec2 position)
{
    return AreaCheck(chanel,other,position,glm::vec2(0.001));
}

