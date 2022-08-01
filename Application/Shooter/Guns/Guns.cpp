#include "Guns.h"

float Application::Gun::Fire(Cave* cave, Player* other)
{
    float damage = 0;
    auto tiles = cave->GetTiles();
    for (int i = 0; i < bullets_per_shot; i++)
    {
        float bullet_angle = angle + Utils::RandFloat(-spread / 2.0, spread / 2.0);
        glm::vec2 current_bullet_position = location;
        int energy = penetration;
        while (energy >= 0 && Utils::Distance2D(location, current_bullet_position) <= range) {
            int block_x = std::round(current_bullet_position.x / 16.0);
            int block_y = std::round(current_bullet_position.y / 16.0);

            block_x = Utils::clamp(block_x, 1, tiles.size() - 2);
            block_y = Utils::clamp(block_y, 1, tiles.at(block_x).size() - 2);

            if (tiles.at(block_x).at(block_y) == cave_tile::block) {
                cave->SetBlock(glm::vec2(block_x, block_y), cave_tile::empty);
                tiles.at(block_x).at(block_y) = cave_tile::empty;
                energy -= 1;
            }

            current_bullet_position = current_bullet_position + Utils::PointAtAngleAndDistance(glm::radians(bullet_angle), tolerance);

            if (Utils::Distance2D(other->GetLocation(), current_bullet_position) <= tolerance) {
                damage = other->Damage(damage); energy = -1;
            }
        }
    }

    return damage;
}
