#include "spritesheet.h"

using namespace Bloom;

Spritesheet::Spritesheet(const char* texture_path,int pixels_per_pixel,glm::vec2 tiling, glm::mat4* view_matrix, glm::mat4 pixel_projection_matrix, glm::vec2 origin){
    this->tiling = tiling;
    this->sprite = new Sprite(texture_path,pixels_per_pixel,view_matrix,pixel_projection_matrix,origin,glm::vec2(1,1),glm::vec2(1,1) / tiling);
    //this->sprite->SetScale(glm::vec2(1) / tiling);
    SetIdx(0);
}

void Spritesheet::Draw(){
    this->sprite->Draw();
}

int Spritesheet::Advance(int step){
    this->idx = idx + step;
    if(idx >= (tiling[0]*tiling[1]))
        idx = 0;

    int x_pos = idx % (int)tiling[0];
    int y_pos = idx / tiling[0];

    this->sprite->SetUVOffset(glm::vec2(x_pos/tiling[0],(1-(1/tiling[1])) - (y_pos/tiling[1])));
    return idx;
}

int Spritesheet::TimedAdvance(float duration, int step){
    std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now()-(this->lastTime);
    double time = diff.count();
    if(time >= duration){
        this->lastTime = std::chrono::high_resolution_clock::now();
        return Advance(step);
    }
    return idx;
}

void Spritesheet::SetIdx(int idx){
    this->idx = idx;
    if(idx >= (tiling[0]*tiling[1]))
        idx = 0;

    int x_pos = idx % (int)tiling[0];
    int y_pos = idx / tiling[0];

    this->sprite->SetUVOffset(glm::vec2(x_pos/tiling[0],(1-(1/tiling[1])) - (y_pos/tiling[1])));
}