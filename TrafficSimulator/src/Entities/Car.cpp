#include "../../include/Entities/Car.h"

Car::Car(const std::vector<sf::Vector2f>& path, const sf::Texture &texture, float initialOffset)
    : m_sprite(texture),
    m_path(path),
    m_traveledDistance(initialOffset), // <-- Inizializziamo con l'offset!
    m_currentSpeed(0.f), // Partiamo fermi
    m_totalDistance(0.f)
{
    // 1. Calcoliamo le distanze cumulative per ogni segmento del percorso
    m_cumulativeDistances.push_back(0.f); // Il primo punto è a distanza 0

    for (size_t i = 1; i < m_path.size(); ++i) {
        sf::Vector2f diff = m_path[i] - m_path[i - 1];
        float segmentLength = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        m_totalDistance += segmentLength;
        m_cumulativeDistances.push_back(m_totalDistance);
    }

    // --- 4. Configurazione Sprite ---
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.size / 2.f);
    float scale = 60.f / bounds.size.x; // Target 60px
    m_sprite.setScale({ scale, scale });

    // Inizializziamo l'auto forzando un aggiornamento a deltaTime = 0
    // Questo la posiziona e la ruota correttamente prima del primo frame
    update(0.f, false, 0.f, 10000.f);
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

    // --- NUOVA LOGICA DI POSIZIONAMENTO (PATHFINDING) ---
    m_traveledDistance += m_currentSpeed * deltaTime;

    // Loop del percorso
    float localDistance = m_traveledDistance;
    while (localDistance >= m_totalDistance && m_totalDistance > 0) {
        localDistance -= m_totalDistance;
    }
    // Sincronizziamo la variabile globale per non farla crescere all'infinito
    m_traveledDistance = localDistance;

    // 1. Troviamo su quale segmento del percorso ci troviamo
    size_t currentSegment = 0;
    for (size_t i = 0; i < m_cumulativeDistances.size() - 1; ++i) {
        if (localDistance >= m_cumulativeDistances[i] && localDistance < m_cumulativeDistances[i + 1]) {
            currentSegment = i;
            break;
        }
    }

    // Se per qualche errore numerico non lo troviamo, usiamo l'ultimo segmento
    if (currentSegment >= m_path.size() - 1) currentSegment = m_path.size() - 2;

    // 2. Calcoliamo posizione e rotazione interpolando sul segmento attuale
    sf::Vector2f startP = m_path[currentSegment];
    sf::Vector2f endP = m_path[currentSegment + 1];

    float startDist = m_cumulativeDistances[currentSegment];
    float endDist = m_cumulativeDistances[currentSegment + 1];
    float segmentLength = endDist - startDist;

    // Percentuale di completamento del segmento attuale (0.0 -> 1.0)
    float t = (localDistance - startDist) / segmentLength;

    // Posizione 2D
    sf::Vector2f newPos = startP + (endP - startP) * t;
    m_sprite.setPosition(newPos);

    // Vettore Direzione e Rotazione
    sf::Vector2f diff = endP - startP;
    if (segmentLength > 0.f) {
        sf::Vector2f dir = diff / segmentLength;
        m_velocity = dir * m_currentSpeed; // Aggiorniamo la velocità vettoriale

        float angleRadians = std::atan2(dir.y, dir.x);
        m_sprite.setRotation(sf::degrees(angleRadians * 180.f / 3.14159265f));
    }
}

void Car::draw(sf::RenderWindow &window) {
    window.draw(m_sprite);
}

float Car::getSpeed() const
{
    return m_currentSpeed;
}
