#include "Enemy.h"

Application::Enemy::Enemy(Game::Engine* app, glm::vec2 position)
{
	Bloom::Sprite* tmp_e_box = new Bloom::Sprite("res/textures/rect16.png", 1, app->GetViewMatrix(), app->GetPixelProjectionMatrix(),glm::vec2(0.5,0.5),glm::vec2(0.3));
	box = tmp_e_box;

    this->position = position;
    velocity = Utils::PointAtAngleAndDistance(atan2(app->GetPixelWindowSize().y / 2 - position.y, app->GetPixelWindowSize().x / 2 - position.x), speed);
    velocity += glm::vec2(rand() % 3);
}

void Application::Enemy::SetPosition(Game::Engine* app) {
    glm::vec2 target_velocity = Utils::PointAtAngleAndDistance(atan2(app->GetPixelWindowSize().y / 2-position.y, app->GetPixelWindowSize().x / 2-position.x),speed);
    velocity.x += (target_velocity.x - velocity.x) * acceleration; velocity.y += (target_velocity.y - velocity.y) * acceleration;

    position += velocity;
}

void Application::Enemy::Draw(Game::Engine* app)
{
    box->SetLocation(glm::vec2((int)position[0], (int)position[1]));

    box->GetShader().use();
    box->GetShader().uniformVec4(box->GetShader().getUniformId("uColor"), glm::vec4(enemyColor, 1));
    box->Draw(app->GetViewMatrix());
}
