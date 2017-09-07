#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
    void load()
    {
        randomize();
        record = 0;
        restart();

        connect(restartButton, restart);
    }

    void restart()
    {
        banana.hide();
        grapes.hide();
        kiwi.hide();
        portalIn.hide();
        portalOut.hide();

        gameoverLabel.hide();
        head = IntVec2(7, 7);
        dir = IntVec2(1, 0);
        timer.start();
        score = 0;
        scoreLabel.setText("0");

        circles.clear();
        auto circle = loadObj<GameObj>("snake/Circle.json");
        circle.setPos(Vec2(7 * 32 - 224, 7 * 32 - 224));
        circles.add(circle);
        placeApple();
    }

    void process(Input input)
    {
		using namespace gamebase::InputKey;

        if (input.pressed(Right))
            dir = IntVec2(1, 0);
        if (input.pressed(Left))
            dir = IntVec2(-1, 0);
        if (input.pressed(Up))
            dir = IntVec2(0, 1);
        if (input.pressed(Down))
            dir = IntVec2(0, -1);
    }

    void move()
    {
        if (timer.isPeriod(500))
        {
            auto newHead = head + dir;
            if (newHead.x < 0 || newHead.x > 14 || newHead.y < 0 || newHead.y > 14)
            {
                timer.stop();
                gameoverLabel.show();
                return;
            }

			Vec2 v(newHead.x * 32 - 224, newHead.y * 32 - 224);
			auto circle = loadObj<GameObj>("snake/Circle.json");
			circle.setPos(v);
			circles.add(circle);
			auto box = circle.box();
			circles.update();

            if (!circles.find(box).size() > 1)
            {
                timer.stop();
                gameoverLabel.show();
                return;
            }

            if (apple.box().intersects(box))
            {
                score++;
                if (score > record)
                {
                    record = score;
                    recordLabel.setText(toString(record));
                }
                placeApple();
            }

            head = newHead;
        }

        scoreLabel.setText(toString(score));
    }

    void placeApple()
    {
        for (;;)
        {
            int x = randomInt(0, 14);
            int y = randomInt(0, 14);
            Vec2 center(x * 32 - 224, y * 32 - 224);
            apple.setPos(center);
            if (circles.find(apple.box()).empty())
                return;
        }
    }

    FromDesign(GameObj, apple);
    FromDesign(GameObj, banana);
    FromDesign(GameObj, grapes);
    FromDesign(GameObj, kiwi);
    FromDesign(GameObj, portalIn);
    FromDesign(GameObj, portalOut);
    LayerFromDesign(void, circles);

    FromDesign(Button, restartButton);
    FromDesign(Label, scoreLabel);
    FromDesign(Label, recordLabel);
    FromDesign(Label, gameoverLabel);

    Timer timer;
    int score;
    int record;
    IntVec2 head;
    IntVec2 dir;
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("Config.json");
    app.setDesign("snake/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
