#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../World/Road.h" // Aggiunto per conoscere la classe Road

class Car {
public:
    // Passiamo la strada iniziale invece del percorso grezzo
    Car(World::Road *initialRoad, const sf::Texture &texture, float initialOffset = 0.f);

    void update(float deltaTime, bool hasLeader, float leaderSpeed, float distanceToLeader);
    void draw(sf::RenderWindow &window);

    void setDesiredSpeed(float speed) { m_desiredSpeed = speed; }
    void setTimeHeadway(float time) { m_timeHeadway = time; }
    float getSpeed() const { return m_currentSpeed; }
    float getTraveledDistance() const { return m_traveledDistance; }

    World::Road *getCurrentRoad() const { return m_currentRoad; } // Nuovo getter!

    bool isFinished() const { return m_finished; }

private:
    void changeRoad(World::Road *newRoad); // Funzione interna per cambiare strada

    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
    float m_currentSpeed;

    World::Road *m_currentRoad; // La strada attuale

    std::vector<float> m_cumulativeDistances;
    float m_totalDistance;
    float m_traveledDistance;

    // Parametri IDM
    float m_desiredSpeed = 120.f;
    float m_minGap = 20.f;
    float m_timeHeadway = 1.5f;
    float m_maxAcceleration = 100.f;
    float m_maxDeceleration = 150.f;
    int   m_accelerationExponent = 4;

    bool m_finished = false;
};