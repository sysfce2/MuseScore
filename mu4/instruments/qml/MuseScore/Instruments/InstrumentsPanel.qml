import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.3

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0
import MuseScore.Instruments 1.0

import "internal"

Item {
    id: root

    Rectangle {
        anchors.fill: parent

        color: ui.theme.backgroundPrimaryColor
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 12

        InstrumentsControlPanel {
            Layout.fillWidth: true

            isMovingUpAvailable: instrumentTreeModel.isMovingUpAvailable
            isMovingDownAvailable: instrumentTreeModel.isMovingDownAvailable
            isRemovingAvailable: instrumentTreeModel.isRemovingAvailable

            onMoveUpRequested: {
                instrumentTreeModel.moveSelectedRowsUp()
            }

            onMoveDownRequested: {
                instrumentTreeModel.moveSelectedRowsDown()
            }

            onRemovingRequested: {
                instrumentTreeModel.removeSelectedRows()
            }
        }

        TreeView {
            id: instrumentsTreeView

            Layout.fillWidth: true
            Layout.fillHeight: true

            model: InstrumentPanelTreeModel {
                id: instrumentTreeModel
            }

            selection: instrumentTreeModel ? instrumentTreeModel.selectionModel : null

            TableViewColumn {
                role: "itemRole"
            }

            style: TreeViewStyle {
                indentation: 0

                frame: Rectangle {
                    border.width: 0
                    color: ui.theme.backgroundPrimaryColor
                }

                backgroundColor: ui.theme.backgroundPrimaryColor

                rowDelegate: Rectangle {
                    id: rowTreeDelegate

                    height: 38
                    width: parent.width
                    color: ui.theme.strokeColor
                }

                branchDelegate: Item {}
            }

            itemDelegate: DropArea {
                id: dropArea

                property string filterKey: "instrument"

                //enabled: model ? model.itemRole.isSelectable : false

                Loader {
                    id: treeItemDelegateLoader

                    property var delegateType: model ? model.itemRole.type : InstrumentTreeItemType.UNDEFINED

                    height: parent.height
                    width: parent.width

                    sourceComponent: delegateType === InstrumentTreeItemType.CONTROL_ADD_STAFF
                                     || delegateType === InstrumentTreeItemType.CONTROL_ADD_DOUBLE_INSTRUMENT ? controlItemDelegateComponent
                                                                                                              : treeItemDelegateComponent

                    Component {
                        id: controlItemDelegateComponent

                        InstrumentsTreeItemControl {

                        }
                    }

                    Component {
                        id: treeItemDelegateComponent

                        InstrumentsTreeItemDelegate {
                            filterKey: dropArea.filterKey
                            attachedControl: instrumentsTreeView
                            isSelected: instrumentsTreeView.selection.selectedIndexes.indexOf(index) !== -1
                            isDragAvailable: instrumentTreeModel.isMovingUpAvailable || instrumentTreeModel.isMovingDownAvailable

                            onClicked: {
                                var isMultipleSelectionModeOn = mouse.modifiers & Qt.ShiftModifier || mouse.modifiers & Qt.ControlModifier;

                                instrumentTreeModel.selectRow(styleData.index, isMultipleSelectionModeOn);
                            }
                        }
                    }
                }

                onEntered: {
                    if (styleData.index === drag.source.index) {
                        return
                    }

                    instrumentTreeModel.moveRows(drag.source.index.parent,
                                                 drag.source.index.row,
                                                 1,
                                                 styleData.index.parent,
                                                 styleData.index.row)
                }

                onPositionChanged: {
                    var i = 0
                }
            }

            alternatingRowColors: false
            headerVisible: false
        }
    }
}
