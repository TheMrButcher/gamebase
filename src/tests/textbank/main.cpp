#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    void load()
    {
        loadTextBank("textbank/Texts.json");
        connect(button, setText);
        text << tr("text2");
    }

    void setText()
    {
        text << tr(textBox.text());
    }

    FromDesign(Button, button);
    FromDesign(TextBox, textBox);
    FromDesign(Label, text);
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("config.json");
    app.setDesign("textbank/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
