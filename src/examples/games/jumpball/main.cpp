#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
    void load()
    {
        randomize();

        record = 0;
        recordLabel << record;
        restart();

        connect(restartButton, restart);
    }

    void restart()
    {
        gameover = false;
        score = 0;
        coinScore = 0;
        scoreLabel << 0;
        ball.setPos(-0.4 * field.width(), 0);
        velo = Vec2(200, 0);
        gameoverLabel.hide();

        columns.clear();
        clouds.clear();
        coins.clear();
        nextColumn = 0;
        nextCloud = 0;
        fillWorld();
    }

    void process(Input input)
    {
        using namespace InputKey;
        if (input.pressed(Up) || input.pressed('w'))
            velo.y += 1700 * timeDelta();
    }

    void move()
    {
        if (gameover)
            return;

        auto bpos = ball.move(velo * timeDelta());
        velo.y -= 500 * timeDelta();

        auto camera = bpos;
        camera.x += 0.4 * field.width();
        camera.y = 0;
        field.setView(camera);

        auto fbox = field.viewBox();
        auto bbox = ball.box();
        if (!bbox.intersects(fbox))
        {
            gameover = true;
            gameoverLabel.show();
        }

        for (auto column : columns.all())
        {
            if (column.intersects(bbox))
            {
                gameover = true;
                gameoverLabel.show();
            }
            if (column.box().r + 50 < field.viewBox().l)
                columns.remove(column);
        }

        for (auto cloud : clouds.all())
        {
            cloud.move(100 * timeDelta(), 0);
            if (cloud.box().r < field.viewBox().l)
                clouds.remove(cloud);
        }

        for (auto coin : coins.all())
        {
            if (coin.intersects(bbox))
            {
                coinScore++;
                coins.remove(coin);
                continue;
            }
            if (coin.box().r < field.viewBox().l)
                coins.remove(coin);
        }

        if (bpos.x > 0)
            score = bpos.x / 250 + 1;
        int totalScore = score + coinScore;
        scoreLabel << totalScore;
        if (totalScore > record)
        {
            record = totalScore;
            recordLabel.setText(toString(record));
        }

        fillWorld();
    }

    void fillWorld()
    {
        auto box = field.viewBox().move(field.width(), 0);
        while (columns.find(box).empty())
        {
            GameObj column;
            int ch = randomInt(150, 400);
            float x = 250 * nextColumn;
            if (nextColumn % 2 == 0)
            {
                column = columns.load("jumpball/Column.json");
                column.setPos(x, -box.h/2 + ch/2);
            }
            else
            {
                column = columns.load("jumpball/ColumnTop.json");
                column.setPos(x, +box.h/2 - ch/2);
            }
            column.setSize(50, ch);

            int coinY = randomInt(-box.h / 4, box.h / 4);
            coins.load("jumpball/Coin.json", x + randomInt(20, 230), coinY);

            ++nextColumn;
        }

        while (clouds.find(box).empty())
        {
            auto x = nextCloud * 300 + randomInt(-100, 100);
            auto y = randomInt(150, box.h / 2);
            auto cloud = clouds.load("jumpball/Cloud.json", x, y);
            cloud.setScale(randomFloat() + 0.5);

            ++nextCloud;
        }
    }

    FromDesign(GameView, field);
    FromDesign(GameObj, ball);
    LayerFromDesign(void, columns);
    LayerFromDesign(void, clouds);
    LayerFromDesign(void, coins);

    FromDesign(Label, scoreLabel);
    FromDesign(Label, recordLabel);
    FromDesign(Label, gameoverLabel);
    FromDesign(Button, restartButton);

    int score;
    int coinScore;
    int record;
    Vec2 velo;
    bool gameover;
    int nextColumn;
    int nextCloud;
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("config.json");
    app.setDesign("jumpball/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
