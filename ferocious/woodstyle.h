/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef WOODSTYLE_H
#define WOODSTYLE_H

// class WoodStyle
// Description: a QProxyStyle that paints all QPushButton instances with a wood texture
// and radial gradient bevel overlay. Activated automatically when the wood theme is in effect.

#include <QProxyStyle>
#include <QStyleOption>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QPixmap>

class WoodStyle : public QProxyStyle
{
public:
    explicit WoodStyle(QStyle *baseStyle = nullptr) : QProxyStyle(baseStyle)
    {
        m_texture.load("/home/juddn/images/wood-seamless-720p.jpg");
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override
    {
        if (element == CE_PushButton) {
            if (auto btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
                drawWoodBevel(btn, painter);
                // Pass label drawing to base with white (or dimmed) text colour
                QStyleOptionButton labelOpt = *btn;
                const bool enabled = option->state & QStyle::State_Enabled;
                labelOpt.palette.setColor(QPalette::ButtonText, enabled ? Qt::white : QColor(140, 140, 140));
                QProxyStyle::drawControl(CE_PushButtonLabel, &labelOpt, painter, widget);
                return;
            }
        }
        QProxyStyle::drawControl(element, option, painter, widget);
    }

    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == CT_PushButton)
            s.setWidth(qMax(s.width(), 66));
        return s;
    }

private:
    QPixmap m_texture;

    void drawWoodBevel(const QStyleOptionButton *option, QPainter *painter) const
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const QRectF r = option->rect;
        const qreal radius = 9.0;

        QPainterPath clip;
        clip.addRoundedRect(r, radius, radius);

        // 1. Wood texture (tiled)
        painter->setClipPath(clip);
        if (!m_texture.isNull()) {
            painter->drawTiledPixmap(option->rect, m_texture);
        } else {
            painter->fillPath(clip, QColor("#301e12")); // fallback if texture missing
        }

        // 2. Radial gradient overlay — matches css origin (cx:0.3, cy:-0.4, radius:1.35)
        const QPointF center(r.width() * 0.3, r.height() * -0.4);
        const qreal gradRadius = qMax(r.width(), r.height()) * 1.35;
        QRadialGradient grad(center, gradRadius, center);

        const bool enabled = option->state & QStyle::State_Enabled;
        const bool pressed = option->state & QStyle::State_Sunken;
        const bool hovered = option->state & QStyle::State_MouseOver;
        // FlashingPushButton sets this property when active
        const bool flashing = option->styleObject && option->styleObject->property("flashing").toBool();

        if (!enabled) {
            grad.setColorAt(0.0, QColor(0,   0,   0, 120));
            grad.setColorAt(1.0, QColor(0,   0,   0, 210));
        } else if (flashing || pressed) {
            grad.setColorAt(0.0, QColor(212, 148, 10, 120));
            grad.setColorAt(1.0, QColor(0,   0,   0, 200));
        } else if (hovered) {
            grad.setColorAt(0.0, QColor(255, 255, 255, 55));
            grad.setColorAt(1.0, QColor(0,   0,   0, 150));
        } else {
            grad.setColorAt(0.0, QColor(255, 255, 255, 25));
            grad.setColorAt(1.0, QColor(0,   0,   0, 165));
        }
        painter->fillPath(clip, grad);

        // 3. Border
        painter->setClipping(false);
        painter->setPen(QPen(QColor("#150d08"), 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(r.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

        painter->restore();
    }
};

#endif // WOODSTYLE_H
