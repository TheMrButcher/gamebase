#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    void load()
    {
		connect(imagePathBox, update);
		connect(updateButton, update);
		connect(clearButton, clear);
		connect(showBackgroundCheckBox, onShowBackgroundCheckBoxClick);
		imagePathBox << "cosmic_space\\mars.png";
		update();
    }

	void process(Input input)
	{
		using namespace gamebase::InputKey;
		if (!field.isMouseOn())
			return;
		if (input.justPressed(MouseLeft)) {
			auto pos = field.mousePos();
			pos.x = (pos.x - field.viewBox().l) / field.width();
			pos.y = (pos.y - field.viewBox().b) / field.height();
			vertices.push_back(pos);
			verticesLayer.load(vertices.size(), "polygon/Vertex.json", field.mousePos());
			polygon.setContour(vertices);
		}

		if (input.justPressed(MouseRight) && !vertices.empty()) {
			vertices.pop_back();
			verticesLayer.remove((int)vertices.size());
			polygon.setContour(vertices);
		}
	}

	void onShowBackgroundCheckBoxClick()
	{
		background.setVisible(showBackgroundCheckBox.isChecked());
	}

	void update()
	{
		polygon.setImageName(imagePathBox.text());
		background.setImageName(imagePathBox.text());
	}

	void clear()
	{
		polygon.clear();
		vertices.clear();
		verticesLayer.clear();
	}

	LayerFromDesign(void, verticesLayer);
	FromDesign(GameView, field);
	FromDesign(Texture, background);
	FromDesign(Polygon, polygon);
	FromDesign(TextBox, imagePathBox);
	FromDesign(Button, updateButton);
	FromDesign(Button, clearButton);
	FromDesign(CheckBox, showBackgroundCheckBox);
	std::vector<PolygonVertex> vertices;
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("config.json");
    app.setDesign("polygon/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
