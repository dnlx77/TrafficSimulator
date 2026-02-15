# Traffic Simulator - Project Context

## Overview
Progetto di simulazione del traffico in C++ con visualizzazione grafica.
- **Obiettivo:** Simulare flussi di traffico, incroci, semafori e AI dei veicoli.
- **IDE:** Visual Studio 2022.
- **Linguaggio:** C++17 (o superiore).
- **Libreria Grafica:** SFML 3.0.0.
- **Gestione Dipendenze:** vcpkg (Manifest Mode).

## Struttura Directory
TrafficSimulator/
├── vcpkg.json            # Manifest file per le dipendenze
├── TrafficSimulator.sln  # Solution file
└── TrafficSimulator/     # Project folder
    ├── src/
    │   ├── main.cpp
    │   ├── Simulation.cpp
    │   └── Entities/
    │       └── Car.cpp
    ├── include/
    │   ├── Simulation.h
    │   └── Entities/
    │       └── Car.h
    └── assets/           # Texture, Font (vuota per ora)

## Configurazione Librerie (vcpkg.json)
```json
{
  "name": "traffic-simulator",
  "version-string": "0.1.0",
  "builtin-baseline": "eacbb92398414434e34199c0953a71b12b5e91ae", 
  "dependencies": [
    {
      "name": "sfml",
      "version>=": "3.0.0"
    }
  ]
}
Codice Sorgente Corrente (Snapshot SFML 3.0)
1. Main Entry Point (src/main.cpp)
C++

#include "../include/Simulation.h"

int main() {
    Simulation sim;
    sim.run();
    return 0;
}
2. Simulation Header (include/Simulation.h)
C++

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
    std::vector<Car> m_cars;
};
3. Simulation Implementation (src/Simulation.cpp)
Note: Implementa il nuovo loop eventi di SFML 3 e i costruttori espliciti.

C++

#include "../../include/Simulation.h"

Simulation::Simulation()
    : m_window(sf::VideoMode({800, 600}), "Traffic Simulator C++ (SFML 3.0)")
{
    m_window.setFramerateLimit(60);
    // Aggiunta auto di test
    m_cars.emplace_back(50.f, 300.f, sf::Color::Red);
    m_cars.emplace_back(50.f, 350.f, sf::Color::Blue);
}

void Simulation::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        processEvents();
        sf::Time dt = clock.restart();
        update(dt);
        render();
    }
}

void Simulation::processEvents() {
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
            }
        }
    }
}

void Simulation::update(sf::Time deltaTime) {
    float dtSeconds = deltaTime.asSeconds();
    for (auto& car : m_cars) {
        car.update(dtSeconds);
    }
}

void Simulation::render() {
    m_window.clear(sf::Color(50, 50, 50)); 
    for (auto& car : m_cars) {
        car.draw(m_window);
    }
    m_window.display();
}
4. Car Header (include/Entities/Car.h)
C++

#pragma once
#include <SFML/Graphics.hpp>

class Car {
public:
    Car(float startX, float startY, sf::Color color);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

private:
    sf::RectangleShape m_shape;
    sf::Vector2f m_velocity;
};
5. Car Implementation (src/Entities/Car.cpp)
Note: Usa sf::Vector2f per dimensioni e posizioni (richiesto da SFML 3).

C++

#include "../../include/Entities/Car.h"

Car::Car(float startX, float startY, sf::Color color)
{
    m_shape.setSize(sf::Vector2f(40.f, 20.f)); 
    m_shape.setOrigin(sf::Vector2f(20.f, 10.f));
    m_shape.setPosition(sf::Vector2f(startX, startY));
    m_shape.setFillColor(color);
    
    m_velocity = sf::Vector2f(100.f, 0.f); 
}

void Car::update(float deltaTime) {
    m_shape.move(m_velocity * deltaTime);

    if (m_shape.getPosition().x > 800.f) {
        m_shape.setPosition(sf::Vector2f(0.f, m_shape.getPosition().y));
    }
}

void Car::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
}
Prossimi Passi (Roadmap)
Resource Manager: Implementare caricamento texture e font per evitare il "White Square Problem".

Assets: Sostituire i rettangoli con sprite di auto.

Mappa Stradale: Definire una struttura dati per le strade (non solo sfondo grigio).


***