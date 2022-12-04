#include "WoodLightWidget.h"

#include <QPainter>
#include <QSizePolicy>

WoodLightWidget::WoodLightWidget(QWidget *parent)
	: QWidget(parent)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	display.clear(false);
}

WoodLightWidget::~WoodLightWidget()
{}

void WoodLightWidget::setDisplay(WoodLight::Display nv)
{
	display = std::move(nv);
	setMinimumSize(QSize(display.Columns, display.Rows));
	update();
}

void WoodLightWidget::paintEvent(QPaintEvent* event) 
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	const auto r = rect();

	const auto pixelDistance = std::min(1.0 * r.width() / (display.Columns + 1), 1.0 * r.height() / (display.Rows + 1));
	const auto pixelRadius = std::max(1.0, pixelDistance * 0.85);

	p.fillRect( rect(), Qt::lightGray);
	p.fillRect( QRect(0, 0, (pixelDistance + 1) * display.Columns, (pixelDistance + 1) * display.Rows), backgroundColor);

	p.setPen(borderColor);
	for (int y = 0; y < display.Rows; ++y)
	{
		const auto cy = (y + 0.5) * pixelDistance;
		for (int x = 0; x < display.Columns; ++x)
		{
			const auto cx = (x + 0.5) * pixelDistance;
			p.setBrush(display.getPixel(x, y) ? onColor : offColor);
			p.drawEllipse(cx, cy, pixelRadius, pixelRadius);
		}
	}
}
