#include "sprite_subdivided.h"

using namespace Bloom;

SubdividedSprite::SubdividedSprite(const char* texture_path, int pixels_per_pixel, glm::mat4* view_matrix, glm::mat4 pixel_projection_matrix, glm::vec2 subdivision, glm::vec2 origin) {
    this->ppp = pixels_per_pixel; this->view_matrix = view_matrix; this->Subdivision = subdivision;
    this->UVoffset = glm::vec2(0, 0); this->UVscale = glm::vec2(1, 1); this->Origin = origin;

    if(subdivision.x <= 0 || subdivision.y <= 0)
        throw std::invalid_argument("subdivision must be positive (above 0).");

    transformation[3] = 1.0; transformation[4] = 1.0;

    glGenVertexArrays(1, &vao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
    glBindVertexArray(vao);

    this->texture = new Bloom::Texture2D(texture_path, false);

    float xScale = (this->texture->GetSize()[0] * ppp); // Sprite width 
    float yScale = (this->texture->GetSize()[1] * ppp); // Sprite width 

    origin[0] = 1 - origin[0];
    origin[1] = 1 - origin[1];

    std::vector<float> dynamic_vertices; glm::vec3 vertex_color;
    for (int idx_y = 0; idx_y <= subdivision.y; idx_y++)
    {
        for (int idx_x = 0; idx_x <= subdivision.x; idx_x++)
        {
            vertex_color = glm::vec3(1);
            dynamic_vertices.push_back((0.5) * (Subdivision.x - (idx_x / Subdivision.x)) * xScale - (0.5 * Subdivision.x * xScale) + ((Origin[0] / 0.5) * 0.25) * xScale); dynamic_vertices.push_back((0.5) * (Subdivision.y - (idx_y / Subdivision.y)) * yScale - (0.5 * Subdivision.y * yScale) + ((Origin[1] / 0.5) * 0.25) * yScale);
            dynamic_vertices.push_back(vertex_color.r); dynamic_vertices.push_back(vertex_color.g); dynamic_vertices.push_back(vertex_color.b);
            dynamic_vertices.push_back(1 - (this->UVscale[0]) * (idx_x / Subdivision.x)); dynamic_vertices.push_back(1 - (this->UVscale[1]) * (idx_y / Subdivision.y));
        }
    }

    glGenBuffers(1, &vbo); // Set vbo id
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind the vbo as an array buffer
    glBufferData(GL_ARRAY_BUFFER, dynamic_vertices.size() * sizeof(float), dynamic_vertices.data(), GL_DYNAMIC_DRAW);

    indicesAmount = subdivision.x * subdivision.y * 6;

    std::vector<int> dynamic_indices; uint16_t BL; uint16_t BR; uint16_t TR; uint16_t TL;
    for (int idx_y = 0; idx_y < subdivision.y; idx_y++)
    {
        for (int idx_x = 0; idx_x < subdivision.x; idx_x++)
        {
            BL = (idx_x + 0 + ((idx_y + 0) * (subdivision.x + 1)));   // BL
            BR = (idx_x + 1 + ((idx_y + 0) * (subdivision.x + 1)));   // BR
            TR = (idx_x + 1 + ((idx_y + 1) * (subdivision.x + 1)));   // TR
            TL = (idx_x + 0 + ((idx_y + 1) * (subdivision.x + 1)));   // TL

            dynamic_indices.push_back(BL); dynamic_indices.push_back(BR); dynamic_indices.push_back(TR);
            dynamic_indices.push_back(BL); dynamic_indices.push_back(TR); dynamic_indices.push_back(TL);
        }
    }

    glGenBuffers(1, &ebo); // Set ebo id
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // bind the ebo as an element array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dynamic_indices.size() * sizeof(int), dynamic_indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), ((void*)(0 * sizeof(float)))); // Position

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), ((void*)(2 * sizeof(float)))); // Color

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), ((void*)(5 * sizeof(float)))); // Texture


    SetShader(new Bloom::Shader("include/Bloom/res/engine/shaders/sprite/spriteshader_vertex.glsl", "include/Bloom/res/engine/shaders/sprite/spriteshader_fragment.glsl"), pixel_projection_matrix);
    original_shader = true;

    model_matrix = glm::mat4(1.0);
    model_matrix = glm::translate(model_matrix, glm::vec3(this->transformation[0], this->transformation[1], 0.0f));
    model_matrix = glm::rotate(model_matrix, (float)glm::radians(this->transformation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(this->transformation[3], this->transformation[4], 1.0f));
}

Bloom::SubdividedSprite::~SubdividedSprite()
{
    if (original_shader)
        delete shader;
    delete texture;
}


