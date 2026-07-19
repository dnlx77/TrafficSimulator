#include "../../include/World/TrafficLight.h"

namespace World {
    TrafficLight::TrafficLight(Road *targetRoad, sf::Vector2f position, float roadPosition, State initialState)
        : m_targetRoad(targetRoad),
        m_state(initialState),
        m_roadPosition(roadPosition),
        m_timer(0.f),
        m_redDuration(6.0f),
        m_greenDuration(6.0f)
    {
        // 1. Configurazione Scatola (Sfondo)
        m_box.setSize(sf::Vector2f(24.f, 24.f));
        m_box.setOrigin(sf::Vector2f(12.f, 12.f));
        m_box.setPosition(position);
        m_box.setFillColor(sf::Color(40, 40, 40)); // Grigio scuro
        m_box.setOutlineThickness(2.f);
        m_box.setOutlineColor(sf::Color::Black);

        // 2. Configurazione Luce
        m_shape.setRadius(8.f); // Leggermente più piccola per stare nel box
        m_shape.setOrigin(sf::Vector2f(8.f, 8.f));
        m_shape.setPosition(position);

        m_shape.setFillColor(m_state == State::Red ? sf::Color::Red : sf::Color::Green);
    }

    void TrafficLight::update(float deltaTime) {
        m_timer += deltaTime;

        if (m_state == State::Red && m_timer >= m_redDuration) {
            m_state = State::Green;
            m_shape.setFillColor(sf::Color::Green);
            m_timer = 0.f;
        }
        else if (m_state == State::Green && m_timer >= m_greenDuration) {
            m_state = State::Red;
            m_shape.setFillColor(sf::Color::Red);
            m_timer = 0.f;
        }
    }

    void TrafficLight::draw(sf::RenderWindow &window) {
        window.draw(m_box);   // Prima disegniamo la scatola
        window.draw(m_shape); // Poi la luce colorata sopra
    }
}