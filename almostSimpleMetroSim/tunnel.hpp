#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
using namespace std;
using namespace sf;
class Tunnel
{
public:
	Tunnel(string texture) {
		setTexture(texture);
	}
	Texture TunnelTex;
	void setTexture(string texture) {
		TunnelTex.loadFromFile(texture);
	}
private:

};