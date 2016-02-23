#include <gamebase/engine/BasicTools.h>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        design = loadObj<CanvasLayout>("pendulum\\Design.json");
        setView(design);

        gv = design->getChild<GameView>("#gv");
        pendulum = design->getChild<GameObj>("#pendulum");
        pendulum->setCallback(bind(&MyApp::dragPendulum, this));
        
        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));

        startAngleBox = design->getChild<TextBox>("#startAngle");
        frictionBox = design->getChild<TextBox>("#friction");
        barLenBox = design->getChild<TextBox>("#barLen");

        angleLabel = design->getChild<Label>("#angleLabel");
        veloLabel = design->getChild<Label>("#velo");
        potentLabel = design->getChild<Label>("#potent");
        kineticLabel = design->getChild<Label>("#kinetic");
        fullLabel = design->getChild<Label>("#full");
        frictionCoef = 0.25;
        barLen = 8;

        startAngleBox->setText("0");
        frictionBox->setText(toString(frictionCoef));
        barLenBox->setText(toString(barLen));

        restart();
    }

    void restart()
    {
        frictionCoef = toDouble(frictionBox->text());
        auto newBarLen = toDouble(barLenBox->text());
        if (newBarLen > 0)
            barLen = newBarLen;

        angle = toDouble(startAngleBox->text()) / 180 * 3.141592653;
        potentialEnergy = barLen * 9.81 * (1 - cos(angle));
        angleVelo = 0;
        kineticEnergy = 0;
        isDragged = false;
        pendulum->setAngle(angle);

        timer.start();
    }

    void move()
    {
        if (!isDragged)
        {
            auto angleAcc = -9.81 * sin(angle) / barLen;
            angleVelo += angleAcc * timeDelta();

            auto velo = barLen * angleVelo;
            
            auto heightDelta = velo * timeDelta() * sin(angle);
            auto energyDelta = -9.81 * heightDelta;

            if (energyDelta > 0 && energyDelta > potentialEnergy)
                energyDelta = potentialEnergy;
            if (energyDelta < 0 && -energyDelta > kineticEnergy)
                energyDelta = -kineticEnergy;
            potentialEnergy -= energyDelta;
            kineticEnergy += energyDelta;
            
            if (velo < 0)
                velo = -sqrt(2 * kineticEnergy);
            else
                velo = sqrt(2 * kineticEnergy);
            velo *= 1 - frictionCoef * timeDelta();
            kineticEnergy = velo * velo / 2;
            angleVelo = velo / barLen;
            angle += angleVelo * timeDelta();
            pendulum->setAngle(angle);
        }

        if (timer.isPeriod(50))
        {
            angleLabel->setText(toString(angle / 3.141592653 * 180, 7));
            veloLabel->setText(toString(angleVelo * barLen, 7));
            potentLabel->setText(toString(potentialEnergy, 7));
            kineticLabel->setText(toString(kineticEnergy, 7));
            fullLabel->setText(toString(potentialEnergy + kineticEnergy, 7));
        }
    }

    void dragPendulum()
    {
        isDragged = pendulum->isPressed();
        auto mpos = gv->mouseCoords();
        auto delta = mpos - pendulum->getOffset();
        angle = delta.angle() + 1.68;
        pendulum->setAngle(angle);
        angleVelo = 0;
        potentialEnergy = barLen * 9.81 * (1 - cos(angle));
    }

    shared_ptr<CanvasLayout> design;
    GameView* gv;
    GameObj* pendulum;
    bool isDragged;
    
    double angle;
    double frictionCoef;
    double barLen;

    double angleVelo;
    double potentialEnergy;
    double kineticEnergy;

    TextBox* startAngleBox;
    TextBox* frictionBox;
    TextBox* barLenBox;

    Label* angleLabel;
    Label* veloLabel;
    Label* potentLabel;
    Label* kineticLabel;
    Label* fullLabel;

    Timer timer;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
