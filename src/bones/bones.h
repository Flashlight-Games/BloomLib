#ifndef BONES
#define BONES

#include <GLM/glm.hpp> // MATRICES
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <vector>
#include <string>

#define PI 3.14159265358979323846   

namespace Bloom{
    class Bone{
        public:
            Bone(Bone* parent,int16_t length);
            static std::vector<Bloom::Bone*> Bones(int amount,int length);

            glm::vec2 GetLocation(){return location;};
            int16_t GetLength(){return length;};
            int16_t GetRotation(){int16_t offset = 0; if(this->Parent != NULL){offset = this->Parent->GetRotation();} return rotation + offset;};
            glm::mat4 GetTransform(){return transform;};

            void SetLocation(glm::vec2 location){this->location = location;};
            void SetRotation(int16_t rotation){this->rotation = rotation;};
            void SetLength(int16_t length){this->length = length;};

            Bone* GetParent(){return Parent;};
            Bone* GetChild(){return Child;};

            glm::vec2 GetEndLocation();
            void SetChild(Bone* child){this->Child = child;};
            

        private:
            glm::vec2 location;
            int16_t length;
            int16_t rotation;

            glm::mat4 transform;
            Bone* Parent = NULL;
            Bone* Child = NULL;
    };
};
#endif