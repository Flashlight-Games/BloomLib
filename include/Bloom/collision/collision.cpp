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

void PhysX::collisionBox::updatePoints()
{
    glm::vec2 this_origin = getOrigin(); glm::vec2 this_scale = getScale();
    if (this_origin.x == 0)
        this_origin.x = 0.001;
    if (this_origin.y == 0)
        this_origin.y = 0.001;
    thisA = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);
    thisB = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    thisC = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    thisD = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);
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
    glm::vec2 this_origin  = A.getOrigin(); glm::vec2 this_scale = A.getScale();
    thisPointB = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((this_origin [0]-1)*this_scale[0],(this_origin [1])*this_scale[1]),0,1),lineAngle);
    thisPointC = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((this_origin [0])*this_scale[0],(this_origin [1]-1)*this_scale[1]),0,1),lineAngle);
    thisPointD = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((this_origin [0]-1)*this_scale[0],(this_origin [1]-1)*this_scale[1]),0,1),lineAngle);
    thisPointA = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((this_origin [0])*this_scale[0],(this_origin [1])*this_scale[1]),0,1),lineAngle);

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

bool collisionBox::check_SAT(collisionBox* other,glm::mat4 transformationMatrix,glm::mat4 otherTransformationMatrix){
    glm::vec2 otherLocation = other->getLocation();
    glm::vec2 otherScale = other->getScale();
    uint16_t otherRotation = other->getRotation();
    float minPenetration;
    float minAxis;

    float PA = oneDimensionalCheck(*this, *other, (*this).getRotation() + 90, transformationMatrix, otherTransformationMatrix);
    if(PA == -1) // A
        return false;
    float PB = oneDimensionalCheck(*this, *other, (*this).getRotation(), transformationMatrix, otherTransformationMatrix);
    if(PB == -1) // B
        return false;
    float PC = oneDimensionalCheck(*this, *other, other->getRotation() + 90, transformationMatrix, otherTransformationMatrix);
    if(PC == -1) // C
        return false;
    float PD = oneDimensionalCheck(*this, *other, other->getRotation(), transformationMatrix, otherTransformationMatrix);
    if(PD == -1) // D
        return false;

    minPenetration = std::min(std::min(PA, PB), std::min(PC, PD));
    if (minPenetration == PA)
        minAxis = (*this).getRotation() + 90;
    else if (minPenetration == PB)
        minAxis = (*this).getRotation();
    else if (minPenetration == PC)
        minAxis = other->getRotation() + 90;
    else if (minPenetration == PD)
        minAxis = other->getRotation();

    return true;
}

bool collisionBox::check_SAT(collisionBox* other){
    glm::mat4 transMatrix = makeTransformationMatrix();
    glm::mat4 otherTransMatrix = other->makeTransformationMatrix();
    return check_SAT(other, transMatrix, otherTransMatrix);
}

bool PhysX::collisionBox::check_Point(glm::vec2 point)
{
    glm::vec2 this_origin = getOrigin(); glm::vec2 this_scale = getScale();
    if (this_origin.x == 0)
        this_origin.x = 0.001;
    if (this_origin.y == 0)
        this_origin.y = 0.001;
    auto this_transformation = makeTransformationMatrix();
    thisA = this_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);
    thisB = this_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    thisC = this_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    thisD = this_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);

    return (point.x >= glm::min(glm::min(thisA, thisB), glm::min(thisC, thisD)).x && point.y >= glm::min(glm::min(thisA, thisB), glm::min(thisC, thisD)).y && point.x <= glm::max(glm::max(thisA, thisB), glm::max(thisC, thisD)).x && point.y <= glm::max(glm::max(thisA, thisB), glm::max(thisC, thisD)).y);
}

