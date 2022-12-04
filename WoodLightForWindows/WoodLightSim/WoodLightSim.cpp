#include "WoodLightSim.h"

#include "WLApp_Menu.h"
#include "WLHttp.h"

#include <QKeyEvent>
#include <QDebug>

#include "SoundEffect.h"

static const auto KeyMap = std::map<int, uint8_t>{
    { Qt::Key_Q, WoodLight::System::UpperLeftKeyCode},
    { Qt::Key_A, WoodLight::System::LowerLeftKeyCode},
    { Qt::Key_O, WoodLight::System::UpperRightKeyCode},
    { Qt::Key_L, WoodLight::System::LowerRightKeyCode}
};

WoodLightSim::WoodLightSim(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    system.initialize();

    setFocusPolicy(Qt::StrongFocus);
    updateTimer.setInterval(40);
    connect(&updateTimer, &QTimer::timeout, this, &WoodLightSim::appUpdate);
    updateTimer.start();

    //String target;
    //const auto response = WoodLight::httpGet(
    //    "http://server:8123/api/states/sensor.server_volume_1_volume_used", 
    //    "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJkZjcxYTk3YWM3YjQ0NzJiYTBlMzVkMDE2MDg1MTA2YyIsImlhdCI6MTY2OTkyNDU5OCwiZXhwIjoxOTg1Mjg0NTk4fQ.XtesXBKRQyCQvyrjB5H2FTXL7bebqNK2MJr4fZz16Sk", 
    //    &target);
    //qDebug() << "httpGet returned " << response << "\n" << target.c_str();


}

WoodLightSim::~WoodLightSim()
{}

void WoodLightSim::keyPressEvent(QKeyEvent* event)
{
    if (!event->isAutoRepeat())
    {
        auto it = KeyMap.find(event->key());
        if (it != KeyMap.end())
            system.keyPressed(it->second);
    }
}

void WoodLightSim::keyReleaseEvent(QKeyEvent* event)
{
    if (!event->isAutoRepeat())
    {
        auto it = KeyMap.find(event->key());
        if (it != KeyMap.end())
            system.keyRelease(it->second);
    }
}

void WoodLightSim::appUpdate()
{
    static auto updateNumber = uint8_t{ 0 };
    ++updateNumber;

    system.process();

    //if (system.display.isChanged())
    //{
        system.display.clearChanged();

        auto d = system.display;

        if (system.keyState & WoodLight::System::LowerLeftKeyMask)
            d.setPixel(0, 6, updateNumber & 4);

        if (system.keyState & WoodLight::System::UpperLeftKeyMask)
            d.setPixel(0, 1, updateNumber & 4);

        if (system.keyState & WoodLight::System::LowerRightKeyMask)
            d.setPixel(WoodLight::Display::Columns-1, 6, updateNumber & 4);

        if (system.keyState & WoodLight::System::UpperRightKeyMask)
            d.setPixel(WoodLight::Display::Columns - 1, 1, updateNumber & 4);

        //ui.widget->setDisplay(system.display);
        ui.widget->setDisplay(d);

    //}

    if (noteDurationLeft <= 0)
    {
        auto tone = system.getTone();
        noteDurationLeft = tone.duration;

        if (tone.freq > 0 && tone.duration > 0)
        {
            if (tonePlayback.valid())
                tonePlayback.wait();

            tonePlayback = std::async([=] {
                SoundEffect se(tone.freq, tone.duration * updateTimer.interval());
                se.Play();
                });
        }
    }

    if (noteDurationLeft > 0)
        --noteDurationLeft;

}