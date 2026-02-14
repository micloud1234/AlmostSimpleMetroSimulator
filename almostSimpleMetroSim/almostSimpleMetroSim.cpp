#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <thread>
#include <memory>
#include <mutex>
#include <atomic>
#include <chrono>
#include <optional>
#include <vector>
#include <string>
#include "mevent.hpp"
#include "train_base.hpp"
#include "703_E.hpp"
#include "draw.hpp"
#include "tunnel.hpp"
#include "includes.h"
 
using namespace std;
using namespace sf;
//using E = Train{ &sprs, 10 }
sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
sf::RenderWindow window(desktopMode, "ASMS");

class MEventBus {
public:
    void emit(const MEvent& event) {
        lock_guard<mutex> lock(mutex_);
        eventLog.push_back(event);
    }

    void drainTo(vector<MEvent>& out) {
        lock_guard<mutex> lock(mutex_);
        if (eventLog.empty()) {
            return;
        }
        out.insert(out.end(), eventLog.begin(), eventLog.end());
        eventLog.clear();
    }

private:
    mutex mutex_;
    vector<MEvent> eventLog;
};

void renderingThread(sf::RenderWindow* window, vector<unique_ptr<Ent_Train>>& carriages, atomic<bool>& running) {
    window->setActive(true);

    while (running.load() && window->isOpen()) {
        window->clear();

        for (int i = 0; i < carriages.size(); i++) {
            for (int j = 0; j < carriages[i]->getSpriteCount(); j++) {
                window->draw(carriages[i]->getSprite(j));
			}
        }

        window->display();
    }
}

struct consist {
    vector<unique_ptr<Ent_Train>> carriages;
    void updatePreassure() {
	}
    void updateWires() {
        array<wire, 32> total;
        for (auto& T : carriages) {
            for (int i = 0; i < 32; i++) {
                total[i].diff += T->sendWires()[i].self;
            }
        }
        wire transf;
        array<wire, 32> set;
        for (auto& T : carriages) {
            for (int i = 0; i < 32; i++) {
                transf.diff = total[i].diff - T->sendWires()[i].self;
                transf.self = T->sendWires()[i].self;
                set[i] = transf;
            }
            T->setWires(set);
        }
    }
};

void simulator(consist& consist, MEventBus& bus, atomic<bool>& running) {
    vector<MEvent> inputEvents;
    inputEvents.reserve(64);

    while (running.load()) {
        auto start = chrono::steady_clock::now();

        inputEvents.clear();
        bus.drainTo(inputEvents);
        for (auto& T : consist.carriages) {
            for (MEvent m : T->getSentMev()) {
                inputEvents.push_back(m);
            }
        }
        consist.updatePreassure();
        consist.updateWires();

        for (auto& T : consist.carriages) {
            T->sim(&inputEvents, 0.01);
        }

        const auto elapsed = chrono::steady_clock::now() - start;
        const auto frameTime = chrono::duration<double>(0.01);
        if (elapsed < frameTime) {
            this_thread::sleep_for(frameTime - elapsed);
        }
    }
}

int main()
{
    try {
        consist consist;
        consist.carriages.push_back(make_unique<Ent_Train_E>(10));
        consist.carriages.push_back(make_unique<Ent_Train_E>(11));
        for (auto& train : consist.carriages) {
            train->setScale();
        }
        for (int i = 0; i < consist.carriages.size(); i++) {
            sf::Sprite sprite = consist.carriages[i]->getSprite(0);
            float posy = (window.getSize().y / 2 - sprite.getGlobalBounds().size.y / 2);
            float posx = (float)i * sprite.getGlobalBounds().size.x - 130 * i;
            Vector2f pos(posx, posy);
            consist.carriages[i]->setPos(pos);
            consist.carriages[i]->updatePos();
        }

        MEventBus bus;
        atomic<bool> running{ true };
        window.setActive(false);
        thread renderThread(renderingThread, &window, ref(consist.carriages), ref(running));
        thread simThread([&]() { simulator(consist, bus, running); });
        while (window.isOpen())
        {
            while (const optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                {
                    running.store(false);
                    window.close();
                }
                else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                {
                    MEvent ev;
                    ev.type = "KeyPressed";
                    ev.sender = "window";
                    ev.key = keyPressed->code;
                    ev.alt = keyPressed->alt;
                    ev.control = keyPressed->control;
                    ev.shift = keyPressed->shift;
                    ev.system = keyPressed->system;
                    bus.emit(ev);
                }
            }
        }

        running.store(false);
        if (renderThread.joinable()) renderThread.join();
        if (simThread.joinable()) simThread.join();
        return 0;
    }
    catch (const std::exception& e) {
        cerr << "Error " << e.what() << endl;
        return 1;
    }
}