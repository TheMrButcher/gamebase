#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
    void load()
    {
        connect(restartButton, restart);
        restart();
    }

    void restart()
    {
		// »нициализаци€ состо€ни€ синего игрока
        blue.setPos(0, 0.4 * field.height());
        blueWinLabel.hide();
		blueScore = 0;
        blueScoreLabel.setText("0");
        
		// »нициализаци€ состо€ни€ красного игрока
        red.setPos(0, -0.4 * field.height());
        redWinLabel.hide();
		redScore = 0;
        redScoreLabel.setText("0");
		
		// —тарт игры
		gameover = false;
		gameDir = -1;
		nextGame();
    }

    void process(Input input)
    {
		using namespace InputKey;

		// ”правление красной платформой
        auto redPos = red.pos();
        auto redBox = red.box();
		auto gbox = field.gameBox();
        auto delta = timeDelta() * 400;
		if (input.pressed('a') && redBox.l > gbox.l)
            redPos.x -= delta;
        if (input.pressed('d') && redBox.r < gbox.r)
			redPos.x += delta;
        red.setPos(redPos);

		// ”правление синей платформой
        auto bluePos = blue.pos();
        auto blueBox = blue.box();
        if (input.pressed(Left) && blueBox.l > gbox.l)
            bluePos.x -= delta;
        if (input.pressed(Right) && blueBox.r < gbox.r)
            bluePos.x += delta;
        blue.setPos(bluePos);
    }

    void move()
    {
		if (gameover)
			return;

		// ƒвижение м€ча
		ball.move(velo * timeDelta());

		auto bbox = ball.box();
		auto gbox = field.gameBox();
		// ћ€ч столкнулс€ со стеной
		if (bbox.l < gbox.l && velo.x < 0)
			velo.x = -velo.x;
		if (bbox.r > gbox.r && velo.x > 0)
			velo.x = -velo.x;

		// ћ€ч столкнулс€ с платформой
		if (bbox.intersects(red) && velo.y < 0)
			velo.y = -velo.y;
		if (bbox.intersects(blue) && velo.y > 0)
			velo.y = -velo.y;

		// ћ€ч улетел вниз, синий набрал очко
		if (bbox.t < gbox.b)
		{
			blueScore++;
			blueScoreLabel << blueScore;
			if (blueScore == 10)
			{
				blueWinLabel.show();
				gameover = true;
			}
			else
			{
				nextGame();
			}
		}

		// ћ€ч улетел наверх, красный набрал очко
		if (bbox.b > gbox.t)
		{
			redScore++;
			redScoreLabel << redScore;
			if (redScore == 10)
			{
				redWinLabel.show();
				gameover = true;
			}
			else
			{
				nextGame();
			}
		}
    }

	// —тарт следующего розыгрыша м€ча
	void nextGame()
	{
		//  аждый ход начальное направление полета мен€етс€ на обратное
		gameDir = -gameDir;
		velo = gameDir * Vec2(200, 200);
		ball.setPos(0, 0);
	}

    FromDesign(Button, restartButton);

    FromDesign(GameView, field);
    FromDesign(GameObj, ball);

    FromDesign(GameObj, blue);
    FromDesign(Label, blueWinLabel);
    FromDesign(Label, blueScoreLabel);
    
    FromDesign(GameObj, red);
    FromDesign(Label, redWinLabel);
    FromDesign(Label, redScoreLabel);

	bool gameover;
	Vec2 velo;
	float gameDir;

	int blueScore;
	int redScore;
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("config.json");
    app.setDesign("tennis/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
