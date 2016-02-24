#include <gamebase/engine/BasicTools.h>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        auto design = loadObj<CanvasLayout>("simple_app\\Design.json");
        setView(design);

        earth = design->getChild<GameObj>("#earth");
    }

    void move()
    {
        if (input().isPressed(SpecialKey::Up))
            earth->setScale(earth->scale() + 0.1);
        if (input().isPressed(SpecialKey::Down))
            earth->setScale(earth->scale() - 0.1);

        if (input().isPressed(SpecialKey::Left))
            earth->setAngle(earth->angle() + 0.1);
        if (input().isPressed(SpecialKey::Right))
            earth->setAngle(earth->angle() - 0.1);
    }

    GameObj* earth;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
