#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include "mevent.hpp"

using namespace std;
using namespace sf;
struct wire {
	int self = 0;
	int diff = 0;
};
class Ent_Train
{
protected:
	struct fSprite {
		Sprite sprite;
		Vector2f relPos;

		fSprite(const Sprite& spr) : sprite(spr) {
			relPos = spr.getPosition();
		}
	};

	int mmlength = 19166;
	array<wire, 32> wireBus;
	float TrainLine = 0.f;
	float BreakLine = 0.f;
	vector<fSprite> sprites;
	Vector2f scale;
	Vector2f pos{};
	int entityId;

	void addSprite(const Sprite& spr) {
		sprites.emplace_back(spr);
	}

public:
	Ent_Train(int id) {
		entityId = id;
	}
	void setPos(Vector2f newPos) {
		pos = newPos;
		updatePos();
	}
	Vector2f getPos() {
		return pos;
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
		for (auto& sprite : sprites) {
			sprite.sprite.setPosition(sprite.relPos + pos);
		}
	}
	Sprite& getSprite(int id) {
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
	virtual vector<MEvent> work(vector<MEvent>* input) {
		vector<MEvent> res;
		return res;
	}
public:
	void sim(vector<MEvent>* input) {
		vector<MEvent> res = work(input);
		events.clear();
		for (MEvent m : res) {
			events.push_back(m);
		}
	}
};
 
