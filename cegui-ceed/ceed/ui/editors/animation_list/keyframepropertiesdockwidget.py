# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\Iromys\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\editors\animation_list\KeyFramePropertiesDockWidget.ui'
#
<<<<<<< HEAD
# Created: Fri Apr 03 20:18:18 2015
=======
# Created: Fri Apr 03 17:53:35 2015
>>>>>>> c571f43318051bacc0fbf7da05af203cfa97782a
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_KeyFramePropertiesDockWidget(object):
    def setupUi(self, KeyFramePropertiesDockWidget):
        KeyFramePropertiesDockWidget.setObjectName("KeyFramePropertiesDockWidget")
        KeyFramePropertiesDockWidget.resize(401, 199)
        self.dockWidgetContents = QtGui.QWidget()
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.gridLayout = QtGui.QGridLayout(self.dockWidgetContents)
        self.gridLayout.setSizeConstraint(QtGui.QLayout.SetDefaultConstraint)
        self.gridLayout.setObjectName("gridLayout")
        self.timePositionSpinBox = QtGui.QDoubleSpinBox(self.dockWidgetContents)
        self.timePositionSpinBox.setDecimals(10)
        self.timePositionSpinBox.setMaximum(10000.0)
        self.timePositionSpinBox.setSingleStep(0.25)
        self.timePositionSpinBox.setObjectName("timePositionSpinBox")
        self.gridLayout.addWidget(self.timePositionSpinBox, 0, 1, 1, 1)
        self.label = QtGui.QLabel(self.dockWidgetContents)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.fixedValueLineEdit = QtGui.QLineEdit(self.dockWidgetContents)
        self.fixedValueLineEdit.setObjectName("fixedValueLineEdit")
        self.gridLayout.addWidget(self.fixedValueLineEdit, 3, 1, 1, 1)
        self.fixedValueLabel = QtGui.QLabel(self.dockWidgetContents)
        self.fixedValueLabel.setObjectName("fixedValueLabel")
        self.gridLayout.addWidget(self.fixedValueLabel, 3, 0, 1, 1)
        self.progressionComboBox = QtGui.QComboBox(self.dockWidgetContents)
        self.progressionComboBox.setObjectName("progressionComboBox")
        self.progressionComboBox.addItem("")
        self.progressionComboBox.addItem("")
        self.progressionComboBox.addItem("")
        self.progressionComboBox.addItem("")
        self.gridLayout.addWidget(self.progressionComboBox, 1, 1, 1, 1)
        self.widget = QtGui.QWidget(self.dockWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.widget.sizePolicy().hasHeightForWidth())
        self.widget.setSizePolicy(sizePolicy)
        self.widget.setObjectName("widget")
        self.horizontalLayout = QtGui.QHBoxLayout(self.widget)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.fixedValueRadioButton = QtGui.QRadioButton(self.widget)
        self.fixedValueRadioButton.setChecked(True)
        self.fixedValueRadioButton.setObjectName("fixedValueRadioButton")
        self.horizontalLayout.addWidget(self.fixedValueRadioButton)
        self.propertyRadioButton = QtGui.QRadioButton(self.widget)
        self.propertyRadioButton.setObjectName("propertyRadioButton")
        self.horizontalLayout.addWidget(self.propertyRadioButton)
        self.gridLayout.addWidget(self.widget, 2, 1, 1, 1)
        self.label_2 = QtGui.QLabel(self.dockWidgetContents)
        self.label_2.setObjectName("label_2")
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.label_4 = QtGui.QLabel(self.dockWidgetContents)
        self.label_4.setObjectName("label_4")
        self.gridLayout.addWidget(self.label_4, 2, 0, 1, 1)
        self.sourcePropertyLabel = QtGui.QLabel(self.dockWidgetContents)
        self.sourcePropertyLabel.setObjectName("sourcePropertyLabel")
        self.gridLayout.addWidget(self.sourcePropertyLabel, 4, 0, 1, 1)
        self.sourcePropertyComboBox = QtGui.QComboBox(self.dockWidgetContents)
        self.sourcePropertyComboBox.setEditable(True)
        self.sourcePropertyComboBox.setObjectName("sourcePropertyComboBox")
        self.gridLayout.addWidget(self.sourcePropertyComboBox, 4, 1, 1, 1)
        spacerItem = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.gridLayout.addItem(spacerItem, 5, 0, 1, 1)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayout.addItem(spacerItem1, 5, 1, 1, 1)
        KeyFramePropertiesDockWidget.setWidget(self.dockWidgetContents)

        self.retranslateUi(KeyFramePropertiesDockWidget)
        QtCore.QObject.connect(self.fixedValueRadioButton, QtCore.SIGNAL("toggled(bool)"), self.fixedValueLabel.setVisible)
        QtCore.QObject.connect(self.fixedValueRadioButton, QtCore.SIGNAL("toggled(bool)"), self.fixedValueLineEdit.setVisible)
        QtCore.QObject.connect(self.fixedValueRadioButton, QtCore.SIGNAL("toggled(bool)"), self.sourcePropertyLabel.setHidden)
        QtCore.QObject.connect(self.fixedValueRadioButton, QtCore.SIGNAL("toggled(bool)"), self.sourcePropertyComboBox.setHidden)
        QtCore.QMetaObject.connectSlotsByName(KeyFramePropertiesDockWidget)

    def retranslateUi(self, KeyFramePropertiesDockWidget):
        KeyFramePropertiesDockWidget.setWindowTitle(QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Selected KeyFrame properties", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Time position", None, QtGui.QApplication.UnicodeUTF8))
        self.fixedValueLabel.setText(QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Fixed value", None, QtGui.QApplication.UnicodeUTF8))
        self.progressionComboBox.setItemText(0, QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Linear", None, QtGui.QApplication.UnicodeUTF8))
        self.progressionComboBox.setItemText(1, QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Quadratic Accelerating", None, QtGui.QApplication.UnicodeUTF8))
        self.progressionComboBox.setItemText(2, QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Quadratic Decelerating", None, QtGui.QApplication.UnicodeUTF8))
        self.progressionComboBox.setItemText(3, QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Discrete", None, QtGui.QApplication.UnicodeUTF8))
        self.fixedValueRadioButton.setText(QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Fixed value", None, QtGui.QApplication.UnicodeUTF8))
        self.propertyRadioButton.setText(QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Property", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Progression", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Key frame value source", None, QtGui.QApplication.UnicodeUTF8))
        self.sourcePropertyLabel.setText(QtGui.QApplication.translate("KeyFramePropertiesDockWidget", "Source property", None, QtGui.QApplication.UnicodeUTF8))

