//  This file is part of SilentEye.
//
//  SilentEye is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  SilentEye is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with SilentEye.  If not, see <http://www.gnu.org/licenses/>.

#ifndef IMAGEWIDGET_H_
#define IMAGEWIDGET_H_

#include <QWidget>
#include <QPixmap>
#include <QMenu>
#include <QContextMenuEvent>
#include "ui_imagewidget.h"

class ImageWidget : public QWidget, Ui::ImageWidget
{
private:
    QMenu* m_menu;
    QSize m_size;
    bool m_hasImg;

public:
    ImageWidget(QWidget* parent=0) : QWidget(parent)
    {
        setupUi(this);
        m_menu = NULL;
        m_hasImg = false;
    }

    void setPixmap(const QPixmap pix)
    {
        m_hasImg = true;
        m_size = pix.size();
        imageLabel->setPixmap(pix);
        updateImageSize();
    }

    void setContextMenu(QMenu* menu)
    {
        m_menu = menu;
    }

    void contextMenuEvent(QContextMenuEvent *event)
    {
        if(m_menu)
            m_menu->exec(event->globalPos());
    }

    void updateImageSize()
    {
        if(!m_hasImg)
            return;

        int width=0, height=0;
        double ratio = m_size.width() / (double)m_size.height();
        double ratioW = this->width() / (double)this->height();

        if((ratio>1 && ratio>ratioW) || (ratio<1 && ratio>ratioW))
        {
            width = this->width();
            height = (int)(width / ratio);
        }
        else
        {
            height = this->height();
            width = (int)(height * ratio);
        }

        imageLabel->setMaximumWidth(width);
        imageLabel->setMaximumHeight(height);
        imageLabel->setMinimumWidth(width);
        imageLabel->setMinimumHeight(height);

        int margin = (this->width()-width)/2;
        verticalLayout->setContentsMargins(margin, 0, 0, 0);
    }

    void resizeEvent(QResizeEvent* event)
    {
        Q_UNUSED(event);
        updateImageSize();
    }
};

#endif

