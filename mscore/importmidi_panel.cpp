#include "importmidi_panel.h"
#include "ui_importmidi_panel.h"
#include "importmidi_model.h"
#include "importmidi_lyrics.h"
#include "importmidi_operations.h"
#include "importmidi_delegate.h"
#include "importmidi_inner.h"
#include "preferences.h"
#include "musescore.h"


namespace Ms {

ImportMidiPanel::ImportMidiPanel(QWidget *parent)
      : QWidget(parent)
      , _ui(new Ui::ImportMidiPanel)
      , _updateUiTimer(new QTimer)
      , _prefferedVisible(false)
      , _importInProgress(false)
      , _reopenInProgress(false)
      {
      _ui->setupUi(this);

      _model = new TracksModel();
      _delegate = new OperationsDelegate(parentWidget(), false);
      _ui->tracksView->setModel(_model);
      _ui->tracksView->setItemDelegate(_delegate);

      setupUi();
      }

ImportMidiPanel::~ImportMidiPanel()
      {
      delete _ui;
      }

void ImportMidiPanel::setMidiFile(const QString &fileName)
      {
      if (_reopenInProgress)
            _reopenInProgress = false;
      if (_midiFile == fileName || _importInProgress)
            return;

      _midiFile = fileName;
      updateUi();
      if (!QFile(_midiFile).exists())
            return;

      MidiOperations::Data &opers = preferences.midiImportOperations;
      MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);

      _model->reset(opers.data()->trackOpers,
                    MidiLyrics::makeLyricsListForUI(),
                    opers.data()->trackCount);

      _ui->tracksView->setFrozenRowCount(_model->frozenRowCount());
      _ui->tracksView->setFrozenColCount(_model->frozenColCount());

      if (opers.data()->processingsOfOpenedFile == 1) {     // initial processing of MIDI file
            resetTableViewState();
            saveTableViewState();
            }
      else {            // switch to already opened MIDI file
            restoreTableViewState();
            }

      _ui->comboBoxCharset->setCurrentText(preferences.midiImportOperations.data()->charset);
      _ui->tracksView->selectRow(opers.data()->selectedRow);
      _ui->tracksView->selectColumn(0);
      }

void ImportMidiPanel::saveTableViewState()
      {
      MidiOperations::Data &opers = preferences.midiImportOperations;
      MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);

      const QByteArray hData = _ui->tracksView->horizontalHeader()->saveState();
      const QByteArray vData = _ui->tracksView->verticalHeader()->saveState();
      opers.data()->HHeaderData = hData;
      opers.data()->VHeaderData = vData;
      }

void ImportMidiPanel::restoreTableViewState()
      {
      MidiOperations::Data &opers = preferences.midiImportOperations;
      MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);

      const QByteArray hData = opers.data()->HHeaderData;
      const QByteArray vData = opers.data()->VHeaderData;
      _ui->tracksView->horizontalHeader()->restoreState(hData);
      _ui->tracksView->verticalHeader()->restoreState(vData);
      }

void ImportMidiPanel::resetTableViewState()
      {
      _ui->tracksView->verticalHeader()->reset();
      }

bool ImportMidiPanel::isMidiFile(const QString &fileName)
      {
      const QString extension = QFileInfo(fileName).suffix().toLower();
      return (extension == "mid" || extension == "midi" || extension == "kar");
      }

void ImportMidiPanel::setupUi()
      {
      connect(_updateUiTimer, SIGNAL(timeout()), this, SLOT(updateUi()));
      connect(_ui->pushButtonApply, SIGNAL(clicked()), SLOT(applyMidiImport()));
      connect(_ui->pushButtonUp, SIGNAL(clicked()), SLOT(moveTrackUp()));
      connect(_ui->pushButtonDown, SIGNAL(clicked()), SLOT(moveTrackDown()));
      connect(_ui->toolButtonHideMidiPanel, SIGNAL(clicked()), SLOT(hidePanel()));

      const QItemSelectionModel *sm = _ui->tracksView->selectionModel();
      connect(sm, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
              SLOT(onCurrentTrackChanged(QModelIndex)));

      _updateUiTimer->start(100);
      updateUi();
                  // tracks view
      _ui->tracksView->verticalHeader()->setDefaultSectionSize(24);
//      _ui->tracksView->horizontalHeader()->setResizeMode(TrackCol::DO_IMPORT,
//                                                        QHeaderView::ResizeToContents);
//      _ui->tracksView->horizontalHeader()->setResizeMode(TrackCol::STAFF_NAME,
//                                                        QHeaderView::Stretch);
//      _ui->tracksView->horizontalHeader()->setResizeMode(TrackCol::INSTRUMENT,
//                                                        QHeaderView::Stretch);
                  // charset
      _ui->comboBoxCharset->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
      fillCharsetList();
      }

