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
struct animDrive {
	animDrive(float start, float end, float dur, bool looped = false) : RelstartPos(start), startPos(start), RelendPos(end),endPos(end), duration(dur), loop(looped) {
		RelcurPos = start;
	}
	animDrive() = default;
	float startPos = 0;
	float endPos = 0;
	float RelstartPos = 0;
	float RelendPos = 0;
	float RelcurPos = 0;
	float duration = 0;
	float relDuration = 0;
	float elapsed = 0;
	bool finished = false;
	bool loop = false;
	void update(float delta) {
		if (finished) return;
		elapsed += delta;
		if (elapsed >= duration) {
			if (loop) {
				elapsed = 0;
				RelcurPos = RelstartPos;
			}
			else {
				RelcurPos = RelendPos;
				finished = true;
			}
		}
		else {
			RelcurPos = RelstartPos + (RelendPos - RelstartPos) * (elapsed / duration);
		}
	}
	void reverse() {
		RelstartPos = endPos;
		RelendPos = startPos;
		elapsed = duration-elapsed;
		finished = false;
		swap(startPos, endPos);
	}
};
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
		animDrive anim;
		fSprite(const Sprite& spr) : sprite(spr) {
			relPos = spr.getPosition();
		}
		void updateAnim(float dt) {
			anim.update(dt);
			relPos.x = anim.RelcurPos;
		}
		bool animRunning() const { return !anim.finished; }
	};

	int mmlength = 19167;
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
	void addSprite(const Sprite& spr, animDrive anim) {
		sprites.emplace_back(spr);
		sprites.back().anim = anim;
	}

public:
	Ent_Train(int id) {
		entityId = id;
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
	void setPos(Vector2f newPos) {
		pos = newPos;
		updatePos();
	}
	Sprite& getSprite(int id) {
		return sprites[id].sprite;
	}
	int getSpriteCount() {
		return sprites.size();
	}
	int getId() {
		return entityId;
	}

	vector<MEvent> getSentMev() {
		return events;
	}

	vector<MEvent> events;
protected:
	virtual vector<MEvent> work(vector<MEvent>* input, float dt) {
		vector<MEvent> res;
		return res;
	}
public:
	void sim(vector<MEvent>* input, float dt) {
		vector<MEvent> res = work(input, dt);
		events.clear();
		for (MEvent m : res) {
			events.push_back(m);
		}
	}
};
 
