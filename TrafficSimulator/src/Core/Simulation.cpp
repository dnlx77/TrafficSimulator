#include "../../include/Core/Simulation.h"
#include <cmath>
#include <algorithm>

// --- NUOVA FUNZIONE MATEMATICA PER LE CURVE ---
std::vector<sf::Vector2f> createRoundedPath(const std::vector<sf::Vector2f> &points, float radius, int segments) {
    if (points.size() < 3) return points; // Troppo pochi punti per fare una curva

    std::vector<sf::Vector2f> smoothPath;
    smoothPath.push_back(points.front()); // Inseriamo il punto di partenza

    for (size_t i = 1; i < points.size() - 1; ++i) {
        sf::Vector2f p_prev = points[i - 1];
        sf::Vector2f p_curr = points[i];
        sf::Vector2f p_next = points[i + 1];

        sf::Vector2f dirPrev = p_prev - p_curr;
        float lenPrev = std::sqrt(dirPrev.x * dirPrev.x + dirPrev.y * dirPrev.y);
        if (lenPrev != 0) dirPrev /= lenPrev;

        sf::Vector2f dirNext = p_next - p_curr;
        float lenNext = std::sqrt(dirNext.x * dirNext.x + dirNext.y * dirNext.y);
        if (lenNext != 0) dirNext /= lenNext;

        // Adattiamo il raggio se il segmento di strada è troppo corto
        float actualRadius = std::min({ radius, lenPrev / 1.5f, lenNext / 1.5f });

        sf::Vector2f curveStart = p_curr + dirPrev * actualRadius;
        sf::Vector2f curveEnd = p_curr + dirNext * actualRadius;

        // Segmento dritto fino all'inizio della curva
        smoothPath.push_back(curveStart);

        // Generiamo i punti della curva di Bézier Quadratica
        for (int j = 1; j < segments; ++j) {
            float t = static_cast<float>(j) / segments;
            float u = 1.0f - t;
            sf::Vector2f pt = (u * u) * curveStart + (2.0f * u * t) * p_curr + (t * t) * curveEnd;
            smoothPath.push_back(pt);
        }

        // Segmento dritto di fine curva
        smoothPath.push_back(curveEnd);
    }

    smoothPath.push_back(points.back()); // Inseriamo il punto finale
    return smoothPath;
}

Simulation::Simulation()
    : m_window(sf::VideoMode({ 800, 600 }), "Traffic Simulator")
{
    m_window.setFramerateLimit(60);

    // --- 1. NODI DELLA MAPPA ---
    // Nodi Esterni (Spawn e Uscite)
    m_nodes.push_back(std::make_unique<World::Node>(sf::Vector2f(50.f, 300.f)));   // 0: Ovest (Spawn)
    m_nodes.push_back(std::make_unique<World::Node>(sf::Vector2f(400.f, 50.f)));   // 1: Nord (Spawn)
    m_nodes.push_back(std::make_unique<World::Node>(sf::Vector2f(750.f, 300.f)));  // 2: Est (Uscita)
    m_nodes.push_back(std::make_unique<World::Node>(sf::Vector2f(400.f, 550.f)));  // 3: Sud (Uscita)

    // Nodi Interni (Le 4 linee di stop attorno al centro dell'incrocio)
    m_nodes.push_back(std::make_unique<World::Node>(sf::Vector2f(350.f, 300.f)));  // 4: Ingresso Ovest
    m_nodes.push_back(std::make_unique<World::Node>(sf::Vector2f(400.f, 250.f)));  // 5: Ingresso Nord
    m_nodes.push_back(std::make_unique<World::Node>(sf::Vector2f(450.f, 300.f)));  // 6: Uscita Est
    m_nodes.push_back(std::make_unique<World::Node>(sf::Vector2f(400.f, 350.f)));  // 7: Uscita Sud

    float roadWidth = 60.f;

    // --- 2. STRADE PRINCIPALI ---
    // m_roads[0]: Ovest -> Incrocio
    m_roads.push_back(std::make_unique<World::Road>(m_nodes[0].get(), m_nodes[4].get(), std::vector<sf::Vector2f>{m_nodes[0]->getPosition(), m_nodes[4]->getPosition()}, roadWidth));
    // m_roads[1]: Nord -> Incrocio
    m_roads.push_back(std::make_unique<World::Road>(m_nodes[1].get(), m_nodes[5].get(), std::vector<sf::Vector2f>{m_nodes[1]->getPosition(), m_nodes[5]->getPosition()}, roadWidth));
    // m_roads[2]: Incrocio -> Est
    m_roads.push_back(std::make_unique<World::Road>(m_nodes[6].get(), m_nodes[2].get(), std::vector<sf::Vector2f>{m_nodes[6]->getPosition(), m_nodes[2]->getPosition()}, roadWidth));
    // m_roads[3]: Incrocio -> Sud
    m_roads.push_back(std::make_unique<World::Road>(m_nodes[7].get(), m_nodes[3].get(), std::vector<sf::Vector2f>{m_nodes[7]->getPosition(), m_nodes[3]->getPosition()}, roadWidth));


    // --- 3. CONNETTORI INTERNI ALL'INCROCIO (La vera magia) ---
    sf::Vector2f center(400.f, 300.f); // Il punto centrale matematico

    // Traffico da OVEST
    // -> Va dritto verso Est (strada dritta)
    m_roads.push_back(std::make_unique<World::Road>(m_nodes[4].get(), m_nodes[6].get(), std::vector<sf::Vector2f>{m_nodes[4]->getPosition(), m_nodes[6]->getPosition()}, roadWidth));
    // -> Svolta a destra verso Sud (usiamo la curva di Bézier per smussare l'angolo!)
    std::vector<sf::Vector2f> pathWestToSouth = { m_nodes[4]->getPosition(), center, m_nodes[7]->getPosition() };
    m_roads.push_back(std::make_unique<World::Road>(m_nodes[4].get(), m_nodes[7].get(), createRoundedPath(pathWestToSouth, 40.f, 15), roadWidth));

    // Traffico da NORD
    // -> Va dritto verso Sud (strada dritta)
    m_roads.push_back(std::make_unique<World::Road>(m_nodes[5].get(), m_nodes[7].get(), std::vector<sf::Vector2f>{m_nodes[5]->getPosition(), m_nodes[7]->getPosition()}, roadWidth));
    // -> Svolta a sinistra verso Est (curva di Bézier ampia)
    std::vector<sf::Vector2f> pathNorthToEast = { m_nodes[5]->getPosition(), center, m_nodes[6]->getPosition() };
    m_roads.push_back(std::make_unique<World::Road>(m_nodes[5].get(), m_nodes[6].get(), createRoundedPath(pathNorthToEast, 40.f, 15), roadWidth));


    // --- 4. SEMAFORI ---
    // Ora i semafori sono agganciati alle due strade di ingresso (m_roads[0] e m_roads[1])
    m_trafficLights.emplace_back(m_roads[0].get(), sf::Vector2f(330.f, 345.f), m_roads[0]->getLength() - 10.f, World::TrafficLight::State::Red);
    m_trafficLights.emplace_back(m_roads[1].get(), sf::Vector2f(355.f, 230.f), m_roads[1]->getLength() - 10.f, World::TrafficLight::State::Green);
}

