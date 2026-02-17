# 🚦 Traffic Simulator - Project Context

Questo file contiene lo stato attuale, la configurazione tecnica e il codice sorgente del progetto **Traffic Simulator**.

---

## 🛠 Tech Stack & Configurazione

* **Linguaggio:** C++ (Standard ISO C++17 o C++20)
* **IDE:** Visual Studio 2022 (MSVC)
* **Libreria Grafica:** SFML **3.0.0**
* **Gestione Dipendenze:** Microsoft vcpkg (Manifest Mode)

---

## 📂 Struttura del Progetto (Modulare)

Il codice è ora organizzato in moduli logici per scalabilità.

```text
TrafficSimulator/
├── vcpkg.json
├── TrafficSimulator.sln
├── PROJECT_CONTEXT.md
└── TrafficSimulator/
    ├── src/
    │   ├── main.cpp              <-- Entry Point
    │   ├── Core/                 <-- Gestione Sistema
    │   │   ├── Simulation.cpp
    │   │   └── ResourceManager.cpp
    │   ├── Entities/             <-- Oggetti Dinamici
    │   │   └── Car.cpp
    │   └── World/                <-- Ambiente Statico
    │       └── Road.cpp
    ├── include/
    │   ├── Core/
    │   │   ├── Simulation.h
    │   │   └── ResourceManager.h
    │   ├── Entities/
    │   │   └── Car.h
    │   └── World/
    │       └── Road.h
    └── assets/
        └── textures/
            └── car_top.png       <-- Asset Auto (Sfondo trasparente)
```

---

## 💻 Codice Sorgente (Snapshot Corrente)

### 1. Main & Core Module

**`src/main.cpp`**
```cpp
#include "../include/Core/Simulation.h"

int main() {
    Simulation sim;
    sim.run();
    return 0;
}
```

**`include/Core/ResourceManager.h`**
```cpp
#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class ResourceManager {
public:
    const sf::Texture& getTexture(const std::string& filename);
    const sf::Font& getFont(const std::string& filename);

private:
    std::map<std::string, sf::Texture> m_textures;
    std::map<std::string, sf::Font> m_fonts;
};
```

**`src/Core/ResourceManager.cpp`**
```cpp
#include "../../include/Core/ResourceManager.h"
#include <iostream>

const sf::Texture& ResourceManager::getTexture(const std::string& filename) {
    auto it = m_textures.find(filename);
    if (it != m_textures.end()) {
        return it->second;
    }
    sf::Texture& texture = m_textures[filename];
    if (!texture.loadFromFile(filename)) {
        std::cerr << "ERRORE: Impossibile caricare texture: " << filename << std::endl;
    } else {
        texture.setSmooth(true);
    }
    return texture;
}

const sf::Font& ResourceManager::getFont(const std::string& filename) {
    auto it = m_fonts.find(filename);
    if (it != m_fonts.end()) {
        return it->second;
    }
    sf::Font& font = m_fonts[filename];
    if (!font.openFromFile(filename)) {
         std::cerr << "ERRORE: Impossibile caricare font: " << filename << std::endl;
    }
    return font;
}
```

**`include/Core/Simulation.h`**
```cpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "ResourceManager.h"
#include "../Entities/Car.h"
#include "../World/Road.h"

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
    
    std::vector<World::Road> m_roads;
    std::vector<Car> m_cars;
};
```

**`src/Core/Simulation.cpp`**
```cpp
#include "../../include/Core/Simulation.h"

Simulation::Simulation()
    : m_window(sf::VideoMode({800, 600}), "Traffic Simulator")
{
    m_window.setFramerateLimit(60);

    // Caricamento Risorse
    const sf::Texture& carTex = m_resourceManager.getTexture("assets/textures/car_top.png");

    // Setup Mondo: Creazione Strada (da sinistra a destra)
    m_roads.emplace_back(sf::Vector2f(0.f, 300.f), sf::Vector2f(800.f, 300.f), 100.f, 2);

    // Setup Entità: Auto
    m_cars.emplace_back(50.f, 280.f, carTex); // Corsia alta
    m_cars.emplace_back(150.f, 320.f, carTex); // Corsia bassa
}

void Simulation::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        processEvents();
        update(clock.restart());
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
    m_window.clear(sf::Color(34, 139, 34)); // Sfondo verde erba

    // Disegno a strati: Prima strade, poi auto
    for (const auto& road : m_roads) {
        road.draw(m_window);
    }
    for (auto& car : m_cars) {
        car.draw(m_window);
    }

    m_window.display();
}
```

