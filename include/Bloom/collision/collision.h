#ifndef COLLISION
#define COLLISION

#define PI 3.14159265358979323846   

#include "Bloom/dependencies.h"
#include "Bloom/sprite/sprite.h"
#include "Bloom/sprite/spritesheet.h"
#include "Bloom/utils/utils.h"
#include <iostream>

namespace PhysX
{
    class collisionBox{
        public:
        collisionBox(){};
        collisionBox(std::string channel,glm::vec2 location = glm::vec2(0,0), glm::vec2 scale = glm::vec2(1,1), uint16_t rotation = 0, glm::vec2 origin = glm::vec2(0.5,0.5));
        collisionBox(std::string channel, Bloom::Sprite* sprite);
        collisionBox(std::string channel, Bloom::Spritesheet* sprite);

        bool check_SAT(collisionBox* other,glm::mat4 transformationMatrix,glm::mat4 otherTransformationMatrix);
        bool check_SAT(collisionBox* other);

        bool check_Point(glm::vec2 point);

        glm::vec2 check_Traced(collisionBox* other, glm::mat4 transformationMatrix, glm::mat4 otherTransformationMatrix);
        glm::vec2 check_Traced(collisionBox* other);

        glm::vec2 check_TracedNormal(collisionBox* other, glm::mat4 transformationMatrix, glm::mat4 otherTransformationMatrix);
        glm::vec2 check_TracedNormal(collisionBox* other);

        glm::vec2 getLocation(){return location;};
        glm::vec2 getCenter() { return location + getScale() * (getOrigin() - glm::vec2(0.5)); }
        glm::vec2 getScale(){return scale;};
        int16_t getRotation(){return rotation;};
        glm::vec2 getOrigin(){return origin;};
        std::string getChannel() { return channel; };

        void setLocation(glm::vec2 location) { if (location != this->location) { this->location = location; updatePoints(); } };
        void setScale(glm::vec2 scale){this->scale = scale; updatePoints();};
        void setRotation(uint16_t rotation) { this->rotation = rotation; updatePoints(); };

        glm::mat4 makeTransformationMatrix();

        void copyTransform(Bloom::Sprite* sprite) { this->location = sprite->GetLocation(); this->rotation = sprite->GetRotation(); this->scale = sprite->GetSize(); }

        private:
        void updatePoints();
        int16_t rotation = 0;
        glm::vec2 location = glm::vec2(0,0);
        glm::vec2 scale = glm::vec2(1,1);
        glm::vec2 origin = glm::vec2(0,0);

        std::string channel;

        glm::vec2 thisA;
        glm::vec2 thisB;
        glm::vec2 thisC;
        glm::vec2 thisD;
    };

    glm::vec2 LineTrace(std::string channel, collisionBox* other, glm::vec2 start, glm::vec2 end);
    glm::vec2 LineTrace(std::vector<std::string> channels, collisionBox* other, glm::vec2 start, glm::vec2 end);
    glm::vec2 LineTraceNormal(std::string channel, collisionBox* other, glm::vec2 start, glm::vec2 end);
    bool AreaCheck_SAT(std::string channel, collisionBox* other,glm::vec2 position,glm::vec2 size);
    bool PointCheck_SAT(std::string chanel, collisionBox* other, glm::vec2 position);
    glm::vec2 GetNormal(glm::vec2 pointA, glm::vec2 pointB);
    glm::vec2 GetLineInterceptionPoint(glm::vec2 startA, glm::vec2 endA, glm::vec2 startB, glm::vec2 endB);
}

#endif