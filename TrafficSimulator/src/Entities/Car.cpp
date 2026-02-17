#include "../../include/Entities/Car.h"

Car::Car(float startX, float startY, const sf::Texture &texture)
    : m_sprite(texture)
{
    // 1. Otteniamo le dimensioni originali dell'immagine
    // In SFML 3.0 getLocalBounds restituisce un Rect<float>
    auto bounds = m_sprite.getLocalBounds();

    // 2. Impostiamo l'origine al centro esatto dell'immagine
    m_sprite.setOrigin(bounds.size / 2.f);

    // --- SCALATURA INTELLIGENTE ---
    // Decidiamo quanto deve essere lunga l'auto nel nostro mondo (es. 60 pixel)
    const float DESIRED_CAR_LENGTH = 60.f;

    // Calcoliamo il fattore di scala.
    // Assumiamo che l'immagine dell'auto sia orientata orizzontalmente (lunghezza = bounds.size.x).
    // Se la tua immagine è verticale, usa bounds.size.y.
    float scaleFactor = DESIRED_CAR_LENGTH / bounds.size.x;

    // Applichiamo la scala uniforme (stesso fattore per X e Y per non deformarla)
    m_sprite.setScale(sf::Vector2f(scaleFactor, scaleFactor));
    // -----------------------------

    // 3. Posizione iniziale
    m_sprite.setPosition(sf::Vector2f(startX, startY));

    // 4. Velocità
    m_velocity = sf::Vector2f(100.f, 0.f);
}

void Car::update(float deltaTime) {
    // Movimento semplice: Posizione = Velocità * Tempo
    m_sprite.move(m_velocity * deltaTime);

    // (Opzionale) Reset posizione se esce dallo schermo per testare il loop
    if (m_sprite.getPosition().x > 800.0f) {
        m_sprite.setPosition(sf::Vector2f(0.0f, m_sprite.getPosition().y));
    }
}

void Car::draw(sf::RenderWindow &window) {
    window.draw(m_sprite);
}