### 2. World Module (Roads)

**`include/World/Road.h`**
```cpp
#pragma once
#include <SFML/Graphics.hpp>

namespace World {
    class Road {
    public:
        Road(sf::Vector2f start, sf::Vector2f end, float width, int lanes);
        void draw(sf::RenderWindow& window);

        sf::Vector2f getStart() const { return m_start; }
        sf::Vector2f getEnd() const { return m_end; }

    private:
        sf::Vector2f m_start, m_end;
        float m_width;
        int m_lanes;
        sf::ConvexShape m_shape;
        sf::VertexArray m_markings;
    };
}
```

**`src/World/Road.cpp`**
```cpp
#include "../../include/World/Road.h"
#include <cmath>

namespace World {
    Road::Road(sf::Vector2f start, sf::Vector2f end, float width, int lanes)
        : m_start(start), m_end(end), m_width(width), m_lanes(lanes)
    {
        sf::Vector2f dir = end - start;
        float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        sf::Vector2f unitDir = dir / len;
        sf::Vector2f unitNormal(-unitDir.y, unitDir.x);
        sf::Vector2f offset = unitNormal * (width / 2.f);

        m_shape.setPointCount(4);
        m_shape.setPoint(0, start - offset);
        m_shape.setPoint(1, end - offset);
        m_shape.setPoint(2, end + offset);
        m_shape.setPoint(3, start + offset);
        m_shape.setFillColor(sf::Color(50, 50, 50));

        m_markings.setPrimitiveType(sf::PrimitiveType::Lines);
        m_markings.resize(2);
        m_markings[0] = {start, sf::Color(255, 255, 255, 180)};
        m_markings[1] = {end, sf::Color(255, 255, 255, 180)};
    }

    void Road::draw(sf::RenderWindow& window) {
        window.draw(m_shape);
        window.draw(m_markings);
    }
}
```

### 3. Entities Module (Car)

**`include/Entities/Car.h`**
```cpp
#pragma once
#include <SFML/Graphics.hpp>

class Car {
public:
    Car(float startX, float startY, const sf::Texture& texture);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
};
```

**`src/Entities/Car.cpp`**
```cpp
#include "../../include/Entities/Car.h"

Car::Car(float startX, float startY, const sf::Texture& texture)
    : m_sprite(texture)
{
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.size / 2.f);
    
    // Scalatura fissa per adattare sprite grandi (target 60px)
    float scale = 60.f / bounds.size.x;
    m_sprite.setScale({scale, scale});
    
    m_sprite.setPosition({startX, startY});
    m_velocity = {100.f, 0.f};
}

void Car::update(float deltaTime) {
    m_sprite.move(m_velocity * deltaTime);

    if (m_sprite.getPosition().x > 800.f) {
        m_sprite.setPosition({0.f, m_sprite.getPosition().y});
    }
}

void Car::draw(sf::RenderWindow& window) {
    window.draw(m_sprite);
}
```

---

## 📋 Roadmap Aggiornata

1.  **[COMPLETATO]** Setup Architettura Modulare (Core, World, Entities).
2.  **[COMPLETATO]** Rendering Strade e Auto con Texture.
3.  **[DA FARE]** **Physics & Movement**: Far sì che le auto seguano la direzione della strada (non solo muoversi su X).
4.  **[DA FARE]** **Intersezioni**: Creare un sistema per collegare più strade (Nodi).
5.  **[DA FARE]** **Car Spawner**: Generare auto automaticamente.