#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace World {
    class Node; // Forward declaration

    class Road {
    public:
        // Ora la strada collega due Nodi e ha una sua geometria (path)
        Road(Node *startNode, Node *endNode, const std::vector<sf::Vector2f> &path, float width);

        void draw(sf::RenderWindow &window);

        // Getters fondamentali per l'Intelligenza Artificiale
        Node *getStartNode() const { return m_startNode; }
        Node *getEndNode() const { return m_endNode; }
        float getLength() const { return m_length; }
        const std::vector<sf::Vector2f> &getWaypoints() const { return m_waypoints; }

    private:
        Node *m_startNode;
        Node *m_endNode;
        float m_width;
        float m_length;

        std::vector<sf::Vector2f> m_waypoints;

        // Geometria per il rendering visivo
        std::vector<sf::ConvexShape> m_segments;
    };
}