void Bloom::SubdividedSprite::SetVertexPositionOffset(int idx, glm::vec2 positionOffset)
{
    float xScale = (this->texture->GetSize()[0] * ppp); // Sprite width 
    float yScale = (this->texture->GetSize()[1] * ppp); // Sprite width 

    idx = (Subdivision.y + 1) * (Subdivision.x + 1) - (idx + 1);

    glm::vec3 vertex_color = glm::vec3(1); std::vector<float> dynamic_vertices; int idx_x = idx % ((int)Subdivision.x+1); int idx_y = idx / ((int)Subdivision.y + 1);
    dynamic_vertices.push_back((0.5) * (Subdivision.x - (idx_x / Subdivision.x)) * xScale - (0.5 * Subdivision.x * xScale) + ((Origin[0] / 0.5) * 0.25) * xScale + positionOffset.x); dynamic_vertices.push_back((0.5) * (Subdivision.y - (idx_y / Subdivision.y)) * yScale - (0.5 * Subdivision.y * yScale) + ((Origin[1] / 0.5) * 0.25) * yScale + positionOffset.y);
    dynamic_vertices.push_back(vertex_color.r); dynamic_vertices.push_back(vertex_color.g); dynamic_vertices.push_back(vertex_color.b);
    dynamic_vertices.push_back(1 - (this->UVscale[0]) * (idx_x / Subdivision.x)); dynamic_vertices.push_back(1 - (this->UVscale[1]) * (idx_y / Subdivision.y));
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, (idx) * 7 * sizeof(float), 7 * sizeof(float), dynamic_vertices.data());
}

void Bloom::SubdividedSprite::SetVertexPositionOffsets(std::vector<glm::vec2> positionOffsets, int start_idx)
{
    float xScale = (this->texture->GetSize()[0] * ppp); // Sprite width 
    float yScale = (this->texture->GetSize()[1] * ppp); // Sprite width 

    glm::vec3 vertex_color = glm::vec3(1); std::vector<float> dynamic_vertices; int idx_x; int idx_y;
    for (int idx = 0; idx < positionOffsets.size(); idx++)
    {
        idx_x = idx % ((int)Subdivision.x + 1); idx_y = idx / ((int)Subdivision.y + 1);
        dynamic_vertices.push_back((0.5) * (Subdivision.x - (idx_x / Subdivision.x)) * xScale - (0.5 * Subdivision.x * xScale) + ((Origin[0] / 0.5) * 0.25) * xScale + positionOffsets.at(idx).x); dynamic_vertices.push_back((0.5) * (Subdivision.y - (idx_y / Subdivision.y)) * yScale - (0.5 * Subdivision.y * yScale) + ((Origin[1] / 0.5) * 0.25) * yScale + positionOffsets.at(idx).y);
        dynamic_vertices.push_back(vertex_color.r); dynamic_vertices.push_back(vertex_color.g); dynamic_vertices.push_back(vertex_color.b);
        dynamic_vertices.push_back(1 - (this->UVscale[0]) * (idx_x / Subdivision.x)); dynamic_vertices.push_back(1 - (this->UVscale[1]) * (idx_y / Subdivision.y));
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, (start_idx) * 7 * sizeof(float), positionOffsets.size() * sizeof(float) * 7, dynamic_vertices.data());
}

void SubdividedSprite::UpdateLocation() {
    model_matrix = glm::mat4(1.0);
    model_matrix = glm::translate(model_matrix, glm::vec3(this->transformation[0], this->transformation[1], 0.0f));
    model_matrix = glm::rotate(model_matrix, (float)glm::radians(this->transformation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(this->transformation[3], this->transformation[4], 1.0f));
}

void SubdividedSprite::Draw() {
    shader->use();
    shader->uniformFloat(uColorUni, 1);
    setTextureAtLocation(GL_TEXTURE0, this->texture->GetID()); shader->uniformInt(uTextureUni, 0);
    shader->uniformMat4(shader->getUniformId("model"), model_matrix);
    shader->uniformMat4(shader->getUniformId("view"), (*(view_matrix)));

    UpdateLocation();

    this->texture->bind();
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, indicesAmount, GL_UNSIGNED_INT, 0);
}

void Bloom::SubdividedSprite::DrawInstanced(int amount)
{
    shader->use();
    shader->uniformFloat(uColorUni, 1);
    setTextureAtLocation(GL_TEXTURE0, this->texture->GetID()); shader->uniformInt(uTextureUni, 0);
    shader->uniformMat4(shader->getUniformId("view"), *(view_matrix));

    UpdateLocation();

    this->texture->bind();
    glBindVertexArray(this->vao);
    glDrawElementsInstanced(GL_TRIANGLES, indicesAmount, GL_UNSIGNED_INT, 0, amount);
}