#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory> // Aggiunto per unique_ptr
#include "ResourceManager.h"
#include "../Entities/Car.h"
#include "../World/Road.h"
#include "../World/Node.h"
#include "../World/TrafficLight.h"

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

    // Usiamo i puntatori unici per gestire la memoria del Grafo
    std::vector<std::unique_ptr<World::Node>> m_nodes;
    std::vector<std::unique_ptr<World::Road>> m_roads;
    std::vector<World::TrafficLight> m_trafficLights;

    std::vector<Car> m_cars;

    float m_spawnTimer = 0.f;
    float m_spawnInterval = 2.0f; // Nasce un'auto ogni 2 secondi
};