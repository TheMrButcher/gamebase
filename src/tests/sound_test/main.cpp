#include <gamebase/Gamebase.h>
#include <gamebase/serial/LoadObj.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    void load()
    {
        audio.preload("sound_test");

        fillTrackBox();
        registerChannel(0);
        updateTextBoxes();

        connect(addButton, addTrack);
        connect(loopButton, loopTrack);
        connect(clearButton, clear);
        connect(playButton, play);
        connect(stopButton, stop);

        connect(channelBox, updateTextBoxes);
        connect(volumeBox, onVolumeChanged);
        connect(speedBox, onSpeedChanged);
    }

    void move()
    {
        updateAllChannels();
    }

    void fillTrackBox()
    {
        static const char* NAMES[] = { "hammer", "camera", "lock", "drum", "bass", "organ", "music" };
        for (auto name : NAMES)
        {
            auto button = loadObj<Button>("sound_test/Option.json");
            button.child<Label>("label") << name;
            trackBox.add(name, button);
        }
        trackBox.setText(NAMES[0]);
    }

    void registerChannel(int channel)
    {
        if (channels.count(channel) > 0)
            return;
        auto layout = channelsList.load<Layout>("sound_test/Channel.json");
        updateChannel(channel, layout);
        channelsArea.update();
        channels[channel] = layout;
    }

    void updateChannel(int channel, Layout layout)
    {
        layout.child<Label>("indexLabel") << channel;
        auto statusSelector = layout.child<Selector>("statusSelector");
        if (audio.isEmpty(channel))
        {
            statusSelector.select(0);
        }
        else
        {
            if (audio.isRunning(channel))
                statusSelector.select(1);
            else
                statusSelector.select(2);
        }

        layout.child<Label>("volumeLabel") << int(audio.volume(channel) * 100.f);
        layout.child<Label>("speedLabel") << int(audio.speed(channel) * 100.f);
    }

    void updateAllChannels()
    {
        for (auto pair : channels)
        {
            updateChannel(pair.first, pair.second);
        }
        channelsArea.update();
    }

    void updateTextBoxes()
    {
        if (channelBox.text().empty())
        {
            volumeBox << int(audio.volume() * 100.f);
            speedBox << int(audio.speed() * 100.f);
        }
        else
        {
            int channel = 0;
            channelBox >> channel;
            volumeBox << int(audio.volume(channel) * 100.f);
            speedBox << int(audio.speed(channel) * 100.f);
        }
    }

    void addTrack()
    {
        int channel = 0;
        if (!channelBox.text().empty())
            channelBox >> channel;
        audio.run("sound_test/" + trackBox.text() + ".ogg", channel);
        registerChannel(channel);
    }

    void loopTrack()
    {
        int channel = 0;
        if (!channelBox.text().empty())
            channelBox >> channel;
        audio.loop("sound_test/" + trackBox.text() + ".ogg", channel);
        registerChannel(channel);
    }

    void clear()
    {
        if (channelBox.text().empty())
        {
            audio.reset();
        }
        else
        {
            int channel = toInt(channelBox.text());
            audio.reset(channel);
        }
    }

    void play()
    {
        if (channelBox.text().empty())
        {
            audio.resume();
        }
        else
        {
            int channel = toInt(channelBox.text());
            audio.resume(channel);
        }
    }

    void stop()
    {
        if (channelBox.text().empty())
        {
            audio.pause();
        }
        else
        {
            int channel = toInt(channelBox.text());
            audio.pause(channel);
        }
    }

    void onVolumeChanged()
    {
        float volume = toFloat(volumeBox.text()) / 100.f;
        if (channelBox.text().empty())
        {
            audio.setVolume(volume);
        }
        else
        {
            int channel = toInt(channelBox.text());
            audio.setVolume(volume, channel);
        }
    }

    void onSpeedChanged()
    {
        float speed = toFloat(speedBox.text()) / 100.f;
        if (channelBox.text().empty())
        {
            audio.setSpeed(speed);
        }
        else
        {
            int channel = toInt(channelBox.text());
            audio.setSpeed(speed, channel);
        }
    }

    FromDesign(ComboBox, trackBox);

    FromDesign(Button, addButton);
    FromDesign(Button, loopButton);
    FromDesign(Button, clearButton);
    FromDesign(Button, playButton);
    FromDesign(Button, stopButton);

    FromDesign(TextBox, channelBox);
    FromDesign(TextBox, volumeBox);
    FromDesign(TextBox, speedBox);

    FromDesign(Layout, channelsArea);
    FromDesign(Layout, channelsList);

    std::map<int, Layout> channels;
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("config.json");
    app.setDesign("sound_test/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
