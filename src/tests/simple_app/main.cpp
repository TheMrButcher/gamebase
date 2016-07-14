#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    MyApp()
    {
        setDesign("simple_app\\Design.json");
    }

    void processInput()
    {
        if (input.upPressed())
            earth.setScale(earth.scale() + 0.1);
        if (input.downPressed())
            earth.setScale(earth.scale() - 0.1);

        if (input.leftPressed())
            earth.setAngle(earth.angle() + 0.1);
        if (input.rightPressed())
            earth.setAngle(earth.angle() - 0.1);
    }

    FromDesign(GameObj, earth);
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
