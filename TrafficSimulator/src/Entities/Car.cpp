#include "../../include/Entities/Car.h"

Car::Car(sf::Vector2f start, sf::Vector2f end, const sf::Texture &texture, float initialOffset)
    : m_sprite(texture),
    m_startPoint(start),
    m_endPoint(end),
    m_traveledDistance(initialOffset), // <-- Inizializziamo con l'offset!
    m_currentSpeed(0.f) // Partiamo fermi
{
    // 1. Calcolo Vettore Direzione e Distanza Totale
    sf::Vector2f diff = end - start;
    m_totalDistance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    // 2. Normalizzazione (Direzione Unitaria)
    sf::Vector2f direction(0.f, 0.f);
    if (m_totalDistance > 0.f) {
        direction = diff / m_totalDistance;
    }

    // 3. Salviamo la direzione in m_velocity
    // (In update la moltiplicheremo per m_currentSpeed)
    m_velocity = direction;

    // --- 4. Configurazione Sprite ---
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.size / 2.f);
    float scale = 60.f / bounds.size.x; // Target 60px
    m_sprite.setScale({ scale, scale });

    // CALCOLO POSIZIONE INIZIALE BASATA SULL'OFFSET
    if (m_totalDistance > 0.f) {
        sf::Vector2f startPos = m_startPoint + (direction * m_traveledDistance);
        m_sprite.setPosition(startPos);
    }
    else {
        m_sprite.setPosition(m_startPoint);
    }

    // --- 5. Rotazione ---
    float angleRadians = std::atan2(direction.y, direction.x);
    float angleDegrees = angleRadians * 180.f / 3.14159265f;
    m_sprite.setRotation(sf::degrees(angleDegrees));
}

void Car::update(float deltaTime, bool hasLeader, float leaderSpeed, float distanceToLeader) {
    float v = m_currentSpeed;
    float s = 10000.f;
    float dV = 0.f;

    // --- Rilevamento Ostacolo ---
    if (hasLeader) {
        // La distanza passata ora è già calcolata dalla Simulation
        s = distanceToLeader - 60.f; // Sottraiamo la lunghezza dell'auto
        s = std::max(0.1f, s);       // Evitiamo divisioni per 0 o crash matematici
        dV = m_currentSpeed - leaderSpeed;
    }

    // --- FORMULA IDM (Invariata) ---
    float freeRoadTerm = 1.0f - std::pow(v / m_desiredSpeed, m_accelerationExponent);
    float desiredGap = m_minGap + (v * m_timeHeadway) +
        (v * dV) / (2.0f * std::sqrt(m_maxAcceleration * m_maxDeceleration));

    float interactionTerm = std::pow(desiredGap / s, 2);
    float acceleration = m_maxAcceleration * (freeRoadTerm - interactionTerm);

    // --- Integrazione Fisica ---
    m_currentSpeed += acceleration * deltaTime;
    if (m_currentSpeed < 0.f) m_currentSpeed = 0.f;

    float step = m_currentSpeed * deltaTime;
    m_traveledDistance += step;

    // Aggiorniamo la velocità vettoriale
    sf::Vector2f dir = (m_endPoint - m_startPoint) / m_totalDistance;
    m_velocity = dir * m_currentSpeed;

    // Loop del mondo
    if (m_traveledDistance >= m_totalDistance) {
        m_traveledDistance -= m_totalDistance;
    }

    // Posizione sullo schermo
    float t = m_traveledDistance / m_totalDistance;
    sf::Vector2f newPos = m_startPoint + (m_endPoint - m_startPoint) * t;
    m_sprite.setPosition(newPos);
}

void Car::draw(sf::RenderWindow &window) {
    window.draw(m_sprite);
}

float Car::getSpeed() const
{
    return m_currentSpeed;
}
