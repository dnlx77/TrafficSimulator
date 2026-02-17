#pragma once
#include <SFML/Graphics.hpp>

namespace World { // Opzionale: usare namespace aiuta a evitare conflitti

    class Road {
    public:
        Road(sf::Vector2f start, sf::Vector2f end, float width, int lanes);

        void draw(sf::RenderWindow &window);

        // Getters utili per dopo
        sf::Vector2f getStart() const { return m_start; }
        sf::Vector2f getEnd() const { return m_end; }

    private:
        sf::Vector2f m_start;
        sf::Vector2f m_end;
        float m_width;
        int m_lanes; // Numero di corsie (per disegnare le strisce)

        sf::ConvexShape m_shape;       // L'asfalto
        sf::VertexArray m_markings;    // Le strisce bianche
    };

}