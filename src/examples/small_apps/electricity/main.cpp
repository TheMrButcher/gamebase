#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

enum Type
{
    Battery,
    Resistor,
    Parallel,
    Voltmeter,
    Ammeter,
    MAX_TYPE
};

struct Wire
{
    Wire()
    {
        to = -1;
    }

    int obj;
    int to;
    Vec2 offset;
    Line line;
};

struct Contacts
{
    Wire plus;
    Wire minus;
};

struct Data
{
    Type type;
    vector<int> contacts;
};

const string FNAMES[] = {
    "electricity\\Battery.json",
    "electricity\\Resistor.json",
    "",
    "electricity\\Voltmeter.json",
    "electricity\\Ammeter.json"
};

const string ONAMES[] = {
    "",
    "resistor",
    "parallel",
    "voltmeter",
    "ammeter"
};

const double NO_CON = -1;
const double HUGE_R = 1000000;

class MyApp : public App
{
public:
    MyApp()
    {
        setDesign("electricity\\Design.json");
    }

    void load()
    {
        nextID = 0;
        connect0(switchButton, switchPower);
        create(Battery, -200, 300);
        dragWireMode = false;
        for (int i = Resistor; i < MAX_TYPE; ++i)
        {
            auto layout = design.child<Layout>(ONAMES[i]);
            auto obj = layout.child<GameObj>("obj");
            connect2(obj, dragFromPanel, obj, Type(i));
        }

        U = 1;
        rIn = 0.001;

        uTextBox.setText("1");
        rInTextBox.setText("0.001");
    }

    void switchPower()
    {
        if (switchButton.isPressed())
        {
            double newU = toDouble(uTextBox.text());
            if (newU >= 0.0)
                U = newU;
            double newRIn = toDouble(rInTextBox.text());
            if (newRIn >= 0.0)
                rIn = newRIn;

            double rOut = resistance(0, -2, 0);
            cout << "Total R=" << rOut << endl;
            if (rOut < 0)
            {
                status.select(1);
                return;
            }
            if (rOut == 0)
                status.select(2);
            if (rOut > 0)
                status.select(3);
            double totalR = rIn + rOut;
            if (totalR == 0)
                return;
            double I = U / totalR;
            cout << "Total I=" << I << endl;
            setI(0, -2, 0, I);
        }
        else
        {
            setI(0, -2, 0, 0);
            status.select(0);
        }
    }

    double resistance(int start, int prev, int id)
    {
        auto& data = objects.data(contacts[id].plus.obj);
        double myR = 0;
        if (data.type == Parallel && data.contacts.front() == id)
        {
            auto id1 = data.contacts[1];
            auto id2 = data.contacts[2];
            auto r1 = resistance(id1, -2, id1);
            auto r2 = resistance(id2, -2, id2);

            if (r1 == NO_CON && r2 == NO_CON)
                return NO_CON;
            if (r1 == NO_CON)
                myR = r2;
            else if (r2 == NO_CON)
                myR = r1;
            else if (r1 != 0 && r2 != 0)
                myR = r1 * r2 / (r1 + r2);
        }

        if (data.type == Resistor)
            myR = 1;
        if (data.type == Voltmeter)
            myR = HUGE_R;

        auto& con = contacts[id];
        int otherID = con.minus.to == prev ? con.plus.to : con.minus.to;
        if (otherID == -1)
            return NO_CON;
        if (otherID == start)
            return myR;
        double otherR = resistance(start, id, otherID);
        if (otherR == NO_CON)
            return NO_CON;
        return myR + otherR;
    }

