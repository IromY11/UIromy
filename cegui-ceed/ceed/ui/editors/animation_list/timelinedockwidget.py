# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\animation_list\TimelineDockWidget.ui'
#
# Created: Mon Apr 06 12:40:08 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_TimelineDockWidget(object):
    def setupUi(self, TimelineDockWidget):
        TimelineDockWidget.setObjectName("TimelineDockWidget")
        TimelineDockWidget.setEnabled(True)
        TimelineDockWidget.resize(909, 183)
        self.dockWidgetContents = QtGui.QWidget()
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.verticalLayout = QtGui.QVBoxLayout(self.dockWidgetContents)
        self.verticalLayout.setContentsMargins(-1, 0, -1, -1)
        self.verticalLayout.setObjectName("verticalLayout")
        self.view = TimelineGraphicsView(self.dockWidgetContents)
        self.view.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.view.setRenderHints(QtGui.QPainter.TextAntialiasing)
        self.view.setViewportUpdateMode(QtGui.QGraphicsView.FullViewportUpdate)
        self.view.setObjectName("view")
        self.verticalLayout.addWidget(self.view)
        self.widget = QtGui.QWidget(self.dockWidgetContents)
        self.widget.setObjectName("widget")
        self.horizontalLayout = QtGui.QHBoxLayout(self.widget)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.playButton = QtGui.QPushButton(self.widget)
        self.playButton.setEnabled(True)
        self.playButton.setText("")
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap("icons/animation_editing/timeline_play.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.playButton.setIcon(icon)
        self.playButton.setIconSize(QtCore.QSize(22, 22))
        self.playButton.setAutoDefault(False)
        self.playButton.setDefault(False)
        self.playButton.setFlat(True)
        self.playButton.setObjectName("playButton")
        self.horizontalLayout.addWidget(self.playButton)
        self.pauseButton = QtGui.QPushButton(self.widget)
        self.pauseButton.setText("")
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap("icons/animation_editing/timeline_pause.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.pauseButton.setIcon(icon1)
        self.pauseButton.setIconSize(QtCore.QSize(22, 22))
        self.pauseButton.setFlat(True)
        self.pauseButton.setObjectName("pauseButton")
        self.horizontalLayout.addWidget(self.pauseButton)
        self.stopButton = QtGui.QPushButton(self.widget)
        self.stopButton.setText("")
        icon2 = QtGui.QIcon()
        icon2.addPixmap(QtGui.QPixmap("icons/animation_editing/timeline_stop.png"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.stopButton.setIcon(icon2)
        self.stopButton.setIconSize(QtCore.QSize(22, 22))
        self.stopButton.setFlat(True)
        self.stopButton.setObjectName("stopButton")
        self.horizontalLayout.addWidget(self.stopButton)
        self.verticalLayout.addWidget(self.widget)
        TimelineDockWidget.setWidget(self.dockWidgetContents)

        self.retranslateUi(TimelineDockWidget)
        QtCore.QMetaObject.connectSlotsByName(TimelineDockWidget)

    def retranslateUi(self, TimelineDockWidget):
        TimelineDockWidget.setWindowTitle(QtGui.QApplication.translate("TimelineDockWidget", "Timeline", None, QtGui.QApplication.UnicodeUTF8))
        self.playButton.setToolTip(QtGui.QApplication.translate("TimelineDockWidget", "Plays from the current position forward", None, QtGui.QApplication.UnicodeUTF8))
        self.pauseButton.setToolTip(QtGui.QApplication.translate("TimelineDockWidget", "Pauses at current position or resumes playback at current position", None, QtGui.QApplication.UnicodeUTF8))
        self.stopButton.setToolTip(QtGui.QApplication.translate("TimelineDockWidget", "Stops playback (pauses and rewinds to the start)", None, QtGui.QApplication.UnicodeUTF8))

from ceed.editors.animation_list.visual import TimelineGraphicsView
