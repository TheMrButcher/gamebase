#include <gamebase/engine/BasicTools.h>

using namespace gamebase;
using namespace std;

struct TowerData {
    int level;
    int minDamage;
    int maxDamage;
    float range;
};

class MyApp : public SimpleApplication
{
public:
    enum GroundType {
        Grass,
        Path,
        Water,
        Sand,
        Start,
        Finish
    };

    enum ObjType {
        None,
        Rock,
        Bush1,
        Bush2,
        Tree1,
        Tree2,
        Tower
    };

    void load()
    {
        randomize();

        design = loadObj<LinearLayout>("towers\\Design.json");
        setView(design);

        std::map<Color, GroundType> colorToGType;
        colorToGType[Color(0, 1, 0)] = Grass;
        colorToGType[Color(0, 0, 0)] = Path;
        colorToGType[Color(0, 0, 1)] = Water;
        colorToGType[Color(1, 1, 0)] = Sand;
        colorToGType[Color(1, 0, 1)] = Start;
        colorToGType[Color(1, 0, 0)] = Finish;

        std::map<Color, ObjType> colorToOType;
        colorToOType[Color(0, 0, 0)] = None;
        colorToOType[Color(1, 1, 1)] = Rock;
        colorToOType[Color(1, 0, 1)] = Bush1;
        colorToOType[Color(1, 0, 0)] = Bush2;
        colorToOType[Color(0, 1, 0)] = Tree1;
        colorToOType[Color(0, 0, 1)] = Tree2;

        gmap = loadMap<GroundType>("towers\\gmap.png", colorToGType);
        omap = loadMap<ObjType>("towers\\omap.png", colorToOType);

        gv = design->getChild<GameView>("#gv");
        gv->setGameBox(BoundingBox(gmap.width * 128, gmap.height * 100));
        auto* ground = gv->getLayer<StaticLayer>(0);
        objects = gv->getLayer<ImmobileLayer>(2);

        w = gmap.width * 128;
        h = gmap.height * 100;
        auto gdx = -w / 2 + 128;
        auto gdy = -h / 2 + 100;
        dx = -w / 2 + 64;
        dy = -h / 2 + 50;
        for (int x = 0; x < gmap.width; ++x)
        {
            for (int y = 0; y < gmap.height; ++y)
            {
                if (omap.get(x, y) == Rock)
                {
                    auto obj = loadObj<Texture>("towers\\Rock.json");
                    obj->setOffset(Vec2(x * 128 + dx, y * 100 + dy));
                    objects->addObject(obj);
                }
                if (omap.get(x, y) == Bush1)
                {
                    auto obj = loadObj<Texture>("towers\\Bush1.json");
                    obj->setOffset(Vec2(x * 128 + dx, y * 100 + dy));
                    objects->addObject(obj);
                }
                if (omap.get(x, y) == Bush2)
                {
                    auto obj = loadObj<Texture>("towers\\Bush2.json");
                    obj->setOffset(Vec2(x * 128 + dx, y * 100 + dy));
                    objects->addObject(obj);
                }
                if (omap.get(x, y) == Tree1)
                {
                    auto obj = loadObj<Texture>("towers\\Tree1.json");
                    obj->setOffset(Vec2(x * 128 + dx, y * 100 + dy));
                    objects->addObject(obj);
                }
                if (omap.get(x, y) == Tree2)
                {
                    auto obj = loadObj<Texture>("towers\\Tree2.json");
                    obj->setOffset(Vec2(x * 128 + dx, y * 100 + dy));
                    objects->addObject(obj);
                }

                if (x % 2 != 0 || y % 2 != 0)
                    continue;

                if (gmap.get(x, y) == Grass)
                {
                    auto obj = loadObj<Texture>("towers\\Grass.json");
                    obj->setOffset(Vec2(x * 128 + gdx, y * 100 + gdy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Path)
                {
                    auto obj = loadObj<Texture>("towers\\Path.json");
                    obj->setOffset(Vec2(x * 128 + gdx, y * 100 + gdy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Water)
                {
                    auto obj = loadObj<Texture>("towers\\Water.json");
                    obj->setOffset(Vec2(x * 128 + gdx, y * 100 + gdy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Sand)
                {
                    auto obj = loadObj<Texture>("towers\\Sand.json");
                    obj->setOffset(Vec2(x * 128 + gdx, y * 100 + gdy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Start)
                {
                    auto obj = loadObj<AnimGameObj>("towers\\Start.json");
                    obj->runAnimation("anim");
                    obj->setOffset(Vec2(x * 128 + gdx, y * 100 + gdy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Finish)
                {
                    auto obj = loadObj<AnimGameObj>("towers\\Finish.json");
                    obj->runAnimation("anim");
                    obj->setOffset(Vec2(x * 128 + gdx, y * 100 + gdy));
                    ground->addObject(obj);
                }
            }
        }

        greenRect = design->getChild<Filled>("#greenRect");
        redRect = design->getChild<Filled>("#redRect");
        range = design->getChild<Texture>("#range");

        restart();

        design->getChild<Button>("#build")->setCallback(
            bind(&MyApp::enterBuildMode, this));
    }

    void enterBuildMode()
    {
        buildMode = true;
    }

    void restart()
    {
        gameover = false;
        score = 0;

        buildMode = false;
        greenRect->setVisible(false);
        redRect->setVisible(false);
    }

    void move()
    {
        if (gameover)
            return;

        auto cpos = gv->viewCenter();
        if (input().isPressed(SpecialKey::Left))
            cpos.x -= 1600 * timeDelta();
        if (input().isPressed(SpecialKey::Right))
            cpos.x += 1600 * timeDelta();
        if (input().isPressed(SpecialKey::Down))
            cpos.y -= 1600 * timeDelta();
        if (input().isPressed(SpecialKey::Up))
            cpos.y += 1600 * timeDelta();
        gv->setViewCenter(cpos);

        if (buildMode && gv->isMouseOn())
        {
            auto mpos = gv->mouseCoords();
            int x = (mpos.x + w/2) / 128;
            int y = (mpos.y + h/2) / 100;
            if (gmap.get(x, y) == Grass && omap.get(x, y) == None)
            {
                greenRect->setVisible(true);
                greenRect->setOffset(x * 128 + dx, y * 100 + dy);
                redRect->setVisible(false);

                if (input().isJustOutpressed(MouseButton::Left))
                {
                    omap.set(x, y, Tower);
                    auto obj = loadObj<GameObj>("towers\\Tower.json");
                    obj->setOffset(Vec2(x * 128 + dx, y * 100 + dy));
                    objects->addObject(obj);
                    obj->setCallback(bind(&MyApp::selectTower, this, obj.get()));
                    auto& data = objects->data<TowerData>(obj);
                    data.level = 5;
                    data.minDamage = 1;
                    data.maxDamage = 2;
                    data.range = 10;
                }
            }
            else
            {
                redRect->setVisible(true);
                redRect->setOffset(x * 128 + dx, y * 100 + dy);
                greenRect->setVisible(false);
            }

            if (input().isJustOutpressed(MouseButton::Right))
            {
                buildMode = false;
                greenRect->setVisible(false);
                redRect->setVisible(false);
            }
        }
    }

    void selectTower(GameObj* tower)
    {
        cout << "Click!" << endl;
        range->setVisible(true);
        range->setOffset(tower->getOffset());

        auto& data = objects->data<TowerData>(tower);
        auto r = data.range;
        range->setFixedBox(256 * r, 200 * r);
        cout << "Range: " << r << endl;
        design->getChild<Label>("#level")->setText(toString(data.level));
    }

    shared_ptr<LinearLayout> design;
    GameMap<GroundType> gmap;
    GameMap<ObjType> omap;
    GameView* gv;
    ImmobileLayer* objects;

    Filled* greenRect;
    Filled* redRect;
    Texture* range;

    int score;
    bool gameover;

    bool buildMode;

    float dx;
    float dy;
    float w;
    float h;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
