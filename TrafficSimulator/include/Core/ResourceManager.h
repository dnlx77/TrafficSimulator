#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>

class ResourceManager {
public:
    // Restituisce un riferimento alla texture. Se non esiste, la carica.
    const sf::Texture &getTexture(const std::string &filename);

    // Restituisce un riferimento al font.
    const sf::Font &getFont(const std::string &filename);

private:
    // Mappa: NomeFile -> Texture
    // Usiamo std::map per mantenere le texture in memoria finché il manager vive
    std::map<std::string, sf::Texture> m_textures;
    std::map<std::string, sf::Font> m_fonts;
};