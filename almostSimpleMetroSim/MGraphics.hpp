//////////////////////////////////////////////
//	all things related to graphical output	//
//////////////////////////////////////////////
//////////////////////////////////////////////

#pragma once



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
    inline sf::Texture deEmpty;
    inline void nothing() {
        //cout << "works!\n";
    }
    class DrawBase {
    public:
        void draw() { drawBase(); for (auto obj : Linked) { obj->draw(); }
        }
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
        Box(Vector2f size, Vector2f pos, Color color, const Font& font, string text, RenderWindow* window) : icon(deEmpty), line(font) {
            this->window = window;
            body.setSize(size);
            body.setFillColor(color);
            body.setPosition(pos);

            line.setString(text);
            line.setCharacterSize(15);

            centerText(pos, size);
        }
        Box(Vector2f size, Vector2f pos, Color color, const Font& font, string text, RenderWindow* window, const Texture& textur, Vector2f spriteOffset) : icon(textur), line(font) {
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

    class Button : public Box {
    public:
        Button(Vector2f size, Vector2f pos, Color color, const Font& font, string text,
            RenderWindow* window, std::function<void()> callback)
            : Box(size, pos, color, font, text, window), work(callback) {
        }

        Button(Vector2f size, Vector2f pos, Color color, const Font& font, string text,
            RenderWindow* window, const Texture& spr, Vector2f spriteOffset, std::function<void()> callback)
            : Box(size, pos, color, font, text, window, spr, spriteOffset), work(callback) {
        }

        void force() { work(); }

        void checkPress(const Event& press) {
            if (const auto* mouseEvent = press.getIf<Event::MouseButtonPressed>()) {
                Vector2i mousePos = mouseEvent->position;
                FloatRect bounds = body.getGlobalBounds();
                if (bounds.contains(Vector2f(mousePos.x, mousePos.y))) {
                    onPress();
                }
            }
        }

    protected:
        virtual void onPress() { work(); }
        std::function<void()> work;
    };

    class TickBox : public Button {
    public:
        TickBox(Vector2f size, Vector2f pos, const Font& font, string text,
            RenderWindow* window, std::function<void()> callback,
            Texture& boxTexture, Texture& checkTexture)
            : Button(size, pos, Color::Transparent, font, text, window, callback),
            box(boxTexture), check(checkTexture), NegativeCheck(checkTexture) {
            box.setPosition(body.getPosition());
            check.setPosition(body.getPosition());
            offsetText(Vector2f(box.getLocalBounds().size.y + 1, 0));
            negCheck = false;
        }

        TickBox(Vector2f size, Vector2f pos, const Font& font, string text,
            RenderWindow* window, std::function<void()> callback,
            Texture& boxTexture, Texture& checkTexture, Texture& negCheckTexture)
            : Button(size, pos, Color::Transparent, font, text, window, callback),
            box(boxTexture), check(checkTexture), NegativeCheck(negCheckTexture) {
            box.setPosition(body.getPosition());
            check.setPosition(body.getPosition());
            NegativeCheck.setPosition(body.getPosition());
            offsetText(Vector2f(box.getLocalBounds().size.y + 1, 0));
            negCheck = true;
        }

        bool isTicked() { return ticked; }

        void tick() { ticked = !ticked; }

        void forceTrue() {
            if (!ticked) work();
            ticked = true;
        }

        void forceFalse() { ticked = false; }

    protected:
        void onPress() override {
            tick();
            work();
        }

        Vector2f setPositionBase(Vector2f pos) override {
            Vector2f delta = pos - body.getPosition();
            box.setPosition(box.getPosition() + delta);
            check.setPosition(check.getPosition() + delta);
            NegativeCheck.setPosition(NegativeCheck.getPosition() + delta);
            return delta;
        }

        void drawBase() override {
            window->draw(box);
            if (ticked) {
                window->draw(check);
            }
            else if (negCheck) {
                window->draw(NegativeCheck);
            }
        }

        Sprite box;
        Sprite check;
        Sprite NegativeCheck;
        bool negCheck = false;
        bool ticked = false;
    };

}