glm::vec2 collisionBox::check_Traced(collisionBox* other, glm::mat4 transformationMatrix, glm::mat4 otherTransformationMatrix) {
    if (this->channel != other->channel)
        return glm::vec2(INFINITY);

    glm::vec2 this_origin = getOrigin(); glm::vec2 this_scale = getScale();
    if (this_origin.x == 0)
        this_origin.x = 0.001;
    if (this_origin.y == 0)
        this_origin.y = 0.001;
    thisA = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin [0] - 1) * this_scale[0], (this_origin [1]) * this_scale[1]), 0, 1);
    thisB = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin [0]) * this_scale[0], (this_origin [1] - 1) * this_scale[1]), 0, 1);
    thisC = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin [0] - 1) * this_scale[0], (this_origin [1] - 1) * this_scale[1]), 0, 1);
    thisD = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin [0]) * this_scale[0], (this_origin [1]) * this_scale[1]), 0, 1);

    glm::vec2 other_origin = other->getOrigin(); glm::vec2 other_scale = other->getScale();
    if (other_origin.x == 0)
        other_origin.x = 0.001;
    if (other_origin.y == 0)
        other_origin.y = 0.001;
    glm::vec2 otherA = other->makeTransformationMatrix() * glm::vec4(glm::vec2((other_origin[0] - 1) * other_scale[0], (other_origin[1]) * other_scale[1]), 0, 1);
    glm::vec2 otherB = other->makeTransformationMatrix() * glm::vec4(glm::vec2((other_origin[0]) * other_scale[0], (other_origin[1] - 1) * other_scale[1]), 0, 1);
    glm::vec2 otherC = other->makeTransformationMatrix() * glm::vec4(glm::vec2((other_origin[0] - 1) * other_scale[0], (other_origin[1] - 1) * other_scale[1]), 0, 1);
    glm::vec2 otherD = other->makeTransformationMatrix() * glm::vec4(glm::vec2((other_origin[0]) * other_scale[0], (other_origin[1]) * other_scale[1]), 0, 1);

   
    glm::vec2 hitpoint;
    hitpoint = LineTrace(this->channel, other, this->getCenter(), thisA);
    if (hitpoint != glm::vec2 INFINITY)
        return hitpoint;
    hitpoint = LineTrace(this->channel, other, this->getCenter(), thisB);
    if (hitpoint != glm::vec2 INFINITY)
        return hitpoint;
    hitpoint = LineTrace(this->channel, other, this->getCenter(), thisC);
    if (hitpoint != glm::vec2 INFINITY)
        return hitpoint;
    hitpoint = LineTrace(this->channel, other, this->getCenter(), thisD);
    if (hitpoint != glm::vec2 INFINITY)
        return hitpoint;

    hitpoint = LineTrace(other->getChannel(), this, other->getCenter(), otherA);
    if (hitpoint != glm::vec2 INFINITY)
        return hitpoint;
    hitpoint = LineTrace(other->getChannel(), this, other->getCenter(), otherB);
    if (hitpoint != glm::vec2 INFINITY)
        return hitpoint;
    hitpoint = LineTrace(other->getChannel(), this, other->getCenter(), otherC);
    if (hitpoint != glm::vec2 INFINITY)
        return hitpoint;
    hitpoint = LineTrace(other->getChannel(), this, other->getCenter(), otherD);
    if (hitpoint != glm::vec2 INFINITY)
        return hitpoint;

    return glm::vec2(INFINITY);
}

glm::vec2 collisionBox::check_Traced(collisionBox* other){
    glm::mat4 transMatrix = makeTransformationMatrix();
    glm::mat4 otherTransMatrix = other->makeTransformationMatrix();
    return check_Traced(other, transMatrix, otherTransMatrix);
}

