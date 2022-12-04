#pragma once

#include <QtWidgets/QWidget>
#include "ui_WoodLightSetup.h"

class WoodLightSetup : public QWidget
{
    Q_OBJECT

public:
    WoodLightSetup(QWidget *parent = nullptr);
    ~WoodLightSetup();

private:
    Ui::WoodLightSetupClass ui;
};
