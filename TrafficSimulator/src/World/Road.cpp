#include "../../include/World/Road.h"
#include "../../include/World/Node.h"
#include <cmath>

namespace World {
    Road::Road(Node *startNode, Node *endNode, const std::vector<sf::Vector2f> &path, float width)
        : m_startNode(startNode),
        m_endNode(endNode),
        m_width(width),
        m_waypoints(path),
        m_length(0.f)
    {
        // 1. Diciamo al nodo di partenza che questa strada parte da lui!
        if (m_startNode) {
            m_startNode->addOutgoingRoad(this);
        }

        // 2. Calcoliamo la lunghezza totale e costruiamo la grafica (i segmenti grigi)
        for (size_t i = 0; i < m_waypoints.size() - 1; ++i) {
            sf::Vector2f start = m_waypoints[i];
            sf::Vector2f end = m_waypoints[i + 1];

            sf::Vector2f dir = end - start;
            float segmentLen = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            m_length += segmentLen;

            // --- Calcolo dei 4 angoli per disegnare l'asfalto ---
            if (segmentLen > 0.001f) {
                sf::Vector2f unitDir = dir / segmentLen;
                sf::Vector2f unitNormal(-unitDir.y, unitDir.x);
                sf::Vector2f offset = unitNormal * (width / 2.f);

                sf::ConvexShape shape(4);
                shape.setPoint(0, start - offset);
                shape.setPoint(1, end - offset);
                shape.setPoint(2, end + offset);
                shape.setPoint(3, start + offset);
                shape.setFillColor(sf::Color(50, 50, 50));

                m_segments.push_back(shape);
            }
        }
    }

    void Road::draw(sf::RenderWindow &window) {
        // Disegniamo tutti i segmenti che compongono la strada
        for (const auto &segment : m_segments) {
            window.draw(segment);
        }
    }
}