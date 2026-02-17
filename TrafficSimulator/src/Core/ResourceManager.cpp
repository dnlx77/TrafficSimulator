#include "../../include/Core/ResourceManager.h"
#include <iostream>

const sf::Texture &ResourceManager::getTexture(const std::string &filename) {
    // 1. Controlla se la texture è già stata caricata
    auto it = m_textures.find(filename);
    if (it != m_textures.end()) {
        return it->second; // Trovata! Restituisci quella esistente
    }

    // 2. Se non c'è, caricala dal disco
    sf::Texture &texture = m_textures[filename];
    // Nota: SFML 3.0 usa ancora loadFromFile, ma restituisce un booleano (o std::optional in futuro)
    if (!texture.loadFromFile(filename)) {
        std::cerr << "ERRORE: Impossibile caricare texture: " << filename << std::endl;
        // Qui potresti caricare una texture di default "rosa shocking" per evidenziare l'errore
    }

    return texture;
}

const sf::Font &ResourceManager::getFont(const std::string &filename) {
    auto it = m_fonts.find(filename);
    if (it != m_fonts.end()) {
        return it->second;
    }

    sf::Font &font = m_fonts[filename];
    if (!font.openFromFile(filename)) { // SFML 3 preferisce openFromFile per i font
        std::cerr << "ERRORE: Impossibile caricare font: " << filename << std::endl;
    }

    return font;
}