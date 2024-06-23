#ifndef GAME_H
#define GAME_H

#include <chrono>
#include <format>
#include <mutex>
#include <string>
#include <vector>

#include "entity.h"
#include "geometry.h"

#include "../sprites/sprites.h"
#include "../species/species.h"

struct RenderedItem {
    std::string spritePath;
    Rect frame;

    RenderedItem(std::string spritePath, Rect frame);
};

class Game {    
private:
    std::mutex mtx;
    
    const SpritesRepository* spritesRepo;
    const SpeciesRepository* speciesRepo;
    const double animationFps;
    const double baseEntitySize;

    std::vector<Entity> entities;

public:
    const double gameFps;
    const std::string screenName;
    
    Game(
        const SpritesRepository* spritesRepo,
        const SpeciesRepository* speciesRepo,
        std::string screenName,
        double gameFps, 
        double animationFps, 
        double baseEntitySize
    );
    
    void update(std::chrono::milliseconds timeSinceLastUpdate);    
    std::optional<Entity *> addEntity(const std::string& species);
    std::vector<Entity *> addEntities(const std::vector<std::string>& species);
    Entity * add(Entity entity);    
    const uint32_t numberOfEntities();
    std::vector<RenderedItem> render();
    std::string description();
};

#endif
