#ifndef COLLISION
#define COLLISION

#define PI 3.14159265358979323846   

#include "../dependencies.h"

namespace PhysX
{
    class collisionBox{
        public:
        collisionBox(){};
        collisionBox(std::string channel,glm::vec2 location = glm::vec2(0,0), glm::vec2 scale = glm::vec2(1,1), uint16_t rotation = 0);

        bool check(collisionBox other);

        glm::vec2 getLocation(){return location;};
        glm::vec2 getScale(){return scale;};
        uint16_t getRotation(){return rotation;};

        void setLocation(glm::vec2 location){this->location = location;};
        void setScale(glm::vec2 scale){this->scale = scale;};
        void setRotation(uint16_t rotation){this->rotation = rotation;};

        private:
        uint16_t rotation = 0;
        glm::vec2 location = glm::vec2(0,0);
        glm::vec2 scale = glm::vec2(1,1);

        std::string channel;
    };
}

#endif