#include "../../include/World/Node.h"

namespace World {
    Node::Node(sf::Vector2f position) : m_position(position) {}

    void Node::addOutgoingRoad(Road *road) {
        m_outgoingRoads.push_back(road);
    }

    const std::vector<Road *> &Node::getOutgoingRoads() const {
        return m_outgoingRoads;
    }

    sf::Vector2f Node::getPosition() const {
        return m_position;
    }
}