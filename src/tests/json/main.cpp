#include <gamebase/Gamebase.h>
#include <gamebase/tools/Json.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    void load()
    {
        loadJson();
        connect(saveButton, saveJson);
        connect(loadButton, loadJson);
    }

    void saveJson()
    {
        json["member"]["text"] = textBox.text();
        json["member"]["int"] = toInt(intBox.text());
        json["array"][0] = toFloat(float1Box.text());
        json["array"][1] = toFloat(float2Box.text());
        json["array"][2] = toFloat(float3Box.text());
        cout << json.serialize(StyledJsonFormat) << endl;
        json.save("../../contrib/bin/NewData.json", StyledJsonFormat);
    }

    void loadJson()
    {
        json = loadJsonFromFile("json/Data.json");
        cout << json.serialize(StyledJsonFormat) << endl;
        textBox << json["member"]["text"].toString();
        intBox << json["member"]["int"].toInt();
        float1Box << json["array"][0].toFloat();
        float2Box << json["array"][1].toFloat();
        float3Box << json["array"][2].toFloat();
    }

    FromDesign(TextBox, textBox);
    FromDesign(TextBox, intBox);
    FromDesign(TextBox, float1Box);
    FromDesign(TextBox, float2Box);
    FromDesign(TextBox, float3Box);
    FromDesign(Button, saveButton);
    FromDesign(Button, loadButton);

    Json json;
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("config.json");
    app.setDesign("json/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
