#ifndef GAME_H
#define GAME_H

#include <format>
#include <mutex>
#include <vector>

#include "entity.h"

class Game {    
private:
    std::mutex mtx;
    double fps;
    std::vector<Entity> entities;

public:
    Game(double fps);
    
    void update(long timeSinceLastUpdate);
    
    Entity * add(Entity entity);
    
    const int numberOfEntities();

    std::string description() const;
};

#endif
