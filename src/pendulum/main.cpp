#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    MyApp()
    {
        setDesign("pendulum\\Design.json");
    }

    void load()
    {
        connect0(pendulum, dragPendulum);
        connect0(design.child<Button>("restart"), restart);
        frictionCoef = 0.25;
        barLen = 8;
        startAngleBox.setText("0");
        frictionBox.setText(toString(frictionCoef));
        barLenBox.setText(toString(barLen));

        restart();
    }

    void restart()
    {
        frictionCoef = toDouble(frictionBox.text());
        auto newBarLen = toDouble(barLenBox.text());
        if (newBarLen > 0)
            barLen = newBarLen;

        angle = toDouble(startAngleBox.text()) / 180 * 3.141592653;
        potentialEnergy = barLen * 9.81 * (1 - cos(angle));
        angleVelo = 0;
        kineticEnergy = 0;
        isDragged = false;
        pendulum.setAngle(angle);

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
            pendulum.setAngle(angle);
        }

        if (timer.isPeriod(50))
        {
            angleLabel.setText(toString(angle / 3.141592653 * 180, 7));
            veloLabel.setText(toString(angleVelo * barLen, 7));
            potentLabel.setText(toString(potentialEnergy, 7));
            kineticLabel.setText(toString(kineticEnergy, 7));
            fullLabel.setText(toString(potentialEnergy + kineticEnergy, 7));        }
    }

    void dragPendulum()
    {
        isDragged = pendulum.isPressed();
        auto mpos = gv.mousePos();
        auto delta = mpos - pendulum.pos();
        angle = delta.angle() + 1.68;
        pendulum.setAngle(angle);
        angleVelo = 0;
        potentialEnergy = barLen * 9.81 * (1 - cos(angle));
        kineticEnergy = 0;
    }

    FromDesign(GameView, gv);
    FromDesign(GameObj, pendulum);

    bool isDragged;
    
    double angle;
    double frictionCoef;
    double barLen;

    double angleVelo;
    double potentialEnergy;
    double kineticEnergy;

    FromDesign2(TextBox, startAngleBox, "startAngle");
    FromDesign2(TextBox, frictionBox, "friction");
    FromDesign2(TextBox, barLenBox, "barLen");

    FromDesign(Label, angleLabel);
    FromDesign2(Label, veloLabel, "velo");
    FromDesign2(Label, potentLabel, "potent");
    FromDesign2(Label, kineticLabel, "kinetic");
    FromDesign2(Label, fullLabel, "full");

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
