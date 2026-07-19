#include "../../include/Entities/Car.h"
#include "../../include/World/Node.h"
#include <cmath>
#include <algorithm>
#include <cstdlib> // Per rand()

Car::Car(World::Road *initialRoad, const sf::Texture &texture, float initialOffset)
    : m_sprite(texture),
    m_currentSpeed(0.f),
    m_traveledDistance(initialOffset)
{
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.size / 2.f);
    float scale = 60.f / bounds.size.x;
    m_sprite.setScale({ scale, scale });

    // Inizializza la prima strada
    changeRoad(initialRoad);
    update(0.f, false, 0.f, 10000.f);
}

void Car::changeRoad(World::Road *newRoad) {
    m_currentRoad = newRoad;
    m_totalDistance = m_currentRoad->getLength();

    // Ricalcoliamo le distanze cumulative per la nuova strada
    m_cumulativeDistances.clear();
    m_cumulativeDistances.push_back(0.f);

    const auto &path = m_currentRoad->getWaypoints();
    float dist = 0.f;
    for (size_t i = 1; i < path.size(); ++i) {
        sf::Vector2f diff = path[i] - path[i - 1];
        dist += std::sqrt(diff.x * diff.x + diff.y * diff.y);
        m_cumulativeDistances.push_back(dist);
    }
}

void Car::update(float deltaTime, bool hasLeader, float leaderSpeed, float distanceToLeader) {
    // --- 1. MATEMATICA IDM ---
    float v = m_currentSpeed;
    float s = 10000.f;
    float dV = 0.f;
    if (hasLeader) {
        s = distanceToLeader - 60.f;
        s = std::max(0.1f, s);
        dV = m_currentSpeed - leaderSpeed;
    }
    float freeRoadTerm = 1.0f - std::pow(v / m_desiredSpeed, static_cast<float>(m_accelerationExponent));
    float desiredGap = m_minGap + (v * m_timeHeadway) + (v * dV) / (2.0f * std::sqrt(m_maxAcceleration * m_maxDeceleration));
    float interactionTerm = std::pow(desiredGap / s, 2.0f);
    float acceleration = m_maxAcceleration * (freeRoadTerm - interactionTerm);

    m_currentSpeed += acceleration * deltaTime;
    if (m_currentSpeed < 0.f) m_currentSpeed = 0.f;

    m_traveledDistance += m_currentSpeed * deltaTime;

    // --- 2. LOGICA DELL'INCROCIO ---
    if (m_traveledDistance >= m_totalDistance) {
        World::Node *endNode = m_currentRoad->getEndNode();
        if (endNode) {
            const auto &nextRoads = endNode->getOutgoingRoads();
            if (!nextRoads.empty()) {
                // Scegliamo una strada a caso!
                int choice = rand() % nextRoads.size();
                World::Road *nextRoad = nextRoads[choice];

                // Resettiamo la distanza e cambiamo strada
                m_traveledDistance -= m_totalDistance;
                changeRoad(nextRoad);
            }
            else {
                // Strada senza uscita: l'auto ha terminato il percorso!
                m_finished = true;
            }
        }
    }

    // --- 3. POSIZIONAMENTO GRAFICO ---
    const auto &path = m_currentRoad->getWaypoints();
    size_t currentSegment = 0;
    for (size_t i = 0; i < m_cumulativeDistances.size() - 1; ++i) {
        if (m_traveledDistance >= m_cumulativeDistances[i] && m_traveledDistance < m_cumulativeDistances[i + 1]) {
            currentSegment = i;
            break;
        }
    }
    if (currentSegment >= path.size() - 1) currentSegment = path.size() - 2;

    sf::Vector2f startP = path[currentSegment];
    sf::Vector2f endP = path[currentSegment + 1];
    float startDist = m_cumulativeDistances[currentSegment];
    float segmentLength = m_cumulativeDistances[currentSegment + 1] - startDist;

    float t = (m_traveledDistance - startDist) / segmentLength;
    sf::Vector2f newPos = startP + (endP - startP) * t;
    m_sprite.setPosition(newPos);

    sf::Vector2f diff = endP - startP;
    if (segmentLength > 0.f) {
        sf::Vector2f dir = diff / segmentLength;
        m_velocity = dir * m_currentSpeed;
        float angleRadians = std::atan2(dir.y, dir.x);
        m_sprite.setRotation(sf::degrees(angleRadians * 180.f / 3.14159265f));
    }
}

void Car::draw(sf::RenderWindow &window) {
    window.draw(m_sprite);
}