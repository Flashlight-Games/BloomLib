#include "bones.h"

Bloom::Bone::Bone(Bone* parent,float length){
    this->Parent = parent; this->length = length;
    if(parent != NULL)
        parent->SetChild(this);
}

void Bloom::Bone::SetInverted(bool inverted)
{
    SetLength(glm::abs(GetLength()) * (inverted ? -1 : 1));
}

glm::vec2 Bloom::Bone::GetEndLocation(){
    if (this->length == 0)
        return this->location;
    int offset = 0;
    if(this->Parent != NULL)
        offset = Parent->GetRotation();
    return glm::vec2(location[0] + length*cos((offset + rotation-90.0)*(PI/180.0)),location[1] + length*sin((offset + rotation-90.0)*(PI/180.0)));
}

glm::vec2 Bloom::Bone::GetEndLocation(float length) {
    if (length == 0)
        return this->location;
    int offset = 0;
    if (this->Parent != NULL)
        offset = Parent->GetRotation();
    return glm::vec2(location[0] + length * cos((offset + rotation - 90.0) * (PI / 180.0)), location[1] + length * sin((offset + rotation - 90.0) * (PI / 180.0)));
}

std::vector<Bloom::Bone*> Bloom::Bone::Bones(int amount,int length){
    std::vector<Bloom::Bone*> bones;
    Bloom::Bone* lBone = NULL;
    Bloom::Bone* tmp_Bone = NULL;
    for(int i = 0; i < amount; i++){
        tmp_Bone = new Bloom::Bone(lBone,length);
        bones.push_back(tmp_Bone);
        lBone = tmp_Bone;
    }

    delete tmp_Bone;

    return bones;
}