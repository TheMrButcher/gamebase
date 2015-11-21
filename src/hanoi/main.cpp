#include <gamebase/engine/BasicTools.h>
#include <gamebase/engine/AnimatedButtonSkin.h>

using namespace gamebase;
using namespace std;

static const int DISK_SIZE = 32;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        randomize();

        design = deserialize<LinearLayout>("hanoi\\Design.json");
        m_view->addObject(design);

        canvas = design->getChild<CanvasLayout>("#main_canvas");
        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));
        design->getChild<Button>("#ai")->setCallback(bind(&MyApp::enterAIMode, this));
        design->getChild<ComboBox>("#tower_height")->setText("3");

        aiMode = false;
    }

    void postload()
    {
        for (int i = 0; i < 3; ++i)
        {
            auto indexStr = toString(i);
            auto tower = design->getChild<CanvasLayout>("#tower" + indexStr);
            tower->getChild<Button>("#base")->setCallback(bind(&MyApp::selectStick, this, i));
            tower->getChild<Button>("#stick")->setCallback(bind(&MyApp::selectStick, this, i));
            auto offset = tower->getOffset();
            offset.y += 64 + 0.5 * DISK_SIZE - 0.5 * tower->height();
            offsets[i] = offset;
            yTop = tower->getOffset().y + 0.5 * tower->height() + DISK_SIZE;
        }
        cout << yTop << endl;

        restart();
    }

    void restart()
    {
        step = 0;
        design->getChild<StaticLabel>("#step")->setText("0");
        canvas->clear();
        hanoiHeight = toInt(design->getChild<ComboBox>("#tower_height")->text());
        Vec2 offset = offsets[0];
        for (int i = 0; i < 3; ++i)
            disks[i].clear();
        diskToMove = 0;
        moveState = None;
        solved = false;
        design->getChild<StaticLabel>("#solved")->setVisible(false);

        for (int i = 0; i < hanoiHeight; ++i)
        {
            auto disk = deserialize<Button>("hanoi\\Disk.json");
            float width = 128 + 0.5 * DISK_SIZE * (hanoiHeight - i);
            disk->getChild<AnimatedButtonSkin>("#skin")->setFixedBox(width, DISK_SIZE);
            disk->setOffset(offset);
            disk->getChild<StaticTextureRect>("#left_light")->setVisible(false);
            disk->getChild<StaticTextureRect>("#mid_light")->setVisible(false);
            disk->getChild<StaticTextureRect>("#right_light")->setVisible(false);
            Color color(randomFloat(), randomFloat(), randomFloat());
            disk->getChild<StaticTextureRect>("#left")->setColor(color);
            disk->getChild<StaticTextureRect>("#mid")->setColor(color);
            disk->getChild<StaticTextureRect>("#right")->setColor(color);
            disk->setCallback(bind(&MyApp::selectDisk, this, disk.get()));
            canvas->addObject(disk);
            offset.y += DISK_SIZE;
            disks[0].push_back(disk.get());
        }
    }

    void enterAIMode()
    {
        restart();

        aiMode = true;
        solutionStep = 0;
        solution.clear();
        generateSolution(0, 2, hanoiHeight);
    }

    void generateSolution(int from, int to, int size)
    {
        if (size == 1)
        {
            solution.push_back(make_pair(from, to));
            return;
        }

        int thirdTower = (from + 1) % 3;
        if (thirdTower == to)
            thirdTower = (thirdTower + 1) % 3;
        
        generateSolution(from, thirdTower, size - 1);
        solution.push_back(make_pair(from, to));
        generateSolution(thirdTower, to, size - 1);

        selectDisk(disks[solution[0].first].back());
        selectStick(solution[0].second);
    }

    void selectStick(int index)
    {
        if (solved || moveState != None)
            return;
        if (!diskToMove)
            return;
        if (!disks[index].empty() && disks[index].back()->width() < diskToMove->width())
            return;
        to = index;
        moveState = Up;
        removeLight(diskToMove);
        ++step;
        design->getChild<StaticLabel>("#step")->setText(boost::lexical_cast<string>(step));
    }

    void selectDisk(Button* disk)
    {
        if (solved || moveState != None)
            return;
        removeLight(diskToMove);
        for (int i = 0; i < 3; ++i)
        {
            if (!disks[i].empty() && disks[i].back() == disk)
            {
                from = i;
                diskToMove = disk;
                if (!aiMode)
                {
                    diskToMove->getChild<StaticTextureRect>("#left_light")->setVisible(true);
                    diskToMove->getChild<StaticTextureRect>("#mid_light")->setVisible(true);
                    diskToMove->getChild<StaticTextureRect>("#right_light")->setVisible(true);
                }
                return;
            }
        }
    }

    void removeLight(Button* disk)
    {
        if (disk)
        {
            disk->getChild<StaticTextureRect>("#left_light")->setVisible(false);
            disk->getChild<StaticTextureRect>("#mid_light")->setVisible(false);
            disk->getChild<StaticTextureRect>("#right_light")->setVisible(false);
        }
    }

    void move()
    {
        if (!diskToMove || moveState == None)
            return;
        float time = timeDelta();
        auto pos = diskToMove->getOffset();
        auto finishPosY = disks[to].size() * DISK_SIZE + offsets[to].y;
        switch (moveState)
        {
        case Up:
            pos.y += 1000 * time;
            if (pos.y > yTop)
            {
                pos.y = yTop;
                moveState = Hor;
            }
            break;

        case Hor:
            if (from < to)
            {
                pos.x += 1000 * time;
                if (pos.x > offsets[to].x)
                {
                    pos.x = offsets[to].x;
                    moveState = Down;
                }
            }
            else
            {
                pos.x -= 1000 * time;
                if (pos.x < offsets[to].x)
                {
                    pos.x = offsets[to].x;
                    moveState = Down;
                }
            }
            break;

        case Down:
            pos.y -= 1000 * time;
            if (pos.y < finishPosY)
            {
                pos.y = finishPosY;
                moveState = None;
                disks[from].pop_back();
                disks[to].push_back(diskToMove);

                if (disks[2].size() == hanoiHeight)
                {
                    solved = true;
                    design->getChild<StaticLabel>("#solved")->setVisible(true);
                }
            }
            break;
        }

        diskToMove->setOffset(pos);
        if (moveState == None) {
            diskToMove = 0;
            if (aiMode)
            {
                ++solutionStep;
                if (solutionStep < solution.size())
                {
                    selectDisk(disks[solution[solutionStep].first].back());
                    selectStick(solution[solutionStep].second);
                }
            }
        }
    }

    shared_ptr<LinearLayout> design;
    CanvasLayout* canvas;
    Vec2 offsets[3];
    float yTop;
    vector<Button*> disks[3];
    int step;

    Button* diskToMove;
    enum MoveState
    {
        None,
        Up,
        Hor,
        Down
    };
    MoveState moveState;
    int from;
    int to;

    int hanoiHeight;
    bool solved;

    bool aiMode;
    vector<pair<int, int>> solution;
    int solutionStep;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
