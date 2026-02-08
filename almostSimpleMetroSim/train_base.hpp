#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
struct MEvent {
	string type;
	string sender;
	void* data = nullptr;

	sf::Keyboard::Key key = sf::Keyboard::Key::Unknown;
	bool alt = false;
	bool control = false;
	bool shift = false;
	bool system = false;
};

using namespace std;
using namespace sf;
struct wire {
	int self = 0;
	int diff = 0;
};
class Train
{
protected:
	Texture empty;
	struct fSprite {
		fSprite(Sprite* spr) : sprite(*spr) {
			relPos = spr->getPosition();
		}
		Sprite sprite;
		Vector2f relPos;
	};

	int mmlength = 19166;
	array<wire, 32> wireBus;
	float TrainLine;
	float BreakLine;
	vector<fSprite> sprites;
	Vector2f scale;
	Vector2f pos;
	int entityId;
public:
	struct simData {
		int* mmlength;
		array<wire, 32>* wireBus;
		float* TrainLine;
		float* BreakLine;
		vector<fSprite>* sprites;
		Vector2f* scale;
		Vector2f* pos;
		int* entityId;
		vector<MEvent>& events;
	};
	Train(vector<Sprite>* sprites, int id) {
		entityId = id;
		for (Sprite sprite : *sprites) {
			this->sprites.push_back(fSprite(&sprite));

		}
	}
	array<wire, 32> sendWires() {
		return wireBus;
	}
	void setWires(array<wire, 32> wires) {
		wireBus = wires;
	}
	void setScale(float coef = 1) {
		scale.x = (mmlength * coef / 10000.0f);
		scale.y = (mmlength * coef / 10000.0f);
		for (int i = 0; i < sprites.size(); i++)
		{
			sprites[i].sprite.setScale(scale);
		}
	}
	void updatePos() {
		for (auto sprite : sprites) {
			sprite.sprite.setPosition(sprite.relPos + pos);
		}
	}
	Sprite getSprite(int id) {
		return sprites[id].sprite;
	}
	int getId() {
		return entityId;
	}

	vector<MEvent> getSentMev() {
		return events;
	}

	vector<MEvent> events;
protected:
	vector<MEvent>(*work)(simData*);
public:
	void sim(vector<MEvent>* input) {
		simData data{ &mmlength,&wireBus, &TrainLine, &BreakLine, &sprites, &scale, &pos, &entityId, *input };
		vector<MEvent> res = work(&data);
		for (MEvent m : res) {
			events.push_back(m);
		}
	}
};
 
