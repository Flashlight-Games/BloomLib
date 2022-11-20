#ifndef BONEKINEMATICS
#define BONEKINEMATICS

#include "Bloom/bones/bones.h"

#include <GLM/glm.hpp> // MATRICES
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>
#include <vector>
#include <string>

namespace Bloom{
    class BoneKinematics{
        public:
            void static forwardKinematics(Bloom::Bone* parent,bool propagate = false);
            void static twoBoneIK(Bloom::Bone* parent, glm::vec2 end_effector,bool inverted = false);
    };
}
#endif