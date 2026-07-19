#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace World {
    class Road; // Forward declaration (diciamo al compilatore che Road esiste)

    class Node {
    public:
        Node(sf::Vector2f position);

        // Aggiunge una strada che parte da questo nodo
        void addOutgoingRoad(Road *road);

        // Restituisce le opzioni possibili per un'auto
        const std::vector<Road *> &getOutgoingRoads() const;

        sf::Vector2f getPosition() const;

    private:
        sf::Vector2f m_position;
        std::vector<Road *> m_outgoingRoads;
    };
}