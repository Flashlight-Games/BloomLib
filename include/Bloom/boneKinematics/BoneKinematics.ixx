#include <GLM/glm.hpp> // MATRICES
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

export module Bloom.BoneKinematics;

#define PI std::numbers::pi_v<float>

import std;
import Bloom.Bones;

export namespace Bloom {
    struct TwoBoneIK {
        Transform2D parent;
        Transform2D child;
    };

    namespace BoneKinematics {
        [[nodiscard]] TwoBoneIK twoBoneIK(glm::vec2 root, glm::vec2 target, float parent_length, float child_length, bool inverted = false) noexcept;
    }
}

Bloom::TwoBoneIK Bloom::BoneKinematics::twoBoneIK(glm::vec2 root, glm::vec2 target, float parent_length, float child_length, bool inverted) noexcept {
    constexpr float RAD_TO_DEG = 180.0f / PI;

    const glm::vec2 delta = target - root;
    const float max_reach = parent_length + child_length;

    float reach_sq = glm::dot(delta, delta);
    const bool clamped = reach_sq >= max_reach * max_reach - 1e-1f;
    float reach = clamped ? max_reach : std::sqrt(reach_sq);
    if (clamped) reach_sq = max_reach * max_reach;

    float c_angle = std::atan2(delta.y, delta.x) * RAD_TO_DEG + 90.0f;

    const float parent_sq = parent_length * parent_length;
    const float child_sq = child_length * child_length;
    const float sign = inverted ? -1.0f : 1.0f;

    const float cos_a = std::clamp((parent_sq + reach_sq - child_sq) / (2.0f * parent_length * reach), -1.0f, 1.0f);
    const float cos_b = std::clamp((parent_sq + child_sq - reach_sq) / (2.0f * parent_length * child_length), -1.0f, 1.0f);

    const float parent_angle = c_angle - std::acos(cos_a) * RAD_TO_DEG * sign;
    // Fix: child bends relative to parent, no need to negate the base 180
    const float child_angle = (180.0f - std::acos(cos_b) * RAD_TO_DEG) * sign;

    const Transform2D parent_t = Transform2D::fromPosAngle(root, parent_angle);
    const Transform2D tip_offset = { 1.0f, 0.0f, 0.0f, -parent_length }; // (s, -c) direction = ty = -length
    const Transform2D child_t = Bones::concat(parent_t, tip_offset);
    const Transform2D child_rot = Bones::concat(child_t, Transform2D::fromPosAngle({ 0, 0 }, child_angle));

    return { .parent = parent_t, .child = child_rot };
}