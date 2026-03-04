#include "../../include/Core/Simulation.h"
#include <cmath>
#include <algorithm>

// --- FUNZIONE MATEMATICA PER ARROTONDARE GLI ANGOLI ---
// Trasforma un percorso a spigoli in un percorso curvo fluido
std::vector<sf::Vector2f> createRoundedPath(const std::vector<sf::Vector2f> &points, float radius, int segments) {
    std::vector<sf::Vector2f> smoothPath;

    // Cicliamo su ogni punto (ignorando l'ultimo perché è uguale al primo)
    for (size_t i = 0; i < points.size() - 1; ++i) {
        // Troviamo il punto precedente, attuale e successivo
        sf::Vector2f p_prev = (i == 0) ? points[points.size() - 2] : points[i - 1];
        sf::Vector2f p_curr = points[i];
        sf::Vector2f p_next = points[i + 1];

        // Direzione dal punto attuale al precedente
        sf::Vector2f dirPrev = p_prev - p_curr;
        float lenPrev = std::sqrt(dirPrev.x * dirPrev.x + dirPrev.y * dirPrev.y);
        dirPrev /= lenPrev;

        // Direzione dal punto attuale al successivo
        sf::Vector2f dirNext = p_next - p_curr;
        float lenNext = std::sqrt(dirNext.x * dirNext.x + dirNext.y * dirNext.y);
        dirNext /= lenNext;

        // Limita il raggio se il segmento è troppo corto
        float actualRadius = std::min({ radius, lenPrev / 2.f, lenNext / 2.f });

        // Punti di controllo della curva di Bézier
        sf::Vector2f curveStart = p_curr + dirPrev * actualRadius;
        sf::Vector2f curveEnd = p_curr + dirNext * actualRadius;

        // Genera i punti intermedi della curva
        for (int j = 0; j <= segments; ++j) {
            float t = static_cast<float>(j) / segments;
            float u = 1.0f - t;
            // Formula della Curva di Bézier Quadratica
            sf::Vector2f pt = (u * u) * curveStart + (2.0f * u * t) * p_curr + (t * t) * curveEnd;
            smoothPath.push_back(pt);
        }
    }

    // Chiudiamo il cerchio
    if (!smoothPath.empty()) smoothPath.push_back(smoothPath.front());
    return smoothPath;
}

// --- COSTRUTTORE SIMULATION ---
Simulation::Simulation()
    : m_window(sf::VideoMode({ 800, 600 }), "Traffic Simulator")
{
    m_window.setFramerateLimit(60);
    const sf::Texture &carTex = m_resourceManager.getTexture("assets/textures/car_top.png");

    // 1. Definiamo i 4 spigoli del rettangolo
    std::vector<sf::Vector2f> sharpPath = {
        {100.f, 100.f}, {700.f, 100.f},
        {700.f, 500.f}, {100.f, 500.f},
        {100.f, 100.f}
    };

    // 2. MAGIA: Arrotondiamo il percorso (Raggio curva: 80 pixel, 15 punti per curva)
    std::vector<sf::Vector2f> trackPath = createRoundedPath(sharpPath, 80.f, 15);

    // 3. Calcoliamo la lunghezza dinamica del nuovo percorso smussato
    m_trackLength = 0.f;
    for (size_t i = 0; i < trackPath.size() - 1; ++i) {
        sf::Vector2f diff = trackPath[i + 1] - trackPath[i];
        m_trackLength += std::sqrt(diff.x * diff.x + diff.y * diff.y);

        // Disegniamo la strada per ogni piccolo segmento!
        m_roads.emplace_back(trackPath[i], trackPath[i + 1], 80.f, 2);
    }

    // 4. Posizioniamo il semaforo a inizio curva
    m_trafficLights.emplace_back(sf::Vector2f(400.f, 60.f), 300.f);

    // 5. Generiamo le auto passandogli il percorso smussato
    m_cars.emplace_back(trackPath, carTex, 0.f);
    m_cars.emplace_back(trackPath, carTex, 100.f);
    m_cars.emplace_back(trackPath, carTex, 200.f);
    m_cars.emplace_back(trackPath, carTex, 300.f);

    m_cars[0].setDesiredSpeed(60.f);
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
            float dist = (carPos >= myPos) ? (carPos - myPos) : ((carPos + m_trackLength) - myPos);
            distanceToLeader = dist;
            leaderSpeed = leaderCar->getSpeed();
            hasLeader = true;
        }

        // --- Controlla Semaforo ---
        if (!m_trafficLights.empty() && m_trafficLights[0].getState() == World::TrafficLight::State::Red) {
            float lightPos = m_trafficLights[0].getRoadPosition();
            float distToLight = (lightPos >= myPos) ? (lightPos - myPos) : ((lightPos + m_trackLength) - myPos);

            // --- CORREZIONE 2: Limite Visivo ---
            // Le auto reagiscono al semaforo SOLO se si trova a meno di 300 pixel di distanza
            if (distToLight < 300.f) {
                if (!hasLeader || distToLight < distanceToLeader) {
                    distanceToLeader = distToLight;
                    leaderSpeed = 0.f; // Il semaforo è fermo!
                    hasLeader = true;
                }
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