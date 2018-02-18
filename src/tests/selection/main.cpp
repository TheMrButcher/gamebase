#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    void load()
    {
        connect(inactiveObj, callback, inactiveObj);
        connect(pressableObj, callback, pressableObj);
        connect(clickableObj, callback, clickableObj);
        connect(prAndClObj, callback, prAndClObj);
        connect(resetButton, reset);
    }

    void callback(GameObj obj)
    {
        incValue(callbackCounter);
        callbackIndicator.setColor(0, 255, 0);
        if (obj.isMouseOn()) {
            incValue(mouseOnCounter);
            mouseOnIndicator.setColor(0, 255, 0);
        } else {
            mouseOnIndicator.setColor(0, 0, 0);
        }
        if (obj.isPressed()) {
            incValue(pressedCounter);
            pressedIndicator.setColor(0, 255, 0);
        } else {
            pressedIndicator.setColor(0, 0, 0);
        }
        if (obj.isClicked()) {
            incValue(clickedCounter);
            clickedIndicator.setColor(0, 255, 0);
        } else {
            clickedIndicator.setColor(0, 0, 0);
        }
        std::cout << "Callback #" << getValue(callbackCounter)
            << ": mouse_on=" << getValue(mouseOnCounter)
            << ", pressed=" << getValue(pressedCounter)
            << ", clicked=" << getValue(clickedCounter)
            << std::endl;
    }

    void reset()
    {
        resetValue(callbackCounter);
        resetValue(mouseOnCounter);
        resetValue(pressedCounter);
        resetValue(clickedCounter);
        callbackIndicator.setColor(0, 0, 0);
        mouseOnIndicator.setColor(0, 0, 0);
        pressedIndicator.setColor(0, 0, 0);
        clickedIndicator.setColor(0, 0, 0);
    }

    int getValue(Label label)
    {
        return toInt(label.text());
    }

    void incValue(Label label)
    {
        label << (toInt(label.text()) + 1);
    }

    void resetValue(Label label)
    {
        label << 0;
    }

    FromDesign(GameObj, inactiveObj);
    FromDesign(GameObj, pressableObj);
    FromDesign(GameObj, clickableObj);
    FromDesign(GameObj, prAndClObj);

    FromDesign(Label, callbackCounter);
    FromDesign(Label, mouseOnCounter);
    FromDesign(Label, pressedCounter);
    FromDesign(Label, clickedCounter);

    FromDesign(FilledRect, callbackIndicator);
    FromDesign(FilledRect, mouseOnIndicator);
    FromDesign(FilledRect, pressedIndicator);
    FromDesign(FilledRect, clickedIndicator);
    
    FromDesign(Button, resetButton);
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("config.json");
    app.setDesign("selection/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
