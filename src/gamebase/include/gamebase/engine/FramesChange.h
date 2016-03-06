#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/IAnimation.h>
#include <gamebase/serial/ISerializable.h>

namespace gamebase {
    
class Atlas;

class GAMEBASE_API FramesChange : public IAnimation, public ISerializable {
public:
    static const int INFINITY = -1;

    FramesChange(
        const std::string& atlasName,
        int startFrameIndex,
        int lastFrameIndex,
        Time period,
        int repeatTimes)
        : m_atlasName(atlasName)
        , m_startFrameIndex(startFrameIndex)
        , m_lastFrameIndex(lastFrameIndex)
        , m_period(period)
        , m_repeatTimes(repeatTimes)
    {}

    virtual void load(const PropertiesRegister& props) override;
    virtual void start() override;
    virtual Time step(Time t) override;

    virtual bool isFinished() const override
    {
        return m_repeatTimes != INFINITY && m_curRepeat >= m_repeatTimes;
    }

    virtual void serialize(Serializer& serializer) const override;

private:
    std::string m_atlasName;
    Atlas* m_atlas;
    Time m_period;
    int m_startFrameIndex;
    int m_lastFrameIndex;
    int m_repeatTimes;

    Time m_curTime;
    int m_curFrameIndex;
    int m_curRepeat;
    bool m_isPositiveDir;
    bool m_needUpdateFrame;
};

}
