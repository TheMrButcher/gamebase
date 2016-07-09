#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    MyApp()
    {
        setDesign("meteor\\Design.json");
    }

    void load()
    {
        randomize();
        sun.anim.run("rotate");
        earth.anim.run("rotate");

        connect0(design.child<Button>("restart"), restart);
        connect0(design.child<Button>("fixcam"), switchCameraMode);
        focusCameraOnFighter = true;

        w = game.gameBox().width();
        h = game.gameBox().height();

        loadObj<GameObj>("meteor\\Meteor0.json");
        loadObj<GameObj>("meteor\\Meteor1.json");
        loadObj<GameObj>("meteor\\Meteor2.json");
        loadObj<GameObj>("meteor\\Meteor3.json");
        loadObj<GameObj>("meteor\\Meteor4.json");

        restart();
    }

    void restart()
    {
        gameover = false;
        gameoverLabel.hide();
        timer.start();
        fireTimer.start();
        
        earth.setPos(800, 0);
        fighter.setAngle(0);
        fighter.setPos(700, -100);
        velo = Vec2(0, 0);

        missiles.clear();
        meteors.clear();
        meteorMarks.clear();
    }

    void switchCameraMode()
    {
        focusCameraOnFighter = !focusCameraOnFighter;
    }

    void processInput()
    {
        float fangle = fighter.angle();
        if (input.leftPressed())
            fangle += 3.14 * timeDelta();
        if (input.rightPressed())
            fangle -= 3.14 * timeDelta();
        if (input.upJustPressed())
        {
            fighter.anim.reset(0);
            fighter.anim.run("turnon");
            fighter.anim.run("move");
        }
        if (input.upJustOutpressed())
        {
            fighter.anim.reset(0);
            fighter.anim.run("turnoff");
        }
        if (input.upPressed())
        {
            Vec2 delta(20.0 * timeDelta(), 0);
            delta.rotate(fangle);
            velo += delta;
        }
        fighter.setAngle(fangle);
        fighterMark.setAngle(fangle);

        if (!focusCameraOnFighter)
        {
            auto cpos = game.view();
            if (input.pressed('a'))
                cpos.x -= 400 * timeDelta();
            if (input.pressed('d'))
                cpos.x += 400 * timeDelta();
            if (input.pressed('s'))
                cpos.y -= 400 * timeDelta();
            if (input.pressed('w'))
                cpos.y += 400 * timeDelta();
            game.setView(cpos);

            if (minimap.isMouseOn() && input.leftButtonPressed())
                game.setView(minimap.mousePos() * 20);
        }

        if (fireTimer.time() > 300 && game.isMouseOn() && input.leftButtonJustPressed())
        {
            auto mpos = game.mousePos();
            auto fpos = fighter.pos();
            auto laser = loadObj<GameObj>("meteor\\Laser.json");
            laser.setPos(fpos);
            laser.setAngle((mpos - fpos).angle());
            missiles.add(laser);
            fireTimer.start();
        }
    }

    void move()
    {
        if (gameover)
            return;

        Vec2 epos = earth.pos();
        epos.rotate(6.28 / 120 * timeDelta());
        earth.setPos(epos);
        earthMark.setPos(epos / 20);

        Vec2 fpos = fighter.pos();
        float fangle = fighter.angle();

        if (fpos.x < game.gameBox().bottomLeft.x)
            velo.x += 100.0 * timeDelta();
        if (fpos.x > game.gameBox().topRight.x)
            velo.x -= 100.0 * timeDelta();
        if (fpos.y < game.gameBox().bottomLeft.y)
            velo.y += 100.0 * timeDelta();
        if (fpos.y > game.gameBox().topRight.y)
            velo.y -= 100.0 * timeDelta();

        fpos += velo * timeDelta();

        fighter.setPos(fpos);
        fighterMark.setPos(fpos / 20);

        if (focusCameraOnFighter)
        {
            game.setView(fpos);
        }
        windowMark.setPos(game.view() / 20);

        if (timer.isPeriod(3000))
        {
            int index = rand() % 5;
            auto meteor = loadObj<GameObj>("meteor\\Meteor" + toString(index) + ".json");
            Vec2 mpos(w / 2, 0);
            mpos.setAngle(randomFloat() * 6.28);
            meteor.setPos(mpos);
            meteor.anim.run("rotate");
            meteors.add(meteor);

            auto mark = loadObj<Texture>("meteor\\MeteorMark.json");
            meteorMarks.insert(meteor.id(), mark);
        }

        feach(auto meteor, meteors.all())
        {
            auto mpos = meteor.pos();
            auto d = epos - mpos;
            d.normalize();
            mpos += d * 150 * timeDelta();
            meteor.setPos(mpos);
            auto mark = meteorMarks.get<Texture>(meteor.id());
            mark.setPos(mpos / 20);

            if (dist(mpos, epos) < 50) {
                gameover = true;
                gameoverLabel.show();
            }
        }

        feach(auto laser, missiles.all())
        {
            auto lpos = laser.pos();
            Vec2 v(600, 0);
            v.rotate(laser.angle());
            lpos += v * timeDelta();
            laser.setPos(lpos);

            feach(auto meteor, meteors.find(laser.movedBox()))
            {
                auto mpos = meteor.pos();
                if (dist(mpos, lpos) < 30)
                {
                    meteorMarks.remove(meteor.id());
                    meteor.anim.reset();
                    meteor.anim.run("explode");
                }
            }

            if (!game.gameBox().contains(lpos))
                missiles.remove(laser);
        }
    }


    FromDesign(GameObj, fighter);
    FromDesign(GameObj, sun);
    FromDesign(GameObj, earth);
    
    FromDesign(GameView, minimap);
    FromDesign2(GameObj, fighterMark, "mfighter");
    FromDesign2(Texture, earthMark, "mearth");
    FromDesign2(Texture, windowMark, "window");

    FromDesign(GameView, game);
    FromDesign(SimpleLayer, meteors);
    FromDesign(SimpleLayer, missiles);
    FromDesign2(SimpleLayer, meteorMarks, "marks");

    FromDesign2(Label, gameoverLabel, "gameover");

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
