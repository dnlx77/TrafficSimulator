#pragma once
#include <SFML/Graphics.hpp>

namespace World {
    class Road; // Forward declaration

    class TrafficLight {
    public:
        enum class State { Red, Green };

        // Ora passiamo la strada di appartenenza e lo stato iniziale!
        TrafficLight(Road *targetRoad, sf::Vector2f position, float roadPosition, State initialState);

        void update(float deltaTime);
        void draw(sf::RenderWindow &window);

        State getState() const { return m_state; }
        float getRoadPosition() const { return m_roadPosition; }
        Road *getTargetRoad() const { return m_targetRoad; } // Nuovo getter

    private:
        Road *m_targetRoad;
        State m_state;
        float m_roadPosition;
        float m_timer;
        float m_redDuration;
        float m_greenDuration;

        sf::CircleShape m_shape;
        sf::RectangleShape m_box;
    };
}