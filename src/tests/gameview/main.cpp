#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    void load()
    {
        connect(relativeCenterFullscreenButton, loadField, "Fullscreen");
        connect(relativeCenterSmallButton, loadField, "Small");
        connect(relativeLeftBottomFullscreenButton, loadFieldLeftBottom, "Fullscreen");
        connect(relativeLeftBottomSmallButton, loadFieldLeftBottom, "Small");
        connect(infiniteFullscreenButton, loadField, "Fullscreen");
        connect(infiniteSmallButton, loadField, "Small");
        connect(fixedFullscreenButton, loadFieldFixed, "Fullscreen");
        connect(fixedSmallButton, loadFieldFixed, "Small");
    }

    void loadField(const std::string& name)
    {
        auto field = fieldContainer.load<GameView>(0, "gameview/" + name + ".json");
        auto layer = field.get("layer");
        for (int x = -10; x <= 10; ++x) {
            for (int y = -10; y <= 10; ++y) {
                auto cell = layer.load("gameview/Cell.json", x * 60, y * 40);
                if ((x + y + 20) % 2 == 0)
                    cell.child<FilledRect>("rect").setColor(255, 255, 255);
                cell.child<Label>("label") << x << ", " << y;
                cell.skin<Layout>().update();
            }
        }
    }

    void loadFieldLeftBottom(const std::string& name)
    {
        loadField(name);
        auto field = fieldContainer.get<GameView>(0);
        field.setView(field.width() / 2 - 30, field.height() / 2 - 20);
    }

    void loadFieldFixed(const std::string& name)
    {
        loadField(name);
        auto field = fieldContainer.get<GameView>(0);
        field.setView(-180, 80);
    }

    FromDesign(Layout, fieldContainer);
    FromDesign(Button, relativeCenterFullscreenButton);
    FromDesign(Button, relativeCenterSmallButton);
    FromDesign(Button, relativeLeftBottomFullscreenButton);
    FromDesign(Button, relativeLeftBottomSmallButton);
    FromDesign(Button, infiniteFullscreenButton);
    FromDesign(Button, infiniteSmallButton);
    FromDesign(Button, fixedFullscreenButton);
    FromDesign(Button, fixedSmallButton);
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("config.json");
    app.setDesign("gameview/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
