/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef WOODBUTTON_H
#define WOODBUTTON_H

// class WoodButton
// Description: a QPushButton which draws a wood texture with a radial gradient bevel overlay.
// All state-dependent rendering (hover, pressed, disabled, flashing) is handled in paintEvent.

#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QStyleOption>

class WoodButton : public QPushButton
{
    Q_OBJECT

public:
    explicit WoodButton(QWidget *parent = nullptr) : QPushButton(parent) { init(); }
    explicit WoodButton(const QString &text, QWidget *parent = nullptr) : QPushButton(text, parent) { init(); }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        const QRectF r = rect();
        const qreal radius = 9.0;

        // Rounded clip path
        QPainterPath clip;
        clip.addRoundedRect(r, radius, radius);

        // 1. Wood texture (tiled)
        p.setClipPath(clip);
        if (!m_texture.isNull()) {
            p.drawTiledPixmap(rect(), m_texture);
        } else {
            p.fillPath(clip, QColor("#301e12")); // fallback colour if texture missing
        }

        // 2. Radial gradient overlay — matches css gradient origin (cx:0.3, cy:-0.4, radius:1.35)
        const QPointF center(r.width() * 0.3, r.height() * -0.4);
        const qreal gradRadius = qMax(r.width(), r.height()) * 1.35;
        QRadialGradient grad(center, gradRadius, center);

        const bool flashing = property("flashing").toBool();

        if (flashing || (isDown() && isEnabled())) {
            // Pressed / flashing: golden tint, darkened edges
            grad.setColorAt(0.0, QColor(212, 148, 10, 120));
            grad.setColorAt(1.0, QColor(0,   0,   0,  200));
        } else if (!isEnabled()) {
            // Disabled: heavily darkened
            grad.setColorAt(0.0, QColor(0, 0, 0, 120));
            grad.setColorAt(1.0, QColor(0, 0, 0, 210));
        } else if (underMouse()) {
            // Hover: slight brightening at highlight
            grad.setColorAt(0.0, QColor(255, 255, 255, 55));
            grad.setColorAt(1.0, QColor(0,   0,   0,  150));
        } else {
            // Normal: subtle bevel (bright highlight, dark edges)
            grad.setColorAt(0.0, QColor(255, 255, 255, 25));
            grad.setColorAt(1.0, QColor(0,   0,   0,  165));
        }
        p.fillPath(clip, grad);

        // 3. Border
        p.setClipping(false);
        p.setPen(QPen(QColor("#150d08"), 1));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(r.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

        // 4. Text
        p.setPen(isEnabled() ? Qt::white : QColor(128, 128, 128));
        p.setFont(font());
        p.drawText(rect(), Qt::AlignCenter, text());
    }

private:
    QPixmap m_texture;

    void init()
    {
        setAttribute(Qt::WA_Hover); // ensure hover enter/leave triggers repaint
        m_texture.load("/home/juddn/images/wood-seamless-720p.jpg");
    }
};

#endif // WOODBUTTON_H
