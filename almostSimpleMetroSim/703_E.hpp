class Ent_Train_E : public Ent_Train
{//208 519
 //306 615
protected:
    Texture tex, salon;
    Texture doorLLTex, doorLRTex, doorRLTex, doorRRTex;
    Texture panelTex, switch1, switch2, switch3, switch1on, switch2on, switch3on;
    Texture VUDLight;
    bool LdoorsOpen = false;
    bool RdoorsOpen = false;
    bool doorsClosed = true;
    Font font;
    void openLdoor() {
        LdoorsOpen = 1;
    }
    void openRdoor() {
        RdoorsOpen = 1;
    }
    void toggleVUD() {
        doorsClosed = !doorsClosed;
    }
    void updeteUiSprites() {
		ui.switches[0].check.setPosition(ui.uiSprites[0].getPosition());
        ui.switches[0].check.setScale(ui.uiSprites[0].getScale());
        ui.switches[0].NegativeCheck.setPosition(ui.uiSprites[0].getPosition());
        ui.switches[0].NegativeCheck.setScale(ui.uiSprites[0].getScale());

    }
public:
    Ent_Train_E(int id, RenderWindow* window) : Ent_Train(id, window), doorLLTex("textures\\wagons\\E\\doorsLL.png"), doorLRTex("textures\\wagons\\E\\doorsLR.png"),
        doorRLTex("textures\\wagons\\E\\doorsRL.png"), doorRRTex("textures\\wagons\\E\\doorsRR.png"), tex("textures\\wagons\\E\\body.png"),
        salon("textures\\wagons\\E\\salon.png"), panelTex("textures\\wagons\\E\\interface\\panel.png"), switch1("textures\\wagons\\E\\interface\\left_switch_off.png"),
        switch2("textures\\wagons\\E\\interface\\middle_switch_off.png"), switch3("textures\\wagons\\E\\interface\\right_switch_off.png"),
        switch1on("textures\\wagons\\E\\interface\\left_switch_on.png"), switch2on("textures\\wagons\\E\\interface\\middle_switch_on.png"),
        switch3on("textures\\wagons\\E\\interface\\right_switch_on.png"), VUDLight("textures\\wagons\\E\\interface\\VUD_light.png") {

        if (!font.openFromFile("fonts/consolas.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
        addSprite(Sprite(doorRLTex), animDrive(0, -50, 0.4, animDrive::EaseType::Linear, true));
        addSprite(Sprite(doorRRTex), animDrive(0, 50, 0.4, animDrive::EaseType::Linear, true));
        addSprite(Sprite(salon));
        addSprite(Sprite(doorLLTex), animDrive(0, -50, 0.4, animDrive::EaseType::Linear, true));
        addSprite(Sprite(doorLRTex), animDrive(0, 50, 0.4, animDrive::EaseType::Linear, true));
        addSprite(Sprite(tex));

        Sprite pTex(panelTex);
        pTex.setScale(Vector2f(0.5f, 0.5f));
        pTex.setPosition(Vector2f(760, 680));
        ui.addUISprite(pTex);

        ui.addButton(Vector2f(50, 50), Vector2f(pTex.getPosition().x + 104, pTex.getPosition().y + 260), Color::Green, font, "L dors", this->window, [this]() { openLdoor(); });
        ui.addButton(Vector2f(46, 45), Vector2f(pTex.getPosition().x + 335.5f, pTex.getPosition().y + 197.5f), Color::Green, font, "R dors", this->window, [this]() { openRdoor(); });
        ui.addSwitch(Vector2f(49.5f, 46.5f), Vector2f(pTex.getPosition().x + 317.5f, pTex.getPosition().y + 294.f), font, "VUD", this->window, [this]() { toggleVUD(); }, mg::deEmpty, switch3, switch3on);
		ui.addUISprite(Sprite(VUDLight));
        ui.uiSprites[1].setPosition(ui.uiSprites[0].getPosition());
        ui.uiSprites[1].setScale(ui.uiSprites[0].getScale());

    }
    vector<MEvent> work(vector<MEvent>* input, float dt) override {
        updeteUiSprites();
        bool lamp;
        if((!sprites[0].anim.backFinished and !sprites[1].anim.backFinished and !sprites[3].anim.backFinished and !sprites[4].anim.backFinished)) lamp = 1;
		else lamp = 0;
        if ((!sprites[0].anim.finished or !sprites[1].anim.finished or !sprites[3].anim.finished or !sprites[4].anim.finished) and
            (!sprites[0].anim.backFinished or !sprites[1].anim.backFinished or !sprites[3].anim.backFinished or !sprites[4].anim.backFinished) or lamp) {
            ui.uiSprites[1].setPosition(ui.uiSprites[0].getPosition());
        }
        else {
			ui.uiSprites[1].setPosition(Vector2f(-1000, -1000));
        }
        if (input) {
            for (const MEvent& m : *input) {
                if (m.sender == "window" && m.type == "KeyPressed") {
                    switch (m.key)
                    {
                    case Keyboard::Key::W:
                        pos.x -= 5;
                        break;
                    case Keyboard::Key::A:
                        if (doorsClosed) break;
                        LdoorsOpen = 1;
                        break;
                    case Keyboard::Key::D:
                        if (doorsClosed) break;
                        RdoorsOpen = 1;
                        break;
                    case Keyboard::Key::S:
                        pos.x += 5;
                    case Keyboard::Key::V:
                        doorsClosed = !doorsClosed;
                        LdoorsOpen = 0;
                        RdoorsOpen = 0;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        if (doorsClosed) {
            LdoorsOpen = 0;
            RdoorsOpen = 0;
        }
        if (RdoorsOpen) {
            sprites[0].updateAnim(dt, true);
            sprites[1].updateAnim(dt, true);
        }
        else {
            sprites[0].updateAnim(dt, false);
            sprites[1].updateAnim(dt, false);
        }
        if (LdoorsOpen) {
            sprites[3].updateAnim(dt, true);
            sprites[4].updateAnim(dt, true);
        }
        else {
            sprites[3].updateAnim(dt, false);
            sprites[4].updateAnim(dt, false);
        }

        updatePos();
        vector<MEvent> res{};
        return res;
    }
};