    void setI(int start, int prev, int id, double I)
    {
        auto& data = objects.data(contacts[id].plus.obj);
        if (data.type == Parallel && data.contacts.front() == id)
        {
            auto id1 = data.contacts[1];
            auto id2 = data.contacts[2];
            auto r1 = resistance(id1, -2, id1);
            auto r2 = resistance(id2, -2, id2);
            if (r1 == NO_CON && r2 == NO_CON)
                return;
            if (r1 == 0 && r2 == 0)
            {
                setI(id1, -2, id1, 0.5 * I);
                setI(id2, -2, id2, 0.5 * I);
                return;
            }

            if (r1 == NO_CON || r2 == 0)
            {
                setI(id2, -2, id2, I);
                return;
            }
            if (r2 == NO_CON || r1 == 0)
            {
                setI(id1, -2, id1, I);
                return;
            }
            double myU = I * r1 * r2 / (r1 + r2);
            setI(id1, -2, id1, myU / r1);
            setI(id2, -2, id2, myU / r2);
        }

        if (data.type == Voltmeter)
        {
            auto canvas = objects.get<Layout>(contacts[id].plus.obj);
            canvas.child<Label>("value").setText(toString(I * HUGE_R, 7) + " V");
        }

        if (data.type == Ammeter)
        {
            auto canvas = objects.get<Layout>(contacts[id].plus.obj);
            canvas.child<Label>("value").setText(toString(I, 7) + " A");
        }

        auto& con = contacts[id];
        int otherID = con.minus.to == prev ? con.plus.to : con.minus.to;
        if (otherID == -1)
            return;
        if (otherID == start)
            return;
        setI(start, id, otherID, I);
    }

    void create(Type type, float x, float y)
    {
        if (type != Parallel)
        {
            auto canvas = loadObj<Layout>(FNAMES[type]);
            canvas.setPos(x, y);
            int id = objects.add(canvas);
            connect1(canvas.child<GameObj>("obj"), drag, id);
            objects.data(id).type = type;
            addContacts(id, id, "minus", "plus");
            return;
        }

        auto left = loadObj<Layout>("electricity\\LeftParallel.json");
        left.setPos(x - 128, y);
        int lid = objects.add(left);
        connect1(left.child<GameObj>("obj"), drag, lid);

        auto right = loadObj<Layout>("electricity\\RightParallel.json");
        right.setPos(x + 128, y);
        int rid = objects.add(right);
        connect1(right.child<GameObj>("obj"), drag, rid);

        objects.data(lid).type = Parallel;
        objects.data(rid).type = Parallel;
        
        addContacts(rid, lid, "minus", "plus");
        addContacts(lid, rid, "minus1", "plus1");
        addContacts(lid, rid, "minus2", "plus2");
    }

    void addContacts(int mid, int pid, string mname, string pname)
    {
        auto conID = nextID++;
        auto mcan = objects.get<Layout>(mid);
        auto pcan = objects.get<Layout>(pid);
        auto minus = mcan.child<GameObj>(mname);
        connect2(minus, dragWire, conID, false);
        auto plus = pcan.child<GameObj>(pname);
        connect2(plus, dragWire, conID, true);

        auto& con = contacts[conID];
        con.minus.obj = mid;
        con.minus.offset = minus.pos();
        con.plus.obj = pid;
        con.plus.offset = plus.pos();

        auto& mdata = objects.data(mid);
        mdata.contacts.push_back(conID);
        if (mid != pid)
        {
            auto& pdata = objects.data(pid);
            pdata.contacts.push_back(conID);
        }
    }

    void processInput()
    {
        auto cpos = gv.view();
        if (input.leftPressed())
            cpos.x -= 400 * timeDelta();
        if (input.rightPressed())
            cpos.x += 400 * timeDelta();
        if (input.downPressed())
            cpos.y -= 400 * timeDelta();
        if (input.upPressed())
            cpos.y += 400 * timeDelta();
        gv.setView(cpos);
        
        if (dragWireMode)
        {
            tempWire.setP2(gv.mousePos());
            if (input.rightButtonJustOutpressed())
            {
                tempWire.hide();
                dragWireMode = false;
            }
        }
    }

    void drag(int id)
    {
        auto canvas = objects.get<Layout>(id);
        auto pos = gv.mousePos();
        canvas.setPos(pos);
        auto& data = objects.data(id);
        feach (int conID, data.contacts)
        {
            auto& con = contacts[conID];
            if (con.plus.line && con.plus.obj == id)
                con.plus.line.setP1(pos + con.plus.offset);
            if (con.minus.line && con.minus.obj == id)
                con.minus.line.setP2(pos + con.minus.offset);
        }
    }

    void removeWireData(int from, bool isFromPlus)
    {
        auto& con = contacts[from];
        auto& wire = isFromPlus ? con.plus : con.minus;
        wire.line = Line();
        wire.to = -1;
    }

    void removeWire(int from, bool isFromPlus)
    {
        auto& con = contacts[from];
        auto& wire = isFromPlus ? con.plus : con.minus;
        if (!wire.line)
            return;
        wires.remove(wire.line);
        removeWireData(wire.to, !isFromPlus);
        removeWireData(from, isFromPlus);
    }

