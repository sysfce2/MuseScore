//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2021 MuseScore BVBA and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#ifndef MU_OFFSET_SELECT_H
#define MU_OFFSET_SELECT_H

#include "ui_offset_select.h"

namespace mu::notation {
class OffsetSelect : public QWidget, public Ui::OffsetSelect
{
    Q_OBJECT

public:
    OffsetSelect(QWidget* parent);
    void setSuffix(const QString&);
    QPointF offset() const;
    void setOffset(const QPointF&);
    void showRaster(bool);

signals:
    void offsetChanged(const QPointF&);

private:
    void blockOffset(bool val);

private slots:
    void _offsetChanged();
};
}

#endif
