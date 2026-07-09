#include <GLM/glm.hpp> // MATRICES
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

export module Bloom.Bones;

import std;

#define PI 3.14159265358979323846   

export namespace Bloom {
    struct Transform2D {
        float c, s;
        float tx, ty;

        // Construct from position + angle
        static Transform2D fromPosAngle(glm::vec2 pos, float rotation_deg) noexcept {
            const float r = glm::radians(rotation_deg);
            return { std::cos(r), std::sin(r), pos.x, pos.y };
        }
    };

    namespace Bones {
        // Concatenate: apply `child` in the local space of `parent`
        // Equivalent to parent * child, but only 4 muls + 4 adds (vs 16+ for mat3)
        [[nodiscard]] inline Transform2D concat(const Transform2D& p, const Transform2D& ch) noexcept;

        // Translate along local Y by `length` (the tip of the bone)
        // Equivalent to concat(t, {1,0, 0,length}) but with the identity terms eliminated
        [[nodiscard]] inline Transform2D translateLocalY(const Transform2D& t, float length) noexcept;

        [[nodiscard]] inline glm::vec2 transformPosition(const Transform2D& t) noexcept;

        [[nodiscard]] inline float transformRotationDeg(const Transform2D& t) noexcept {
            return glm::degrees(std::atan2(t.s, t.c));
        }
    }

    class Bone {
    public:
        Bone(Bone* parent, float length);
        static std::vector<Bloom::Bone*> Bones(int amount, int length);

        glm::vec2 GetLocation() { return location; };
        int16_t GetLength() { return length; };
        int16_t GetRotation() { int16_t offset = 0; if (this->Parent != nullptr) { offset = this->Parent->GetRotation(); } return rotation + offset; };
        glm::mat4 GetTransform() { return transform; };

        void SetLocation(glm::vec2 location) { this->location = location; };
        void SetRotation(int16_t rotation) { this->rotation = rotation; };
        void SetLength(int16_t length) { this->length = length; };
        void SetInverted(bool inverted);
        void SetParent(Bone* parent);

        Bone* GetParent() { return Parent; };
        Bone* GetChild() { return Child; };

        glm::vec2 GetEndLocation();
        glm::vec2 GetEndLocation(float length);
        void SetChild(Bone* child) { this->Child = child; };


    private:
        glm::vec2 location;
        float length;
        float rotation;

        glm::mat4 transform;
        Bone* Parent = NULL;
        Bone* Child = NULL;
    };
};

Bloom::Bone::Bone(Bone* parent, float length) {
    this->Parent = parent; this->length = length;
    if (parent != NULL)
        parent->SetChild(this);
}
void Bloom::Bone::SetInverted(bool inverted){
    SetLength(glm::abs(GetLength()) * (inverted ? -1 : 1));
}

void Bloom::Bone::SetParent(Bone* parent)
{
    this->Parent = parent;
    parent->SetChild(this);
}
glm::vec2 Bloom::Bone::GetEndLocation() {
    if (this->length == 0)
        return this->location;
    int offset = 0;
    if (this->Parent != NULL)
        offset = Parent->GetRotation();
    return glm::vec2(location[0] + length * cos((offset + rotation - 90.0) * (PI / 180.0)), location[1] + length * sin((offset + rotation - 90.0) * (PI / 180.0)));
}
glm::vec2 Bloom::Bone::GetEndLocation(float length) {
    if (length == 0)
        return this->location;
    int offset = 0;
    if (this->Parent != NULL)
        offset = Parent->GetRotation();
    return glm::vec2(location[0] + length * cos((offset + rotation - 90.0) * (PI / 180.0)), location[1] + length * sin((offset + rotation - 90.0) * (PI / 180.0)));
}
std::vector<Bloom::Bone*> Bloom::Bone::Bones(int amount, int length) {
    std::vector<Bloom::Bone*> bones;
    Bloom::Bone* lBone = NULL;
    Bloom::Bone* tmp_Bone = NULL;
    for (int i = 0; i < amount; i++) {
        tmp_Bone = new Bloom::Bone(lBone, length);
        bones.push_back(tmp_Bone);
        lBone = tmp_Bone;
    }

    delete tmp_Bone;

    return bones;
}

Bloom::Transform2D Bloom::Bones::concat(const Bloom::Transform2D& p, const Bloom::Transform2D& ch) noexcept {
    return {
        .c = p.c * ch.c - p.s * ch.s,
        .s = p.s * ch.c + p.c * ch.s,
        .tx = p.c * ch.tx - p.s * ch.ty + p.tx,
        .ty = p.s * ch.tx + p.c * ch.ty + p.ty,
    };
}
Bloom::Transform2D Bloom::Bones::translateLocalY(const Bloom::Transform2D& t, float length) noexcept {
    return {
        .c = t.c,
        .s = t.s,
        .tx = t.tx - t.s * length,
        .ty = t.ty + t.c * length,
    };
}
glm::vec2 Bloom::Bones::transformPosition(const Bloom::Transform2D& t) noexcept {
    return { t.tx, t.ty };
}