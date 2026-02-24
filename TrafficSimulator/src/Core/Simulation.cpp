#include "../../include/Core/Simulation.h"

Simulation::Simulation()
    : m_window(sf::VideoMode({ 800, 600 }), "Traffic Simulator")
{
    m_window.setFramerateLimit(60);

    const sf::Texture &carTex = m_resourceManager.getTexture("assets/textures/car_top.png");

    m_roads.emplace_back(sf::Vector2f(0.f, 300.f), sf::Vector2f(800.f, 300.f), 100.f, 2);

    // Mettiamo il semaforo a metà strada (a 400 pixel dall'inizio)
    // Posizione su schermo: (400, 240) per metterlo a lato della corsia
    m_trafficLights.emplace_back(sf::Vector2f(400.f, 240.f), 400.f);

    // Auto varie e veloci
    m_cars.emplace_back(m_roads[0].getStart(), m_roads[0].getEnd(), carTex, 0.f);
    m_cars.emplace_back(m_roads[0].getStart(), m_roads[0].getEnd(), carTex, 80.f);
    m_cars.emplace_back(m_roads[0].getStart(), m_roads[0].getEnd(), carTex, 160.f);
    m_cars.emplace_back(m_roads[0].getStart(), m_roads[0].getEnd(), carTex, 240.f);
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
    float dt = deltaTime.asSeconds();

    // 1. Aggiorna semafori
    for (auto &light : m_trafficLights) {
        light.update(dt);
    }

    // 2. Ordina le auto
    std::sort(m_cars.begin(), m_cars.end(), [](const Car &a, const Car &b) {
        return a.getTraveledDistance() > b.getTraveledDistance();
        });

    // Per ora abbiamo una sola strada lunga 800px
    float roadLength = 800.f;

    // 3. Logica dell'Ostacolo
    for (size_t i = 0; i < m_cars.size(); ++i) {
        bool hasLeader = false;
        float leaderSpeed = 0.f;
        float distanceToLeader = 10000.f;
        float myPos = m_cars[i].getTraveledDistance();

        // --- Controlla Auto Davanti ---
        Car *leaderCar = nullptr;
        if (i == 0) {
            if (m_cars.size() > 1) leaderCar = &m_cars.back();
        }
        else {
            leaderCar = &m_cars[i - 1];
        }

        if (leaderCar) {
            float carPos = leaderCar->getTraveledDistance();
            float dist = (carPos > myPos) ? (carPos - myPos) : ((carPos + roadLength) - myPos);
            distanceToLeader = dist;
            leaderSpeed = leaderCar->getSpeed();
            hasLeader = true;
        }

        // --- Controlla Semaforo ---
        if (!m_trafficLights.empty() && m_trafficLights[0].getState() == World::TrafficLight::State::Red) {
            float lightPos = m_trafficLights[0].getRoadPosition();
            float distToLight = (lightPos > myPos) ? (lightPos - myPos) : ((lightPos + roadLength) - myPos);

            // Se il semaforo è davanti a me, ED È PIÙ VICINO dell'auto che ho davanti,
            // allora il semaforo diventa il mio bersaglio principale!
            if (!hasLeader || distToLight < distanceToLeader) {
                distanceToLeader = distToLight;
                leaderSpeed = 0.f; // Il semaforo è fermo!
                hasLeader = true;
            }
        }

        m_cars[i].update(dt, hasLeader, leaderSpeed, distanceToLeader);
    }
}

void Simulation::render() {
    m_window.clear(sf::Color(30, 100, 30)); // Sfondo verde (erba) per vedere meglio la strada

    // Disegniamo PRIMA le strade (così le auto ci passano sopra)
    for (auto &road : m_roads) {
        road.draw(m_window);
    }

    for (auto &light : m_trafficLights) {
        light.draw(m_window);
    }

    // Poi le auto
    for (auto &car : m_cars) {
        car.draw(m_window);
    }

    m_window.display();
}