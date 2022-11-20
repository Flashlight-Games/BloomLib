#include "bones.h"

Bloom::Bone::Bone(Bone* parent,int16_t length){
    this->Parent = parent; this->length = length;
    if(parent != NULL)
        parent->SetChild(this);
}

Bloom::Bone::~Bone()
{
    delete Parent;
    delete Child;
}

glm::vec2 Bloom::Bone::GetEndLocation(){
    int offset = 0;
    if(this->Parent != NULL)
        offset = Parent->GetRotation();
    return glm::vec2(location[0] + length*cos((offset + rotation-90)*(PI/180)),location[1] + length*sin((offset + rotation-90)*(PI/180)));
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