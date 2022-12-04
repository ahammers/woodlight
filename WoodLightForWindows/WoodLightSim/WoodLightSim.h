#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WoodLightSim.h"
#include <QTimer>
#include "WLSystem.h"
#include <future>

class WoodLightSim : public QMainWindow
{
    Q_OBJECT

public:
    WoodLightSim(QWidget *parent = nullptr);
    ~WoodLightSim();

    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void appUpdate();

private:
    Ui::WoodLightSimClass ui;
    QTimer updateTimer;
    WoodLight::System system;

    int noteDurationLeft = 0;

    std::future<void> tonePlayback;
};
