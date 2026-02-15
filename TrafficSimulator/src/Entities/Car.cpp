#include "../../include/Entities/Car.h"

Car::Car(float startX, float startY, sf::Color color)
    : m_speed(100.0f) // Velocità in pixel al secondo
{
    m_shape.setSize(sf::Vector2f(40.0f, 20.0f)); // Dimensioni auto
    m_shape.setOrigin(sf::Vector2f(20.0f, 10.0f)); // Centro dell'auto
    m_shape.setPosition(sf::Vector2f(startX, startY));
    m_shape.setFillColor(color);
    m_velocity = { m_speed, 0.0f }; // Per ora va solo dritta verso destra
}

void Car::update(float deltaTime) {
    // Movimento semplice: Posizione = Velocità * Tempo
    m_shape.move(m_velocity * deltaTime);

    // (Opzionale) Reset posizione se esce dallo schermo per testare il loop
    if (m_shape.getPosition().x > 800.0f) {
        m_shape.setPosition(sf::Vector2f(0.0f, m_shape.getPosition().y));
    }
}

void Car::draw(sf::RenderWindow &window) {
    window.draw(m_shape);
}