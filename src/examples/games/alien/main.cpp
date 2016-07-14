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

        design = deserialize<LinearLayout>("alien\\Design.json");
        m_view->addObject(design);

        shipsLayout = design->getChild<CanvasLayout>("#ships");
        missilesLayout = design->getChild<CanvasLayout>("#missiles");
        fighter = design->getChild<StaticTextureRect>("#fighter");

        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));
    }

    void postload()
    {
        gameBox = shipsLayout->box();
        restart();
    }

    void restart()
    {
        gameover = false;
        design->getChild<StaticLabel>("#gameover")->setVisible(false);
        score = 0;
        design->getChild<StaticLabel>("#score")->setText("0");
        timer.start();

        shipsLayout->clear();
        missilesLayout->clear();
        aliens.clear();
        missiles.clear();

        fighter->setOffset(Vec2(0, -300));
        bullets = 30;
    }

    void move()
    {
        if (gameover)
            return;

        if (isReloading && reloadTimer.time() > 1000)
        {
            bullets = 30;
            design->getChild<StaticLabel>("#bullets")->setText("30");
            isReloading = false;
        }
        
        Vec2 fpos = fighter->getOffset();
        auto fbox = fighter->movedBox();

        for (int i = missiles.size() - 1; i >= 0; --i)
        {
            auto mbox = missiles[i]->movedBox();
            Vec2 mpos = missiles[i]->getOffset();
            bool needToRemove = !mbox.intersects(gameBox);
            if (missiles[i]->color().r == 1)
            {
                for (int j = aliens.size() - 1; j >= 0; --j)
                {
                    auto abox = aliens[j]->movedBox();

                    if (abox.intersects(mbox))
                    {
                        killAlien(j);
                        needToRemove = true;
                        score++;
                        design->getChild<StaticLabel>("#score")->setText(toString(score));
                        break;
                    }
                }
                mpos.y += timeDelta() * 500;
            }
            else
            {
                if (mbox.intersects(fbox))
                {
                    gameover = true;
                    design->getChild<StaticLabel>("#gameover")->setVisible(true);
                    return;
                }
                mpos.y -= timeDelta() * 500;
            }
            
            missiles[i]->setOffset(mpos);
            if (needToRemove)
            {
                missilesLayout->removeObject(missiles[i]);
                missiles.erase(missiles.begin() + i);
            }
        }

        for (int i = aliens.size() - 1; i >= 0; --i)
        {
            auto abox = aliens[i]->movedBox();
            if (abox.intersects(fbox))
            {
                gameover = true;
                design->getChild<StaticLabel>("#gameover")->setVisible(true);
                return;
            }

            if (abox.topRight.y < gameBox.bottomLeft.y)
            {
                killAlien(i);
                continue;
            }

            Vec2 apos = aliens[i]->getOffset();
            apos.y -= timeDelta() * 200;
            aliens[i]->setOffset(apos);

            if (shootTimers[i].isPeriod(1000))
            {
                auto missile = deserialize<StaticFilledRect>("alien\\Laser.json");
                missile->setColor(Color(0, 0.5f, 0));
                missile->setOffset(Vec2(apos.x, apos.y + 12));
                missiles.push_back(missile);
                missilesLayout->addObject(missile);
            }
        }

        if (timer.isPeriod(3000))
        {
            int x = randomInt(gameBox.bottomLeft.x + 64, gameBox.topRight.x - 64);
            auto alien = deserialize<StaticTextureRect>("alien\\AlienShip.json");
            alien->setOffset(Vec2(x, gameBox.topRight.y + 32));
            aliens.push_back(alien);
            shipsLayout->addObject(alien);
            shootTimers.push_back(Timer());
            shootTimers.back().start();
        }

        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Left))
        {
            float delta = -300 * timeDelta();
            if (fbox.bottomLeft.x + delta > gameBox.bottomLeft.x)
                fpos.x += delta;
        }
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Right))
        {
            float delta = 300 * timeDelta();
            if (fbox.topRight.x + delta < gameBox.topRight.x)
                fpos.x += delta;
        }
        if (m_inputRegister.keys.isJustPressed('r'))
        {
            isReloading = true;
            reloadTimer.start();
        }
        if (!isReloading && bullets > 0 && m_inputRegister.keys.isJustPressed('f'))
        {
            --bullets;
            design->getChild<StaticLabel>("#bullets")->setText(toString(bullets));

            auto missile = deserialize<StaticFilledRect>("alien\\Laser.json");
            missile->setColor(Color(1, 0, 0));
            missile->setOffset(Vec2(fpos.x, fpos.y + 44));
            missiles.push_back(missile);
            missilesLayout->addObject(missile);
        }

        fighter->setOffset(fpos);
    }

    void killAlien(int i)
    {
        shipsLayout->removeObject(aliens[i]);
        shootTimers.erase(shootTimers.begin() + i);
        aliens.erase(aliens.begin() + i);
    }

    shared_ptr<LinearLayout> design;
    CanvasLayout* missilesLayout;
    CanvasLayout* shipsLayout;
    BoundingBox gameBox;

    StaticTextureRect* fighter;
    vector<shared_ptr<StaticTextureRect>> aliens;
    vector<Timer> shootTimers;
    vector<shared_ptr<StaticFilledRect>> missiles;

    int bullets;
    bool isReloading;
    Timer reloadTimer;

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
