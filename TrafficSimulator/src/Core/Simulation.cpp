#include "../../include/Core/Simulation.h"

Simulation::Simulation()
    : m_window(sf::VideoMode({ 800, 600 }), "Traffic Simulator")
{
    m_window.setFramerateLimit(60);

    // 1. Carichiamo risorse
    const sf::Texture &carTex = m_resourceManager.getTexture("assets/textures/car_top.png");

    // 2. Creiamo una strada (Da (50,300) a (750, 300), larga 80 pixel)
    // Nota: L'ordine è importante! Creiamo le strade PRIMA delle auto se vogliamo spawnare le auto sopra
    m_roads.emplace_back(sf::Vector2f(0.f, 300.f), sf::Vector2f(800.f, 300.f), 80.f, 2);

    // 3. Creiamo le auto (sopra la strada)
    m_cars.emplace_back(50.f, 280.f, carTex); // Corsia sopra
    m_cars.emplace_back(150.f, 320.f, carTex); // Corsia sotto
}

void Simulation::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        processEvents();
        // Calcola il tempo passato dall'ultimo frame (delta time)
        // Fondamentale per rendere la simulazione indipendente dagli FPS
        sf::Time dt = clock.restart();
        update(dt);
        render();
    }
}

void Simulation::processEvents() {
    // *** CAMBIAMENTO SFML 3.0 ***
    // pollEvent restituisce un std::optional.
    // Dobbiamo controllare se c'è un evento e poi di che tipo è.

    while (const auto event = m_window.pollEvent()) {
        // Metodo 1: is<T>() per controllare il tipo
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        // Metodo 2: getIf<T>() se ti servono i dati dell'evento (es. tasto premuto)
        else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // Nota l'uso dell'enum Scoped: Key::Escape
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
            }
        }
    }
}

void Simulation::update(sf::Time deltaTime) {
    float dtSeconds = deltaTime.asSeconds();
    for (auto &car : m_cars) {
        car.update(dtSeconds);
    }
}

void Simulation::render() {
    m_window.clear(sf::Color(30, 100, 30)); // Sfondo verde (erba) per vedere meglio la strada

    // Disegniamo PRIMA le strade (così le auto ci passano sopra)
    for (auto &road : m_roads) {
        road.draw(m_window);
    }

    // Poi le auto
    for (auto &car : m_cars) {
        car.draw(m_window);
    }

    m_window.display();
}