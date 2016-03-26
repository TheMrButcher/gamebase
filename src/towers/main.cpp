#include <gamebase/engine/BasicTools.h>

using namespace gamebase;
using namespace std;

struct TowerData
{
    int level;
    int minDamage;
    int maxDamage;
    float range;
    Timer timer;
};

struct EnemyData
{
    int hp;
    Timer timer;
};

struct ArrowData
{
    int damage;
    int targetID;
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

        gv = design->getChild<GameView>("gv");
        gv->setGameBox(BoundingBox(gmap.width * 128, gmap.height * 100));
        auto* ground = gv->getLayer<StaticLayer>(0);
        objects = gv->getLayer<ImmobileLayer>(2);
        towers = gv->getLayer<ImmobileLayer>(3);
        enemies = gv->getLayer<Layer>(4);
        arrows = gv->getLayer<Layer>(5);

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
                    start = IntVec2(x, y);
                    gmap.set(x, y, Path);
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Finish)
                {
                    auto obj = loadObj<AnimGameObj>("towers\\Finish.json");
                    obj->runAnimation("anim");
                    obj->setOffset(Vec2(x * 128 + gdx, y * 100 + gdy));
                    finish = IntVec2(x, y);
                    gmap.set(x, y, Path);
                    ground->addObject(obj);
                }
            }
        }

        greenRect = design->getChild<Filled>("greenRect");
        redRect = design->getChild<Filled>("redRect");
        range = design->getChild<StaticGameObj>("range");

        findPath(IntVec2(-1, -1), start);
        restart();

        connect0(design->getChild<Button>("build"), enterBuildMode);
        connect0(design->getChild<Button>("upgrade"), upgradeTower);
    }

    void enterBuildMode()
    {
        buildMode = true;
    }

    void restart()
    {
        gameover = false;
        score = 0;
        money = 10;
        hp = 20;
        waveLevel = 0;
        curTower = nullptr;
        design->getChild<Button>("upgrade")->setVisible(false);
        design->getChild<CanvasLayout>("props")->setVisible(false);
        
        startWave();

        buildMode = false;
        greenRect->setVisible(false);
        redRect->setVisible(false);
    }

    void startWave()
    {
        isWave = true;
        leftToSpawn = 20;
        ++waveLevel;
        design->getChild<Label>("level")->setText(toString(waveLevel));
    }

    void move()
    {
        if (gameover)
            return;

        if (isWave)
        {
            if (timer.isPeriod(400))
            {
                auto enemy = loadObj<GameObj>("towers\\Enemy.json");
                enemy->setOffset(
                    start.x * 128 - w / 2 + 64 + randomFloat() * 128,
                    start.y * 100 - h / 2 + 50 + randomFloat() * 100);
                for (int i = 0; i < path.size(); ++i)
                    enemy->runAnimation(path[i]);
                enemy->runAnimation("move", 1);
                enemy->runAnimation("start", 2);
                enemies->addObject(enemy);
                auto& data = enemies->data<EnemyData>(enemy);
                data.hp = 3 * waveLevel;
                if (--leftToSpawn <= 0)
                    isWave = false;
            }
        }
        else
        {
            if (timer.isPeriod(12000))
                startWave();
        }

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
            if (gmap.get(x, y) == Grass && omap.get(x, y) == None && money >= 5)
            {
                greenRect->setVisible(true);
                greenRect->setOffset(x * 128 + dx, y * 100 + dy);
                redRect->setVisible(false);

                if (input().isJustOutpressed(MouseButton::Left))
                {
                    omap.set(x, y, Tower);
                    auto obj = loadObj<GameObj>("towers\\Tower.json");
                    obj->setOffset(Vec2(x * 128 + dx, y * 100 + dy));
                    towers->addObject(obj);
                    connect1(obj, selectTower, obj.get());
                    money -= 5;
                    design->getChild<Label>("money")->setText(toString(money));

                    auto& data = towers->data<TowerData>(obj);
                    data.level = 1;
                    data.minDamage = 1;
                    data.maxDamage = 2;
                    data.range = 2;
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

        if (!buildMode && gv->isMouseOn())
        {
            if (input().isJustOutpressed(MouseButton::Right))
            {
                curTower = nullptr;
                design->getChild<Button>("upgrade")->setVisible(false);
                design->getChild<CanvasLayout>("props")->setVisible(false);
                range->setVisible(false);
            }
        }

        feach(auto tower, towers->all<GameObj>())
        {
            auto& tdata = towers->data<TowerData>(tower);
            if (tdata.timer.isPeriod(1000))
            {
                Box rbox(256 * tdata.range, 200 * tdata.range);
                rbox.move(tower->getOffset());
                auto nearEnemies = enemies->findByBox<GameObj>(rbox);
                feach(auto enemy, nearEnemies)
                {
                    auto delta = enemy->getOffset() - tower->getOffset();
                    delta.y *= 256.0 / 200;
                    if (delta.length() > 128 * tdata.range || enemy->isChannelRunning(3))
                        continue;
                    auto arrow = loadObj<GameObj>("towers\\Arrow.json");
                    arrow->setOffset(tower->getOffset());
                    arrows->addObject(arrow);

                    auto& adata = arrows->data<ArrowData>(arrow);
                    adata.damage = randomInt(tdata.minDamage, tdata.maxDamage);
                    adata.targetID = enemy->id();
                    break;
                }
            }
        }

        feach(auto arrow, arrows->all<GameObj>())
        {
            auto& adata = arrows->data<ArrowData>(arrow);
            if (!enemies->hasObject(adata.targetID))
            {
                arrows->removeObject(arrow);
                continue;
            }

            auto enemy = enemies->getObject<GameObj>(adata.targetID);
            auto apos = arrow->getOffset();
            auto delta = enemy->getOffset() - apos;
            if (delta.length() < 16)
            {
                auto& edata = enemies->data<EnemyData>(enemy);
                edata.hp -= adata.damage;
                enemy->pauseChannel(0);
                edata.timer.start();
                if (edata.hp <= 0 && !enemy->isChannelRunning(3))
                {
                    enemy->runAnimation("exp", 3);
                    money++;
                    design->getChild<Label>("money")->setText(toString(money));
                }
                arrows->removeObject(arrow);
                continue;
            }

            arrow->setAngle(delta.angle());
            delta.normalize();
            delta *= timeDelta() * 500;
            apos += delta;
            arrow->setOffset(apos);
        }

        feach(auto enemy, enemies->all<GameObj>())
        {
            if (enemy->isChannelEmpty(0))
            {
                hp--;
                if (hp == 0)
                {
                    gameover = true;
                    design->getChild<Label>("gameover")->setVisible(true);
                    return;
                }
                enemies->removeObject(enemy);
                design->getChild<Label>("hp")->setText(toString(hp));
            }
            else if (enemy->isChannelPaused(0)) {
                auto& edata = enemies->data<EnemyData>(enemy);
                if (edata.timer.isPeriod(1000))
                    enemy->resumeChannel(0);
            }
        }
    }

    void selectTower(GameObj* tower)
    {
        if (buildMode)
            return;

        range->setVisible(true);
        range->setOffset(tower->getOffset());
        
        design->getChild<CanvasLayout>("props")->setVisible(true);
        auto& data = towers->data<TowerData>(tower);
        range->setScale(data.range);
        design->getChild<Label>("levelLabel")->setText(toString(data.level));
        design->getChild<Label>("damageLabel")->setText(toString(data.minDamage) + "-" + toString(data.maxDamage));
        design->getChild<Label>("rangeLabel")->setText(toString(data.range + 0.0001, 3));

        curTower = tower;
        design->getChild<Button>("upgrade")->setVisible(true);
    }

    void upgradeTower()
    {
        if (!curTower)
            return;
        
        auto& data = towers->data<TowerData>(curTower);
        if (money < data.level * 2 + 1)
            return;
        
        money -= data.level * 2 + 1;
        design->getChild<Label>("money")->setText(toString(money));

        data.level++;
        data.minDamage = data.level;
        data.maxDamage = data.level * 2;
        data.range += 0.2;
        selectTower(curTower);
    }

    bool findPath(IntVec2 prev, IntVec2 cur)
    {
        cout << "Cur: " << cur << endl;
        if (cur == finish)
            return true;

        IntVec2 steps[] = {
            IntVec2(0, 2),
            IntVec2(2, 0),
            IntVec2(0, -2),
            IntVec2(-2, 0)
        };

        string anims[] = {
            "up", "right", "down", "left"
        };

        for (int i = 0; i < 4; ++i)
        {
            auto next = cur + steps[i];
            if (prev != next && gmap.get(next) == Path)
            {
                path.push_back(anims[i]);
                if (findPath(cur, next))
                    return true;
                path.pop_back();
            }
        }
        return false;
    }

    shared_ptr<LinearLayout> design;
    GameMap<GroundType> gmap;
    GameMap<ObjType> omap;
    GameView* gv;
    ImmobileLayer* objects;
    ImmobileLayer* towers;
    Layer* enemies;
    Layer* arrows;

    Filled* greenRect;
    Filled* redRect;
    StaticGameObj* range;
    GameObj* curTower;

    int score;
    bool gameover;

    bool buildMode;

    float dx;
    float dy;
    float w;
    float h;

    IntVec2 start;
    IntVec2 finish;
    vector<string> path;

    Timer timer;
    bool isWave;
    int waveLevel;
    int leftToSpawn;

    int money;
    int hp;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
