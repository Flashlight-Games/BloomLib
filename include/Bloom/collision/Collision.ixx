#define PI 3.14159265358979323846   

#include "Bloom/dependencies.h"
#include <iostream>

export module Bloom.Collision;

import Bloom.Sprite;
import std;

float getPointOnLineAtAngle(glm::vec2 point, uint16_t angle) {
    return (point.x + (point.y * tan(angle * PI / 180))) / (1 + pow(tan(angle * PI / 180), 2));
}

glm::vec2 getRotatedRectPoint_RB(glm::vec2 center, glm::vec2 size, uint16_t angle) {
    return glm::vec2(center.x + ((size.x / 2) * cos(angle * PI / 180)) + ((size.y / 2) * sin(angle * PI / 180)), center.y + ((size.x / 2) * sin(angle * PI / 180)) - ((size.y / 2) * cos(angle * PI / 180)));
}

glm::vec2 getRotatedRectPoint_LB(glm::vec2 center, glm::vec2 size, uint16_t angle) {
    return glm::vec2(center.x - ((size.x / 2) * cos(angle * PI / 180)) + ((size.y / 2) * sin(angle * PI / 180)), center.y - ((size.x / 2) * sin(angle * PI / 180)) - ((size.y / 2) * cos(angle * PI / 180)));
}

glm::vec2 getRotatedRectPoint_LT(glm::vec2 center, glm::vec2 size, uint16_t angle) {
    return glm::vec2(center.x - ((size.x / 2) * cos(angle * PI / 180)) - ((size.y / 2) * sin(angle * PI / 180)), center.y - ((size.x / 2) * sin(angle * PI / 180)) + ((size.y / 2) * cos(angle * PI / 180)));
}

glm::vec2 getRotatedRectPoint_RT(glm::vec2 center, glm::vec2 size, uint16_t angle) {
    return glm::vec2(center.x + ((size.x / 2) * cos(angle * PI / 180)) - ((size.y / 2) * sin(angle * PI / 180)), center.y + ((size.x / 2) * sin(angle * PI / 180)) + ((size.y / 2) * cos(angle * PI / 180)));
}

glm::vec2 GetLineInterceptionPoint(glm::vec2 startA, glm::vec2 endA, glm::vec2 startB, glm::vec2 endB) {
    glm::vec2 directionA = endA - startA;
    glm::vec2 directionB = endB - startB;

    float denom = directionA.x * directionB.y - directionA.y * directionB.x;

    if (denom == 0) {
        return glm::vec2(INFINITY);
    }

    float ua = (directionB.x * (startA.y - startB.y) - directionB.y * (startA.x - startB.x)) / denom;
    float ub = (directionA.x * (startA.y - startB.y) - directionA.y * (startA.x - startB.x)) / denom;

    if (0 <= ua && ua <= 1 && 0 <= ub && ub <= 1) {
        return startA + ua * directionA;
    }

    return glm::vec2(INFINITY);
}

export namespace Bloom {
    class collisionBox {
    public:
        collisionBox(std::string channel, glm::vec2 location = glm::vec2(0, 0), glm::vec2 scale = glm::vec2(1, 1), uint16_t rotation = 0, glm::vec2 origin = glm::vec2(0.5, 0.5)) {
            this->location = location;
            this->rotation = rotation;
            this->scale = scale;
            this->origin = origin;

            this->channel = channel;
        }
        collisionBox(std::string channel, Bloom::Sprite* sprite) {
            this->location = sprite->GetLocation();
            this->rotation = sprite->GetRotation();
            this->scale = sprite->GetSize();
            this->origin = sprite->GetOrigin();

            this->channel = channel;
        }

