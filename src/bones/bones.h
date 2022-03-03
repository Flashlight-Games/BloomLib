#ifndef BONES
#define BONES

#include "../dependencies.h"

namespace Bloom{
    class Bone{
        public:
            Bone(Bone* parent,int16_t length);

            glm::vec2 GetLocation(){return location;};
            int16_t GetLength(){return length;};
            int16_t GetRotation(){int16_t offset = 0; if(this->Parent != NULL){offset = this->Parent->GetRotation();} return rotation + offset;};
            glm::mat4 GetTransform(){return transform;};

            void SetLocation(glm::vec2 location){this->location = location; updateTransform();};
            void SetRotation(int16_t rotation){this->rotation = rotation; updateTransform();};
            void SetLength(int16_t length){this->length = length; updateTransform();};

            Bone* GetParent(){return Parent;};
            Bone* GetChild(){return Child;};

            glm::vec2 GetEndLocation();
            void SetChild(Bone* child){this->Child = child;};

        private:
            void updateTransform();
            glm::vec2 location;
            int16_t length;
            int16_t rotation;

            glm::mat4 transform;
            Bone* Parent = NULL;
            Bone* Child = NULL;
    };
};
#endif