void Simulation::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();

    // 1. Aggiorna Semafori
    for (auto &light : m_trafficLights) {
        light.update(dt);
    }

    // 2. SPAWNER A DUE INGRESSI
    m_spawnTimer += dt;
    if (m_spawnTimer >= m_spawnInterval) {
        m_spawnTimer = 0.f;
        m_spawnInterval = 1.0f + static_cast<float>(rand() % 150) / 100.f;

        // Tiriamo i dadi (0 o 1) per decidere se l'auto nasce a Ovest o a Nord
        World::Road *spawnRoad = (rand() % 2 == 0) ? m_roads[0].get() : m_roads[1].get();

        const sf::Texture &carTex = m_resourceManager.getTexture("assets/textures/car_top.png");
        m_cars.emplace_back(spawnRoad, carTex, 0.f);
        m_cars.back().setDesiredSpeed(80.f + (rand() % 40));
    }

    // 3. DESPAWNER
    m_cars.erase(std::remove_if(m_cars.begin(), m_cars.end(),
        [](const Car &car) { return car.isFinished(); }), m_cars.end());

    // 4. FISICA IDM (Auto + Semafori)
    for (size_t i = 0; i < m_cars.size(); ++i) {
        bool hasLeader = false;
        float leaderSpeed = 0.f;
        float distanceToLeader = 10000.f;

        World::Road *myRoad = m_cars[i].getCurrentRoad();
        float myPos = m_cars[i].getTraveledDistance();

        // Rilevamento Ostacolo 1: Altre Auto
        for (size_t j = 0; j < m_cars.size(); ++j) {
            if (i == j) continue;
            if (m_cars[j].getCurrentRoad() == myRoad) {
                float otherPos = m_cars[j].getTraveledDistance();
                if (otherPos > myPos) {
                    float dist = otherPos - myPos;
                    if (dist < distanceToLeader) {
                        distanceToLeader = dist;
                        leaderSpeed = m_cars[j].getSpeed();
                        hasLeader = true;
                    }
                }
            }
        }

        // Rilevamento Ostacolo 2: Semafori!
        for (const auto &light : m_trafficLights) {
            // Se il semaforo è sulla MIA strada ed è ROSSO...
            if (light.getTargetRoad() == myRoad && light.getState() == World::TrafficLight::State::Red) {
                float lightPos = light.getRoadPosition();

                // Se il semaforo è davanti a me...
                if (lightPos > myPos) {
                    float distToLight = lightPos - myPos;

                    // Se il semaforo è nel mio campo visivo (es. < 250px) 
                    // E se è più vicino dell'auto che ho davanti... divento io il Leader!
                    if (distToLight < 250.f && distToLight < distanceToLeader) {
                        distanceToLeader = distToLight;
                        leaderSpeed = 0.f; // Velocità 0 = Muro/Frenata
                        hasLeader = true;
                    }
                }
            }
        }

        m_cars[i].update(dt, hasLeader, leaderSpeed, distanceToLeader);
    }
}

// [IN RENDER RICORDA DI AGGIUNGERE IL .get() AI PUNTATORI]
void Simulation::processEvents() {
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) m_window.close();
        else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) m_window.close();
        }
    }
}

void Simulation::render() {
    // Sfondo verde prato
    m_window.clear(sf::Color(34, 139, 34));

    // 1. Disegna le strade (sotto)
    for (const auto &road : m_roads) {
        road->draw(m_window);
    }

    // 2. Disegna i semafori (al centro) <-- ECCO LA RIGA MANCANTE
    for (auto &light : m_trafficLights) {
        light.draw(m_window);
    }

    // 3. Disegna le auto (sopra)
    for (auto &car : m_cars) {
        car.draw(m_window);
    }

    m_window.display();
}

void Simulation::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        processEvents();
        update(clock.restart());
        render();
    }
}