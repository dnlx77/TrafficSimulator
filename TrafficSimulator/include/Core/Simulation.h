#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../Entities/Car.h"
#include "../World/Road.h"
#include "ResourceManager.h"

class Simulation {
public:
    Simulation();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    sf::RenderWindow m_window;
    ResourceManager m_resourceManager;

    std::vector<World::Road> m_roads; // Lista delle strade
    std::vector<Car> m_cars; // Vettore per gestire n auto
};