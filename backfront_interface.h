#include <sf/Graphics.hpp>

#pragma once

int playing(sf::RenderWindow& window, sf::Sprite bg, sf::String questionTitle,
			std::array<std::string, 4> choicesR, unsigned int correctAnswer, bool player);