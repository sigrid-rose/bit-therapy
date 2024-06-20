#ifndef ENTITY_H
#define ENTITY_H

#include <memory>
#include <string>
#include <vector>

#include "geometry.h"
#include "species.h"
#include "sprites.h"
#include "sprite_set.h"

class EntityCapability;

class Entity {    
private:
    double fps;
    Species species;
    SpriteSet spriteSet;
    std::vector<std::shared_ptr<EntityCapability>> capabilities;
    Sprite currentSprite;

    void resetSpeed(double settingsBaseSize, double settingsSpeedMultiplier);

public:
    Rect frame;
    Vector2d direction;
    double speed;

    Entity(
        double fps, 
        double settingsBaseSize, 
        double settingsSpeedMultiplier,
        Species species, 
        SpriteSet spriteSet, 
        Rect frame
    );
    
    const std::string currentSpriteFrame() const;
    const std::string speciesId() const;

    void update(long timeSinceLastUpdate);
    void changeSprite(std::string animationName);
    void setFrame(Rect newFrame);

    std::string description() const;
};

class EntityCapability {
public:
    virtual ~EntityCapability() = default;
    virtual void update(long timeSinceLastUpdate, Entity * entity) {};
};

#endif