void ImportMidiPanel::onCurrentTrackChanged(const QModelIndex &currentIndex)
      {
      if (currentIndex.isValid()) {
            MidiOperations::Data &opers = preferences.midiImportOperations;
            MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);
            opers.data()->selectedRow = currentIndex.row();
            }
      }

void ImportMidiPanel::fillCharsetList()
      {
      QFontMetrics fm(_ui->comboBoxCharset->font());

      _ui->comboBoxCharset->clear();
      QList<QByteArray> charsets = QTextCodec::availableCodecs();
      qSort(charsets.begin(), charsets.end());
      int idx = 0;
      int maxWidth = 0;
      for (const auto &charset: charsets) {
            _ui->comboBoxCharset->addItem(charset);
            if (charset == MidiCharset::defaultCharset())
                  _ui->comboBoxCharset->setCurrentIndex(idx);
            int newWidth = fm.width(charset);
            if (newWidth > maxWidth)
                  maxWidth = newWidth;
            ++idx;
            }
      _ui->comboBoxCharset->view()->setMinimumWidth(maxWidth);
      }

void ImportMidiPanel::updateUi()
      {
      _ui->pushButtonApply->setEnabled(canImportMidi());

      const int visualIndex = currentVisualIndex();
      _ui->pushButtonUp->setEnabled(canMoveTrackUp(visualIndex));
      _ui->pushButtonDown->setEnabled(canMoveTrackDown(visualIndex));
      }

void ImportMidiPanel::hidePanel()
      {
      if (isVisible()) {
            setVisible(false);
            emit closeClicked();
            _prefferedVisible = false;
            }
      }

void ImportMidiPanel::applyMidiImport()
      {
      if (!canImportMidi())
            return;

      _importInProgress = true;

      auto &opers = preferences.midiImportOperations;
      MidiOperations::CurrentMidiFileSetter setCurrentMidiFile(opers, _midiFile);
                  // update charset
      if (opers.data()->charset != _ui->comboBoxCharset->currentText()) {
            opers.data()->charset = _ui->comboBoxCharset->currentText();
                        // need to update model because of charset change
            _model->updateCharset();
            }
      mscore->openScore(_midiFile);
      opers.data()->trackOpers = _model->trackOpers();
      saveTableViewState();
      _importInProgress = false;
      }

bool ImportMidiPanel::canImportMidi() const
      {
      return QFile(_midiFile).exists() && _model->trackCountForImport() > 0;
      }

bool ImportMidiPanel::canMoveTrackUp(int visualIndex) const
      {
      return _model->trackCount() > 1 && visualIndex > 1;
      }

bool ImportMidiPanel::canMoveTrackDown(int visualIndex) const
      {
      return _model->trackCount() > 1
                  && visualIndex < _model->trackCount() && visualIndex > 0;
      }

int ImportMidiPanel::currentVisualIndex() const
      {
      const auto selectedItems = _ui->tracksView->selectionModel()->selection().indexes();
      int curRow = -1;
      if (!selectedItems.isEmpty())
            curRow = selectedItems[0].row();
      const int visIndex = _ui->tracksView->verticalHeader()->visualIndex(curRow);

      return visIndex;
      }

void ImportMidiPanel::excludeMidiFile(const QString &fileName)
      {
                  // because button "Apply" of MIDI import operations
                  // causes reopen of the current score
                  // we need to prevent MIDI import panel from closing at that moment
      if (_importInProgress || _reopenInProgress)
            return;

      auto &opers = preferences.midiImportOperations;
      opers.excludeFile(fileName);
      if (fileName == _midiFile)
            _midiFile = "";
      }

void ImportMidiPanel::setPrefferedVisible(bool visible)
      {
      _prefferedVisible = visible;
      }

void ImportMidiPanel::setReopenInProgress()
      {
      _reopenInProgress = true;
      }

} // namespace Ms
