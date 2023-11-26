#include "KinematicBody.h"

PhysX::KinematicBody::KinematicBody(std::string channel, float mass, Bloom::Sprite* source, float pixels_to_meters, glm::vec2 gravity, float bounce, float drag)
{
    this->pixels_to_meters = pixels_to_meters;
	this->collision = new PhysX::collisionBox(channel, source);
    collision->setLocation(location);
    collision->setRotation(0);
    this->mass = mass;
    this->gravity = gravity;
    this->bounciness = bounce;
    this->drag = drag;
}

PhysX::KinematicBody::KinematicBody(float mass, float pixels_to_meters, glm::vec2 gravity, float bounce, float drag)
{
    this->pixels_to_meters = pixels_to_meters;
    this->collision = new PhysX::collisionBox("");
    collision->setLocation(location);
    collision->setRotation(0);
    this->mass = mass;
    this->gravity = gravity;
    this->bounciness = bounce;
    this->drag = drag;
}

PhysX::KinematicBody::~KinematicBody()
{
    delete collision;
}

glm::vec2 PhysX::KinematicBody::GetLocation(float DeltaTime_ms, std::vector<PhysX::collisionBox*> other_boxes)
{
    hit = false;
    float DeltaTime = DeltaTime_ms / 1000.0f;

    // PHYSICS START
    for (int i = 0; i < applied_forces.size(); i++)
    {
        net_force += applied_forces.at(i);
    }
    applied_forces.clear();

    auto force_gravity = gravity * mass * 4.0f;
    net_force += force_gravity;
    auto force_drag = -velocity * drag;
    net_force += force_drag;

    acceleration = (net_force* DeltaTime) / glm::vec2(mass); // ACCELERATION
    velocity += acceleration;// VELOCITY
    
    
    if (other_boxes.size() > 0) {
        for (int i = 0; i < other_boxes.size(); i++)
        {
            auto other = other_boxes.at(i);
            glm::vec2 depth = GetDepth(other_boxes);
            
            if (collision->check_SAT(other)) {
                auto hit_normal = PhysX::LineTraceNormal(collision->getChannel(), other, collision->getLocation(), other->getLocation());
                if (glm::length(depth) > 0 && glm::length(depth) < glm::length(collision->getScale()) && glm::dot(glm::normalize(depth), glm::normalize(velocity)) < 0.0) {
                    location += depth;
                }
                if (glm::dot(glm::normalize(hit_normal), glm::normalize(velocity)) < 0.0) {
                    velocity += (glm::abs(velocity) * hit_normal) * glm::vec2(1.00 + bounciness);
                }
                hit = true;
            }
        }
    }
    
    auto updated_velocity = velocity * DeltaTime; // UPDATE VELOCITY
    location += updated_velocity; //UPDATE LOCATION

    // PHYSICS END
    collision->setLocation(location);
    net_force = glm::vec2(0);
	return (location* pixels_to_meters);
}

glm::vec2 PhysX::KinematicBody::GetLocationSmooth(float DeltaTime_s, std::vector<PhysX::collisionBox*> other_boxes)
{
    hit = false;
    float DeltaTime = DeltaTime_s / 1000;

    // PHYSICS START
    for (int i = 0; i < applied_forces.size(); i++)
    {
        net_force += applied_forces.at(i);
    }
    applied_forces.clear();

    auto force_gravity = gravity * mass * 4.0f;
    net_force += force_gravity;
    auto force_drag = -velocity * drag;
    net_force += force_drag;

    acceleration = (net_force * DeltaTime) / glm::vec2(mass); // ACCELERATION
    velocity += acceleration;// VELOCITY


    if (other_boxes.size() > 0) {
        for (int i = 0; i < other_boxes.size(); i++)
        {
            auto other = other_boxes.at(i);
            glm::vec2 depth = GetDepth(other_boxes);

            if (collision->check_SAT(other)) {
                auto hit_normal = PhysX::LineTraceNormal(collision->getChannel(), other, collision->getLocation(), other->getLocation());
                if (glm::length(depth) > 0 && glm::length(depth) < glm::length(collision->getScale()) && glm::dot(glm::normalize(depth), glm::normalize(velocity)) < 0.0) {
                    location += depth;
                }
                if (glm::dot(glm::normalize(hit_normal), glm::normalize(velocity)) < 0.0) {
                    velocity += (glm::abs(velocity) * hit_normal) * glm::vec2(1.00 + bounciness);
                }
                hit = true;
            }
        }
    }

    auto updated_velocity = velocity * DeltaTime * pixels_to_meters; // UPDATE VELOCITY
    location += updated_velocity; //UPDATE LOCATION

    glm::vec2 updated_location = location;

    if (glm::length(updated_velocity) > 0) {
        if (abs(updated_velocity.x) > abs(updated_velocity.y)) {
            updated_location.x = round(location.x);
            updated_location.y = round(location.y + (updated_location.x - location.x) * updated_velocity.y / updated_velocity.x);
        }
        else {
            updated_location.y = round(location.y);
            updated_location.x = round(location.x + (updated_location.y - location.y) * updated_velocity.x / updated_velocity.y);
        }
    }

    // PHYSICS END
    collision->setLocation(location);
    net_force = glm::vec2(0);
    return (updated_location);
}

glm::vec2 PhysX::KinematicBody::GetDepth(std::vector<PhysX::collisionBox*> other_boxes)
{
    if (other_boxes.size() > 0) {
        for (int i = 0; i < other_boxes.size(); i++)
        {
            auto other = other_boxes.at(i);

            if (collision->check_SAT(other)) {
                if (PhysX::LineTrace(collision->getChannel(), other, collision->getCenter(), other->getCenter()) != glm::vec2(INFINITY)) {
                    auto result = PhysX::LineTrace(collision->getChannel(), other, collision->getCenter(), other->getCenter()) - PhysX::LineTrace(collision->getChannel(), collision, other->getCenter(), collision->getCenter());
                    return result;
                }
            }
        }
    }
    return glm::vec2(0);
}
