# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\imageset\DockWidget.ui'
#
<<<<<<< HEAD
<<<<<<< HEAD
# Created: Wed Apr 29 10:56:49 2015
=======
# Created: Mon May 18 01:37:27 2015
>>>>>>> 8d71f48158ce6577737b930a8dc24036e2e85210
=======
# Created: Sat May 23 15:53:32 2015
>>>>>>> 5c99a2e5037babbf80457f6614ef5105ed6e4b86
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_DockWidget(object):
    def setupUi(self, DockWidget):
        DockWidget.setObjectName("DockWidget")
        DockWidget.setEnabled(False)
        DockWidget.resize(337, 728)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(DockWidget.sizePolicy().hasHeightForWidth())
        DockWidget.setSizePolicy(sizePolicy)
        DockWidget.setMinimumSize(QtCore.QSize(337, 525))
        DockWidget.setFocusPolicy(QtCore.Qt.ClickFocus)
        self.dockWidgetContents = QtGui.QWidget()
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.verticalLayout = QtGui.QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setSpacing(6)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setContentsMargins(6, 0, 6, 0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.imagesetGroupBox = QtGui.QGroupBox(self.dockWidgetContents)
        self.imagesetGroupBox.setMinimumSize(QtCore.QSize(0, 172))
        self.imagesetGroupBox.setObjectName("imagesetGroupBox")
        self.gridLayout_2 = QtGui.QGridLayout(self.imagesetGroupBox)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.nativeVertRes = QtGui.QLineEdit(self.imagesetGroupBox)
        self.nativeVertRes.setObjectName("nativeVertRes")
        self.gridLayout_2.addWidget(self.nativeVertRes, 5, 2, 1, 1)
        self.nativeHorzRes = QtGui.QLineEdit(self.imagesetGroupBox)
        self.nativeHorzRes.setObjectName("nativeHorzRes")
        self.gridLayout_2.addWidget(self.nativeHorzRes, 5, 1, 1, 1)
        self.nativeResolutionLabel = QtGui.QLabel(self.imagesetGroupBox)
        self.nativeResolutionLabel.setObjectName("nativeResolutionLabel")
        self.gridLayout_2.addWidget(self.nativeResolutionLabel, 5, 0, 1, 1)
        self.autoScaled = QtGui.QComboBox(self.imagesetGroupBox)
        self.autoScaled.setObjectName("autoScaled")
        self.autoScaled.addItem("")
        self.autoScaled.addItem("")
        self.autoScaled.addItem("")
        self.autoScaled.addItem("")
        self.autoScaled.addItem("")
        self.autoScaled.addItem("")
        self.gridLayout_2.addWidget(self.autoScaled, 4, 1, 1, 2)
        self.name = QtGui.QLineEdit(self.imagesetGroupBox)
        self.name.setObjectName("name")
        self.gridLayout_2.addWidget(self.name, 0, 1, 1, 2)
        self.imageLoad = QtGui.QPushButton(self.imagesetGroupBox)
        self.imageLoad.setObjectName("imageLoad")
        self.gridLayout_2.addWidget(self.imageLoad, 2, 2, 1, 1)
        self.image = FileLineEdit(self.imagesetGroupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.image.sizePolicy().hasHeightForWidth())
        self.image.setSizePolicy(sizePolicy)
        self.image.setObjectName("image")
        self.gridLayout_2.addWidget(self.image, 1, 1, 1, 2)
        self.imageLabel = QtGui.QLabel(self.imagesetGroupBox)
        self.imageLabel.setObjectName("imageLabel")
        self.gridLayout_2.addWidget(self.imageLabel, 1, 0, 1, 1)
        self.nameLabel = QtGui.QLabel(self.imagesetGroupBox)
        self.nameLabel.setObjectName("nameLabel")
        self.gridLayout_2.addWidget(self.nameLabel, 0, 0, 1, 1)
        self.autoScaledLabel = QtGui.QLabel(self.imagesetGroupBox)
        self.autoScaledLabel.setObjectName("autoScaledLabel")
        self.gridLayout_2.addWidget(self.autoScaledLabel, 4, 0, 1, 1)
        self.line_2 = QtGui.QFrame(self.imagesetGroupBox)
        self.line_2.setFrameShape(QtGui.QFrame.HLine)
        self.line_2.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_2.setObjectName("line_2")
        self.gridLayout_2.addWidget(self.line_2, 3, 0, 1, 3)
        self.verticalLayout.addWidget(self.imagesetGroupBox)
        self.imagesGroupBox = QtGui.QGroupBox(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.imagesGroupBox.sizePolicy().hasHeightForWidth())
        self.imagesGroupBox.setSizePolicy(sizePolicy)
        self.imagesGroupBox.setMinimumSize(QtCore.QSize(0, 120))
        self.imagesGroupBox.setObjectName("imagesGroupBox")
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.imagesGroupBox)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.widget = QtGui.QWidget(self.imagesGroupBox)
        self.widget.setObjectName("widget")
        self.horizontalLayout = QtGui.QHBoxLayout(self.widget)
        self.horizontalLayout.setSpacing(1)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.filterBox = QtGui.QLineEdit(self.widget)
        self.filterBox.setObjectName("filterBox")
        self.horizontalLayout.addWidget(self.filterBox)
        self.filterResetButton = QtGui.QPushButton(self.widget)
        self.filterResetButton.setText("")
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("icons/imageset_editing/filter_reset.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.filterResetButton.setIcon(icon)
        self.filterResetButton.setIconSize(QtCore.QSize(16, 16))
        self.filterResetButton.setAutoDefault(False)
        self.filterResetButton.setDefault(False)
        self.filterResetButton.setFlat(False)
        self.filterResetButton.setObjectName("filterResetButton")
        self.horizontalLayout.addWidget(self.filterResetButton)
        self.verticalLayout_2.addWidget(self.widget)
        self.list = QtGui.QListWidget(self.imagesGroupBox)
        self.list.setEditTriggers(QtGui.QAbstractItemView.DoubleClicked|QtGui.QAbstractItemView.EditKeyPressed|QtGui.QAbstractItemView.SelectedClicked)
        self.list.setAlternatingRowColors(False)
        self.list.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
        self.list.setIconSize(QtCore.QSize(32, 32))
        self.list.setSelectionRectVisible(True)
        self.list.setObjectName("list")
        self.verticalLayout_2.addWidget(self.list)
        self.verticalLayout.addWidget(self.imagesGroupBox)
        self.propertiesGroupBox = QtGui.QGroupBox(self.dockWidgetContents)
        self.propertiesGroupBox.setObjectName("propertiesGroupBox")
        self.gridLayout = QtGui.QGridLayout(self.propertiesGroupBox)
        self.gridLayout.setObjectName("gridLayout")
        self.yColumnLabel = QtGui.QLabel(self.propertiesGroupBox)
        self.yColumnLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.yColumnLabel.setObjectName("yColumnLabel")
        self.gridLayout.addWidget(self.yColumnLabel, 0, 2, 1, 1)
        self.sizeLabel = QtGui.QLabel(self.propertiesGroupBox)
        self.sizeLabel.setObjectName("sizeLabel")
        self.gridLayout.addWidget(self.sizeLabel, 2, 0, 1, 1)
        self.positionY = QtGui.QLineEdit(self.propertiesGroupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.positionY.sizePolicy().hasHeightForWidth())
        self.positionY.setSizePolicy(sizePolicy)
        self.positionY.setObjectName("positionY")
        self.gridLayout.addWidget(self.positionY, 1, 2, 1, 1)
        self.positionLabel = QtGui.QLabel(self.propertiesGroupBox)
        self.positionLabel.setObjectName("positionLabel")
        self.gridLayout.addWidget(self.positionLabel, 1, 0, 1, 1)
        self.autoScaledPerImageLabel = QtGui.QLabel(self.propertiesGroupBox)
        self.autoScaledPerImageLabel.setObjectName("autoScaledPerImageLabel")
        self.gridLayout.addWidget(self.autoScaledPerImageLabel, 5, 0, 1, 1)
        self.height = QtGui.QLineEdit(self.propertiesGroupBox)
        self.height.setObjectName("height")
        self.gridLayout.addWidget(self.height, 2, 2, 1, 1)
        self.width = QtGui.QLineEdit(self.propertiesGroupBox)
        self.width.setObjectName("width")
        self.gridLayout.addWidget(self.width, 2, 1, 1, 1)
        self.offsetX = QtGui.QLineEdit(self.propertiesGroupBox)
        self.offsetX.setObjectName("offsetX")
        self.gridLayout.addWidget(self.offsetX, 3, 1, 1, 1)
        self.offsetY = QtGui.QLineEdit(self.propertiesGroupBox)
        self.offsetY.setObjectName("offsetY")
        self.gridLayout.addWidget(self.offsetY, 3, 2, 1, 1)
        self.offsetLabel = QtGui.QLabel(self.propertiesGroupBox)
        self.offsetLabel.setObjectName("offsetLabel")
        self.gridLayout.addWidget(self.offsetLabel, 3, 0, 1, 1)
        self.positionX = QtGui.QLineEdit(self.propertiesGroupBox)
        self.positionX.setObjectName("positionX")
        self.gridLayout.addWidget(self.positionX, 1, 1, 1, 1)
        self.xColumnLabel = QtGui.QLabel(self.propertiesGroupBox)
        self.xColumnLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.xColumnLabel.setObjectName("xColumnLabel")
        self.gridLayout.addWidget(self.xColumnLabel, 0, 1, 1, 1)
        self.autoScaledPerImage = QtGui.QComboBox(self.propertiesGroupBox)
        self.autoScaledPerImage.setObjectName("autoScaledPerImage")
        self.autoScaledPerImage.addItem("")
        self.autoScaledPerImage.setItemText(0, "")
        self.autoScaledPerImage.addItem("")
        self.autoScaledPerImage.addItem("")
        self.autoScaledPerImage.addItem("")
        self.autoScaledPerImage.addItem("")
        self.autoScaledPerImage.addItem("")
        self.autoScaledPerImage.addItem("")
        self.gridLayout.addWidget(self.autoScaledPerImage, 5, 1, 1, 2)
        self.nativeResPerImageLabel = QtGui.QLabel(self.propertiesGroupBox)
        self.nativeResPerImageLabel.setObjectName("nativeResPerImageLabel")
        self.gridLayout.addWidget(self.nativeResPerImageLabel, 6, 0, 1, 1)
        self.nativeHorzResPerImage = QtGui.QLineEdit(self.propertiesGroupBox)
        self.nativeHorzResPerImage.setObjectName("nativeHorzResPerImage")
        self.gridLayout.addWidget(self.nativeHorzResPerImage, 6, 1, 1, 1)
        self.nativeVertResPerImage = QtGui.QLineEdit(self.propertiesGroupBox)
        self.nativeVertResPerImage.setObjectName("nativeVertResPerImage")
        self.gridLayout.addWidget(self.nativeVertResPerImage, 6, 2, 1, 1)
        self.line = QtGui.QFrame(self.propertiesGroupBox)
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName("line")
        self.gridLayout.addWidget(self.line, 4, 0, 1, 3)
        self.verticalLayout.addWidget(self.propertiesGroupBox)
        DockWidget.setWidget(self.dockWidgetContents)
        self.nameLabel.setBuddy(self.name)

        self.retranslateUi(DockWidget)
        QtCore.QObject.connect(self.filterResetButton, QtCore.SIGNAL("clicked()"), self.filterBox.clear)
        QtCore.QMetaObject.connectSlotsByName(DockWidget)

    def retranslateUi(self, DockWidget):
        DockWidget.setWindowTitle(QtGui.QApplication.translate("DockWidget", "Imageset Editing", None, QtGui.QApplication.UnicodeUTF8))
        self.imagesetGroupBox.setTitle(QtGui.QApplication.translate("DockWidget", "Imageset", None, QtGui.QApplication.UnicodeUTF8))
        self.nativeResolutionLabel.setToolTip(QtGui.QApplication.translate("DockWidget", "Native resolution", None, QtGui.QApplication.UnicodeUTF8))
        self.nativeResolutionLabel.setText(QtGui.QApplication.translate("DockWidget", "Native res.", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaled.setItemText(0, QtGui.QApplication.translate("DockWidget", "false", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaled.setItemText(1, QtGui.QApplication.translate("DockWidget", "vertical", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaled.setItemText(2, QtGui.QApplication.translate("DockWidget", "horizontal", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaled.setItemText(3, QtGui.QApplication.translate("DockWidget", "min", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaled.setItemText(4, QtGui.QApplication.translate("DockWidget", "max", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaled.setItemText(5, QtGui.QApplication.translate("DockWidget", "true", None, QtGui.QApplication.UnicodeUTF8))
        self.imageLoad.setText(QtGui.QApplication.translate("DockWidget", "Load", None, QtGui.QApplication.UnicodeUTF8))
        self.imageLabel.setText(QtGui.QApplication.translate("DockWidget", "Underlying texture", None, QtGui.QApplication.UnicodeUTF8))
        self.nameLabel.setText(QtGui.QApplication.translate("DockWidget", "Name", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaledLabel.setText(QtGui.QApplication.translate("DockWidget", "Auto scaled", None, QtGui.QApplication.UnicodeUTF8))
        self.imagesGroupBox.setTitle(QtGui.QApplication.translate("DockWidget", "Image definitions", None, QtGui.QApplication.UnicodeUTF8))
        self.filterBox.setPlaceholderText(QtGui.QApplication.translate("DockWidget", "Filter definitions by name", None, QtGui.QApplication.UnicodeUTF8))
        self.filterResetButton.setToolTip(QtGui.QApplication.translate("DockWidget", "Reset the filter", None, QtGui.QApplication.UnicodeUTF8))
        self.list.setSortingEnabled(True)
        self.propertiesGroupBox.setTitle(QtGui.QApplication.translate("DockWidget", "Selected image definition properties", None, QtGui.QApplication.UnicodeUTF8))
        self.yColumnLabel.setText(QtGui.QApplication.translate("DockWidget", "Y", None, QtGui.QApplication.UnicodeUTF8))
        self.sizeLabel.setText(QtGui.QApplication.translate("DockWidget", "Size", None, QtGui.QApplication.UnicodeUTF8))
        self.positionLabel.setText(QtGui.QApplication.translate("DockWidget", "Position", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaledPerImageLabel.setText(QtGui.QApplication.translate("DockWidget", "Auto scaled", None, QtGui.QApplication.UnicodeUTF8))
        self.offsetLabel.setText(QtGui.QApplication.translate("DockWidget", "Offset", None, QtGui.QApplication.UnicodeUTF8))
        self.xColumnLabel.setText(QtGui.QApplication.translate("DockWidget", "X", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaledPerImage.setItemText(1, QtGui.QApplication.translate("DockWidget", "false", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaledPerImage.setItemText(2, QtGui.QApplication.translate("DockWidget", "vertical", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaledPerImage.setItemText(3, QtGui.QApplication.translate("DockWidget", "horizontal", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaledPerImage.setItemText(4, QtGui.QApplication.translate("DockWidget", "min", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaledPerImage.setItemText(5, QtGui.QApplication.translate("DockWidget", "max", None, QtGui.QApplication.UnicodeUTF8))
        self.autoScaledPerImage.setItemText(6, QtGui.QApplication.translate("DockWidget", "true", None, QtGui.QApplication.UnicodeUTF8))
        self.nativeResPerImageLabel.setText(QtGui.QApplication.translate("DockWidget", "Native res.", None, QtGui.QApplication.UnicodeUTF8))

from ceed.qtwidgets import FileLineEdit
