#include <gamebase/engine/BasicTools.h>
#include <gamebase/engine/Timer.h>
#include <gamebase/engine/AnimatedObjectConstruct.h>
#include <gamebase/math/IntVector.h>
#include <deque>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        randomize();

        design = deserialize<LinearLayout>("meteor\\Design.json");
        m_view->addObject(design);

        fighter = design->getChild<AnimatedObjectConstruct>("#fighter");
        sun = design->getChild<AnimatedObjectConstruct>("#sun");
        sun->runAnimation("rotate");
        earth = design->getChild<AnimatedObjectConstruct>("#earth");
        earth->runAnimation("rotate");

        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));
    }

    void postload()
    {
        gameBox = design->getChild<CanvasLayout>("#game")->box();
        restart();
    }

    void restart()
    {
        gameover = false;
        //design->getChild<StaticLabel>("#gameover")->setVisible(false);
        score = 0;
        //design->getChild<StaticLabel>("#score")->setText("0");
        timer.start();

        fighter->setAngle(0);
        fighter->setOffset(Vec2(0, -250));\
        velo = Vec2(0, 0);
    }

    void move()
    {
        if (gameover)
            return;

        Vec2 epos = earth->getOffset();
        epos.rotate(6.28 / 120 * timeDelta());
        earth->setOffset(epos);

        Vec2 fpos = fighter->getOffset();
        float fangle = fighter->angle();

        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Left))
            fangle += 3.14 / 4 * timeDelta();
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Right))
            fangle -= 3.14 / 4 * timeDelta();
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Up))
        {
            Vec2 delta(20.0 * timeDelta(), 0);
            delta.rotate(fangle);
            velo += delta;
        }

        if (fpos.x < gameBox.bottomLeft.x)
            velo.x += 100.0 * timeDelta();
        if (fpos.x > gameBox.topRight.x)
            velo.x -= 100.0 * timeDelta();
        if (fpos.y < gameBox.bottomLeft.y)
            velo.y += 100.0 * timeDelta();
        if (fpos.y > gameBox.topRight.y)
            velo.y -= 100.0 * timeDelta();

        fpos += velo * timeDelta();

        fighter->setOffset(fpos);
        fighter->setAngle(fangle);
    }

    shared_ptr<LinearLayout> design;
    BoundingBox gameBox;

    AnimatedObjectConstruct* fighter;
    AnimatedObjectConstruct* sun;
    AnimatedObjectConstruct* earth;

    Vec2 velo;

    int score;
    bool gameover;
    Timer timer;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
