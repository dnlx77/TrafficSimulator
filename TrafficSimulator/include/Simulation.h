#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Entities/Car.h"

class Simulation {
public:
    Simulation();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    sf::RenderWindow m_window;
    std::vector<Car> m_cars; // Vettore per gestire n auto
};