#include "../../include/World/TrafficLight.h"

namespace World {
    TrafficLight::TrafficLight(sf::Vector2f position, float roadPosition)
        : m_state(State::Red),
        m_roadPosition(roadPosition),
        m_timer(0.f),
        m_redDuration(5.0f), // 5 secondi rosso
        m_greenDuration(6.0f) // 6 secondi verde
    {
        m_shape.setRadius(12.f);
        m_shape.setOrigin(sf::Vector2f(12.f, 12.f));
        m_shape.setPosition(position);
        m_shape.setFillColor(sf::Color::Red);

        // Un piccolo bordo per renderlo più visibile
        m_shape.setOutlineThickness(2.f);
        m_shape.setOutlineColor(sf::Color::Black);
    }

    void TrafficLight::update(float deltaTime) {
        m_timer += deltaTime;

        // Logica del timer per scambiare i colori
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
        window.draw(m_shape);
    }
}