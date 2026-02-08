//////////////////////////////////////////////
//	all things related to graphical output	//
//////////////////////////////////////////////
//////////////////////////////////////////////





#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include "rounded_rectangle.h"

using namespace std;
using namespace sf;

namespace mg {
    sf::Texture deEmpty;

    void def() {
        const unsigned char Data[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0x15, 0xc4, 0x89, 0x00, 0x00, 0x00,
    0x0d, 0x49, 0x44, 0x41, 0x54, 0x08, 0x1d, 0x63, 0xf8, 0xff, 0xff, 0x3f,
    0x03, 0x00, 0x08, 0xfc, 0x02, 0xfe, 0xe6, 0x0c, 0xff, 0xab, 0x00, 0x00,
    0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60
        };
        deEmpty.loadFromMemory(Data, sizeof(Data));
    }
    void nothing() {
        //cout << "works!\n";
    }
    class DrawBase {
    public:
        void draw() { drawBase(); for (auto obj : Linked) { obj->draw(); } }
        void setPosition(Vector2f pos) { Vector2f delta = setPositionBase(pos); for (auto obj : Linked) obj->setPosition(obj->getPosition() + delta); }

        virtual Vector2f getPosition() { return Vector2f(0, 0); }

        void link(DrawBase* obj) {
            Linked.push_back(obj);
        }
    protected:
        virtual void drawBase() {}
        virtual Vector2f setPositionBase(Vector2f pos) { return Vector2f(0, 0); }
        vector<DrawBase*> Linked;
    };

    class SubWindow : public DrawBase {
    public:
        SubWindow(RenderWindow& window, Vector2f pos, Vector2f size, float radius, Color color = Color::White) {
            this->window = &window;
            body.setSize(size);
            body.setPosition(pos);
            body.setFillColor(color);
            body.setRadius(radius);
        }
        void addObject(DrawBase* object) {
            object->setPosition(object->getPosition() + body.getPosition());
            Objects.push_back(object);
        }

        void show() { isOpen = true; }
        void hide() { isOpen = false; }
    protected:
        Vector2f setPositionBase(Vector2f pos) override {

            for (int i = 0; i < Objects.size(); i++)
            {
                Objects[i]->setPosition(Objects[i]->getPosition() + (pos - body.getPosition()));
            }
            body.setPosition(pos);
            return pos - body.getPosition();
        }
        void drawBase() override {
            if (isOpen) {
                window->draw(body);
                for (int i = 0; i < Objects.size(); i++)
                {
                    Objects[i]->draw();
                }
            }
        }
        bool isOpen = false;
        RoundedRectangle body;
        vector<DrawBase*> Objects;
        RenderWindow* window;
    };

    class Box : public DrawBase {
    public:
        Box(Vector2f size, Vector2f pos, Color color, Font font, string text, RenderWindow* window) : icon(deEmpty), line(font) {
            this->window = window;
            body.setSize(size);
            body.setFillColor(color);
            body.setPosition(pos);

            line.setString(text);
            line.setCharacterSize(15);

            centerText(pos, size);
        }
        Box(Vector2f size, Vector2f pos, Color color, Font font, string text, RenderWindow* window, Texture textur, Vector2f spriteOffset) : icon(textur), line(font) {
            this->window = window;
            body.setSize(size);
            body.setFillColor(color);
            body.setPosition(pos);

            line.setString(text);
            line.setCharacterSize(15);

            centerText(pos, size);

            icon.setPosition(body.getPosition() + spriteOffset);
        }
        void scale(Vector2f scale) {
            body.setScale(scale);
            line.setScale(scale);
        }
        Vector2f getPosition() override {
            return body.getPosition();
        }

        void offsetText(Vector2f offset) {
            line.setPosition(line.getPosition() + offset);
        }



