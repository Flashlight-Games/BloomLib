#include "boneKinematics.h"

void Bloom::BoneKinematics::forwardKinematics(Bloom::Bone* parent,bool propagate){
    if(parent->GetChild() != NULL){
        parent->GetChild()->SetLocation(parent->GetEndLocation());
        if(propagate)
            forwardKinematics(parent->GetChild(),propagate);
    }
}

void Bloom::BoneKinematics::twoBoneIK(Bloom::Bone* parent, glm::vec2 end_effector,bool inverted){
    float c_angle =  -atan((parent->GetLocation().x - end_effector.x)/(parent->GetLocation().y - end_effector.y)) * 180 / PI;
    float c_distance = sqrt(pow(parent->GetLocation().x - end_effector.x,2) + pow(parent->GetLocation().y - end_effector.y,2));

    c_distance = std::min((float)(parent->GetLength() + parent->GetChild()->GetLength()),c_distance);

    if((parent->GetLocation().y - end_effector.y) < 0){
        c_angle += 180;
        if((parent->GetLocation().x - end_effector.x) == 0){
            c_angle = 179;
        }
    }

    float a = acos((pow(parent->GetLength(),2)+pow(c_distance,2)-pow(parent->GetChild()->GetLength(),2))/(2*parent->GetLength()*c_distance)) * 180 / PI * (inverted ? -1 : 1);
    float b = 180 - (acos((pow(parent->GetLength(),2)+pow(parent->GetChild()->GetLength(),2)-pow(c_distance,2))/(2*parent->GetLength()*parent->GetChild()->GetLength())) * 180 / PI * (inverted ? -1 : 1));

    parent->SetRotation(c_angle+a);
    parent->GetChild()->SetLocation(parent->GetEndLocation());
    parent->GetChild()->SetRotation(-b);
    }

glm::vec2 Bloom::BoneKinematics::twoBoneIK_Angles(Bloom::Bone* parent, glm::vec2 end_effector, bool inverted)
{
    float c_angle = -atan((parent->GetLocation().x - end_effector.x) / (parent->GetLocation().y - end_effector.y)) * 180 / PI;
    float c_distance = sqrt(pow(parent->GetLocation().x - end_effector.x, 2) + pow(parent->GetLocation().y - end_effector.y, 2));

    c_distance = std::min((float)(parent->GetLength() + parent->GetChild()->GetLength()), c_distance);

    if ((parent->GetLocation().y - end_effector.y) < 0) {
        c_angle += 180;
        if ((parent->GetLocation().x - end_effector.x) == 0) {
            c_angle = 179;
        }
    }

    float a = acos((pow(parent->GetLength(), 2) + pow(c_distance, 2) - pow(parent->GetChild()->GetLength(), 2)) / (2 * parent->GetLength() * c_distance)) * 180 / PI * (inverted ? -1 : 1);
    float b = 180 - (acos((pow(parent->GetLength(), 2) + pow(parent->GetChild()->GetLength(), 2) - pow(c_distance, 2)) / (2 * parent->GetLength() * parent->GetChild()->GetLength())) * 180 / PI * (inverted ? -1 : 1));

    return glm::vec2(c_angle + a, -b);
}
