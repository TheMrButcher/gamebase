#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    MyApp()
    {
        setDesign("jumpball\\Design.json");
    }

    void load()
    {
        randomize();
        connect0(design.child<Button>("restart"), restart);
        score = 0;
        record = 0;
        restart();
    }

    void restart()
    {
        gameover = false;
        ball.setPos(-450, 0);
        columns.setPos(0, 0);
        velocity = 0;
        score = 0;
        design.child<Label>("score").setText("0");

        columns.clear();
        for (int i = 0; i < 50; ++i)
        {
            auto column = loadObj<Texture>("jumpball\\Column.json");
            auto height = randomInt(100, columns.box().height() - 192);
            column.setSizes(64, height);
            column.setPos(i * 250, 0.5f * (height - columns.box().height()));
            columns.add(column);
        }
    }

    void processInput()
    {
        if (input.upJustPressed())
            velocity += 100;
    }

    void move()
    {
        if (gameover)
            return;

        auto columnsPos = columns.pos();
        auto ballPos = ball.pos();
        auto ballBox = ball.movedBox();

        int newScore = 0;
        int i = 1;

        feach (const auto& column, columns.all<Texture>())
        {
            auto columnBox = column.movedBox();
            columnBox.move(columnsPos);
            if (ballBox.intersects(columnBox))
                gameover = true;
            if (columnBox.topRight.x < ballBox.bottomLeft.x)
                newScore = i;
            ++i;
        }

        if (newScore == 50)
            gameover = true;

        if (ballBox.bottomLeft.y < columns.box().bottomLeft.y
            || ballBox.topRight.y > columns.box().topRight.y)
            gameover = true;

        if (score != newScore)
        {
            score = newScore;
            design.child<Label>("score").setText(toString(score));
        }

        if (gameover)
        {
            if (record < score)
            {
                record = score;
                design.child<Label>("record").setText(toString(record));
            }
            return;
        }

        auto time = timeDelta();

        columnsPos.x -= 200 * time;
        columns.setPos(columnsPos);

        ballPos.y += velocity * time;
        ball.setPos(ballPos);
        
        velocity -= 150 * time;
    }

    FromDesign(Texture, ball);
    FromDesign(Canvas, columns);

    float velocity;
    int score;
    int record;
    bool gameover;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
