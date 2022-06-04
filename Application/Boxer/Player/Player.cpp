#include "Player.h"

Application::Player::Player(Game::Engine* app)
{
	Bloom::Sprite* tmp_e_box = new Bloom::Sprite("res/textures/rect16.png", 1, app->GetViewMatrix(), app->GetPixelProjectionMatrix(),glm::vec2(0.5,0.5),glm::vec2(0.1));
	box = tmp_e_box;

    position.x = app->GetPixelWindowSize().x / 2; position.y = app->GetPixelWindowSize().y / 2;
    for (int i = 0; i < (sizeof past_positions / sizeof past_positions[0]) - 1; i++)
    {
        past_positions[i] = position;
    };
}

void Application::Player::SetPosition(Game::Engine* app,int radius) {
    glm::vec2 targetPosition = glm::vec2(app->GetCursorPosition().x, app->GetCursorPosition().y);
    if (Utils::Distance2D(glm::vec2(app->GetPixelWindowSize().x / 2, app->GetPixelWindowSize().y / 2), targetPosition) < radius) {
        position[0] += (position[0] - targetPosition.x) * -0.05f; position[1] += (position[1] - targetPosition.y) * -0.05f;    
    }
    else
    {
        float angle = (atan2(targetPosition.y- (app->GetPixelWindowSize().y / 2), targetPosition.x - (app->GetPixelWindowSize().x / 2)));
        position[0] += (position[0] - ((radius - 1) *cos(angle)+app->GetPixelWindowSize().x/2)-0.5) * -0.05f; position[1] += (position[1] - ((radius-1) * sin(angle) + app->GetPixelWindowSize().y/2)-0.5) * -0.05f;
    }

    past_positions[(sizeof past_positions / sizeof past_positions[0]) - 1] = glm::vec2((int)position[0], (int)position[1]);
    for (int i = 0; i < (sizeof past_positions / sizeof past_positions[0]) - 1; i++)
    {
        past_positions[i] = past_positions[i + 1];
    };
}

void Application::Player::Draw(Game::Engine* app)
{
    for (int i = 0; i < (sizeof past_positions / sizeof past_positions[0]) - 1; i++)
    {
        float dist = Utils::Distance2D(app->GetScale() * past_positions[i], app->GetScale() * past_positions[i + 1]);

        for (int p = 0; p < dist; p++)
        {
            box->SetLocation(Utils::Lerp(past_positions[i], past_positions[i + 1], p / dist));

            box->GetShader().use();
            box->GetShader().uniformVec4(box->GetShader().getUniformId("uColor"), glm::vec4(Utils::Lerp(trailColor, playerColor, i / (float)(sizeof past_positions / sizeof past_positions[0])), i / (float)(sizeof past_positions / sizeof past_positions[0])));
            box->Draw(app->GetViewMatrix());
        }

        box->SetLocation(past_positions[i]);

        box->GetShader().use();
        box->GetShader().uniformVec4(box->GetShader().getUniformId("uColor"), glm::vec4(Utils::Lerp(trailColor, playerColor, i / (float)(sizeof past_positions / sizeof past_positions[0])), i / (float)(sizeof past_positions / sizeof past_positions[0])));
        box->Draw(app->GetViewMatrix());
    };

    box->SetLocation(glm::vec2((int)position[0], (int)position[1]));

    box->GetShader().use();
    box->GetShader().uniformVec4(box->GetShader().getUniformId("uColor"), glm::vec4(playerColor, 1));
    box->Draw(app->GetViewMatrix());
}
