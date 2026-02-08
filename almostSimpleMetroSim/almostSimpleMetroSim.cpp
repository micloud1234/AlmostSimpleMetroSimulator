#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <thread>
#include <memory>
#include <mutex>
#include <atomic>
#include <chrono>
#include <optional>
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

void draw(vector<Train>& consist, const vector<sf::Texture>& tunnels, MEventBus& bus, atomic<bool>& running) {
    vector<sf::Sprite*> wagonSprites;
    for (size_t i = 0; i < consist.size(); i++)
    {
        wagonSprites.push_back(&consist[i].getSprite(0));
    }

    for (auto& train : consist)
    {
        train.setScale();
    }

    for (int i = 0; i < wagonSprites.size(); i++)
    {
        cout << wagonSprites.size() << " " << i;

        float posy = (window.getSize().y / 2 - wagonSprites[i]->getGlobalBounds().size.y / 2);
        float posx = (float)i * wagonSprites[i]->getGlobalBounds().size.x - 130 * i;
        Vector2f pos(posx, posy);
        wagonSprites[i]->setPosition(pos);
    }
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

        window.clear();

        for (int i = 0; i < wagonSprites.size(); i++)
        {
            window.draw(*wagonSprites[i]);
        }

        window.display();
    }
}
//struct MEvent {
//    string type;
//    string sender;
//    void* data;
//};
struct consist {
    vector<Train> carriages;
    void updatePreassure() {

    }
    void updateWires() {
        array<wire, 32> total;
        for (Train T : carriages) {
            for (int i = 0; i < 32; i++) {
                total[i].diff += T.sendWires()[i].self;
            }
        }
        wire transf;
        array<wire, 32> set;
        for (Train T : carriages) {
            for (int i = 0; i < 32; i++) {
                transf.diff = total[i].diff - T.sendWires()[i].self;
                transf.self = T.sendWires()[i].self;
                set[i] = transf;
            }
            T.setWires(set);
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
        for (Train& T : consist.carriages) {
            for (MEvent m : T.getSentMev()) {
                inputEvents.push_back(m);
            }
        }
        consist.updatePreassure();
        consist.updateWires();

        for (Train& T : consist.carriages) {
            T.sim(&inputEvents);
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
    Texture tex("textures\\wagons\\E_kvr.png");
    Sprite spr(tex);
    vector<Sprite> sprs;
    sprs.push_back(spr);
    consist consist;
    consist.carriages.push_back(Train(&sprs, 10));
    consist.carriages.push_back(Train(&sprs, 11));

    vector<Texture> tunnels;
    MEventBus bus;
    atomic<bool> running{ true };
    thread simulate([&]() { simulator(consist, bus, running); });

    draw(consist.carriages, tunnels, bus, running);

    running.store(false);
    if (simulate.joinable()) {
        simulate.join();
    }

}


//6 zadac na kodvars