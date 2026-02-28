#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include "mevent.hpp"
#include "MGraphics.hpp"

using namespace std;
using namespace sf;

struct animDrive {
    enum class EaseType {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,
        Custom
    };

    animDrive(float start, float end, float dur, EaseType ease = EaseType::Linear,
        bool revEase = false, bool looped = false)
        : RelstartPos(start), RelendPos(end), duration(dur), loop(looped) {
        RelcurPos = start;
        this->ease = ease;
        this->revEase = revEase;
    }

    EaseType ease = EaseType::Linear;

    float applyEase(float t) {
        EaseType ease = this->ease;
        if (ease == EaseType::EaseIn && revEase) ease = EaseType::EaseOut;
        else if (ease == EaseType::EaseOut && revEase) ease = EaseType::EaseIn;
        switch (ease) {
        case EaseType::Linear:   return t;
        case EaseType::EaseIn:   return t * t;
        case EaseType::EaseOut:  return t * (2 - t);
        case EaseType::EaseInOut:
            return t < 0.5f ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
        case EaseType::Custom:   return customEase(t);
        default:                 return t;
        }
    }

    animDrive() = default;
    float RelstartPos = 0;
    float RelendPos = 0;
    float RelcurPos = 0;
    float duration = 0;
    float elapsed = 0;
    bool finished = false;
    bool backFinished = true;
    bool loop = false;
    bool revEase = false;
    float(*customEase)(float) = nullptr;

    void stepForward(float delta) {
        if (finished) return;
        backFinished = false;
        elapsed += delta;
        if (duration <= 0) { RelcurPos = RelendPos; finished = true; return; }
        else if (elapsed >= duration) { RelcurPos = RelendPos; finished = true; return; }
        RelcurPos = RelstartPos + (RelendPos - RelstartPos) * applyEase(elapsed / duration);
    }

    void stepBackward(float delta) {
        if (backFinished) return;
        finished = false;
        elapsed -= delta;
        if (duration <= 0) { elapsed = 0.0f; RelcurPos = RelstartPos; backFinished = true; return; }
        else if (elapsed <= 0.0f) { elapsed = 0.0f; RelcurPos = RelstartPos; backFinished = true; return; }
        bool wasRev = revEase;
        revEase = !revEase;
        RelcurPos = RelstartPos + (RelendPos - RelstartPos) * applyEase(elapsed / duration);
        revEase = wasRev;
    }

    void customStep(float delta, std::function<float(float)> func) {
        float moveDelta = func(delta);
        RelcurPos += moveDelta;
        if (RelcurPos >= RelendPos) { RelcurPos = RelendPos; finished = true; backFinished = false; }
        else if (RelcurPos <= RelstartPos) { RelcurPos = RelstartPos; finished = false; backFinished = true; }
        else { finished = false; backFinished = false; }
    }
};

struct wire {
    int self = 0;
    int diff = 0;
};

class Ent_Train : public mg::DrawBase
{
protected:
    class trainInterface {
    public:
        void addSwitch(Vector2f size, Vector2f pos, const Font& font, string text,
            sf::RenderWindow* window, std::function<void()> callback,
            Texture& boxTexture, Texture& checkTexture, Texture& negCheckTexture) {
            switches.emplace_back(size, pos, font, text, window, callback,
                boxTexture, checkTexture, negCheckTexture);
        }

        void addButton(Vector2f size, Vector2f pos, Color color, const Font& font, string text,
            sf::RenderWindow* window, std::function<void()> callback) {
            buttons.emplace_back(size, pos, color, font, text, window, callback);
        }

        void addUISprite(const Sprite& spr) {
            uiSprites.push_back(spr);
        }

        void checkEvents(const Event& ev) {
            for (auto& box : buttons)  box.checkPress(ev);
            for (auto& sw : switches) sw.checkPress(ev);
        }

        void draw(sf::RenderWindow* window) {
            for (auto& spr : uiSprites) window->draw(spr);
            for (auto& box : buttons)  box.draw();
            for (auto& sw : switches) sw.draw();
        }
        vector<mg::Button>  buttons;
        vector<mg::TickBox> switches;
        vector<Sprite>      uiSprites;
    private:

    };
    struct fSprite {
        Sprite sprite;
        Vector2f relPos;
        animDrive anim;
        bool drawn;
        fSprite(const Sprite& spr) : sprite(spr) {
            relPos = spr.getPosition();
        }

        void updateAnim(float dt, bool forward = true) {
            if (forward) anim.stepForward(dt);
            else         anim.stepBackward(dt);
            relPos.x = anim.RelcurPos;
        }

        void draw() {}
    };
    int mmlength = 20000;
    array<wire, 32> wireBus;
    float TrainLine = 0.f;
    float BreakLine = 0.f;
    vector<fSprite> sprites;
    Vector2f scale;
    Vector2f pos{};
    int entityId;
    RenderWindow* window;
    trainInterface ui;

    void addSprite(const Sprite& spr) {
        sprites.emplace_back(spr);
    }
    void addSprite(const Sprite& spr, animDrive anim) {
        sprites.emplace_back(spr);
        sprites.back().anim = anim;
    }

    void drawBase() override {
        for (auto& s : sprites) {
            window->draw(s.sprite);
        }
    }

    virtual vector<MEvent> work(vector<MEvent>* input, float dt) {
        return {};
    }

public:
    bool isHead = false;
    vector<MEvent> events;

    Ent_Train(int id, sf::RenderWindow* window) {
        entityId = id;
        this->window = window;
    }

    Vector2f getPos() { return pos; }
    int      getId() { return entityId; }
    int      getSpriteCount() { return (int)sprites.size(); }

    Sprite& getSprite(int id) { return sprites[id].sprite; }

    array<wire, 32> sendWires() { return wireBus; }
    void            setWires(array<wire, 32> wires) { wireBus = wires; }

    void setScale(float coef = 1) {
        scale.x = scale.y = mmlength * coef / 10000.0f;
        for (auto& s : sprites) s.sprite.setScale(scale);
    }

    void updatePos() {
        for (auto& s : sprites) s.sprite.setPosition(s.relPos + pos);
    }

    void setPos(Vector2f newPos) {
        pos = newPos;
        updatePos();
    }

    vector<MEvent> getSentMev() { return events; }

    void sim(vector<MEvent>* input, float dt) {
        vector<MEvent> res = work(input, dt);
        events.assign(res.begin(), res.end());
    }

    void checkPreses(const Event& ev) {
        if (!isHead) return;
        ui.checkEvents(ev);
    }
    void drawUI() {
        if (!isHead) return;
        ui.draw(window);
    }
};