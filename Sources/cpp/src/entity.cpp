#include "entity.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "geometry.h"
#include "file_utils.h"
#include "linear_movement.h"
#include "sprites.h"
#include "sprite_set.h"

Entity::Entity(
    double fps, 
    double settingsBaseSize, 
    double settingsSpeedMultiplier,
    Species species, 
    SpriteSet spriteSet, 
    Rect frame
) :
    fps(fps),
    speed(0.0),
    species(species), 
    spriteSet(spriteSet), 
    frame(frame),
    direction(Vector2d(1.0, 0.0)),
    capabilities(std::vector<std::shared_ptr<EntityCapability>>()),
    currentSprite(spriteSet.movementSprite(fps))
{
    resetSpeed(settingsBaseSize, settingsSpeedMultiplier);
    changeSprite(SPRITE_NAME_MOVEMENT); 
    std::shared_ptr<LinearMovement> lm = std::make_shared<LinearMovement>();
    capabilities.push_back(lm);
}

const std::string Entity::currentSpriteFrame() const {
    return currentSprite.currentFrame();
}

const std::string Entity::speciesId() const {
    return species.id;
}

void Entity::update(long timeSinceLastUpdate) {
    for (auto& capability : capabilities) {
        capability->update(timeSinceLastUpdate, this);
    }
    currentSprite.update(timeSinceLastUpdate);
}

void Entity::changeSprite(std::string animationName) {
    currentSprite = spriteSet.sprite(animationName, fps);
}

std::string Entity::description() const {
    auto spriteName = fileName(currentSpriteFrame());
    std::stringstream ss; 
    ss << species.id << " " << spriteName << " " << frame.description() << std::endl;
    return ss.str();
}

void Entity::resetSpeed(double settingsBaseSize, double settingsSpeedMultiplier) {
    double sizeMultiplier = frame.w / settingsBaseSize;
    speed = 30.0 * species.speed * sizeMultiplier * settingsSpeedMultiplier;
}