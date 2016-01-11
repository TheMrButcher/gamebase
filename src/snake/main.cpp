#include <gamebase/engine/BasicTools.h>
#include <gamebase/engine/Timer.h>
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

        design = deserialize<CanvasLayout>("snake\\Design.json");
        m_view->addObject(design);

        fieldLayout = design->getChild<CanvasLayout>("#field");

        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));

        record = 0;
    }

    void postload()
    {
        restart();
    }

    void restart()
    {
        gameover = false;
        design->getChild<StaticLabel>("#gameover")->setVisible(false);
        score = 0;
        design->getChild<StaticLabel>("#score")->setText("0");
        timer.start();
        fieldLayout->clear();

        circles.clear();
        auto circle = deserialize<StaticTextureRect>("snake\\Circle.json");
        circle->setOffset(Vec2(0, 0));
        curPos = IntVec2(7, 7);
        circles.push_back(circle);
        fieldLayout->addObject(circle);
        dir = IntVec2(1, 0);

        apple = deserialize<StaticTextureRect>("snake\\Apple.json");
        fieldLayout->addObject(apple);
        randomizeApple();
    }

    void move()
    {
        if (gameover)
            return;

        if (timer.isPeriod(500))
        {
            auto newPos = curPos + dir;
            if (newPos.x < 0 || newPos.y < 0 || newPos.x > 14 || newPos.y > 14)
            {
                gameover = true;
                design->getChild<StaticLabel>("#gameover")->setVisible(true);
                return;
            }

            auto circle = deserialize<StaticTextureRect>("snake\\Circle.json");
            circle->setOffset(Vec2(newPos.x * 32 - 224, newPos.y * 32 - 224));
            fieldLayout->addObject(circle);
            
            for (int i = 0; i < circles.size(); ++i)
            {
                if (circles[i]->movedBox().intersects(circle->movedBox()))
                {
                    gameover = true;
                    design->getChild<StaticLabel>("#gameover")->setVisible(true);
                    return;
                }
            }
            
            bool needPopBack = true;
            if (apple->movedBox().intersects(circle->movedBox()))
            {
                score++;
                design->getChild<StaticLabel>("#score")->setText(toString(score));
                if (score > record)
                {
                    record = score;
                    design->getChild<StaticLabel>("#record")->setText(toString(record));
                }
                randomizeApple();
                needPopBack = false;
            }
            
            curPos = newPos;
            circles.push_front(circle);

            if (needPopBack)
            {
                fieldLayout->removeObject(circles.back());
                circles.pop_back();
            }
        }

        if (m_inputRegister.specialKeys.isJustPressed(SpecialKey::Left))
            dir = IntVec2(-1, 0);
        if (m_inputRegister.specialKeys.isJustPressed(SpecialKey::Right))
            dir = IntVec2(1, 0);
        if (m_inputRegister.specialKeys.isJustPressed(SpecialKey::Up))
            dir = IntVec2(0, 1);
        if (m_inputRegister.specialKeys.isJustPressed(SpecialKey::Down))
            dir = IntVec2(0, -1);
    }
    
    void randomizeApple()
    {
        for (;;)
        {
            int x = rand() % 15;
            int y = rand() % 15;
            apple->setOffset(Vec2(x * 32 - 224, y * 32 - 224));

            bool hasConflict = false;
            for (int i = 0; i < circles.size(); ++i)
            {
                if (circles[i]->movedBox().intersects(apple->movedBox()))
                    hasConflict = true;
            }

            if (!hasConflict)
                return;
        }
    }

    shared_ptr<CanvasLayout> design;
    CanvasLayout* fieldLayout;

    deque<shared_ptr<StaticTextureRect>> circles;
    shared_ptr<StaticTextureRect> apple;

    IntVec2 curPos;
    IntVec2 dir;

    int score;
    int record;

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
