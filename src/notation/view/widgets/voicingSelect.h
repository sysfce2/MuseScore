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
//============================================================================

#ifndef MU_NOTATION_VOICINGSELECT_H
#define MU_NOTATION_VOICINGSELECT_H

#include "ui_voicing_select.h"

namespace mu::notation {
class VoicingSelect : public QWidget, public Ui::VoicingSelect
{
    Q_OBJECT

public:
    VoicingSelect(QWidget* parent);
    int getVoicing() { return voicingBox->currentIndex(); }
    bool getLiteral() { return interpretBox->currentIndex(); }
    int getDuration() { return durationBox->currentIndex(); }

    void setVoicing(int idx);
    void setLiteral(bool literal);
    void setDuration(int duration);

signals:
    void voicingChanged(bool, int, int);

private:
    void blockVoicingSignals(bool);

private slots:
    void _voicingChanged();
};
}

#endif // MU_NOTATION_VOICINGSELECT_H
