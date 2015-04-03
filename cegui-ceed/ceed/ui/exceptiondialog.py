# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\mybahaoui\Documents\GitHub\UIromy\cegui-ceed\ceed\ui\ExceptionDialog.ui'
#
# Created: Fri Apr 03 17:53:35 2015
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_ExceptionDialog(object):
    def setupUi(self, ExceptionDialog):
        ExceptionDialog.setObjectName("ExceptionDialog")
        ExceptionDialog.resize(506, 546)
        ExceptionDialog.setSizeGripEnabled(False)
        ExceptionDialog.setModal(True)
        self.verticalLayout = QtGui.QVBoxLayout(ExceptionDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.widget = QtGui.QWidget(ExceptionDialog)
        self.widget.setObjectName("widget")
        self.horizontalLayout = QtGui.QHBoxLayout(self.widget)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label_2 = QtGui.QLabel(self.widget)
        self.label_2.setText("")
        self.label_2.setPixmap(QtGui.QPixmap("icons/unhandled_exception.png"))
        self.label_2.setAlignment(QtCore.Qt.AlignCenter)
        self.label_2.setObjectName("label_2")
        self.horizontalLayout.addWidget(self.label_2)
        self.label = QtGui.QLabel(self.widget)
        self.label.setWordWrap(True)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.verticalLayout.addWidget(self.widget)
        self.line = QtGui.QFrame(ExceptionDialog)
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName("line")
        self.verticalLayout.addWidget(self.line)
        self.label_3 = QtGui.QLabel(ExceptionDialog)
        self.label_3.setWordWrap(True)
        self.label_3.setObjectName("label_3")
        self.verticalLayout.addWidget(self.label_3)
        self.line_3 = QtGui.QFrame(ExceptionDialog)
        self.line_3.setFrameShape(QtGui.QFrame.HLine)
        self.line_3.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_3.setObjectName("line_3")
        self.verticalLayout.addWidget(self.line_3)
        self.mantisLink = QtGui.QLabel(ExceptionDialog)
        self.mantisLink.setTextFormat(QtCore.Qt.AutoText)
        self.mantisLink.setAlignment(QtCore.Qt.AlignCenter)
        self.mantisLink.setOpenExternalLinks(True)
        self.mantisLink.setObjectName("mantisLink")
        self.verticalLayout.addWidget(self.mantisLink)
        self.line_4 = QtGui.QFrame(ExceptionDialog)
        self.line_4.setFrameShape(QtGui.QFrame.HLine)
        self.line_4.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_4.setObjectName("line_4")
        self.verticalLayout.addWidget(self.line_4)
        self.details = QtGui.QTextBrowser(ExceptionDialog)
        self.details.setObjectName("details")
        self.verticalLayout.addWidget(self.details)
        self.line_2 = QtGui.QFrame(ExceptionDialog)
        self.line_2.setFrameShape(QtGui.QFrame.HLine)
        self.line_2.setFrameShadow(QtGui.QFrame.Sunken)
        self.line_2.setObjectName("line_2")
        self.verticalLayout.addWidget(self.line_2)
        self.buttonBox = QtGui.QDialogButtonBox(ExceptionDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Close|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setCenterButtons(False)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(ExceptionDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), ExceptionDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), ExceptionDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(ExceptionDialog)

    def retranslateUi(self, ExceptionDialog):
        ExceptionDialog.setWindowTitle(QtGui.QApplication.translate("ExceptionDialog", "Encountered unhandled exception!", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("ExceptionDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'MS Shell Dlg 2\'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt;\">Exception was thrown and wasn\'t caught!</span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">After you press OK, the editor </span><span style=\" font-size:8pt; font-style:italic;\">might </span><span style=\" font-size:8pt;\">not be in a workable state. Some things might continue working, some will be broken. The exact situation is hard to guess at this point.</span></p>\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">Please backup your work before saving ANYTHING because in these situations, nobody can be sure what survived!</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">Otherwise you might as well ruin your data!</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("ExceptionDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'MS Shell Dlg 2\'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Following are exception details, these might help you judge the damage. If you wish to contribute to this project, please consider posting them.</span></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt; font-style:italic;\">(the editor should never let any exceptions be unhandled, so this is definitely a bug already!)</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.mantisLink.setText(QtGui.QApplication.translate("ExceptionDialog", "<a href=\"http://www.cegui.org.uk/mantis/bug_report_page.php\">Report a bug.</a>", None, QtGui.QApplication.UnicodeUTF8))

