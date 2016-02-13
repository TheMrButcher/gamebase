#include <gamebase/engine/BasicTools.h>
#include <gamebase/engine/Timer.h>
#include <gamebase/engine/AnimatedObjectConstruct.h>
#include <gamebase/engine/GameView.h>
#include <gamebase/engine/SimpleLayer.h>
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
        fighterMark = design->getChild<AnimatedObjectConstruct>("#mfighter");
        sun = design->getChild<AnimatedObjectConstruct>("#sun");
        sun->runAnimation("rotate");
        earth = design->getChild<AnimatedObjectConstruct>("#earth");
        earth->runAnimation("rotate");
        earthMark = design->getChild<StaticTextureRect>("#mearth");
        game = design->getChild<GameView>("#game");
        missiles = game->getLayer<SimpleLayer>(0);
        meteors = game->getLayer<SimpleLayer>(2);
        
        minimap = design->getChild<GameView>("#minimap");
        windowMark = design->getChild<StaticTextureRect>("#window");
        meteorMarks = design->getChild<SimpleLayer>("#marks");

        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));
        design->getChild<Button>("#fixcam")->setCallback(bind(&MyApp::switchCameraMode, this));
        focusCameraOnFighter = true;

        w = game->gameBox().width();
        h = game->gameBox().height();

        deserialize<AnimatedObjectConstruct>("meteor\\Meteor0.json");
        deserialize<AnimatedObjectConstruct>("meteor\\Meteor1.json");
        deserialize<AnimatedObjectConstruct>("meteor\\Meteor2.json");
        deserialize<AnimatedObjectConstruct>("meteor\\Meteor3.json");
        deserialize<AnimatedObjectConstruct>("meteor\\Meteor4.json");

        restart();
    }

    void restart()
    {
        gameover = false;
        design->getChild<StaticLabel>("#gameover")->setVisible(false);
        score = 0;
        //design->getChild<StaticLabel>("#score")->setText("0");
        timer.start();
        fireTimer.start();
        
        earth->setOffset(Vec2(800, 0));
        fighter->setAngle(0);
        fighter->setOffset(Vec2(700, -100));
        velo = Vec2(0, 0);

        missiles->clear();
        meteors->clear();
        meteorMarks->clear();
    }

    void switchCameraMode()
    {
        focusCameraOnFighter = !focusCameraOnFighter;
    }

    void move()
    {
        if (gameover)
            return;

        Vec2 epos = earth->getOffset();
        epos.rotate(6.28 / 120 * timeDelta());
        earth->setOffset(epos);
        earthMark->setOffset(epos / 20);

        Vec2 fpos = fighter->getOffset();
        float fangle = fighter->angle();

        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Left))
            fangle += 3.14 * timeDelta();
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Right))
            fangle -= 3.14 * timeDelta();
        if (m_inputRegister.specialKeys.isJustPressed(SpecialKey::Up)) {
            fighter->resetChannel(0);
            fighter->runAnimation("turnon");
            fighter->runAnimation("move");
        }
        if (m_inputRegister.specialKeys.isJustOutpressed(SpecialKey::Up)) {
            fighter->resetChannel(0);
            fighter->runAnimation("turnoff");
        }
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Up))
        {
            Vec2 delta(20.0 * timeDelta(), 0);
            delta.rotate(fangle);
            velo += delta;
        }

        if (fpos.x < game->gameBox().bottomLeft.x)
            velo.x += 100.0 * timeDelta();
        if (fpos.x > game->gameBox().topRight.x)
            velo.x -= 100.0 * timeDelta();
        if (fpos.y < game->gameBox().bottomLeft.y)
            velo.y += 100.0 * timeDelta();
        if (fpos.y > game->gameBox().topRight.y)
            velo.y -= 100.0 * timeDelta();

        fpos += velo * timeDelta();

        fighter->setOffset(fpos);
        fighter->setAngle(fangle);

        fighterMark->setOffset(fpos / 20);
        fighterMark->setAngle(fangle);

        if (focusCameraOnFighter) {
            game->setViewCenter(fpos);
        } else {
            auto cpos = game->viewCenter();
            if (m_inputRegister.keys.isPressed('a'))
                cpos.x -= 400 * timeDelta();
            if (m_inputRegister.keys.isPressed('d'))
                cpos.x += 400 * timeDelta();
            if (m_inputRegister.keys.isPressed('s'))
                cpos.y -= 400 * timeDelta();
            if (m_inputRegister.keys.isPressed('w'))
                cpos.y += 400 * timeDelta();
            game->setViewCenter(cpos);

            if (minimap->isMouseOn() && m_inputRegister.mouseButtons.isPressed(MouseButton::Left)) {
                auto mouseCoords = minimap->mouseCoords();
                game->setViewCenter(mouseCoords * 20);
            }
        }
        windowMark->setOffset(game->viewCenter() / 20);

        if (timer.isPeriod(3000)) {
            int index = 0; //rand() % 5;
            auto meteor = deserialize<AnimatedObjectConstruct>("meteor\\Meteor" + toString(index) + ".json");
            if (rand() % 2) {
                meteor->setOffset(Vec2(randomInt(-w / 2, w / 2), (h / 2 + 50) * (1 - 2 * (rand() % 2))));
            } else {
                meteor->setOffset(Vec2((w / 2 + 50) * (1 - 2 * (rand() % 2)), randomInt(-h / 2, h / 2)));
            }
            meteor->runAnimation("rotate");
            meteors->addObject(meteor);

            auto mark = deserialize<StaticTextureRect>("meteor\\MeteorMark.json");
            meteorMarks->insertObject(meteor->id(), mark);
        }

        auto curMeteors = meteors->getObjects<AnimatedObjectConstruct>();
        for (int i = 0; i < curMeteors.size(); ++i) {
            auto mpos = curMeteors[i]->getOffset();
            auto d = epos - mpos;
            d.normalize();
            mpos += d * 150 * timeDelta();
            curMeteors[i]->setOffset(mpos);
            auto* mark = meteorMarks->getObject<StaticTextureRect>(curMeteors[i]->id());
            mark->setOffset(mpos / 20);

            if (dist(mpos, epos) < 50) {
                gameover = true;
                design->getChild<StaticLabel>("#gameover")->setVisible(true);
            }
        }

        if (fireTimer.time() > 300 && game->isMouseOn() && m_inputRegister.mouseButtons.isJustPressed(MouseButton::Left)) {
            auto mousePos = game->mouseCoords();
            auto laser = deserialize<AnimatedObjectConstruct>("meteor\\Laser.json");
            laser->setOffset(fpos);
            laser->setAngle((mousePos - fpos).angle());
            missiles->addObject(laser);
            fireTimer.start();
        }

        auto curLasers = missiles->getObjects<AnimatedObjectConstruct>();
        for (int i = 0; i < curLasers.size(); ++i) {
            auto lpos = curLasers[i]->getOffset();
            Vec2 v(600, 0);
            v.rotate(curLasers[i]->angle());
            lpos += v * timeDelta();
            curLasers[i]->setOffset(lpos);

            if (!game->gameBox().contains(lpos))
                missiles->removeObject(curLasers[i]);

            for (int j = 0; j < curMeteors.size(); ++j) {
                if (!curMeteors[j])
                    continue;
                auto mpos = curMeteors[j]->getOffset();
                if (dist(mpos, lpos) < 30) {
                    meteorMarks->removeObject(curMeteors[j]->id());
                    curMeteors[j]->resetAllChannels();
                    curMeteors[j]->runAnimation("explode");
                    curMeteors[j] = nullptr;
                }
            }
        }
    }

    shared_ptr<LinearLayout> design;

    AnimatedObjectConstruct* fighter;
    AnimatedObjectConstruct* sun;
    AnimatedObjectConstruct* earth;
    
    GameView* minimap;
    AnimatedObjectConstruct* fighterMark;
    StaticTextureRect* earthMark;
    StaticTextureRect* windowMark;

    GameView* game;
    SimpleLayer* meteors;
    SimpleLayer* missiles;
    SimpleLayer* meteorMarks;

    Vec2 velo;

    int score;
    bool gameover;
    Timer timer;
    Timer fireTimer;
    bool focusCameraOnFighter;

    float w, h;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
