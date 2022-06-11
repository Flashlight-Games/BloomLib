#include "UpgradeRadius.h"

Application::Upgrade_Radius::Upgrade_Radius(Game::Engine* app, glm::vec2 position)
{
	Bloom::Sprite* tmp_e_box = new Bloom::Sprite("res/textures/rect16.png", 1, app->GetViewMatrix(), app->GetPixelProjectionMatrix(),glm::vec2(0.5,0.5),glm::vec2(0.3));
	box = tmp_e_box;

    this->position = position;
    velocity = Utils::PointAtAngleAndDistance(atan2(app->GetPixelWindowSize().y / 2 - position.y, app->GetPixelWindowSize().x / 2 - position.x), speed);
}

void Application::Upgrade_Radius::SetPosition(Game::Engine* app) {
    velocity = Utils::Lerp(velocity, Utils::PointAtAngleAndDistance(atan2(app->GetPixelWindowSize().y / 2 - position.y, app->GetPixelWindowSize().x / 2 - position.x), speed), acceleration);

    position += velocity;
    box->SetRotation(glm::degrees(atan2(position.y - app->GetPixelWindowSize().y / 2, position.x - app->GetPixelWindowSize().x / 2)));
}

void Application::Upgrade_Radius::Draw(Game::Engine* app)
{
    box->SetLocation(glm::vec2((int)position[0], (int)position[1]));

    box->GetShader().use();
    box->GetShader().uniformVec4(box->GetShader().getUniformId("uColor"), glm::vec4(upgradeColor, 1));
    box->Draw(app->GetViewMatrix());
}