glm::vec2 PhysX::collisionBox::check_TracedNormal(collisionBox* other, glm::mat4 transformationMatrix, glm::mat4 otherTransformationMatrix)
{
    if (this->channel != other->channel)
        return glm::vec2(INFINITY);

    glm::vec2 this_origin = getOrigin(); glm::vec2 this_scale = getScale();
    if (this_origin.x == 0)
        this_origin.x = 0.001;
    if (this_origin.y == 0)
        this_origin.y = 0.001;
    auto this_transformation = makeTransformationMatrix();
    thisA = this_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);
    thisB = this_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    thisC = this_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    thisD = this_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);

    glm::vec2 other_origin = other->getOrigin(); glm::vec2 other_scale = other->getScale();
    if (other_origin.x == 0)
        other_origin.x = 0.001;
    if (other_origin.y == 0)
        other_origin.y = 0.001;
    auto other_transformation = other->makeTransformationMatrix();
    glm::vec2 otherA = other_transformation * glm::vec4(glm::vec2((other_origin[0] - 1) * other_scale[0], (other_origin[1]) * other_scale[1]), 0, 1);
    glm::vec2 otherB = other_transformation * glm::vec4(glm::vec2((other_origin[0]) * other_scale[0], (other_origin[1] - 1) * other_scale[1]), 0, 1);
    glm::vec2 otherC = other_transformation * glm::vec4(glm::vec2((other_origin[0] - 1) * other_scale[0], (other_origin[1] - 1) * other_scale[1]), 0, 1);
    glm::vec2 otherD = other_transformation * glm::vec4(glm::vec2((other_origin[0]) * other_scale[0], (other_origin[1]) * other_scale[1]), 0, 1);


    glm::vec2 hitpoint;
    hitpoint = LineTraceNormal(this->channel, other, this->getCenter(), thisA);
    if (hitpoint != glm::vec2 (0))
        return hitpoint;
    hitpoint = LineTraceNormal(this->channel, other, this->getCenter(), thisB);
    if (hitpoint != glm::vec2 (0))
        return hitpoint;
    hitpoint = LineTraceNormal(this->channel, other, this->getCenter(), thisC);
    if (hitpoint != glm::vec2 (0))
        return hitpoint;
    hitpoint = LineTraceNormal(this->channel, other, this->getCenter(), thisD);
    if (hitpoint != glm::vec2 (0))
        return hitpoint;
    
    hitpoint = -LineTraceNormal(other->getChannel(), this, other->getCenter(), otherA);
    if (hitpoint != glm::vec2 (0))
        return hitpoint;
    hitpoint = -LineTraceNormal(other->getChannel(), this, other->getCenter(), otherB);
    if (hitpoint != glm::vec2 (0))
        return hitpoint;
    hitpoint = -LineTraceNormal(other->getChannel(), this, other->getCenter(), otherC);
    if (hitpoint != glm::vec2 (0))
        return hitpoint;
    hitpoint = -LineTraceNormal(other->getChannel(), this, other->getCenter(), otherD);
    if (hitpoint != glm::vec2 (0))
        return hitpoint;
    return glm::vec2(0);
}

glm::vec2 PhysX::collisionBox::check_TracedNormal(collisionBox* other)
{
    glm::mat4 transMatrix = makeTransformationMatrix();
    glm::mat4 otherTransMatrix = other->makeTransformationMatrix();
    return check_TracedNormal(other, transMatrix, otherTransMatrix);
}

glm::vec2 PhysX::GetLineInterceptionPoint(glm::vec2 startA, glm::vec2 endA, glm::vec2 startB, glm::vec2 endB) {
    bool Avertical = (endA.x - startA.x) == 0;
    bool Bvertical = (endB.x - startB.x) == 0;

    if (!Avertical && !Bvertical) {
        double slopeA = (endA.y - startA.y) / ((endA.x - startA.x));
        double slopeB = (endB.y - startB.y) / ((endB.x - startB.x));
        float heightA = slopeA * (-startA.x) + startA.y;
        float heightB = slopeB * (-startB.x) + startB.y;

        if (slopeA == slopeB)
            return glm::vec2(INFINITY);
        float interceptionX = (heightB - heightA) / ((slopeA - slopeB));
        float interceptionY = slopeA * (interceptionX - startA.x) + startA.y;

        return glm::vec2(interceptionX, interceptionY);
    }
    else if (Avertical) {
        double slopeB = (endB.y - startB.y) / ((endB.x - startB.x));
        float heightB = slopeB * (-startB.x) + startB.y;

        float interceptionX = startA.x;
        float interceptionY = slopeB * startA.x + heightB;
        return glm::vec2(interceptionX, interceptionY);
    }
    else if (Bvertical) {
        double slopeA = (endA.y - startA.y) / ((endA.x - startA.x));
        float heightA = slopeA * (-startA.x) + startA.y;

        float interceptionX = startB.x;
        float interceptionY = slopeA * startB.x + heightA;
        return glm::vec2(interceptionX, interceptionY);
    }
    else
        return glm::vec2(INFINITY);
}

