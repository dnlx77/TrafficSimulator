#include "../../include/World/Road.h"
#include <cmath>

namespace World {

    Road::Road(sf::Vector2f start, sf::Vector2f end, float width, int lanes)
        : m_start(start), m_end(end), m_width(width), m_lanes(lanes)
    {
        // 1. Matematica Vettoriale per trovare i 4 angoli della strada
        sf::Vector2f direction = end - start;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        // Vettore unitario (direzione normalizzata)
        sf::Vector2f unitDir = direction / length;

        // Vettore normale (perpendicolare alla direzione) per dare spessore
        // Se dir è (x, y), la normale è (-y, x)
        sf::Vector2f unitNormal(-unitDir.y, unitDir.x);

        // Offset dal centro alla "banchina"
        sf::Vector2f offset = unitNormal * (width / 2.f);

        // 2. Creiamo la forma dell'asfalto
        m_shape.setPointCount(4);
        m_shape.setPoint(0, start - offset); // Alto-Sx
        m_shape.setPoint(1, end - offset);   // Alto-Dx
        m_shape.setPoint(2, end + offset);   // Basso-Dx
        m_shape.setPoint(3, start + offset); // Basso-Sx

        m_shape.setFillColor(sf::Color(60, 60, 60)); // Grigio scuro

        // 3. (Semplice) Linea centrale tratteggiata
        // Per ora facciamo una linea semplice, poi la miglioreremo
        m_markings.setPrimitiveType(sf::PrimitiveType::Lines);
        m_markings.resize(2);
        m_markings[0].position = start;
        m_markings[0].color = sf::Color(255, 255, 255, 150); // Bianco semi-trasparente
        m_markings[1].position = end;
        m_markings[1].color = sf::Color(255, 255, 255, 150);
    }

    void Road::draw(sf::RenderWindow &window) {
        window.draw(m_shape);
        window.draw(m_markings);
    }

}