        float oneDimensionalCheck(Bloom::collisionBox* A, Bloom::collisionBox* B, uint16_t lineAngle, glm::mat4 transformationMatrix, glm::mat4 otherTransformationMatrix) {
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
            glm::vec2 this_origin = A->getOrigin(); glm::vec2 this_scale = A->getScale();
            thisPointB = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1), lineAngle);
            thisPointC = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1), lineAngle);
            thisPointD = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1), lineAngle);
            thisPointA = getPointOnLineAtAngle(transformationMatrix * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1), lineAngle);

            glm::vec2 originB = B->getOrigin(); glm::vec2 scaleB = B->getScale();
            otherPointA = getPointOnLineAtAngle(otherTransformationMatrix * glm::vec4(glm::vec2((originB[0] - 1) * scaleB[0], (originB[1]) * scaleB[1]), 0, 1), lineAngle);
            otherPointB = getPointOnLineAtAngle(otherTransformationMatrix * glm::vec4(glm::vec2((originB[0]) * scaleB[0], (originB[1] - 1) * scaleB[1]), 0, 1), lineAngle);
            otherPointC = getPointOnLineAtAngle(otherTransformationMatrix * glm::vec4(glm::vec2((originB[0] - 1) * scaleB[0], (originB[1] - 1) * scaleB[1]), 0, 1), lineAngle);
            otherPointD = getPointOnLineAtAngle(otherTransformationMatrix * glm::vec4(glm::vec2((originB[0]) * scaleB[0], (originB[1]) * scaleB[1]), 0, 1), lineAngle);

            thisMin = std::min(std::min(thisPointA, thisPointB), std::min(thisPointC, thisPointD));
            thisMax = std::max(std::max(thisPointA, thisPointB), std::max(thisPointC, thisPointD));

            otherMin = std::min(std::min(otherPointA, otherPointB), std::min(otherPointC, otherPointD));
            otherMax = std::max(std::max(otherPointA, otherPointB), std::max(otherPointC, otherPointD));

            float penetrationDepth = abs(otherMax - thisMin);
            penetrationDepth = (otherMin < thisMax && otherMax > thisMin) ? penetrationDepth : -1;
            return penetrationDepth;
        }

        bool check_SAT(collisionBox* other, glm::mat4 transformationMatrix, glm::mat4 otherTransformationMatrix) {
            glm::vec2 otherLocation = other->getLocation();
            glm::vec2 otherScale = other->getScale();
            uint16_t otherRotation = other->getRotation();
            float minPenetration;
            float minAxis;

            float PA = oneDimensionalCheck(this, other, (*this).getRotation() + 90, transformationMatrix, otherTransformationMatrix);
            if (PA == -1) // A
                return false;
            float PB = oneDimensionalCheck(this, other, (*this).getRotation(), transformationMatrix, otherTransformationMatrix);
            if (PB == -1) // B
                return false;
            float PC = oneDimensionalCheck(this, other, other->getRotation() + 90, transformationMatrix, otherTransformationMatrix);
            if (PC == -1) // C
                return false;
            float PD = oneDimensionalCheck(this, other, other->getRotation(), transformationMatrix, otherTransformationMatrix);
            if (PD == -1) // D
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
        bool check_SAT(collisionBox* other) {
            glm::mat4 transMatrix = makeTransformationMatrix();
            glm::mat4 otherTransMatrix = other->makeTransformationMatrix();
            return check_SAT(other, transMatrix, otherTransMatrix);
        }

        bool check_Point(glm::vec2 point) {
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

        glm::vec2 LineTrace(std::vector<std::string> channels, collisionBox* other, glm::vec2 start, glm::vec2 end, float width = 0.0) {
            if (!channels.empty() && std::find(channels.begin(), channels.end(), other->getChannel()) == channels.end()) {
                return glm::vec2(INFINITY);
            }

            if (other->check_Point(start)) return start;
            if (other->check_Point(end)) return end;

            glm::vec2 this_origin = other->getOrigin();
            glm::vec2 this_scale = other->getScale() + glm::vec2(width);

            auto other_transformation = other->makeTransformationMatrix();

            glm::vec2 pointA = other_transformation * glm::vec4(glm::vec2((this_origin.x - 1) * this_scale.x, (this_origin.y) * this_scale.y), 0, 1);
            glm::vec2 pointB = other_transformation * glm::vec4(glm::vec2((this_origin.x) * this_scale.x, (this_origin.y - 1) * this_scale.y), 0, 1);
            glm::vec2 pointC = other_transformation * glm::vec4(glm::vec2((this_origin.x - 1) * this_scale.x, (this_origin.y - 1) * this_scale.y), 0, 1);
            glm::vec2 pointD = other_transformation * glm::vec4(glm::vec2((this_origin.x) * this_scale.x, (this_origin.y) * this_scale.y), 0, 1);

            std::vector<glm::vec2> corners = { pointA, pointB, pointC, pointD };

            glm::vec2 closestIntersection = glm::vec2(INFINITY);
            float minDist = INFINITY;

            for (size_t i = 0; i < corners.size(); ++i) {
                glm::vec2 intersection = GetLineInterceptionPoint(start, end, corners[i], corners[(i + 1) % corners.size()]);
                if (intersection != glm::vec2(INFINITY)) {
                    float dist = glm::length(start - intersection);
                    if (dist < minDist) {
                        minDist = dist;
                        closestIntersection = intersection;
                    }
                }
            }

            if (closestIntersection != glm::vec2(INFINITY) &&
                closestIntersection.x >= std::min(start.x, end.x) && closestIntersection.x <= std::max(start.x, end.x) &&
                closestIntersection.y >= std::min(start.y, end.y) && closestIntersection.y <= std::max(start.y, end.y)) {
                return closestIntersection;
            }

            return glm::vec2(INFINITY);
        }
        glm::vec2 GetNormal(glm::vec2 pointA, glm::vec2 pointB) {
            float dx = pointB.x - pointA.x;
            float dy = pointB.y - pointA.y;
            auto normalA = glm::vec2(-dy, dx);
            auto normalB = glm::vec2(dy, -dx);

            return glm::normalize(normalA);
        }
        glm::vec2 LineTraceNormal(std::string channel, collisionBox* other, glm::vec2 start, glm::vec2 end) {
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

            if (glm::distance(other->getCenter(), BC) <= radius && glm::distance(start, BC) <= minDist)
                minDist = glm::distance(start, BC);
            if (glm::distance(other->getCenter(), CA) <= radius && glm::distance(start, CA) <= minDist)
                minDist = glm::distance(start, CA);
            if (glm::distance(other->getCenter(), AD) <= radius && glm::distance(start, AD) <= minDist)
                minDist = glm::distance(start, AD);
            if (glm::distance(other->getCenter(), DB) <= radius && glm::distance(start, DB) <= minDist)
                minDist = glm::distance(start, DB);

            glm::vec2 return_point;
            glm::vec2 return_normal;

            if (minDist == glm::distance(start, BC)) {
                return_point = BC;
                return_normal = GetNormal(pointB, pointC);
            }
            else if (minDist == glm::distance(start, CA)) {
                return_point = CA;
                return_normal = GetNormal(pointC, pointA);
            }
            else if (minDist == glm::distance(start, AD)) {
                return_point = AD;
                return_normal = GetNormal(pointA, pointD);
            }
            else if (minDist == glm::distance(start, DB)) {
                return_point = DB;
                return_normal = GetNormal(pointD, pointB);
            }
            else
                return glm::vec2(0);

            if (return_point.x > std::min(start.x, end.x) && return_point.x < std::max(start.x, end.x) && return_point.y > std::min(start.y, end.y) && return_point.y < std::max(start.y, end.y))
                return return_normal;
            else
                return glm::vec2(0);

        }

        glm::vec2 check_Traced(collisionBox* other, glm::mat4 transformationMatrix, glm::mat4 otherTransformationMatrix) {
            if (this->channel != other->channel)
                return glm::vec2(INFINITY);

            glm::vec2 this_origin = getOrigin(); glm::vec2 this_scale = getScale();
            if (this_origin.x == 0)
                this_origin.x = 0.001;
            if (this_origin.y == 0)
                this_origin.y = 0.001;
            thisA = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);
            thisB = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
            thisC = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin[0] - 1) * this_scale[0], (this_origin[1] - 1) * this_scale[1]), 0, 1);
            thisD = makeTransformationMatrix() * glm::vec4(glm::vec2((this_origin[0]) * this_scale[0], (this_origin[1]) * this_scale[1]), 0, 1);

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
            hitpoint = LineTrace({ this->channel }, other, this->getCenter(), thisA);
            if (hitpoint != glm::vec2 INFINITY)
                return hitpoint;
            hitpoint = LineTrace({ this->channel }, other, this->getCenter(), thisB);
            if (hitpoint != glm::vec2 INFINITY)
                return hitpoint;
            hitpoint = LineTrace({ this->channel }, other, this->getCenter(), thisC);
            if (hitpoint != glm::vec2 INFINITY)
                return hitpoint;
            hitpoint = LineTrace({ this->channel }, other, this->getCenter(), thisD);
            if (hitpoint != glm::vec2 INFINITY)
                return hitpoint;

            hitpoint = LineTrace({ other->getChannel() }, this, other->getCenter(), otherA);
            if (hitpoint != glm::vec2 INFINITY)
                return hitpoint;
            hitpoint = LineTrace({ other->getChannel() }, this, other->getCenter(), otherB);
            if (hitpoint != glm::vec2 INFINITY)
                return hitpoint;
            hitpoint = LineTrace({ other->getChannel() }, this, other->getCenter(), otherC);
            if (hitpoint != glm::vec2 INFINITY)
                return hitpoint;
            hitpoint = LineTrace({other->getChannel()}, this, other->getCenter(), otherD);
            if (hitpoint != glm::vec2 INFINITY)
                return hitpoint;

            return glm::vec2(INFINITY);
        }

        glm::vec2 check_Traced(collisionBox* other) {
            glm::mat4 transMatrix = makeTransformationMatrix();
            glm::mat4 otherTransMatrix = other->makeTransformationMatrix();
            return check_Traced(other, transMatrix, otherTransMatrix);
        }

        glm::vec2 check_TracedNormal(collisionBox* other, glm::mat4 transformationMatrix, glm::mat4 otherTransformationMatrix) {
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
            if (hitpoint != glm::vec2(0))
                return hitpoint;
            hitpoint = LineTraceNormal(this->channel, other, this->getCenter(), thisB);
            if (hitpoint != glm::vec2(0))
                return hitpoint;
            hitpoint = LineTraceNormal(this->channel, other, this->getCenter(), thisC);
            if (hitpoint != glm::vec2(0))
                return hitpoint;
            hitpoint = LineTraceNormal(this->channel, other, this->getCenter(), thisD);
            if (hitpoint != glm::vec2(0))
                return hitpoint;

            hitpoint = -LineTraceNormal(other->getChannel(), this, other->getCenter(), otherA);
            if (hitpoint != glm::vec2(0))
                return hitpoint;
            hitpoint = -LineTraceNormal(other->getChannel(), this, other->getCenter(), otherB);
            if (hitpoint != glm::vec2(0))
                return hitpoint;
            hitpoint = -LineTraceNormal(other->getChannel(), this, other->getCenter(), otherC);
            if (hitpoint != glm::vec2(0))
                return hitpoint;
            hitpoint = -LineTraceNormal(other->getChannel(), this, other->getCenter(), otherD);
            if (hitpoint != glm::vec2(0))
                return hitpoint;
            return glm::vec2(0);
        }

        glm::vec2 check_TracedNormal(collisionBox* other) {
            glm::mat4 transMatrix = makeTransformationMatrix();
            glm::mat4 otherTransMatrix = other->makeTransformationMatrix();
            return check_TracedNormal(other, transMatrix, otherTransMatrix);
        }

        glm::vec2 getLocation() const { return location; };
        glm::vec2 getCenter() const { return location + getScale() * (getOrigin() - glm::vec2(0.5)); }
        glm::vec2 getScale() const { return scale; };
        int16_t getRotation() const { return rotation; };
        glm::vec2 getOrigin() const { return origin; };
        std::string getChannel() const { return channel; };

        void setLocation(glm::vec2 location) { if (location != this->location) { this->location = location; updatePoints(); } };
        void setScale(glm::vec2 scale) { this->scale = scale; updatePoints(); };
        void setRotation(uint16_t rotation) { this->rotation = rotation; updatePoints(); };

        glm::mat4 makeTransformationMatrix() {
            glm::mat4 result = glm::mat4(1);
            result = glm::translate(result, glm::vec3(location[0], location[1], 0.0f));
            result = glm::rotate(result, (float)glm::radians((float)this->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
            return result;
        }

        void copyTransform(Bloom::Sprite* sprite) { this->location = sprite->GetLocation(); this->rotation = sprite->GetRotation(); this->scale = sprite->GetSize(); }

    private:
        void updatePoints() {
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
        int16_t rotation = 0;
        glm::vec2 location = glm::vec2(0, 0);
        glm::vec2 scale = glm::vec2(1, 1);
        glm::vec2 origin = glm::vec2(0, 0);

        std::string channel;

        glm::vec2 thisA;
        glm::vec2 thisB;
        glm::vec2 thisC;
        glm::vec2 thisD;
    };

    glm::vec2 LineTrace(std::string channel, collisionBox* other, glm::vec2 start, glm::vec2 end) {
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

        if (glm::distance(other->getCenter(), BC) <= radius && glm::distance(start, BC) <= minDist) {
            minDist = glm::distance(start, BC);
        } if (glm::distance(other->getCenter(), CA) <= radius && glm::distance(start, CA) <= minDist) {
            minDist = glm::distance(start, CA);
        } if (glm::distance(other->getCenter(), AD) <= radius && glm::distance(start, AD) <= minDist) {
            minDist = glm::distance(start, AD);
        } if (glm::distance(other->getCenter(), DB) <= radius && glm::distance(start, DB) <= minDist) {
            minDist = glm::distance(start, DB);
        }

        glm::vec2 return_point = glm::vec2(INFINITY);


        if (minDist == glm::distance(start, BC))
            return_point = BC;
        else if (minDist == glm::distance(start, CA))
            return_point = CA;
        else if (minDist == glm::distance(start, AD))
            return_point = AD;
        else if (minDist == glm::distance(start, DB))
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
    
    bool AreaCheck_SAT(std::string channel, collisionBox* other, glm::vec2 position, glm::vec2 size) {
        collisionBox* box = new collisionBox(channel, position, size, 0, glm::vec2(0.5, 0.5));
        bool result = box->check_SAT(other);
        delete box;
        return result;
    }
    bool PointCheck_SAT(std::string chanel, collisionBox* other, glm::vec2 position) {
        return AreaCheck_SAT(chanel, other, position, glm::vec2(0.001));
    }
}