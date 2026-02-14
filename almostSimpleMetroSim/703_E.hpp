class Ent_Train_E : public Ent_Train
{
protected:
	Texture tex;
	Texture doorLTex, doorRTex;
	bool doorsOpen = false;
	bool prevDoorsOpen = false;
public:
    Ent_Train_E(int id) : Ent_Train(id), doorLTex("textures\\wagons\\doorsLL.png"), doorRTex("textures\\wagons\\doorsLR.png"), tex("textures\\wagons\\body.png") {
        addSprite(Sprite(doorLTex), animDrive(0, -45, 0.5));
		addSprite(Sprite(doorRTex), animDrive(0, 45, 0.5));
        addSprite(Sprite(tex));
    }
    vector<MEvent> work(vector<MEvent>* input, float dt) override {

        if (input) {
            for (const MEvent& m : *input) {
                if (m.sender == "window" && m.type == "KeyPressed") {
                    if (m.key == Keyboard::Key::W) {
                        pos.x -= 5;
                    }
                    if (m.key == Keyboard::Key::A) {
						doorsOpen = !doorsOpen;
                    }
                }
            }
        }
        if (doorsOpen != prevDoorsOpen) {
            sprites[0].anim.reverse();
            sprites[1].anim.reverse();
            prevDoorsOpen = doorsOpen;
        }
        if (sprites[0].animRunning() || sprites[1].animRunning()) {
            sprites[0].updateAnim(dt);
            sprites[1].updateAnim(dt);
        }
        updatePos();
        vector<MEvent> res{};
        return res;
    }
};