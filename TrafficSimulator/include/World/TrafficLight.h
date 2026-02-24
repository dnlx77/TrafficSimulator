#pragma once
#include <SFML/Graphics.hpp>

namespace World {
    class TrafficLight {
    public:
        enum class State { Red, Green };

        // position: coordinate (x,y) per disegnarlo sullo schermo
        // roadPosition: a quanti pixel dall'inizio della strada si trova (es. 400.f)
        TrafficLight(sf::Vector2f position, float roadPosition);

        void update(float deltaTime);
        void draw(sf::RenderWindow &window);

        State getState() const { return m_state; }
        float getRoadPosition() const { return m_roadPosition; }

    private:
        State m_state;
        float m_roadPosition;
        float m_timer;
        float m_redDuration;
        float m_greenDuration;

        sf::CircleShape m_shape;
    };
}