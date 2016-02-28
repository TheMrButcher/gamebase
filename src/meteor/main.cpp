#include <gamebase/engine/BasicTools.h>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        randomize();

        design = loadObj<LinearLayout>("meteor\\Design.json");
        setView(design);

        fighter = design->getChild<AnimGameObj>("fighter");
        fighterMark = design->getChild<AnimGameObj>("mfighter");

        sun = design->getChild<AnimGameObj>("sun");
        sun->runAnimation("rotate");

        earth = design->getChild<AnimGameObj>("earth");
        earth->runAnimation("rotate");
        earthMark = design->getChild<Texture>("mearth");

        game = design->getChild<GameView>("game");
        missiles = game->getLayer<SimpleLayer>(0);
        meteors = game->getLayer<SimpleLayer>(2);
        
        minimap = design->getChild<GameView>("minimap");
        windowMark = design->getChild<Texture>("window");
        meteorMarks = design->getChild<SimpleLayer>("marks");

        connect0(design->getChild<Button>("restart"), restart);
        connect0(design->getChild<Button>("fixcam"), switchCameraMode);
        focusCameraOnFighter = true;

        w = game->gameBox().width();
        h = game->gameBox().height();

        loadObj<AnimGameObj>("meteor\\Meteor0.json");
        loadObj<AnimGameObj>("meteor\\Meteor1.json");
        loadObj<AnimGameObj>("meteor\\Meteor2.json");
        loadObj<AnimGameObj>("meteor\\Meteor3.json");
        loadObj<AnimGameObj>("meteor\\Meteor4.json");

        restart();
    }

    void restart()
    {
        gameover = false;
        design->getChild<Label>("#gameover")->setVisible(false);
        timer.start();
        fireTimer.start();
        
        earth->setOffset(800, 0);
        fighter->setAngle(0);
        fighter->setOffset(700, -100);
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

        if (input().isPressed(SpecialKey::Left))
            fangle += 3.14 * timeDelta();
        if (input().isPressed(SpecialKey::Right))
            fangle -= 3.14 * timeDelta();
        if (input().isJustPressed(SpecialKey::Up))
        {
            fighter->resetChannel(0);
            fighter->runAnimation("turnon");
            fighter->runAnimation("move");
        }
        if (input().isJustOutpressed(SpecialKey::Up))
        {
            fighter->resetChannel(0);
            fighter->runAnimation("turnoff");
        }
        if (input().isPressed(SpecialKey::Up))
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

        if (focusCameraOnFighter)
        {
            game->setViewCenter(fpos);
        }
        else
        {
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

            if (minimap->isMouseOn() && input().isPressed(MouseButton::Left))
            {
                auto mouseCoords = minimap->mouseCoords();
                game->setViewCenter(mouseCoords * 20);
            }
        }
        windowMark->setOffset(game->viewCenter() / 20);

        if (timer.isPeriod(3000))
        {
            int index = rand() % 5;
            auto meteor = loadObj<AnimGameObj>("meteor\\Meteor" + toString(index) + ".json");
            Vec2 mpos(w / 2, 0);
            mpos.setAngle(randomFloat() * 6.28);
            meteor->setOffset(mpos);
            meteor->runAnimation("rotate");
            meteors->addObject(meteor);

            auto mark = loadObj<Texture>("meteor\\MeteorMark.json");
            meteorMarks->insertObject(meteor->id(), mark);
        }

        auto curMeteors = meteors->getObjects<AnimGameObj>();
        feach(auto meteor, curMeteors)
        {
            auto mpos = meteor->getOffset();
            auto d = epos - mpos;
            d.normalize();
            mpos += d * 150 * timeDelta();
            meteor->setOffset(mpos);
            auto* mark = meteorMarks->getObject<Texture>(meteor->id());
            mark->setOffset(mpos / 20);

            if (dist(mpos, epos) < 50) {
                gameover = true;
                design->getChild<Label>("#gameover")->setVisible(true);
            }
        }

        if (fireTimer.time() > 300 && game->isMouseOn() && input().isJustPressed(MouseButton::Left))
        {
            auto mousePos = game->mouseCoords();
            auto laser = loadObj<AnimGameObj>("meteor\\Laser.json");
            laser->setOffset(fpos);
            laser->setAngle((mousePos - fpos).angle());
            missiles->addObject(laser);
            fireTimer.start();
        }

        auto curLasers = missiles->getObjects<AnimGameObj>();
        feach(auto laser, curLasers)
        {
            auto lpos = laser->getOffset();
            Vec2 v(600, 0);
            v.rotate(laser->angle());
            lpos += v * timeDelta();
            laser->setOffset(lpos);

            if (!game->gameBox().contains(lpos))
                missiles->removeObject(laser);

            feach(auto meteor, curMeteors)
            {
                if (!meteor)
                    continue;
                auto mpos = meteor->getOffset();
                if (dist(mpos, lpos) < 30)
                {
                    meteorMarks->removeObject(meteor->id());
                    meteor->resetAllChannels();
                    meteor->runAnimation("explode");
                    meteor = nullptr;
                }
            }
        }
    }

    shared_ptr<LinearLayout> design;

    AnimGameObj* fighter;
    AnimGameObj* sun;
    AnimGameObj* earth;
    
    GameView* minimap;
    AnimGameObj* fighterMark;
    Texture* earthMark;
    Texture* windowMark;

    GameView* game;
    SimpleLayer* meteors;
    SimpleLayer* missiles;
    SimpleLayer* meteorMarks;

    Vec2 velo;

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
