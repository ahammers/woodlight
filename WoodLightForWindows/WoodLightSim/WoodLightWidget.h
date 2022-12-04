#pragma once

#include <QWidget>
#include <array>

#include "WLDisplay.h"

class WoodLightWidget  : public QWidget
{
	Q_OBJECT

public:
	WoodLightWidget(QWidget *parent);
	~WoodLightWidget();

	void setDisplay(WoodLight::Display);
	
protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	QColor backgroundColor = qRgb(80, 70, 10);
	QColor offColor = backgroundColor;
	QColor onColor = qRgb(255, 100, 30);
	QColor borderColor = qRgb(70, 60, 0);

	WoodLight::Display display;
};
