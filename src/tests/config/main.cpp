#include <gamebase/Gamebase.h>
#include <gamebase/tools/Json.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    void load()
    {
        widthBox << config.width();
        heightBox << config.height();
        minWidthBox << config.minWidth();
        minHeightBox << config.minHeight();
        maxWidthBox << config.maxWidth();
        maxHeightBox << config.maxHeight();
        windowTitleBox << config.windowTitle();
        showConsoleBox.setChecked(config.isConsoleVisible());
        fullscreenModeBox.setChecked(config.screenMode() == FullscreenMode);

        connect(applyButton, applySettings);
    }

    void process(Input input)
    {
        using namespace InputKey;
        if (input.pressed(Escape))
            close();
    }

    void applySettings()
    {
        config.setWidth(toInt(widthBox.text()));
        config.setHeight(toInt(heightBox.text()));
        config.setMinWidth(toInt(minWidthBox.text()));
        config.setMinHeight(toInt(minHeightBox.text()));
        config.setMaxWidth(toInt(maxWidthBox.text()));
        config.setMaxHeight(toInt(maxHeightBox.text()));
        config.setWindowTitle(windowTitleBox.text());
        config.setConsoleVisible(showConsoleBox.isChecked());
        config.setScreenMode(fullscreenModeBox.isChecked() ? FullscreenMode : WindowMode);

        cout << "New config: " << config.toJson().serialize(StyledJsonFormat) << endl;
    }

    FromDesign(TextBox, widthBox);
    FromDesign(TextBox, heightBox);
    FromDesign(TextBox, minWidthBox);
    FromDesign(TextBox, minHeightBox);
    FromDesign(TextBox, maxWidthBox);
    FromDesign(TextBox, maxHeightBox);
    FromDesign(TextBox, windowTitleBox);
    FromDesign(CheckBox, showConsoleBox);
    FromDesign(CheckBox, fullscreenModeBox);
    FromDesign(Button, applyButton);
};

int main(int argc, char** argv)
{
    if (!fileExists("customConfig.json")) {
        auto text = loadTextFile("config.json");
        std::ofstream file("customConfig.json");
        file << text;
    }

    MyApp app;
    app.setConfig("customConfig.json");
    app.setDesign("config/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
