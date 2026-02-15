#include "../include/Simulation.h"

Simulation::Simulation()
    : m_window(sf::VideoMode({ 800, 600 }), "Traffic Simulator C++")
{
    m_window.setFramerateLimit(60);

    // Aggiungiamo un paio di auto per testare
    m_cars.emplace_back(50.0f, 300.0f, sf::Color::Red);
    m_cars.emplace_back(50.0f, 350.0f, sf::Color::Blue);
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
    m_window.clear(sf::Color(50, 50, 50)); // Colore asfalto scuro

    for (auto &car : m_cars) {
        car.draw(m_window);
    }

    m_window.display();
}