    protected:
        Vector2f setPositionBase(Vector2f pos) override {
            Vector2f delta = pos - body.getPosition();
            body.setPosition(pos);
            line.setPosition(line.getPosition() + delta);
            icon.setPosition(icon.getPosition() + delta);
            return delta;
        }
        void drawBase() override {
            window->draw(body);
            window->draw(line);
            //window->draw(icon);
        }
        void centerText(Vector2f pos, Vector2f size) {
            FloatRect textBounds = line.getLocalBounds();
            line.setOrigin(Vector2f(textBounds.position.x + textBounds.size.x / 2.0f,
                textBounds.position.y + textBounds.size.y / 2.0f));
            line.setPosition(Vector2f(pos.x + size.x / 2.0f, pos.y + size.y / 2.0f));
        }

        RenderWindow* window;
        RectangleShape body;
        Text line;
        Sprite icon;
    };

    // can be any sf::Drawable
    template<typename T>
    class AnyShape : public DrawBase {
    public:
        AnyShape(RenderWindow* window, T shape) : shape(
            
            
            
            
            move(shape)) {
            this->window = window;
        }

        Vector2f getPosition() override {
            return shape.getPosition();
        }

        Vector2f setPositionBase(Vector2f pos) override {
            shape.setPosition(pos);
            return pos - shape.getPosition();
        }

    protected:
        void drawBase() override {
            window->draw(shape);
        }

        RenderWindow* window;
        T shape;
    };

    class Button : public Box {
    public:
        Button(Vector2f size, Vector2f pos, Color color, Font font, string text, RenderWindow* window, void(*callback)(void))
            : Box(size, pos, color, font, text, window) {
            work = callback;
        }

        Button(Vector2f size, Vector2f pos, Color color, Font font, string text, RenderWindow* window, Texture spr, Vector2f spriteOffset, void(*callback)(void))
            : Box(size, pos, color, font, text, window, spr, spriteOffset) {
            work = callback;
        }
        void force() {
            (*work)();
        }
        void checkPress(Event& press) {
            if (const auto* mouseEvent = press.getIf<Event::MouseButtonPressed>()) {
                Vector2i mousePos = mouseEvent->position;
                FloatRect bounds = body.getGlobalBounds();

                if (bounds.contains(Vector2f(mousePos.x, mousePos.y))) {
                    onPress();
                }
            }
        }
    protected:
        virtual void onPress() {
            (*work)();
        }
        void(*work)(void);
    };

   /*class Slider : public Button {
    public:
        Slider(Vector2f size, Vector2f pos, Color color, Font font, string text, RenderWindow* window, void(*callback)(void))
            : Button(size, pos, Color::Transparent, font, text, window, callback) {
            handle.setRadius(size.y / 2.0f);
            handle.setFillColor(Color::Red);
            handle.setPosition(body.getPosition());
        }
    protected:
        void onPress() override {
            Vector2i mousePos = Mouse::getPosition(*window);

            (*work)();
            Vector2f setPositionBase(Vector2f pos) override {
                Vector2f delta = pos - body.getPosition();
                handle.setPosition(handle.getPosition() + delta);
                return delta;
            }
            void drawBase() override {
                window->draw(body);
                window->draw(handle);
            }
            CircleShape handle;
            float percent = 0.0f;
        }
    };*/

    class TickBox : public Button {
    public:
        TickBox(Vector2f size, Vector2f pos, Font font, string text, RenderWindow* window, void(*callback)(void), Texture& boxTexture, Texture& checkTexture)
            : Button(size, pos, Color::Transparent, font, text, window, callback), box(boxTexture), check(checkTexture) {
            box.setPosition(body.getPosition());
            check.setPosition(body.getPosition());
            offsetText(Vector2f(box.getLocalBounds().size.y + 1, 0));
        }

        bool isTicked() {
            return ticked;
        }
        void tick() {
            ticked = !ticked;
        }
        void forceTrue() {
            if (!ticked) {
                (*work)();
            }
            ticked = true;
        }
        void forceFalse() {
            ticked = false;
        }
    protected:
        void onPress() override {
            tick();
            (*work)();
        }

        Vector2f setPositionBase(Vector2f pos) override {
            Vector2f delta = pos - body.getPosition();
            box.setPosition(box.getPosition() + delta);
            check.setPosition(check.getPosition() + delta);
            return delta;
        }
        void drawBase() override {
            window->draw(box);
            if (ticked) {
                window->draw(check);
            }
        }
        Sprite box;
        Sprite check;
        bool ticked = false;
    };
    
}
