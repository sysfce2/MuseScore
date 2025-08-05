/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <QObject>

#include "modularity/ioc.h"
#include "ui/iwindowscontroller.h"

namespace muse::ui {
class WindowsModel : public QObject, public Injectable
{
    Q_OBJECT

    Inject<ui::IWindowsController> windowsController = { this };

    Q_PROPERTY(
        QRect mainWindowTitleBarMoveArea READ mainWindowTitleBarMoveArea WRITE setMainWindowTitleBarMoveArea NOTIFY mainWindowTitleBarMoveAreaChanged)

public:
    explicit WindowsModel(QObject* parent = nullptr);

    QRect mainWindowTitleBarMoveArea() const;

public slots:
    void setMainWindowTitleBarMoveArea(const QRect& area);

signals:
    void mainWindowTitleBarMoveAreaChanged(QRect titleBarMoveArea);
};
}
