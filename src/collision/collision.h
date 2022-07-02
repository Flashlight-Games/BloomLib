#ifndef COLLISION
#define COLLISION

#define PI 3.14159265358979323846   

#include "dependencies.h"
#include "sprite/sprite.h"

namespace PhysX
{
    class collisionBox{
        public:
        collisionBox(){};
        collisionBox(std::string channel,glm::vec2 location = glm::vec2(0,0), glm::vec2 scale = glm::vec2(1,1), uint16_t rotation = 0, glm::vec2 origin = glm::vec2(0.5,0.5));
        collisionBox(std::string channel, Bloom::Sprite* sprite);

        bool check(collisionBox other,glm::mat4 transformationMatrix,glm::mat4 otherTransformationMatrix);
        bool check(collisionBox other);

        glm::vec2 getLocation(){return location;};
        glm::vec2 getScale(){return scale;};
        int16_t getRotation(){return rotation;};
        glm::vec2 getOrigin(){return origin;};

        void setLocation(glm::vec2 location){this->location = location;};
        void setScale(glm::vec2 scale){this->scale = scale;};
        void setRotation(uint16_t rotation){this->rotation = rotation;};

        glm::mat4 makeTransformationMatrix();

        private:
        int16_t rotation = 0;
        glm::vec2 location = glm::vec2(0,0);
        glm::vec2 scale = glm::vec2(1,1);
        glm::vec2 origin = glm::vec2(0,0);

        std::string channel;
    };
}

#endif