#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional> // Serve per gestire "nessuna auto davanti"

class Car {
public:
    Car(const std::vector<sf::Vector2f> &path, const sf::Texture &texture, float intialOffset= 0.f);

    // Ora l'update accetta un puntatore opzionale all'auto davanti
    // Se è nullptr, significa che la strada è libera
    void update(float deltaTime, bool hasLeader, float leaderSpeed, float distanceToLeader);

    void draw(sf::RenderWindow &window);

    // Getters per la fisica
    float getSpeed() const;
    float getTraveledDistance() const { return m_traveledDistance; }
    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }
    void setDesiredSpeed(float speed) { m_desiredSpeed = speed; }
    void setTimeHeadway(float time) { m_timeHeadway = time; }

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;     // Vettore velocità reale
    float m_currentSpeed;        // Modulo della velocità (scalare)

    // Posizione sulla strada
    std::vector<sf::Vector2f> m_path;           // Tutti i punti della strada
    std::vector<float> m_cumulativeDistances;   // Distanza raggiunta ad ogni punto
    float m_totalDistance;                      // Lunghezza totale del percorso
    float m_traveledDistance;

    // --- Parametri IDM (Fisica) ---
    float m_desiredSpeed = 120.f;      // v0 (pixel/s, circa 40-50 km/h in scala)
    float m_minGap = 20.f;             // s0 (distanza minima paraurti-paraurti)
    float m_timeHeadway = 1.5f;        // T (tempo di sicurezza, es. 1.5 secondi)
    float m_maxAcceleration = 100.f;   // a (pixel/s^2)
    float m_maxDeceleration = 150.f;   // b (pixel/s^2, frenata confortevole)
    int   m_accelerationExponent = 4;  // delta
};