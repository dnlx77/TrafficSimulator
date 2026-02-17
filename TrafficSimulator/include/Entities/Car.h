#pragma once
#include <SFML/Graphics.hpp>

class Car {
public:
    // Costruttore: posizione iniziale e colore
    Car(float startX, float startY, const sf::Texture &texture);

    // Aggiorna la logica (movimento)
    void update(float deltaTime);

    // Disegna l'auto sulla finestra
    void draw(sf::RenderWindow &window);

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
    float m_speed;
};