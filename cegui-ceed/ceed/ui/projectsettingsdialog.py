# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\ProjectSettingsDialog.ui'
#
# Created: Wed Mar 25 17:43:03 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_ProjectSettingsDialog(object):
    def setupUi(self, ProjectSettingsDialog):
        ProjectSettingsDialog.setObjectName("ProjectSettingsDialog")
        ProjectSettingsDialog.resize(600, 600)
        self.verticalLayout_2 = QtGui.QVBoxLayout(ProjectSettingsDialog)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.scrollArea = QtGui.QScrollArea(ProjectSettingsDialog)
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.setObjectName("scrollArea")
        self.scrollAreaWidgetContents_2 = QtGui.QWidget()
        self.scrollAreaWidgetContents_2.setGeometry(QtCore.QRect(0, 0, 562, 685))
        self.scrollAreaWidgetContents_2.setObjectName("scrollAreaWidgetContents_2")
        self.verticalLayout = QtGui.QVBoxLayout(self.scrollAreaWidgetContents_2)
        self.verticalLayout.setObjectName("verticalLayout")
        self.basicSettings = QtGui.QGroupBox(self.scrollAreaWidgetContents_2)
        self.basicSettings.setObjectName("basicSettings")
        self.formLayout = QtGui.QFormLayout(self.basicSettings)
        self.formLayout.setFieldGrowthPolicy(QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout.setObjectName("formLayout")
        self.label_2 = QtGui.QLabel(self.basicSettings)
        self.label_2.setObjectName("label_2")
        self.formLayout.setWidget(0, QtGui.QFormLayout.LabelRole, self.label_2)
        self.baseDirectory = FileLineEdit(self.basicSettings)
        self.baseDirectory.setObjectName("baseDirectory")
        self.formLayout.setWidget(0, QtGui.QFormLayout.FieldRole, self.baseDirectory)
        self.label_10 = QtGui.QLabel(self.basicSettings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_10.sizePolicy().hasHeightForWidth())
        self.label_10.setSizePolicy(sizePolicy)
        self.label_10.setWordWrap(True)
        self.label_10.setObjectName("label_10")
        self.formLayout.setWidget(1, QtGui.QFormLayout.FieldRole, self.label_10)
        self.label_11 = QtGui.QLabel(self.basicSettings)
        self.label_11.setObjectName("label_11")
        self.formLayout.setWidget(2, QtGui.QFormLayout.LabelRole, self.label_11)
        self.CEGUIVersion = QtGui.QComboBox(self.basicSettings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.CEGUIVersion.sizePolicy().hasHeightForWidth())
        self.CEGUIVersion.setSizePolicy(sizePolicy)
        self.CEGUIVersion.setEditable(True)
        self.CEGUIVersion.setObjectName("CEGUIVersion")
        self.formLayout.setWidget(2, QtGui.QFormLayout.FieldRole, self.CEGUIVersion)
        self.label_13 = QtGui.QLabel(self.basicSettings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_13.sizePolicy().hasHeightForWidth())
        self.label_13.setSizePolicy(sizePolicy)
        self.label_13.setWordWrap(True)
        self.label_13.setObjectName("label_13")
        self.formLayout.setWidget(3, QtGui.QFormLayout.FieldRole, self.label_13)
        self.label = QtGui.QLabel(self.basicSettings)
        self.label.setObjectName("label")
        self.formLayout.setWidget(4, QtGui.QFormLayout.LabelRole, self.label)
        self.CEGUIDefaultResolution = QtGui.QComboBox(self.basicSettings)
        self.CEGUIDefaultResolution.setEditable(True)
        self.CEGUIDefaultResolution.setObjectName("CEGUIDefaultResolution")
        self.CEGUIDefaultResolution.addItem("")
        self.CEGUIDefaultResolution.addItem("")
        self.CEGUIDefaultResolution.addItem("")
        self.CEGUIDefaultResolution.addItem("")
        self.CEGUIDefaultResolution.addItem("")
        self.CEGUIDefaultResolution.addItem("")
        self.formLayout.setWidget(4, QtGui.QFormLayout.FieldRole, self.CEGUIDefaultResolution)
        self.label_15 = QtGui.QLabel(self.basicSettings)
        self.label_15.setWordWrap(True)
        self.label_15.setObjectName("label_15")
        self.formLayout.setWidget(5, QtGui.QFormLayout.FieldRole, self.label_15)
        self.verticalLayout.addWidget(self.basicSettings)
        self.resourcePathSettings = QtGui.QGroupBox(self.scrollAreaWidgetContents_2)
        self.resourcePathSettings.setObjectName("resourcePathSettings")
        self.formLayout_2 = QtGui.QFormLayout(self.resourcePathSettings)
        self.formLayout_2.setFieldGrowthPolicy(QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout_2.setObjectName("formLayout_2")
        self.label_3 = QtGui.QLabel(self.resourcePathSettings)
        self.label_3.setObjectName("label_3")
        self.formLayout_2.setWidget(3, QtGui.QFormLayout.LabelRole, self.label_3)
        self.rootDirectoryContainer = QtGui.QWidget(self.resourcePathSettings)
        self.rootDirectoryContainer.setObjectName("rootDirectoryContainer")
        self.horizontalLayout = QtGui.QHBoxLayout(self.rootDirectoryContainer)
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.resourceDirectory = FileLineEdit(self.rootDirectoryContainer)
        self.resourceDirectory.setObjectName("resourceDirectory")
        self.horizontalLayout.addWidget(self.resourceDirectory)
        self.resourceDirectoryApplyButton = QtGui.QPushButton(self.rootDirectoryContainer)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.resourceDirectoryApplyButton.sizePolicy().hasHeightForWidth())
        self.resourceDirectoryApplyButton.setSizePolicy(sizePolicy)
        self.resourceDirectoryApplyButton.setObjectName("resourceDirectoryApplyButton")
        self.horizontalLayout.addWidget(self.resourceDirectoryApplyButton)
        self.formLayout_2.setWidget(3, QtGui.QFormLayout.FieldRole, self.rootDirectoryContainer)
        self.line_2 = QtGui.QFrame(self.resourcePathSettings)
        self.line_2.setFrameShape(QtGui.QFrame.HLine)
        self.line_2.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_2.setObjectName("line_2")
        self.formLayout_2.setWidget(4, QtGui.QFormLayout.FieldRole, self.line_2)
        self.label_9 = QtGui.QLabel(self.resourcePathSettings)
        self.label_9.setWordWrap(True)
        self.label_9.setObjectName("label_9")
        self.formLayout_2.setWidget(5, QtGui.QFormLayout.FieldRole, self.label_9)
        self.label_4 = QtGui.QLabel(self.resourcePathSettings)
        self.label_4.setObjectName("label_4")
        self.formLayout_2.setWidget(6, QtGui.QFormLayout.LabelRole, self.label_4)
        self.imagesetsPath = FileLineEdit(self.resourcePathSettings)
        self.imagesetsPath.setObjectName("imagesetsPath")
        self.formLayout_2.setWidget(6, QtGui.QFormLayout.FieldRole, self.imagesetsPath)
        self.label_5 = QtGui.QLabel(self.resourcePathSettings)
        self.label_5.setObjectName("label_5")
        self.formLayout_2.setWidget(7, QtGui.QFormLayout.LabelRole, self.label_5)
        self.fontsPath = FileLineEdit(self.resourcePathSettings)
        self.fontsPath.setObjectName("fontsPath")
        self.formLayout_2.setWidget(7, QtGui.QFormLayout.FieldRole, self.fontsPath)
        self.label_7 = QtGui.QLabel(self.resourcePathSettings)
        self.label_7.setObjectName("label_7")
        self.formLayout_2.setWidget(8, QtGui.QFormLayout.LabelRole, self.label_7)
        self.looknfeelsPath = FileLineEdit(self.resourcePathSettings)
        self.looknfeelsPath.setObjectName("looknfeelsPath")
        self.formLayout_2.setWidget(8, QtGui.QFormLayout.FieldRole, self.looknfeelsPath)
        self.label_6 = QtGui.QLabel(self.resourcePathSettings)
        self.label_6.setObjectName("label_6")
        self.formLayout_2.setWidget(9, QtGui.QFormLayout.LabelRole, self.label_6)
        self.schemesPath = FileLineEdit(self.resourcePathSettings)
        self.schemesPath.setObjectName("schemesPath")
        self.formLayout_2.setWidget(9, QtGui.QFormLayout.FieldRole, self.schemesPath)
        self.label_8 = QtGui.QLabel(self.resourcePathSettings)
        self.label_8.setObjectName("label_8")
        self.formLayout_2.setWidget(10, QtGui.QFormLayout.LabelRole, self.label_8)
        self.layoutsPath = FileLineEdit(self.resourcePathSettings)
        self.layoutsPath.setObjectName("layoutsPath")
        self.formLayout_2.setWidget(10, QtGui.QFormLayout.FieldRole, self.layoutsPath)
        self.label_14 = QtGui.QLabel(self.resourcePathSettings)
        self.label_14.setObjectName("label_14")
        self.formLayout_2.setWidget(11, QtGui.QFormLayout.LabelRole, self.label_14)
        self.xmlSchemasPath = FileLineEdit(self.resourcePathSettings)
        self.xmlSchemasPath.setObjectName("xmlSchemasPath")
        self.formLayout_2.setWidget(11, QtGui.QFormLayout.FieldRole, self.xmlSchemasPath)
        self.line = QtGui.QFrame(self.resourcePathSettings)
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName("line")
        self.formLayout_2.setWidget(13, QtGui.QFormLayout.FieldRole, self.line)
        self.label_12 = QtGui.QLabel(self.resourcePathSettings)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_12.sizePolicy().hasHeightForWidth())
        self.label_12.setSizePolicy(sizePolicy)
        self.label_12.setWordWrap(True)
        self.label_12.setObjectName("label_12")
        self.formLayout_2.setWidget(14, QtGui.QFormLayout.FieldRole, self.label_12)
        self.verticalLayout.addWidget(self.resourcePathSettings)
        self.scrollArea.setWidget(self.scrollAreaWidgetContents_2)
        self.verticalLayout_2.addWidget(self.scrollArea)
        self.buttonBox = QtGui.QDialogButtonBox(ProjectSettingsDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setCenterButtons(False)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout_2.addWidget(self.buttonBox)

        self.retranslateUi(ProjectSettingsDialog)
        self.CEGUIDefaultResolution.setCurrentIndex(2)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), ProjectSettingsDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), ProjectSettingsDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(ProjectSettingsDialog)

    def retranslateUi(self, ProjectSettingsDialog):
        ProjectSettingsDialog.setWindowTitle(QtGui.QApplication.translate("ProjectSettingsDialog", "Project settings", None, QtGui.QApplication.UnicodeUTF8))
        self.basicSettings.setTitle(QtGui.QApplication.translate("ProjectSettingsDialog", "Basic settings", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Base directory", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Even though base directory is shown as absolute path here, it\'s stored relative to the directory where the project file is. Be very careful when changing this, all paths might be rendered invalid when changing this!", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Target CEGUI version", None, QtGui.QApplication.UnicodeUTF8))
        self.label_13.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'Ubuntu\'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Warning:</span> Target CEGUI version is what this editor will assume your assets are in when loading all the schemes found and when opening any files when this project is opened! Changing this in existing projects with potentially incompatible datafiles might be disastrous, you definitely want to migrate your project to a newer version instead in that case.</p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Default CEGUI resolution", None, QtGui.QApplication.UnicodeUTF8))
        self.CEGUIDefaultResolution.setItemText(0, QtGui.QApplication.translate("ProjectSettingsDialog", "800x600", None, QtGui.QApplication.UnicodeUTF8))
        self.CEGUIDefaultResolution.setItemText(1, QtGui.QApplication.translate("ProjectSettingsDialog", "1024x768", None, QtGui.QApplication.UnicodeUTF8))
        self.CEGUIDefaultResolution.setItemText(2, QtGui.QApplication.translate("ProjectSettingsDialog", "1280x720", None, QtGui.QApplication.UnicodeUTF8))
        self.CEGUIDefaultResolution.setItemText(3, QtGui.QApplication.translate("ProjectSettingsDialog", "1600x1200", None, QtGui.QApplication.UnicodeUTF8))
        self.CEGUIDefaultResolution.setItemText(4, QtGui.QApplication.translate("ProjectSettingsDialog", "1650x1050", None, QtGui.QApplication.UnicodeUTF8))
        self.CEGUIDefaultResolution.setItemText(5, QtGui.QApplication.translate("ProjectSettingsDialog", "1920x1080", None, QtGui.QApplication.UnicodeUTF8))
        self.label_15.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "This is the resolution that CEGUI embedded instances will be at when their resolution is at \'Project default\'.", None, QtGui.QApplication.UnicodeUTF8))
        self.resourcePathSettings.setTitle(QtGui.QApplication.translate("ProjectSettingsDialog", "CEGUI resource paths settings", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Resource directory", None, QtGui.QApplication.UnicodeUTF8))
        self.rootDirectoryContainer.setToolTip(QtGui.QApplication.translate("ProjectSettingsDialog", "Setting this automatically sets all the following paths according to CEGUI\'s usual scheme.", None, QtGui.QApplication.UnicodeUTF8))
        self.resourceDirectoryApplyButton.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Apply", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Use resource directory as a time saver only, it\'s not actually saved in the project file, it just sets the 5 paths according to CEGUI\'s standard scheme used in demos and stock datafiles.", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Imagesets", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Fonts", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "LookNFeels", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Schemes", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Layouts", None, QtGui.QApplication.UnicodeUTF8))
        self.label_14.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "XML Schemas", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setText(QtGui.QApplication.translate("ProjectSettingsDialog", "Even though these paths are shown as absolute paths here, they are saved as relative paths (relative to the resource directory). This way your project may be shared with other developers, for example with a version control system.", None, QtGui.QApplication.UnicodeUTF8))

from ceed.qtwidgets import FileLineEdit
