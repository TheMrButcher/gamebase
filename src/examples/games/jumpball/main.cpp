#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    void load()
    {
        randomize();
        connect(design.child<Button>("restart"), restart);
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
		design.child<Label>("score") << 0;

        columns.clear();
        for (int i = 0; i < 50; ++i)
        {
			auto height = randomInt(100, columns.box().height() - 192);
			auto column = columns.load<Texture>("jumpball\\Column.json");
            column.setSize(64, height);
			column.setPos(i * 250, 0.5f * (height - columns.height()));
			cout << column.width() << " x " << column.height() << endl;
        }
		columns.update();
    }

    void process(Input input)
    {
		using namespace InputKey;
        if (input.justPressed(Up))
            velocity += 100;
    }

    void move()
    {
        if (gameover)
            return;

        auto columnsPos = columns.pos();
        auto ballPos = ball.pos();
        auto ballBox = ball.box();

        int newScore = 0;
        int i = 1;

        for (auto column : columns.all<Texture>())
        {
            auto columnBox = column.box();
            columnBox.move(columnsPos);
            if (ballBox.intersects(columnBox))
                gameover = true;
            if (columnBox.r < ballBox.l)
                newScore = i;
            ++i;
        }

        if (newScore == 50)
            gameover = true;

        if (ballBox.b < columns.box().b || ballBox.t > columns.box().t)
            gameover = true;

        if (score != newScore)
        {
            score = newScore;
            design.child<Label>("score") << score;
        }

        if (gameover)
        {
            if (record < score)
            {
                record = score;
                design.child<Label>("record") << record;
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
    FromDesign(Layout, columns);

    float velocity;
    int score;
    int record;
    bool gameover;
};

int main(int argc, char** argv)
{
    MyApp app;
	app.setDesign("jumpball\\Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
