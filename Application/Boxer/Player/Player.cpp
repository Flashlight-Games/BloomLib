#include "Player.h"

Application::Player::Player(Game::Engine* app)
{
	Bloom::Sprite* tmp_e_box = new Bloom::Sprite("res/textures/rect16.png", 1, app->GetViewMatrix(), app->GetPixelProjectionMatrix(),glm::vec2(0.5,0.5),glm::vec2(0.1));
	box = tmp_e_box;

    position.x = app->GetPixelWindowSize().x / 2; position.y = app->GetPixelWindowSize().y / 2;
    for (int i = 0; i < (sizeof past_positions / sizeof past_positions[0]); i++)
    {
        past_positions[i] = position;
    };

    collision_box = new PhysX::collisionBox("main", box);

    trail_thread = new std::thread(&Player::Trail,this,app);
}

void Application::Player::SetPosition(Game::Engine* app) {
    glm::vec2 targetPosition = glm::vec2(app->GetCursorPosition().x, app->GetCursorPosition().y);
    if (Utils::Distance2D(glm::vec2(app->GetPixelWindowSize().x / 2, app->GetPixelWindowSize().y / 2), targetPosition) < move_radius) {
        position[0] += ((position[0] - targetPosition.x) * -speed) * app->GetGameSpeed(); position[1] += ((position[1] - targetPosition.y) * -speed) * app->GetGameSpeed();
    }
    else
    {
        float angle = (atan2(targetPosition.y- (app->GetPixelWindowSize().y / 2), targetPosition.x - (app->GetPixelWindowSize().x / 2)));
        position[0] += ((position[0] - ((move_radius - 1) *cos(angle)+app->GetPixelWindowSize().x/2)-0.5) * -speed) * app->GetGameSpeed(); position[1] += ((position[1] - ((move_radius -1) * sin(angle) + app->GetPixelWindowSize().y/2)-0.5) * -speed) * app->GetGameSpeed();
    }
    collision_box->setLocation(position);
}

void Application::Player::Trail(Game::Engine* app) {
    while (true){
        Utils::SyncSleep(app);


        this->past_positions[(sizeof this->past_positions / sizeof this->past_positions[0]) - 1] = glm::vec2((int)position[0], (int)position[1]);
        for (int i = 0; i < (sizeof this->past_positions / sizeof this->past_positions[0]) - 1; i++)
        {
            this->past_positions[i] = this->past_positions[i + 1];
        };
    }
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
