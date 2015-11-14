#include <gamebase/engine/SimpleApplication.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/AnimatedButtonSkin.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/StaticTextureRect.h>
#include <gamebase/engine/ComboBox.h>
#include <gamebase/engine/Timer.h>
#include <gamebase/math/IntVector.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <boost/lexical_cast.hpp>
#include <Windows.h>

using namespace gamebase;
using namespace std;

static const int DISK_SIZE = 32;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        design = deserialize<LinearLayout>("hanoi\\Design.json");
        m_view->addObject(design);

        canvas = design->getChild<CanvasLayout>("#main_canvas");
        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));
        design->getChild<ComboBox>("#tower_height")->setText("3");
    }

    void postload()
    {
        for (int i = 0; i < 3; ++i)
        {
            auto indexStr = boost::lexical_cast<string>(i);
            auto tower = design->getChild<CanvasLayout>("#tower" + indexStr);
            tower->getChild<Button>("#base")->setCallback(bind(&MyApp::selectStick, this, i));
            tower->getChild<Button>("#stick")->setCallback(bind(&MyApp::selectStick, this, i));
            auto offset = tower->position().offset;
            offset.y += 64 + 0.5 * DISK_SIZE - 0.5 * tower->box().height();
            offsets[i] = offset;
            yTop = tower->position().offset.y + 0.5 * tower->box().height() + DISK_SIZE;
        }
        cout << yTop << endl;

        restart();
    }

    void restart()
    {
        step = 0;
        design->getChild<StaticLabel>("#step")->setTextAndLoad("0");
        canvas->clear();
        hanoiHeight = boost::lexical_cast<int>(design->getChild<ComboBox>("#tower_height")->text());
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
            disk->setPosition(std::make_shared<FixedOffset>(offset));
            disk->getChild<StaticTextureRect>("#left_light")->setVisible(false);
            disk->getChild<StaticTextureRect>("#mid_light")->setVisible(false);
            disk->getChild<StaticTextureRect>("#right_light")->setVisible(false);
            Color color((rand() % 256) / 255.0, (rand() % 256) / 255.0, (rand() % 256) / 255.0);
            disk->getChild<StaticTextureRect>("#left")->setColor(color);
            disk->getChild<StaticTextureRect>("#mid")->setColor(color);
            disk->getChild<StaticTextureRect>("#right")->setColor(color);
            disk->setCallback(bind(&MyApp::selectDisk, this, disk.get()));
            canvas->addObject(disk);
            offset.y += DISK_SIZE;
            disks[0].push_back(disk.get());
        }
    }

    void selectStick(int index)
    {
        if (solved || moveState != None)
            return;
        if (!diskToMove)
            return;
        if (!disks[index].empty() && disks[index].back()->box().width() < diskToMove->box().width())
            return;
        to = index;
        moveState = Up;
        removeLight(diskToMove);
        ++step;
        design->getChild<StaticLabel>("#step")->setTextAndLoad(boost::lexical_cast<string>(step));
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
                diskToMove->getChild<StaticTextureRect>("#left_light")->setVisible(true);
                diskToMove->getChild<StaticTextureRect>("#mid_light")->setVisible(true);
                diskToMove->getChild<StaticTextureRect>("#right_light")->setVisible(true);
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
        float time = TimeState::realTime().delta / 1000.0;
        auto pos = diskToMove->offset<FixedOffset>()->get();
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

        diskToMove->offset<FixedOffset>()->set(pos);
        if (moveState == None)
            diskToMove = 0;
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
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