glm::vec2 PhysX::LineTrace(std::string channel, collisionBox* other, glm::vec2 start, glm::vec2 end)
{
    if (other->getChannel() != channel)
        return glm::vec2(INFINITY);

    if (other->check_Point(start))
        return start;

    if (other->check_Point(end))
        return end;

    glm::vec2 this_origin = other->getOrigin(); glm::vec2 this_scale = other->getScale();
    if (this_origin.x == 0)
        this_origin.x = 0.001;
    if (this_origin.y == 0)
        this_origin.y = 0.001;
    auto other_transformation = other->makeTransformationMatrix();
    glm::vec2 pointA = other_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);
    glm::vec2 pointB = other_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    glm::vec2 pointC = other_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    glm::vec2 pointD = other_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);

    auto BC = GetLineInterceptionPoint(start, end, pointB, pointC);
    auto CA = GetLineInterceptionPoint(start, end, pointC, pointA);
    auto AD = GetLineInterceptionPoint(start, end, pointA, pointD);
    auto DB = GetLineInterceptionPoint(start, end, pointD, pointB);

    float minDist = INFINITY;
    float tolerance = 0;
    float radius = std::sqrt(std::pow(other->getScale().x * 0.5, 2) + std::pow(other->getScale().y * 0.5, 2)) + tolerance;

    if (Utils::Distance2D(other->getCenter(), BC) <= radius && Utils::Distance2D(start, BC) <= minDist) {
        minDist = Utils::Distance2D(start, BC);
    } if (Utils::Distance2D(other->getCenter(), CA) <= radius && Utils::Distance2D(start, CA) <= minDist) {
        minDist = Utils::Distance2D(start, CA);
    } if (Utils::Distance2D(other->getCenter(), AD) <= radius && Utils::Distance2D(start, AD) <= minDist) {
        minDist = Utils::Distance2D(start, AD);
    } if (Utils::Distance2D(other->getCenter(), DB) <= radius && Utils::Distance2D(start, DB) <= minDist) {
        minDist = Utils::Distance2D(start, DB);
    }

    glm::vec2 return_point = glm::vec2(INFINITY);


    if (minDist == Utils::Distance2D(start, BC))
        return_point = BC;
    else if (minDist == Utils::Distance2D(start, CA))
        return_point = CA;
    else if (minDist == Utils::Distance2D(start, AD))
        return_point = AD;
    else if (minDist == Utils::Distance2D(start, DB))
        return_point = DB;
    else
        return return_point;

    if (return_point.x >= std::min(start.x, end.x) && return_point.x <= std::max(start.x, end.x) && return_point.y >= std::min(start.y, end.y) && return_point.y <= std::max(start.y, end.y)) {
        return return_point;
    }
    else {
        return glm::vec2(INFINITY);
    }
}

glm::vec2 PhysX::LineTrace(std::vector<std::string> channels, collisionBox* other, glm::vec2 start, glm::vec2 end)
{
    if (channels.size() > 0) {
        if (std::find(channels.begin(), channels.end(), other->getChannel()) == channels.end())
            return glm::vec2(INFINITY);
    }

    if (other->check_Point(start))
        return start;

    if (other->check_Point(end))
        return end;

    glm::vec2 this_origin = other->getOrigin(); glm::vec2 this_scale = other->getScale();
    if (this_origin.x == 0)
        this_origin.x = 0.001;
    if (this_origin.y == 0)
        this_origin.y = 0.001;
    auto other_transformation = other->makeTransformationMatrix();
    glm::vec2 pointA = other_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);
    glm::vec2 pointB = other_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    glm::vec2 pointC = other_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    glm::vec2 pointD = other_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);

    auto BC = GetLineInterceptionPoint(start, end, pointB, pointC);
    auto CA = GetLineInterceptionPoint(start, end, pointC, pointA);
    auto AD = GetLineInterceptionPoint(start, end, pointA, pointD);
    auto DB = GetLineInterceptionPoint(start, end, pointD, pointB);

    float minDist = INFINITY;
    float tolerance = 0;
    float radius = std::sqrt(std::pow(other->getScale().x * 0.5, 2) + std::pow(other->getScale().y * 0.5, 2)) + tolerance;

    if (Utils::Distance2D(other->getCenter(), BC) <= radius && Utils::Distance2D(start, BC) <= minDist) {
        minDist = Utils::Distance2D(start, BC);
    } if (Utils::Distance2D(other->getCenter(), CA) <= radius && Utils::Distance2D(start, CA) <= minDist) {
        minDist = Utils::Distance2D(start, CA);
    } if (Utils::Distance2D(other->getCenter(), AD) <= radius && Utils::Distance2D(start, AD) <= minDist) {
        minDist = Utils::Distance2D(start, AD);
    } if (Utils::Distance2D(other->getCenter(), DB) <= radius && Utils::Distance2D(start, DB) <= minDist) {
        minDist = Utils::Distance2D(start, DB);
    }

    glm::vec2 return_point = glm::vec2(INFINITY);


    if (minDist == Utils::Distance2D(start, BC))
        return_point = BC;
    else if (minDist == Utils::Distance2D(start, CA))
        return_point = CA;
    else if (minDist == Utils::Distance2D(start, AD))
        return_point = AD;
    else if (minDist == Utils::Distance2D(start, DB))
        return_point = DB;
    else
        return return_point;

    if (return_point.x >= std::min(start.x, end.x) && return_point.x <= std::max(start.x, end.x) && return_point.y >= std::min(start.y, end.y) && return_point.y <= std::max(start.y, end.y)) {
        return return_point;
    }
    else {
        return glm::vec2(INFINITY);
    }
}

