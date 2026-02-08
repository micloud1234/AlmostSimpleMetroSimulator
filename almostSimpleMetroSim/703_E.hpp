using namespace std;
using namespace sf;

vector<MEvent> simE(Train::simData* data) {
	for (MEvent m : data->events) {
		if (m.sender == "window" && m.type == "KeyPressed") {
			if (m.key == Keyboard::Key::W) {
				data->pos->x += 5;
			}
		}
		data->events.erase(data->events.begin());
	}
}