#pragma once
#include <string>
#include <SFML/Window/Keyboard.hpp>

struct MEvent {
	std::string type;  // Use std::string instead of string
	std::string sender;
	void* data = nullptr;

	sf::Keyboard::Key key = sf::Keyboard::Key::Unknown;
	bool alt = false;
	bool control = false;
	bool shift = false;
	bool system = false;
};