glm::vec2 PhysX::GetNormal(glm::vec2 pointA, glm::vec2 pointB) {
    float dx = pointB.x - pointA.x;
    float dy = pointB.y - pointA.y;
    auto normalA = glm::vec2(-dy, dx);
    auto normalB = glm::vec2(dy, -dx);
    
    return glm::normalize(normalA);
}

glm::vec2 PhysX::LineTraceNormal(std::string channel, collisionBox* other, glm::vec2 start, glm::vec2 end)
{
    glm::vec2 this_origin = other->getOrigin(); glm::vec2 this_scale = other->getScale();
    if (this_origin.x == 0)
        this_origin.x = 0.001;
    if (this_origin.y == 0)
        this_origin.y = 0.001;
    auto other_transformation = other->makeTransformationMatrix();
    glm::vec2 pointA = other_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);
    glm::vec2 pointB = other_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    glm::vec2 pointC = other_transformation * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
    glm::vec2 pointD = other_transformation * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);

    auto BC = GetLineInterceptionPoint(start, end, pointB, pointC);
    auto CA = GetLineInterceptionPoint(start, end, pointC, pointA);
    auto AD = GetLineInterceptionPoint(start, end, pointA, pointD);
    auto DB = GetLineInterceptionPoint(start, end, pointD, pointB);

    float minDist = INFINITY;
    float tolerance = -1;
    float radius = std::sqrt(std::pow(other->getScale().x * 0.5, 2) + std::pow(other->getScale().y * 0.5, 2)) + tolerance;

    if (Utils::Distance2D(other->getCenter(), BC) <= radius && Utils::Distance2D(start, BC) <= minDist)
        minDist = Utils::Distance2D(start, BC);
    if (Utils::Distance2D(other->getCenter(), CA) <= radius && Utils::Distance2D(start, CA) <= minDist)
        minDist = Utils::Distance2D(start, CA);
    if (Utils::Distance2D(other->getCenter(), AD) <= radius && Utils::Distance2D(start, AD) <= minDist)
        minDist = Utils::Distance2D(start, AD);
    if (Utils::Distance2D(other->getCenter(), DB) <= radius && Utils::Distance2D(start, DB) <= minDist)
        minDist = Utils::Distance2D(start, DB);

    glm::vec2 return_point;
    glm::vec2 return_normal;

    if (minDist == Utils::Distance2D(start, BC)) {
        return_point = BC;
        return_normal = GetNormal(pointB, pointC);
    } else if (minDist == Utils::Distance2D(start, CA)){
        return_point = CA;
        return_normal = GetNormal(pointC, pointA);
    } else if (minDist == Utils::Distance2D(start, AD)){
        return_point = AD;
        return_normal = GetNormal(pointA, pointD);
    } else if (minDist == Utils::Distance2D(start, DB)) {
        return_point = DB;
        return_normal = GetNormal(pointD, pointB);
    } else
        return glm::vec2(0);

    if (return_point.x > std::min(start.x, end.x) && return_point.x < std::max(start.x, end.x) && return_point.y > std::min(start.y, end.y) && return_point.y < std::max(start.y, end.y))
        return return_normal;
    else
        return glm::vec2(0);

}

bool PhysX::AreaCheck_SAT(std::string channel, collisionBox* other, glm::vec2 position, glm::vec2 size)
{
    collisionBox* box = new collisionBox(channel, position, size, 0, glm::vec2(0.5, 0.5));
    bool result = box->check_SAT(other);
    delete box;
    return result;
}

bool PhysX::PointCheck_SAT(std::string chanel, collisionBox* other, glm::vec2 position)
{
    return AreaCheck_SAT(chanel,other,position,glm::vec2(0.001));
}