    void setWireData(int from, int to, Wire& plus, Wire& minus, Line line)
    {
        auto& fcon = contacts[from];
        auto& tcon = contacts[to];
        auto fpos = objects.get<Layout>(fcon.plus.obj).pos();
        auto tpos = objects.get<Layout>(tcon.minus.obj).pos();
        plus.line = line;
        plus.to = to;
        minus.line = line;
        minus.to = from;
        line.setP1(fpos + plus.offset);
        line.setP2(tpos + minus.offset);
    }

    void createWire(int from, int to, bool isFromPlus)
    {
        auto& fcon = contacts[from];
        auto& tcon = contacts[to];
        auto line = loadObj<Line>("electricity\\Wire.json");
        wires.add(line);
        if (isFromPlus)
            setWireData(from, to, fcon.plus, tcon.minus, line);
        else
            setWireData(to, from, tcon.plus, fcon.minus, line);
    }

    void connect(int from, int to, bool isFromPlus)
    {
        auto& fcon = contacts[from];
        auto& tcon = contacts[to];
        if (!fcon.plus.line && tcon.minus.line)
            connect(to, from, !isFromPlus);
        if (!fcon.plus.line)
        {
            createWire(from, to, false);
            if (from != to)
                createWire(from, to, true);
            return;
        }
        if (tcon.minus.line)
            return;
        int third = isFromPlus ? fcon.plus.to : fcon.minus.to;
        removeWire(from, isFromPlus);
        createWire(from, to, isFromPlus);
        createWire(to, third, isFromPlus);
    }

    void disconnect(int id)
    {
        auto& con = contacts[id];
        if (con.plus.to == -1)
            return;
        if (con.plus.to == id)
        {
            removeWire(id, true);
            return;
        }

        int mid = con.plus.to;
        int pid = con.minus.to;
        removeWire(id, false);
        removeWire(id, true);

        if (pid == mid)
            return;
        
        createWire(mid, pid, false);
    }

    void dragWire(int id, bool isPlus)
    {
        if (switchButton.isPressed())
            return;
        auto& con = contacts[id];
        auto& wire = isPlus ? con.plus : con.minus;
        if (dragWireMode)
        {
            if (id == wireFrom && isPlus == isWireFromPlus)
            {
                disconnect(id);
            }
            else
            {
                if (isPlus == isWireFromPlus)
                    return;

                int fobj = isPlus ? contacts[wireFrom].minus.obj : contacts[wireFrom].plus.obj;
                int tobj = isPlus ? con.plus.obj : con.minus.obj;
                if (fobj == tobj)
                    return;

                connect(wireFrom, id, isWireFromPlus);
            }
            tempWire.hide();
            dragWireMode = false;
        }
        else
        {
            wireFrom = id;
            isWireFromPlus = isPlus;
            tempWire.show();
            tempWire.setP1(objects.get<Layout>(isPlus ? con.plus.obj : con.minus.obj).pos() + wire.offset);
            tempWire.setP2(gv.mousePos());
            dragWireMode = true;
        }
    }

    void dragFromPanel(GameObj obj, Type type)
    {
        if (switchButton.isPressed())
            return;
        auto opos = obj.pos();
        auto posOnScreen = input.mousePos();
        if (opos.isZero())
        {
            startDragVec = posOnScreen;
            opos.y = 1;
        }
        else
        {
            opos = posOnScreen - startDragVec;
        }

        if (!obj.isPressed())
        {
            auto posOnView = gv.mousePos();
            create(type, posOnView.x, posOnView.y);
            opos = Vec2();
        }
        obj.setPos(opos);
    }

    FromDesign2(ToggleButton, switchButton, "switch");
    FromDesign(Selector, status);

    FromDesign(GameView, gv);
    LayerFromDesign(void, wires);
    LayerFromDesign2(Data, objects, "objs");

    FromDesign2(Line, tempWire, "wire");
    bool dragWireMode;
    int wireFrom;
    bool isWireFromPlus;

    Vec2 startDragVec;

    int nextID;
    map<int, Contacts> contacts;

    double U;
    double rIn;

    FromDesign2(TextBox, uTextBox, "U");
    FromDesign2(TextBox, rInTextBox, "rIn");
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
