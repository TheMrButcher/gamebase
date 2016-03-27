#include <gamebase/engine/BasicTools.h>

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
        line = 0;
    }

    int obj;
    int to;
    Vec2 offset;
    Line* line;
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

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        design = loadObj<CanvasLayout>("electricity\\Design.json");
        setView(design);

        status = design->getChild<Selector>("status");

        gv = design->getChild<GameView>("gv");
        wires = gv->getLayer<Layer>(0);
        objects = gv->getLayer<Layer>(1);

        tempWire = gv->getChild<Line>("wire");

        nextID = 0;

        switchButton = design->getChild<ToggleButton>("switch");
        connect0(switchButton, switchPower);

        create(Battery, -200, 300);

        dragWireMode = false;

        for (int i = Resistor; i < MAX_TYPE; ++i)
        {
            auto canvas = design->getChild<CanvasLayout>(ONAMES[i]);
            auto obj = canvas->getChild<GameObj>("obj");
            connect2(obj, dragFromPanel, obj, Type(i));
        }

        U = 1;
        rIn = 0.001;

        uTextBox = design->getChild<TextBox>("U");
        rInTextBox = design->getChild<TextBox>("rIn");
        uTextBox->setText("1");
        rInTextBox->setText("0.001");
    }

    void switchPower()
    {
        if (switchButton->isPressed())
        {
            double newU = toDouble(uTextBox->text());
            if (newU >= 0.0)
                U = newU;
            double newRIn = toDouble(rInTextBox->text());
            if (newRIn >= 0.0)
                rIn = newRIn;

            double rOut = resistance(0, -2, 0);
            cout << "Total R=" << rOut << endl;
            if (rOut < 0)
            {
                status->select(1);
                return;
            }
            if (rOut == 0)
                status->select(2);
            if (rOut > 0)
                status->select(3);
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
            status->select(0);
        }
    }

    double resistance(int start, int prev, int id)
    {
        auto& data = objects->data<Data>(contacts[id].plus.obj);
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
        auto& data = objects->data<Data>(contacts[id].plus.obj);
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
            auto canvas = objects->getObject<CanvasLayout>(contacts[id].plus.obj);
            canvas->getChild<Label>("value")->setText(toString(I * HUGE_R, 7) + " V");
        }

        if (data.type == Ammeter)
        {
            auto canvas = objects->getObject<CanvasLayout>(contacts[id].plus.obj);
            canvas->getChild<Label>("value")->setText(toString(I, 7) + " A");
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
            auto canvas = loadObj<CanvasLayout>(FNAMES[type]);
            canvas->setOffset(x, y);
            int id = objects->addObject(canvas);
            connect1(canvas->getChild<GameObj>("obj"), drag, id);

            objects->data<Data>(id).type = type;
            
            addContacts(id, id, "minus", "plus");
            return;
        }

        auto left = loadObj<CanvasLayout>("electricity\\LeftParallel.json");
        left->setOffset(x - 128, y);
        int lid = objects->addObject(left);
        connect1(left->getChild<GameObj>("obj"), drag, lid);

        auto right = loadObj<CanvasLayout>("electricity\\RightParallel.json");
        right->setOffset(x + 128, y);
        int rid = objects->addObject(right);
        connect1(right->getChild<GameObj>("obj"), drag, rid);

        objects->data<Data>(lid).type = Parallel;
        objects->data<Data>(rid).type = Parallel;
        
        addContacts(rid, lid, "minus", "plus");
        addContacts(lid, rid, "minus1", "plus1");
        addContacts(lid, rid, "minus2", "plus2");
    }

    void addContacts(int mid, int pid, string mname, string pname)
    {
        auto conID = nextID++;
        auto mcan = objects->getObject<CanvasLayout>(mid);
        auto pcan = objects->getObject<CanvasLayout>(pid);
        auto minus = mcan->getChild<GameObj>(mname);
        connect2(minus, dragWire, conID, false);
        auto plus = pcan->getChild<GameObj>(pname);
        connect2(plus, dragWire, conID, true);

        auto& con = contacts[conID];
        con.minus.obj = mid;
        con.minus.offset = minus->getOffset();
        con.plus.obj = pid;
        con.plus.offset = plus->getOffset();

        auto& mdata = objects->data<Data>(mid);
        mdata.contacts.push_back(conID);
        if (mid != pid)
        {
            auto& pdata = objects->data<Data>(pid);
            pdata.contacts.push_back(conID);
        }
    }
    
    void move()
    {
        auto cpos = gv->viewCenter();
        if (input().isPressed(SpecialKey::Left))
            cpos.x -= 400 * timeDelta();
        if (input().isPressed(SpecialKey::Right))
            cpos.x += 400 * timeDelta();
        if (input().isPressed(SpecialKey::Down))
            cpos.y -= 400 * timeDelta();
        if (input().isPressed(SpecialKey::Up))
            cpos.y += 400 * timeDelta();
        gv->setViewCenter(cpos);

        if (dragWireMode)
        {
            tempWire->setEnd(gv->mouseCoords());
            if (input().isJustOutpressed(MouseButton::Right))
            {
                tempWire->setVisible(false);
                dragWireMode = false;
            }
        }
    }

    void drag(int id)
    {
        auto canvas = objects->getObject<CanvasLayout>(id);
        auto pos = gv->mouseCoords();
        canvas->setOffset(pos);
        auto& data = objects->data<Data>(id);
        feach (int conID, data.contacts)
        {
            auto& con = contacts[conID];
            if (con.plus.line && con.plus.obj == id)
                con.plus.line->setStart(pos + con.plus.offset);
            if (con.minus.line && con.minus.obj == id)
                con.minus.line->setEnd(pos + con.minus.offset);
        }
    }

    void removeWireData(int from, bool isFromPlus)
    {
        auto& con = contacts[from];
        auto& wire = isFromPlus ? con.plus : con.minus;
        wire.line = 0;
        wire.to = -1;
    }

    void removeWire(int from, bool isFromPlus)
    {
        auto& con = contacts[from];
        auto& wire = isFromPlus ? con.plus : con.minus;
        if (!wire.line)
            return;
        wires->removeObject(wire.line);
        removeWireData(wire.to, !isFromPlus);
        removeWireData(from, isFromPlus);
    }

    void setWireData(int from, int to, Wire& plus, Wire& minus, Line* line)
    {
        auto& fcon = contacts[from];
        auto& tcon = contacts[to];
        auto fpos = objects->getObject<CanvasLayout>(fcon.plus.obj)->getOffset();
        auto tpos = objects->getObject<CanvasLayout>(tcon.minus.obj)->getOffset();
        plus.line = line;
        plus.to = to;
        minus.line = line;
        minus.to = from;
        line->setStart(fpos + plus.offset);
        line->setEnd(tpos + minus.offset);
    }

    void createWire(int from, int to, bool isFromPlus)
    {
        auto& fcon = contacts[from];
        auto& tcon = contacts[to];
        auto line = loadObj<Line>("electricity\\Wire.json");
        wires->addObject(line);
        if (isFromPlus)
            setWireData(from, to, fcon.plus, tcon.minus, line.get());
        else
            setWireData(to, from, tcon.plus, fcon.minus, line.get());
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
        if (switchButton->isPressed())
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
            tempWire->setVisible(false);
            dragWireMode = false;
        }
        else
        {
            wireFrom = id;
            isWireFromPlus = isPlus;
            tempWire->setVisible(true);
            tempWire->setStart(objects->getObject<CanvasLayout>(isPlus ? con.plus.obj : con.minus.obj)->getOffset() + wire.offset);
            tempWire->setEnd(gv->mouseCoords());
            dragWireMode = true;
        }
    }

    void dragFromPanel(GameObj* obj, Type type)
    {
        if (switchButton->isPressed())
            return;
        auto opos = obj->getOffset();
        auto posOnScreen = input().mousePosition();
        if (opos.isZero())
        {
            startDragVec = posOnScreen;
            opos.y = 1;
        }
        else
        {
            opos = posOnScreen - startDragVec;
        }

        if (!obj->isPressed())
        {
            auto posOnView = gv->mouseCoords();
            create(type, posOnView.x, posOnView.y);
            opos = Vec2();
        }
        obj->setOffset(opos);
    }

    shared_ptr<CanvasLayout> design;
    ToggleButton* switchButton;
    Selector* status;

    GameView* gv;
    Layer* wires;
    Layer* objects;

    Line* tempWire;
    bool dragWireMode;
    int wireFrom;
    bool isWireFromPlus;

    Vec2 startDragVec;

    int nextID;
    map<int, Contacts> contacts;

    double U;
    double rIn;

    TextBox* uTextBox;
    TextBox* rInTextBox;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
