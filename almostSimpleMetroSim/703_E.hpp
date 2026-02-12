class Ent_Train_E : public Ent_Train
{
protected:
	Texture tex;
public:
    Ent_Train_E(int id) : Ent_Train(id), tex("textures\\wagons\\E_kvr.png") {
        addSprite(Sprite(tex));
    }
    vector<MEvent> work(vector<MEvent>* input) override {

        if (input) {
            for (const MEvent& m : *input) {

                if (m.sender == "window" && m.type == "KeyPressed") {

                    if (m.key == Keyboard::Key::W) {
                        pos.x -= 5;
                        updatePos();
                    }
                }
            }
        }

        vector<MEvent> res{};
        return res;
    }
};
