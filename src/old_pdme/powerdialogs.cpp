/****************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**     Universidad Nacional de Rosario - Argentina.
**  Contact: PowerDEVS Information (kofman@fceia.unr.edu.ar, fbergero@fceia.unr.edu.ar)
**
**  This file is part of PowerDEVS.
**
**  PowerDEVS is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  PowerDEVS is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with PowerDEVS.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QtCore/QVariant>
#include <QtGui>
#include "powerdialogs.h"
#include "parser.h"
#include "gpxblock.h"
#include "gpxeditionscene.h"
#include "gpxatomic.h"
#include "gpxcoupled.h"
#include "gpxinport.h"
#include "gpxoutport.h"
#include "gpxconnectionpoint.h"

#define DIALOGW 			452
#define DIALOGH 			472
#define BASE_MENU_BUFFER 	7

 PowerDialogs::PowerDialogs(QWidget * parent):QDialog(parent)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the following slots ared used by pathSetupDialog.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PowerDialogs::searchAtomicEditor()
{
	QDir dir(QCoreApplication::applicationDirPath());
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString binPath =
	    settings.value("Path/binPath",
			   "Value not found in file powerdevs.ini").toString();
	QString fileName =
	    QFileDialog::getOpenFileName(this, tr("Atomics Editor"), binPath,
					 "Program Files *;;All Files (*.*)");
	if (!fileName.isEmpty())
		editorLed->setText(dir.relativeFilePath(fileName));
	return;
}

void PowerDialogs::searchAtomicPath()
{
	QDir dir(QCoreApplication::applicationDirPath());
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString binPath =
	    settings.value("Path/binPath",
			   "Value not found in file powerdevs.ini").toString();
	QString dirName =
	    QFileDialog::getExistingDirectory(this, tr("Path Browser"), binPath,
					      QFileDialog::
					      ShowDirsOnly | QFileDialog::
					      DontResolveSymlinks);
	if (!dirName.isEmpty())
		pathLed->setText(dir.relativeFilePath(dirName));
	return;
}

void PowerDialogs::searchHelpFile()
{
	QDir dir(QCoreApplication::applicationDirPath());
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString binPath =
	    settings.value("Path/binPath",
			   "Value not found in file powerdevs.ini").toString();
	QString fileName =
	    QFileDialog::getOpenFileName(this, "Help File", binPath,
					 "Help Files *;;HyperText Files (*.html);;All Files (*.*)");
	if (!fileName.isEmpty())
		helpLed->setText(dir.relativeFilePath(fileName));
	return;
}

void PowerDialogs::cancelPathSetup()
{
	delete helpLed;
	delete filterLed;
	delete pathLed;
	delete editorLed;
	delete paramLed;
	delete dialog;
}

void PowerDialogs::savePathSettings()
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	settings.setValue("Path/atomicsEditor", editorLed->text());
	settings.setValue("Path/parametersEditor", paramLed->text());
	settings.setValue("Path/atomicsPath", pathLed->text());
	settings.setValue("Path/filterPath", filterLed->text());
	settings.setValue("Path/helpFile", helpLed->text());
	delete helpLed;
	delete filterLed;
	delete pathLed;
	delete editorLed;
	delete paramLed;
	delete dialog;
}

void PowerDialogs::pathSetupDialog()
{
	dialog = new QDialog(this);
	if (dialog->objectName().isEmpty())
		dialog->setObjectName(QString::fromUtf8("Set Path and Tools"));
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->setFixedSize(DIALOGW, DIALOGH);
	dialog->setWindowTitle(tr("Seth Path and Tools"));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QVBoxLayout *atomicLayout = new QVBoxLayout;
	QGridLayout *helpLayout = new QGridLayout;
	QGridLayout *editorLayout = new QGridLayout;
	QGridLayout *pathLayout = new QGridLayout;
	QGridLayout *helpFileLayout = new QGridLayout;

	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString atomicsEditor =
	    settings.value("Path/atomicsEditor",
			   "Value not found in file powerdevs.ini").toString();
	QString parametersEditor =
	    settings.value("Path/parametersEditor",
			   "Value not found in file powerdevs.ini").toString();
	QString atomicsPath =
	    settings.value("Path/atomicsPath",
			   "Value not found in file powerdevs.ini").toString();
	QString filterPath =
	    settings.value("Path/filterPath",
			   "Value not found in file powerdevs.ini").toString();
	QString helpFile =
	    settings.value("Path/helpFile",
			   "Value not found in file powerdevs.ini").toString();

	QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
	buttonBox->setEnabled(true);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->
	    setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);
	QTabWidget *sptTab = new QTabWidget(dialog);
	QWidget *atomicsTab = new QWidget();
	QGroupBox *editorGbx = new QGroupBox(atomicsTab);
	editorGbx->setAutoFillBackground(true);
	editorGbx->setTitle(tr("Atomic Models Editor"));
	QLabel *editorLbl = new QLabel(tr("Binary:"), editorGbx);
	QLabel *paramLbl = new QLabel(editorGbx);
	paramLbl->setText(tr("Parameters: "));
	paramLed = new QLineEdit(editorGbx);
	paramLed->setText(parametersEditor);
	QToolButton *editorBtn = new QToolButton(editorGbx);
	editorBtn->setText(tr("..."));
	editorLed = new QLineEdit(editorGbx);
	editorLed->setText(atomicsEditor);
	QGroupBox *pathGbx = new QGroupBox(atomicsTab);
	pathGbx->setAutoFillBackground(true);
	pathGbx->setTitle(tr("Atomic Models Path"));
	QLabel *pathLbl = new QLabel(tr("Path"), pathGbx);
	pathLed = new QLineEdit(pathGbx);
	pathLed->setText(atomicsPath);
	QLabel *filterLbl = new QLabel(pathGbx);
	filterLbl->setText(tr("Filter:"));
	filterLed = new QLineEdit(pathGbx);
	filterLed->setText(filterPath);
	QToolButton *pathBtn = new QToolButton(pathGbx);
	pathBtn->setText(tr("..."));
	sptTab->addTab(atomicsTab, tr("Atomics"));
	QWidget *helpFileTab = new QWidget();
	QGroupBox *helpGbx = new QGroupBox(helpFileTab);
	helpGbx->setAutoFillBackground(false);
	helpGbx->setTitle(tr("Help File"));
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(10);
	sizePolicy.setHeightForWidth(helpGbx->sizePolicy().hasHeightForWidth());
	helpGbx->setSizePolicy(sizePolicy);
	QLabel *helpLbl = new QLabel(tr("File:"), helpGbx);
	helpLed = new QLineEdit(helpGbx);
	helpLed->setText(helpFile);
	QToolButton *helpBtn = new QToolButton(helpGbx);
	helpBtn->setText(tr("..."));
	sptTab->addTab(helpFileTab, tr("Help File"));
	sptTab->setCurrentIndex(0);

	connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(accepted()), this,
		SLOT(savePathSettings()));
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelPathSetup()));
	connect(editorBtn, SIGNAL(clicked()), this,
		SLOT(searchAtomicEditor()));
	connect(pathBtn, SIGNAL(clicked()), this, SLOT(searchAtomicPath()));
	connect(helpBtn, SIGNAL(clicked()), this, SLOT(searchHelpFile()));

	mainLayout->addWidget(sptTab);
	mainLayout->addWidget(buttonBox);
	dialog->setLayout(mainLayout);

	editorLayout->addWidget(editorLbl, 0, 0);
	editorLayout->addWidget(editorLed, 0, 1, 1, 4);
	editorLayout->addWidget(editorBtn, 0, 5, Qt::AlignRight);
	editorLayout->addWidget(paramLbl, 1, 0);
	editorLayout->addWidget(paramLed, 1, 1, 1, 4);
	editorGbx->setLayout(editorLayout);

	pathLayout->addWidget(pathLbl, 0, 0);
	pathLayout->addWidget(pathLed, 0, 1, 1, 4);
	pathLayout->addWidget(pathBtn, 0, 5, Qt::AlignRight);
	pathLayout->addWidget(filterLbl, 1, 0);
	pathLayout->addWidget(filterLed, 1, 1, 1, 4);
	pathGbx->setLayout(pathLayout);

	atomicLayout->addWidget(editorGbx);
	atomicLayout->addWidget(pathGbx);
	helpFileLayout->addWidget(helpLbl, 0, 0);
	helpFileLayout->addWidget(helpLed, 0, 1, 1, 4);
	helpFileLayout->addWidget(helpBtn, 0, 5, Qt::AlignRight);
	helpGbx->setLayout(helpFileLayout);
	helpLayout->addWidget(helpGbx, 0, 0, Qt::AlignTop);
	atomicsTab->setLayout(atomicLayout);
	helpFileTab->setLayout(helpLayout);
	dialog->show();
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the following slots are used by librarySetupDialog.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void PowerDialogs::saveToolBoxSettings()
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	settings.setValue("libraries", libLst);
	int items = lsLstWdt->count();
	for (int k = 0; k < items; k++) {
		if (lsLstWdt->item(k)->checkState())
			libEnabled.replace(k, "true");
		else
			libEnabled.replace(k, "false");
	}
	settings.setValue("libraryEnabled", libEnabled);
	delete pathLbl;
	delete lsLstWdt;
	delete dialog;
	emit writeToolBoxSettings();
}

void PowerDialogs::cancelLibrarySetup()
{
	delete pathLbl;
	delete lsLstWdt;
	delete dialog;
}

void PowerDialogs::openLibrary()
{
	QDir dir(QCoreApplication::applicationDirPath());
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString libpath =
	    settings.value("path/libPath",
			   "Value not found in file powerdevs.ini").toString();
	QString fileName =
	    QFileDialog::getOpenFileName(this, tr("Open Library"), "../bin",
					 "Libraries of PowerDEVS (*.pdl);;All Files (*.*)");
	if (fileName == NULL)
		return;
	modelCoupled *model = parseModel(dir.relativeFilePath(fileName));
	QString iconLib = model->graphics->icon;
	if (iconLib == "None")
		iconLib = "";
	if (iconLib.startsWith("\%"))
		iconLib =
		    libpath + "/" + iconLib.mid(1).replace("\%", "/").toLower();
	QListWidgetItem *it =
	    new QListWidgetItem(QIcon(iconLib), model->name, lsLstWdt);
	it->setToolTip(model->desc);
	it->setCheckState(Qt::Checked);
	libLst << dir.relativeFilePath(fileName);
	return;
}

void PowerDialogs::removeLibrary()
{
	libLst.removeAt(lsLstWdt->currentRow());
	lsLstWdt->takeItem(lsLstWdt->currentRow());
	return;
}

void PowerDialogs::updatePathLbl(int row)
{
	pathLbl->setText(libLst.value(row));
	return;
}

void PowerDialogs::upLibLst()
{
	int row = lsLstWdt->currentRow();
	if (row > 0) {
		libLst.move(row, row - 1);
		libEnabled.move(row, row - 1);
		QListWidgetItem *tmp = lsLstWdt->takeItem(row);
		lsLstWdt->insertItem(row - 1, tmp);
		lsLstWdt->setCurrentItem(tmp);
	}
	return;
}

void PowerDialogs::downLibLst()
{
	int row = lsLstWdt->currentRow();
	if (row < (lsLstWdt->count() - 1) && row >= 0) {
		libLst.move(row, row + 1);
		libEnabled.move(row, row + 1);
		QListWidgetItem *tmp = lsLstWdt->takeItem(row);
		lsLstWdt->insertItem(row + 1, tmp);
		lsLstWdt->setCurrentItem(tmp);
	}
	return;
}

void PowerDialogs::librarySetupDialog()
{
	dialog = new QDialog(this);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->setFixedSize(DIALOGW, DIALOGH);
	dialog->setWindowTitle(tr("Add/Remove Libraries"));

	QGridLayout *mainLayout = new QGridLayout;
	QGridLayout *libraryLayout = new QGridLayout;
	QVBoxLayout *pathLayout = new QVBoxLayout;

	QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
	buttonBox->setEnabled(true);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->
	    setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);

	QGroupBox *lsGbx = new QGroupBox(dialog);
	lsGbx->setAutoFillBackground(true);
	lsGbx->setTitle(tr("Library"));

	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	libLst = settings.value("libraries", QStringList()).toStringList();
	libEnabled =
	    settings.value("libraryEnabled", QStringList()).toStringList();
	QString libPath =
	    settings.value("path/libPath",
			   "Value not found in file powerdevs.ini").toString();

	lsLstWdt = new QListWidget(lsGbx);
	lsLstWdt->setSelectionMode(QAbstractItemView::SingleSelection);

	foreach(QString str, libLst) {
		modelCoupled *model = parseModel(str);
		QString iconLib = model->graphics->icon;
		if (iconLib == "None")
			iconLib = "";
		if (iconLib.startsWith("\%"))
			iconLib =
			    libPath + "/" + iconLib.mid(1).replace("\%",
								   "/").
			    toLower();
		QListWidgetItem *it =
		    new QListWidgetItem(QIcon(iconLib), model->name, lsLstWdt);
		it->setToolTip(model->desc);
		if (libEnabled.value(libLst.indexOf(str)) == "true")
			it->setCheckState(Qt::Checked);
		else
			it->setCheckState(Qt::Unchecked);
	}
	if (lsLstWdt->count() > 0) {
		lsLstWdt->setFocus();
		lsLstWdt->item(0)->setSelected(true);
	}

	QPushButton *addBtn = new QPushButton(lsGbx);
	addBtn->setText(tr("Add"));

	QPushButton *removeBtn = new QPushButton(lsGbx);
	removeBtn->setText(tr("Remove"));

	QToolButton *upBtn = new QToolButton(lsGbx);
	upBtn->setArrowType(Qt::UpArrow);
	QToolButton *downBtn = new QToolButton(lsGbx);
	downBtn->setArrowType(Qt::DownArrow);

	QGroupBox *pathGbx = new QGroupBox(dialog);
	pathGbx->setAutoFillBackground(true);
	pathGbx->setTitle(tr("Path"));

	pathLbl = new QLabel(pathGbx);
	pathLbl->setText(libLst.value(0));

	libraryLayout->addWidget(lsLstWdt, 0, 0, 5, 1);
	libraryLayout->addWidget(upBtn, 1, 3);
	libraryLayout->addWidget(downBtn, 1, 4);
	libraryLayout->addWidget(addBtn, 2, 2, 1, 4);
	libraryLayout->addWidget(removeBtn, 3, 2, 1, 4);
	lsGbx->setLayout(libraryLayout);
	pathLayout->addWidget(pathLbl);
	pathGbx->setLayout(pathLayout);
	mainLayout->addWidget(lsGbx, 0, 0, 3, 1);
	mainLayout->addWidget(pathGbx, 4, 0);
	mainLayout->addWidget(buttonBox, 5, 0);
	dialog->setLayout(mainLayout);

	connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(accepted()), this,
		SLOT(saveToolBoxSettings()));
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelLibrarySetup()));
	connect(addBtn, SIGNAL(clicked()), this, SLOT(openLibrary()));
	connect(removeBtn, SIGNAL(clicked()), this, SLOT(removeLibrary()));
	connect(upBtn, SIGNAL(clicked()), this, SLOT(upLibLst()));
	connect(downBtn, SIGNAL(clicked()), this, SLOT(downLibLst()));
	connect(lsLstWdt, SIGNAL(currentRowChanged(int)), this,
		SLOT(updatePathLbl(int)));
	dialog->show();
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the following slots are used by menuSetup Dialog.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PowerDialogs::updateEditMenu(int row)
{
	if (row < 0)
		row = 0;
	if (row >= menuEditLstWdt->count())
		row = menuEditLstWdt->count() - 1;
	if (menuEditLst.count() > 0) {
		int base = BASE_MENU_BUFFER * menuEditLstWdt->currentRow();
		QSettings settings(QCoreApplication::applicationDirPath() +
				   "/powerdevs.ini", QSettings::IniFormat);
		menuEditLed->setText(menuEditLst.value(row));
		fileLed->setText(menuValuesBuff.value(base + 2));
		paramMenuLed->setText(menuValuesBuff.value(base + 3));
		shorcutCbx->setCurrentIndex(shorcutLst.
					    indexOf(menuValuesBuff.
						    value(base + 4)));
		if (iconPxm.load(menuValuesBuff.value(base + 5)))
			iconPxm = iconPxm.scaled(QSize(30, 30));
		else
			iconPxm = QPixmap();
		toolbarLbl->setPixmap(iconPxm);
		if (menuValuesBuff.value(base + 6).toInt())
			needsOpenModel->setCheckState(Qt::Checked);
		else
			needsOpenModel->setCheckState(Qt::Unchecked);
		if (menuValuesBuff.value(base + 7).toInt())
			saveModelBefore->setCheckState(Qt::Checked);
		else
			saveModelBefore->setCheckState(Qt::Unchecked);
		return;
	} else {
		menuEditLed->setText("");
		menuEditLed->setEnabled(false);
		menuEditLstWdt->setEnabled(false);
		removeItemBtn->setEnabled(false);
		fileLed->setText("");
		paramMenuLed->setText("");
		shorcutCbx->setCurrentIndex(0);
		iconPxm = QPixmap();
		toolbarLbl->setPixmap(iconPxm);
		needsOpenModel->setCheckState(Qt::Unchecked);
		saveModelBefore->setCheckState(Qt::Unchecked);
		return;
	}
}

void PowerDialogs::selectIconPxm()
{
	QDir dir(QCoreApplication::applicationDirPath());
	QString fileName =
	    QFileDialog::getOpenFileName(this, "Select Icon", "../bin",
					 "Image Files (*.bmp *.jpg *.png);;All Files (*.*)");
	if (!fileName.isEmpty()) {
		iconPxm.load(dir.relativeFilePath(fileName));
		iconPxm = iconPxm.scaled(QSize(30, 30));
		toolbarLbl->setPixmap(iconPxm);
		menuValuesBuff.replace(BASE_MENU_BUFFER *
				       menuEditLstWdt->currentRow() + 5,
				       dir.relativeFilePath(fileName));
	}
	return;
}

void PowerDialogs::removeIconPxm()
{
	iconPxm = QPixmap();
	toolbarLbl->setPixmap(iconPxm);
	menuValuesBuff.replace(BASE_MENU_BUFFER * menuEditLstWdt->currentRow() +
			       5, "");
	return;
}

void PowerDialogs::updateMenuEditLstWdt(const QString & text)
{
	if (menuEditLst.count() > 0) {
		menuValuesBuff.
		    replace((BASE_MENU_BUFFER * menuEditLstWdt->currentRow() +
			     1), text);
		menuEditLst.replace(menuEditLstWdt->currentRow(), text);
		menuEditLstWdt->currentItem()->setText(text);
		return;
	}
	return;
}

void PowerDialogs::updateFileLed(const QString & text)
{
	if (menuEditLst.count() > 0) {
		menuValuesBuff.replace(BASE_MENU_BUFFER *
				       menuEditLstWdt->currentRow() + 2, text);
		return;
	}
}

void PowerDialogs::updateNeedsOpenModel(int state)
{
	if (menuEditLst.count() > 0) {
		if (state == 2)
			menuValuesBuff.replace(BASE_MENU_BUFFER *
					       menuEditLstWdt->currentRow() + 6,
					       "1");
		else
			menuValuesBuff.replace(BASE_MENU_BUFFER *
					       menuEditLstWdt->currentRow() + 6,
					       "0");
		return;
	}
}

void PowerDialogs::updateSaveModelBefore(int state)
{
	if (menuEditLst.count() > 0) {
		if (state == 2)
			menuValuesBuff.replace(BASE_MENU_BUFFER *
					       menuEditLstWdt->currentRow() + 7,
					       "1");
		else
			menuValuesBuff.replace(BASE_MENU_BUFFER *
					       menuEditLstWdt->currentRow() + 7,
					       "0");
		return;
	}
}

void PowerDialogs::updateParamLed(const QString & text)
{
	if (menuEditLst.count() > 0) {
		menuValuesBuff.replace(BASE_MENU_BUFFER *
				       menuEditLstWdt->currentRow() + 3, text);
		return;
	}
}

void PowerDialogs::updateMenuEditBuffer(const QString & text)
{
	menuValuesBuff.replace(0, text);
	return;
}

void PowerDialogs::updateShorcutCbx(const QString & text)
{
	if (menuEditLst.count() > 0) {
		QString sc =
		    menuValuesBuff.value(BASE_MENU_BUFFER *
					 menuEditLstWdt->currentRow() + 4);
		if (shorcutDisabled.contains(text) && text != sc) {
			QMessageBox qMessage(QMessageBox::Critical,
					     QString("PowerDEVS"),
					     QString
					     ("The shorcut is already selected"));
			qMessage.exec();
			shorcutCbx->setCurrentIndex(shorcutLst.indexOf(sc));
		} else {
			menuValuesBuff.replace(BASE_MENU_BUFFER *
					       menuEditLstWdt->currentRow() + 4,
					       text);
			shorcutDisabled.removeOne(sc);
			shorcutDisabled << text;
		}
		return;
	}
}

bool PowerDialogs::checkMenuValues()
{
	// Check if new menu values are OK.
	if (menuLed->text() == "") {
		QMessageBox qMessage(QMessageBox::Critical,
				     QString("PowerDEVS"),
				     QString("Wrong Menu Name!!!"));
		qMessage.exec();
		return false;
	}
	int i = 1;
	while (i < menuValuesBuff.size()) {
		if (menuValuesBuff.value(i) == "") {
			QMessageBox qMessage(QMessageBox::Critical,
					     QString("PowerDEVS"),
					     QString("Wrong Item Name!!!"));
			qMessage.exec();
			return false;
		}
		if (menuValuesBuff.value(i + 1) == "") {
			QMessageBox qMessage(QMessageBox::Critical,
					     QString("PowerDEVS"),
					     QString("Wrong File Program!!!"));
			qMessage.exec();
			return false;
		}
		i += BASE_MENU_BUFFER;
	}
	return true;
}

void PowerDialogs::saveEditMenuSettings()
{
	if (!checkMenuValues())
		return;
	foreach(QStringList item, menuBuff) {
		if (!(item.value(0).compare(menuValuesBuff.value(0)))) {
			menuBuff.removeOne(item);
			break;
		}
	}
	menuLstWdt->currentItem()->setText(menuLed->text());
	menuBuff << menuValuesBuff;
	delete menuEditLed;
	delete menuEditLstWdt;
	delete menuLed;
	delete menuGbx;
	delete saveModelBefore;
	delete needsOpenModel;
	delete commandGbx;
	delete shorcutCbx;
	delete toolbarLbl;
	delete toolbarGbx;
	delete editMenuBbx;
	delete emDialog;
	return;
}

void PowerDialogs::saveNewMenu()
{
	if (!checkMenuValues())
		return;
	menuLstWdt->addItem(menuLed->text());
	menuLst << menuLed->text();
	menuBuff << menuValuesBuff;
	delete menuLed;
	delete menuEditLed;
	delete menuEditLstWdt;
	delete menuGbx;
	delete shorcutCbx;
	delete saveModelBefore;
	delete needsOpenModel;
	delete commandGbx;
	delete toolbarLbl;
	delete toolbarGbx;
	delete removeItemBtn;
	delete editMenuBbx;
	delete emDialog;
	return;
}

void PowerDialogs::cancelEditMenu()
{
	delete menuLed;
	delete menuEditLed;
	delete menuEditLstWdt;
	delete menuGbx;
	delete shorcutCbx;
	delete saveModelBefore;
	delete needsOpenModel;
	delete commandGbx;
	delete toolbarLbl;
	delete toolbarGbx;
	delete removeItemBtn;
	delete editMenuBbx;
	delete emDialog;
}	

void PowerDialogs::addMenuItem()
{
	menuValuesBuff << "" << "" << "" << "" << "" << "0" << "0";
	menuEditLst << "";
	menuEditLstWdt->addItem("");
	menuEditLstWdt->setCurrentRow(menuEditLstWdt->count() - 1);
	commandGbx->setEnabled(true);
	menuEditLed->setEnabled(true);
	menuEditLstWdt->setEnabled(true);
	toolbarGbx->setEnabled(true);
	removeItemBtn->setEnabled(true);
	needsOpenModel->setEnabled(true);
	saveModelBefore->setEnabled(true);
	editMenuBbx->button(QDialogButtonBox::Ok)->setEnabled(true);
	menuEditLed->setFocus();
	return;
}

void PowerDialogs::saveEditMenu()
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	//qDebug("Path: %s",QCoreApplication::applicationDirPath());
	QStringList userMenus =
	    settings.value("userMenus", QStringList()).toStringList();
	qDebug() << "Menues de usuario: " << userMenus;

	// Get modified menu names.

	QStringList newMenuNames = QStringList();
	for (int k = 0; k < menuLstWdt->count(); k++) {
		menuLstWdt->setCurrentRow(k);
		newMenuNames << menuLstWdt->currentItem()->text();
	}

	// Update [General/userMenus] key.

	settings.setValue("userMenus", newMenuNames);

	// Update [menu] group.

	foreach(QString item, userMenus) {
		if (!(newMenuNames.contains(item))) {
			//if(!(menuLst.contains(item))){
			settings.remove("menu/" + item);
		}
	}

	QStringList update = QStringList();
	foreach(QString usrMenu, menuLst) {
		foreach(QStringList buff, menuBuff) {
			if (buff.value(0) == usrMenu) {
				update = buff;
				QString newM =
				    newMenuNames.value(menuLst.
						       indexOf(usrMenu));
				settings.remove("menu/" + buff.value(0));
				int i = 1;
				QStringList subM = QStringList();
				while (i < buff.size()) {
					QStringList add = QStringList();
					add << buff.value(i +
							  1) << buff.value(i +
									   2) <<
					    buff.value(i + 3) << buff.value(i +
									    4)
					    << buff.value(i +
							  5) << buff.value(i +
									   6);
					subM << buff.value(i);
					settings.setValue("menu/" + newM + "/" +
							  buff.value(i), add);
					i += BASE_MENU_BUFFER;
				}
				settings.setValue("menu/" + newM, subM);
				break;
			}
		}
	}
	delete menuLstWdt;
	delete dialog;
	emit updateMenus();
	return;
}

void PowerDialogs::cancelMenuSetup()
{
	delete menuLstWdt;
	delete dialog;
}

void PowerDialogs::removeMenu()
{
	menuLst.removeAt(menuLstWdt->currentRow());
	QListWidgetItem *item = menuLstWdt->currentItem();
	menuLstWdt->removeItemWidget(item);
	delete item;
	return;
}

void PowerDialogs::removeItem()
{
	int rowToDelete = menuEditLstWdt->currentRow();
	int k = 1;
	QListWidgetItem *item = menuEditLstWdt->currentItem();
	QString str = item->text();
	menuEditLstWdt->removeItemWidget(item);
	delete item;
	int top = menuValuesBuff.count();
	while (k < top) {
		if (menuValuesBuff.value(k) == str) {
			menuValuesBuff.removeAt(k);
			menuValuesBuff.removeAt(k);
			menuValuesBuff.removeAt(k);
			menuValuesBuff.removeAt(k);
			menuValuesBuff.removeAt(k);
			menuValuesBuff.removeAt(k);
			menuValuesBuff.removeAt(k);
			break;
		}
		k += BASE_MENU_BUFFER;
	}
	menuEditLst.removeAt(rowToDelete);
	if (menuEditLstWdt->count() == 0)
		disableNewMenuButtons();
	return;
}

void PowerDialogs::disableNewMenuButtons()
{
	commandGbx->setEnabled(false);
	menuEditLed->setEnabled(false);
	menuEditLed->setText("");
	menuEditLstWdt->setEnabled(false);
	toolbarGbx->setEnabled(false);
	editMenuBbx->button(QDialogButtonBox::Ok)->setEnabled(false);
	removeItemBtn->setEnabled(false);
	needsOpenModel->setEnabled(false);
	saveModelBefore->setEnabled(false);
}

void PowerDialogs::editMenu(int btn)
{
	emDialog = new QDialog(this);
	emDialog->setWindowModality(Qt::ApplicationModal);
	emDialog->setFixedSize(DIALOGW, DIALOGH);

	QGridLayout *mainLayout = new QGridLayout;
	QVBoxLayout *menuLayout = new QVBoxLayout;
	QGridLayout *commandLayout = new QGridLayout;
	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	QHBoxLayout *addMenuLayout = new QHBoxLayout;

	editMenuBbx = new QDialogButtonBox(emDialog);
	editMenuBbx->setEnabled(true);
	editMenuBbx->setOrientation(Qt::Horizontal);
	editMenuBbx->
	    setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	editMenuBbx->setCenterButtons(true);

	menuGbx = new QGroupBox(emDialog);
	menuGbx->setAutoFillBackground(true);
	menuGbx->setTitle(tr("Menu"));

	menuLed = new QLineEdit(menuGbx);
	QLabel *menuItemsLbl = new QLabel(tr("Menu Items:"), menuGbx);

	commandGbx = new QGroupBox(emDialog);
	commandGbx->setAutoFillBackground(true);
	commandGbx->setTitle(tr("Command"));

	toolbarGbx = new QGroupBox(emDialog);
	toolbarGbx->setAutoFillBackground(true);
	toolbarGbx->setTitle(tr("Toolbar Icon"));

	menuEditLed = new QLineEdit(menuGbx);
	menuEditLstWdt = new QListWidget(menuGbx);
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	shorcutDisabled = QStringList();

	foreach(QString menu, menuLst) {
		QStringList menus =
		    settings.value("menu/" + menu,
				   QStringList()).toStringList();
		foreach(QString item, menus) {
			shorcutDisabled << settings.value("menu/" + menu + "/" +
							  item,
							  QStringList()).
			    toStringList().value(2);
		}
	}
	menuValuesBuff = QStringList();
	menuEditLst = QStringList();

	QLabel *fileLbl = new QLabel(tr("File program:"), commandGbx);

	fileLed = new QLineEdit(commandGbx);
	QToolButton *fileBtn = new QToolButton(commandGbx);
	fileBtn->setText(tr("..."));

	QLabel *paramMenuLbl = new QLabel(tr("Parameters:"), commandGbx);
	paramMenuLed = new QLineEdit(commandGbx);
	QLabel *shorcutLbl = new QLabel(tr("Shorcut:"), commandGbx);
	shorcutLst =
	    QStringList() << "None" << "F2" << "F3" << "F4" << "F5" << "F6" <<
	    "F7" << "F8" << "F9" << "F10" << "F11" << "F12" << "Ctrl+F1" <<
	    "Ctrl+F2" << "Ctrl+F3" << "Ctrl+F4" << "Ctrl+F5" << "Ctrl+F6" <<
	    "Ctrl+F7" << "Ctrl+F8" << "Ctrl+F9" << "Ctrl+F10" << "Ctrl+F11" <<
	    "Ctrl+F12";
	shorcutCbx = new QComboBox(commandGbx);
	shorcutCbx->addItems(shorcutLst);
	QPushButton *selectBtn = new QPushButton(tr("Select..."), toolbarGbx);
	QPushButton *removeBtn = new QPushButton(tr("Remove"), toolbarGbx);
	toolbarLbl = new QLabel(toolbarGbx);
	toolbarLbl->setMinimumSize(QSize(30, 30));
	saveModelBefore = new QCheckBox(tr("Save Model before"), commandGbx);
	needsOpenModel = new QCheckBox(tr("Needs an open Model"), commandGbx);
	QString actualMenu;

	switch (btn) {
	case 1:
		emDialog->setWindowTitle(tr("Edit Menu"));
		actualMenu = menuLst.value(menuLstWdt->currentRow());
		menuLed->setText(menuLstWdt->currentItem()->text());
		if (menuBuff.isEmpty()) {
			menuEditLst =
			    settings.value("menu/" + actualMenu,
					   QStringList()).toStringList();
			menuValuesBuff << actualMenu;
			qDebug() << menuEditLst;
			foreach(QString menu, menuEditLst) {
				menuValuesBuff << menu;
				menuValuesBuff =
				    menuValuesBuff + settings.value("menu/" +
								    actualMenu +
								    "/" + menu,
								    QStringList
								    ()).
				    toStringList();
				qDebug() << menuValuesBuff;
			}
		} else {
			int notFound = 1;
			foreach(QStringList item, menuBuff) {
				if (!(item.value(0).compare(actualMenu))) {
					qDebug() << item.value(0);
					qDebug() << actualMenu;
					menuValuesBuff = menuValuesBuff + item;
					int i = 1;
					while (i < item.count()) {
						menuEditLst << item.value(i);
						i += BASE_MENU_BUFFER;
					}
					notFound = 0;
					break;
				}
			}
			if (notFound) {
				menuEditLst =
				    settings.value("menu/" + actualMenu,
						   QStringList()).
				    toStringList();
				menuValuesBuff << actualMenu;
				foreach(QString menu, menuEditLst) {
					menuValuesBuff << menu;
					menuValuesBuff =
					    menuValuesBuff +
					    settings.value("menu/" +
							   actualMenu + "/" +
							   menu,
							   QStringList()).
					    toStringList();
				}
			}
		}
		menuEditLstWdt->addItems(menuEditLst);
		menuEditLstWdt->setCurrentRow(0);
		menuEditLed->setText(menuEditLst.value(0));
		fileLed->setText(menuValuesBuff.value(2));
		paramMenuLed->setText(menuValuesBuff.value(3));
		if (menuValuesBuff.value(4) == "")
			shorcutCbx->setCurrentIndex(0);
		else
			shorcutCbx->setCurrentIndex(shorcutLst.
						    indexOf(menuValuesBuff.
							    value(4)));
		iconPxm = QPixmap(menuValuesBuff.value(5));
		iconPxm = iconPxm.scaled(QSize(30, 30));
		if (menuValuesBuff.value(6).toInt())
			needsOpenModel->setCheckState(Qt::Checked);
		else
			needsOpenModel->setCheckState(Qt::Unchecked);
		if (menuValuesBuff.value(7).toInt())
			saveModelBefore->setCheckState(Qt::Checked);
		else
			saveModelBefore->setCheckState(Qt::Unchecked);
		toolbarLbl->setPixmap(iconPxm);
		mainLayout->addWidget(menuGbx, 0, 0, 3, 1);
		mainLayout->addWidget(commandGbx, 0, 1, 2, 1);
		mainLayout->addWidget(toolbarGbx, 2, 1);
		mainLayout->addWidget(editMenuBbx, 3, 0, 1, 2);
		connect(editMenuBbx, SIGNAL(accepted()), this,
			SLOT(saveEditMenuSettings()));
		break;
	case 2:
		emDialog->setWindowTitle(tr("New Menu"));
		iconPxm = QPixmap();
		iconPxm = iconPxm.scaled(QSize(30, 30));
		toolbarLbl->setPixmap(iconPxm);
		menuValuesBuff << menuLed->text();
		QGroupBox *addMenuGbx = new QGroupBox(emDialog);
		addMenuGbx->setAutoFillBackground(true);
		addMenuGbx->setTitle(tr("Menu Item"));
		QPushButton *newBtn = new QPushButton(tr("Add"), addMenuGbx);
		removeItemBtn = new QPushButton(tr("Remove"), addMenuGbx);
		disableNewMenuButtons();
		connect(newBtn, SIGNAL(clicked()), this, SLOT(addMenuItem()));
		connect(removeItemBtn, SIGNAL(clicked()), this,
			SLOT(removeItem()));
		addMenuLayout->setSizeConstraint(QLayout::SetFixedSize);
		addMenuLayout->addWidget(newBtn);
		addMenuLayout->addWidget(removeItemBtn);
		addMenuGbx->setLayout(addMenuLayout);
		mainLayout->addWidget(addMenuGbx, 3, 1);
		mainLayout->addWidget(menuGbx, 0, 0, 4, 1);
		mainLayout->addWidget(commandGbx, 0, 1, 2, 1);
		mainLayout->addWidget(toolbarGbx, 2, 1);
		mainLayout->addWidget(editMenuBbx, 4, 0, 1, 2);
		connect(editMenuBbx, SIGNAL(accepted()), this,
			SLOT(saveNewMenu()));
		connect(menuLed, SIGNAL(textChanged(const QString &)), this,
			SLOT(updateMenuEditBuffer(const QString &)));
		break;
	}

	menuLed->setFocus();
	menuLayout->addWidget(menuLed);
	menuLayout->addWidget(menuItemsLbl);
	menuLayout->addWidget(menuEditLed);
	menuLayout->addWidget(menuEditLstWdt);
	menuGbx->setLayout(menuLayout);

	toolbarLayout->addWidget(selectBtn);
	toolbarLayout->addWidget(removeBtn);
	toolbarLayout->addWidget(toolbarLbl);
	toolbarGbx->setLayout(toolbarLayout);

	commandLayout->addWidget(fileLbl, 0, 0);
	commandLayout->addWidget(fileLed, 1, 0);
	commandLayout->addWidget(fileBtn, 1, 1);
	commandLayout->addWidget(paramMenuLbl, 2, 0);
	commandLayout->addWidget(paramMenuLed, 3, 0, 1, 2);
	commandLayout->addWidget(shorcutLbl, 4, 0);
	commandLayout->addWidget(shorcutCbx, 5, 0, 1, 2);
	commandLayout->addWidget(needsOpenModel, 6, 0, 1, 2);
	commandLayout->addWidget(saveModelBefore, 7, 0, 1, 2);
	commandGbx->setLayout(commandLayout);

	emDialog->setLayout(mainLayout);

	connect(editMenuBbx, SIGNAL(accepted()), emDialog, SLOT(accept()));
	connect(editMenuBbx, SIGNAL(rejected()), emDialog, SLOT(reject()));
	connect(editMenuBbx, SIGNAL(rejected()), this, SLOT(cancelEditMenu()));
	connect(menuEditLstWdt, SIGNAL(currentRowChanged(int)), this,
		SLOT(updateEditMenu(int)));
	connect(selectBtn, SIGNAL(clicked()), this, SLOT(selectIconPxm()));
	connect(removeBtn, SIGNAL(clicked()), this, SLOT(removeIconPxm()));
	connect(menuEditLed, SIGNAL(textChanged(const QString &)), this,
		SLOT(updateMenuEditLstWdt(const QString &)));
	connect(fileLed, SIGNAL(textChanged(const QString &)), this,
		SLOT(updateFileLed(const QString &)));
	connect(paramMenuLed, SIGNAL(textChanged(const QString &)), this,
		SLOT(updateParamLed(const QString &)));
	connect(shorcutCbx, SIGNAL(currentIndexChanged(const QString &)), this,
		SLOT(updateShorcutCbx(const QString &)));
	connect(needsOpenModel, SIGNAL(stateChanged(int)), this,
		SLOT(updateNeedsOpenModel(int)));
	connect(saveModelBefore, SIGNAL(stateChanged(int)), this,
		SLOT(updateSaveModelBefore(int)));

	emDialog->show();
	return;
}

void PowerDialogs::menuSetupDialog()
{
	dialog = new QDialog(this);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->setFixedSize(DIALOGW, DIALOGH);
	dialog->setWindowTitle(tr("Menu Setup"));

	QGridLayout *mainLayout = new QGridLayout;

	QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
	buttonBox->setEnabled(true);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->
	    setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);

	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	menuLst = settings.value("userMenus", QStringList()).toStringList();

	menuLstWdt = new QListWidget(dialog);
	menuLstWdt->addItems(menuLst);
	menuLstWdt->setCurrentRow(0);

	QPushButton *editBtn = new QPushButton("Edit", dialog);
	QPushButton *addBtn = new QPushButton("Add", dialog);
	QPushButton *removeBtn = new QPushButton("Remove", dialog);

	QSignalMapper *sgnMapper = new QSignalMapper(dialog);
	buttonBox->addButton(editBtn, QDialogButtonBox::ActionRole);
	buttonBox->addButton(addBtn, QDialogButtonBox::ActionRole);
	buttonBox->addButton(removeBtn, QDialogButtonBox::ActionRole);
	sgnMapper->setMapping(editBtn, 1);
	sgnMapper->setMapping(addBtn, 2);

	connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveEditMenu()));
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelMenuSetup()));
	connect(sgnMapper, SIGNAL(mapped(int)), this, SLOT(editMenu(int)));
	connect(editBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
	connect(addBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
	connect(removeBtn, SIGNAL(clicked()), this, SLOT(removeMenu()));

	mainLayout->addWidget(menuLstWdt, 0, 0, 1, 2);
	mainLayout->addWidget(buttonBox, 1, 0);
	dialog->setLayout(mainLayout);
	dialog->show();
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// All the following slots are used by paramDialog.
/////////////////////////////////////////////////////////////////////////////////////////////////

// This slot is used to add inPortsor an outPort when one of the parameters value is %inputs and the graphic block 
// is an Atomic, otherwise the value is ignored.

void PowerDialogs::addPorts(GpxConnectionPoint::ConnectionPointType p, int i)
{
	if (p == GpxConnectionPoint::InPort) {
		if (myAtomic->blockType() == GpxBlock::SimpleAtomic) {
			int portsCant = edtParams.value(i)->text().toInt();
			int inPortsOrig = myAtomic->numberOfInPorts();
			if (inPortsOrig < portsCant) {	// The Atomic has more ports. 
				for (int k = inPortsOrig; k < portsCant; k++)
					myAtomic->
					    addPort(GpxConnectionPoint::InPort);
			} else {
				for (int k = inPortsOrig; k > portsCant; k--) {
					GpxConnectionPoint *p =
					    myAtomic->getInportAt(k - 1);
					myAtomic->removePort(p,
							     GpxConnectionPoint::
							     InPort);
				}
			}
		}
	} else {
		if (myAtomic->blockType() == GpxBlock::SimpleAtomic) {
			int outPortsOrig = myAtomic->numberOfOutPorts();
			int portsCant = edtParams.value(i)->text().toInt();
			if (outPortsOrig < portsCant) {
				for (int k = outPortsOrig;
				     k < myAtomic->numberOfOutPorts(); k++)
					myAtomic->
					    addPort(GpxConnectionPoint::
						    OutPort);
			} else {
				for (int k = outPortsOrig;
				     k > myAtomic->numberOfOutPorts(); k--) {
					GpxConnectionPoint *p =
					    myAtomic->getOutportAt(k - 1);
					myAtomic->removePort(p,
							     GpxConnectionPoint::
							     OutPort);
				}
			}
		}
	}
}

void PowerDialogs::saveParams()
{
	int k, i = 0;
	double d = 0;
	bool existIP = false;
	bool existOP = false;
	foreach(modelParameter * p, params) {
		switch (p->type) {
		case STR:
			if (p->strValue == "%Inputs") {
				if (!existIP) {
					existIP = true;
					addPorts(GpxConnectionPoint::InPort, i);
				} else
					p->strValue = "";
			} else {
				if (p->strValue == "%Outputs") {
					if (!existOP) {
						existOP = true;
						addPorts(GpxConnectionPoint::
							 OutPort, i);
					} else
						p->strValue = "";
				} else {
					if (edtParams.value(i)->text() ==
					    "%Inputs") {
						if (!existIP) {
							existIP = true;
							p->strValue = "%Inputs";
						} else
							p->strValue = "";
					} else {
						if (edtParams.value(i)->
						    text() == "%Outputs") {
							if (!existOP) {
								existOP = true;
								p->strValue =
								    "%Outputs";
							} else
								p->strValue =
								    "";
						} else
							p->strValue =
							    edtParams.value(i)->
							    text();
					}
				}
			}
			break;
		case VAL:
			d = edtParams.value(i)->text().toDouble();
			p->dValue = d;
			break;
		case SCILAB:
			d = edtParams.value(i)->text().toDouble();
			p->dValue = d;
			break;
		case LST:
			k = edtParams.value(i)->text().toInt();
			p->lsValue = cbxParams.value(k)->currentIndex() + 1;
			break;
		}
		i++;
	}
	edtParams.clear();
	cbxParams.clear();
	myAtomic = NULL;
	myCoupled = NULL;
	delete descLbl;
	delete dialog;
	emit dirtyChanged();
	return;
}

void PowerDialogs::cancelParams()
{
	edtParams.clear();
	cbxParams.clear();
	myAtomic = NULL;
	myCoupled = NULL;
	delete descLbl;
	delete dialog;
}	

void PowerDialogs::saveModelParams()
{
	int k, i = 0;
	double d = 0;
	foreach(modelParameter * p, params) {
		switch (p->type) {
		case STR:
			if (edtParams.value(i)->text() == "%Inputs")
				p->strValue = "";
			else {
				if (edtParams.value(i)->text() == "%Outputs")
					p->strValue = "";
				else
					p->strValue =
					    edtParams.value(i)->text();
			}
			break;
		case VAL:
			d = edtParams.value(i)->text().toDouble();
			p->dValue = d;
			break;
		case SCILAB:
			d = edtParams.value(i)->text().toDouble();
			p->dValue = d;
			break;
		case LST:
			k = edtParams.value(i)->text().toInt();
			p->lsValue = cbxParams.value(k)->currentIndex() + 1;
			break;
		}
		i++;
	}
	edtParams.clear();
	cbxParams.clear();
	myModelCoupled->desc = descLbl->toPlainText();
	myModelCoupled = NULL;
	delete descLbl;
	delete dialog;
	emit dirtyChanged();
	return;
}

void PowerDialogs::blockParamDialog(GpxBlock * a)
{
	if (a->blockType() == GpxBlock::SimpleAtomic) {
		myAtomic = (GpxAtomic *) a;
	} else
		myCoupled = (GpxCoupled *) a;
	paramDialog(a->getName(), a->getDescription(), true, a->getParams());
}

void PowerDialogs::modelParamDialog(modelCoupled * mp)
{
	myModelCoupled = mp;
	paramDialog(mp->name, mp->desc, false, mp->params);
}

void PowerDialogs::paramDialog(QString name, QString desc, bool bt,
			       QList < modelParameter * >parameters)
{
	dialog = new QDialog(this);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->setWindowTitle(tr("Parameters: ") + name);
	dialog->setFixedWidth(DIALOGW);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QVBoxLayout *descLayout = new QVBoxLayout;
	QVBoxLayout *paramLayout = new QVBoxLayout;

	QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
	buttonBox->setEnabled(true);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);

	QGroupBox *descGbx = new QGroupBox(dialog);
	descGbx->setAutoFillBackground(true);
	descGbx->setTitle(tr("Description"));
	QGroupBox *paramGbx = new QGroupBox(dialog);
	paramGbx->setAutoFillBackground(true);
	paramGbx->setTitle(tr("Parameters"));

	descLbl = new QTextEdit(dialog);
	descLbl->setMaximumHeight(80);
	if (bt)
		descLbl->setReadOnly(true);
	descLbl->setText(desc);
	descLayout->addWidget(descLbl);
	descGbx->setLayout(descLayout);
	mainLayout->addWidget(descGbx);
	params = parameters;
	edtParams = QList < QLineEdit * >();
	QList < QLabel * >lblParams = QList < QLabel * >();
	if (!(params.isEmpty())) {
		int k = 0;
		foreach(modelParameter * p, params) {
			QLabel *tmpLbl = new QLabel(paramGbx);
			QLineEdit *tmpLed = new QLineEdit(paramGbx);
			QComboBox *tmpCbx = new QComboBox(paramGbx);
			tmpCbx->setVisible(false);
			switch (p->type) {
			case STR:
				tmpLbl->setText(p->name + ": " + p->desc);
				lblParams << tmpLbl;
				if (p->strValue == "%Inputs")
					tmpLed->setText(QString().
							setNum(myAtomic->
							       numberOfInPorts
							       ()));
				else {
					if (p->strValue == "%Outputs")
						tmpLed->setText(QString().
								setNum
								(myAtomic->
								 numberOfOutPorts
								 ()));
					else
						tmpLed->setText(p->strValue);
				}
				edtParams << tmpLed;
				break;
			case VAL:
				tmpLbl->setText(p->name + ": " + p->desc);
				lblParams << tmpLbl;
				tmpLed->setText(QString().setNum(p->dValue));
				edtParams << tmpLed;
				break;
			case LST:
				tmpLbl->setText(p->name + ": " + p->desc);
				lblParams << tmpLbl;
				tmpLed->setText(QString().setNum(k));
				tmpLed->setVisible(false);
				tmpCbx->addItems(p->lsValues);
				tmpCbx->setCurrentIndex(p->lsValue - 1);
				tmpCbx->setVisible(true);
				cbxParams << tmpCbx;
				edtParams << tmpLed;
				break;
			case SCILAB:
				tmpLbl->setText(p->name + ": " + p->desc);
				lblParams << tmpLbl;
				tmpLed->setText(QString().setNum(p->dValue));
				edtParams << tmpLed;
				break;
			}
			paramLayout->addWidget(lblParams.
					       value(params.indexOf(p)));
			if (p->type == LST) {
				paramLayout->addWidget(cbxParams.value(k));
				k++;
			} else {
				paramLayout->addWidget(edtParams.
						       value(params.
							     indexOf(p)));
			}
		}
		buttonBox->addButton(QDialogButtonBox::Cancel);
		paramGbx->setLayout(paramLayout);
		mainLayout->addWidget(paramGbx);
	} else
		paramGbx->setVisible(false);

	mainLayout->addWidget(buttonBox);
	dialog->setLayout(mainLayout);
	connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
	if (bt)
		connect(buttonBox, SIGNAL(accepted()), this,
			SLOT(saveParams()));
	else
		connect(buttonBox, SIGNAL(accepted()), this,
			SLOT(saveModelParams()));
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelParams()));
	dialog->show();
	dialog->setFixedHeight(dialog->height());
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the following slots are used by priorityDialog.
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void PowerDialogs::updatePriority(int p)
{
	int row = priLstWdt->currentRow();
	if (p == 1) {
		if (row > 0) {
			QListWidgetItem *tmp = priLstWdt->takeItem(row);
			priLstWdt->insertItem(row - 1, tmp);
			priLstWdt->setCurrentItem(tmp);
			childs.swap(row, row - 1);
		}
		return;
	} else {
		if (row < (priLstWdt->count() - 1) && row >= 0) {
			QListWidgetItem *tmp = priLstWdt->takeItem(row);
			priLstWdt->insertItem(row + 1, tmp);
			priLstWdt->setCurrentItem(tmp);
			childs.swap(row, row + 1);
		}
		return;
	}
}

void PowerDialogs::savePriority()
{
	myEdtScene->coupledData->childs = childs;
	int i = 0;
	foreach(modelChild * mc, myEdtScene->coupledData->childs) {
		if (mc->childType == ATOMIC)
			mc->atomic->priority = i;
		else
			mc->coupled->priority = i;
		i++;
	}
	myEdtScene = NULL;
	childs.clear();
	delete myEdtScene;
	delete priLstWdt;
	delete dialog;
	emit dirtyChanged();
	return;
}

void PowerDialogs::cancelPriority()
{
	myEdtScene = NULL;
	childs.clear();
	delete myEdtScene;
	delete priLstWdt;
	delete dialog;
}

void PowerDialogs::priorityDialog(GpxEditionScene * mc)
{
	dialog = new QDialog(this);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->setFixedSize(DIALOGW, DIALOGH);
	dialog->setWindowTitle(tr("Select Priority"));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QGridLayout *priorityLayout = new QGridLayout;

	QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
	buttonBox->setEnabled(true);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->
	    setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);

	QGroupBox *priGbx = new QGroupBox(dialog);
	priGbx->setAutoFillBackground(true);
	priGbx->setTitle(tr("Priority"));

	priLstWdt = new QListWidget(priGbx);

	myEdtScene = mc;
	childs = myEdtScene->getChilds();
	foreach(modelChild * m, childs) {
		if (m->childType == ATOMIC)
			priLstWdt->addItem(m->atomic->name);
		else
			priLstWdt->addItem(m->coupled->name);
	}
	priLstWdt->setCurrentRow(0);

	QToolButton *upBtn = new QToolButton(priGbx);
	upBtn->setArrowType(Qt::UpArrow);
	QToolButton *downBtn = new QToolButton(priGbx);
	downBtn->setArrowType(Qt::DownArrow);

	QSignalMapper *sgnMapper = new QSignalMapper(dialog);
	sgnMapper->setMapping(upBtn, 1);
	sgnMapper->setMapping(downBtn, 2);

	priorityLayout->addWidget(priLstWdt, 0, 0, 3, 1);
	priorityLayout->addWidget(upBtn, 1, 1);
	priorityLayout->addWidget(downBtn, 2, 1);
	priGbx->setLayout(priorityLayout);

	mainLayout->addWidget(priGbx);
	mainLayout->addWidget(buttonBox);
	dialog->setLayout(mainLayout);

	connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(savePriority()));
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelPriority()));
	connect(sgnMapper, SIGNAL(mapped(int)), this,
		SLOT(updatePriority(int)));
	connect(upBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
	connect(downBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
	dialog->show();
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//      All the following slots are used by editAtomicDialog.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PowerDialogs::swapPorts(int p)
{
	int row, swap;

	if (p == 1 || p == 2)
		row = inputLstWdt->currentRow();
	else
		row = outputLstWdt->currentRow();

	switch (p) {
	case 1:
		if (row == 0)
			return;
		else
			swap = row - 1;
		break;
	case 2:
		if (row == inputLstWdt->count() - 1)
			return;
		else
			swap = row + 1;
		break;
	case 3:
		if (row == 0)
			return;
		else
			swap = row - 1;
		break;
	case 4:
		if (row == outputLstWdt->count() - 1)
			return;
		else
			swap = row + 1;
		break;
	}
	if (p == 1 || p == 2) {
		QListWidgetItem *tmp = inputLstWdt->takeItem(row);
		inputLstWdt->insertItem(swap, tmp);
		inputLstWdt->setCurrentItem(tmp);
		// Only a coupled, if there is an atomic here, its an error!!!!
		//myCoupled->getData()->lsPorts.swap(row,swap);
		myInPorts.swap(row, swap);
	} else {
		QListWidgetItem *tmp = outputLstWdt->takeItem(row);
		outputLstWdt->insertItem(swap, tmp);
		outputLstWdt->setCurrentItem(tmp);
		// The same as before.
		//myCoupled->getData()->lsPorts.swap(row,swap);
		myOutPorts.swap(row, swap);
	}
	return;
}

void PowerDialogs::updateFiles(QListWidgetItem * item)
{
	if (!directoryChange) {
		fi->setFile(fileDir->absoluteFilePath(item->text()));
		dirPathLbl->setText(dir->
				    relativeFilePath(fi->absoluteFilePath()));
	}
	return;
}

void PowerDialogs::updateDirs(QListWidgetItem * item)
{
	directoryChange = true;
	fileInfLst = fileDir->entryInfoList();
	if (atomicsLstWdt->count() != fileInfLst.count()) {
		foreach(QFileInfo f, fileInfLst)
		    QList < QListWidgetItem * >itemExist =
		    atomicsLstWdt->findItems(f.fileName(), Qt::MatchExactly);
	}
	fileDir->setPath(atomicsPath + "/" + item->text());
	fileLstWdt->clear();
	// Folder file list.
	folderFileInfLst = fileDir->entryInfoList();
	foreach(QFileInfo f, folderFileInfLst) {
		fileLstWdt->addItem(f.fileName());
	}
	directoryChange = false;
	return;
}

void PowerDialogs::newCodeFile()
{
	emit editAtomicCode("");
}

void PowerDialogs::editCode()
{
	if(dir->exists(fi->absoluteFilePath()))
		emit editAtomicCode(dir->relativeFilePath(fi->absoluteFilePath()));
	else
		emit editAtomicCode("");
	return;
}

// Cambio de los leds (cuando cambia de fila en la grilla)
void PowerDialogs::editAtomicValues(int row)
{
	if (row >= 0) {
		paramNameLed->setText(paramTblWdt->item(row, 0)->text());
		typeCbx->setCurrentIndex(typeCbx->
					 findText(paramTblWdt->item(row, 1)->
						  text()));
		paramDescTed->setText(paramTblWdt->item(row, 3)->text());
		/* The number of values in myParams has to be different than 1, because when you delete an
		   item you read from myParams with the grid updated and it signals segmentation fault     */
		if (paramTblWdt->item(row, 1)->text() == "LIST"
		    && myParams.count() != 1) {
			if (myParams.value(row)->type == LST) {
				valueCbx->clear();
				valueCbx->addItems(myParams.value(row)->
						   lsValues);
				valueCbx->setCurrentIndex(myParams.value(row)->
							  lsValue);
				paramTblWdt->item(row,
						  2)->setText(valueCbx->
							      currentText());
			} else {
				if (myParams.value(row)->lsValues.isEmpty()) {
					myParams.value(row)->lsValues =
					    QStringList();
					myParams.value(row)->lsValue = 0;
					valueCbx->clear();
				} else {
					valueCbx->clear();
					valueCbx->addItems(myParams.value(row)->
							   lsValues);
					valueCbx->setCurrentIndex(myParams.
								  value(row)->
								  lsValue);
					paramTblWdt->item(row,
							  2)->setText(valueCbx->
								      currentText
								      ());
				}
			}
			edtBtn->setEnabled(true);
			valueCbx->setVisible(true);
			valueLed->setVisible(false);
		} else {
			valueLed->setText(paramTblWdt->item(row, 2)->text());
			edtBtn->setEnabled(false);
			valueCbx->setVisible(false);
			valueLed->setVisible(true);
		}
	}
	return;
}

// Update grid values.
void PowerDialogs::updateAtomicValues(int column)
{
	int row = paramTblWdt->currentRow();
	switch (column) {
	case 0:
		paramTblWdt->item(row, 0)->setText(paramNameLed->text());
		myParams.value(row)->name = paramNameLed->text();
		break;
	case 1:
		paramTblWdt->item(row, 1)->setText(typeCbx->currentText());
		if (typeCbx->currentText() == "LIST") {
			valueLed->setVisible(false);
			valueLed->setText("");
			valueCbx->setVisible(true);
			edtBtn->setEnabled(true);
			if (myParams.value(row)->type == LST) {
				valueCbx->clear();
				valueCbx->addItems(myParams.value(row)->
						   lsValues);
				valueCbx->setCurrentIndex(myParams.value(row)->
							  lsValue);
				paramTblWdt->item(row,
						  2)->setText(valueCbx->
							      currentText());
			} else {
				if (myParams.value(row)->lsValues.isEmpty()) {
					myParams.value(row)->lsValues =
					    QStringList();
					myParams.value(row)->lsValue = 0;
					valueCbx->clear();
				} else {
					valueCbx->clear();
					valueCbx->addItems(myParams.value(row)->
							   lsValues);
					valueCbx->setCurrentIndex(myParams.
								  value(row)->
								  lsValue);
					paramTblWdt->item(row,
							  2)->setText(valueCbx->
								      currentText
								      ());
				}
			}
		} else {
			valueLed->setVisible(true);
			valueCbx->setVisible(false);
			edtBtn->setEnabled(false);
		}
		myParams.value(row)->setTypeFromString(typeCbx->currentText());
		break;
	case 2:
		paramTblWdt->item(row, 2)->setText(valueLed->text());
		switch (myParams.value(row)->type) {
		case STR:
			myParams.value(row)->strValue = valueLed->text();
			break;
		case VAL:
			bool ok;
			double d = valueLed->text().toDouble(&ok);
			if (ok)
				myParams.value(row)->dValue = d;
			else
				myParams.value(row)->dValue = 0.0;
			break;
		}
		break;
	case 3:
		paramTblWdt->item(row, 2)->setText(valueCbx->currentText());
		myParams.value(row)->lsValue = valueCbx->currentIndex();
		break;
	case 4:
		paramTblWdt->item(row, 3)->setText(paramDescTed->toPlainText());
		myParams.value(row)->desc = paramDescTed->toPlainText();
		break;
	}
	return;
}

void PowerDialogs::newParam()
{
	int row = paramTblWdt->rowCount();
	if (row == 0)
		enableEditAtomicParams();
	modelParameter *tmp = new modelParameter();
	tmp->name = "";
	tmp->desc = "";
	tmp->type = STR;
	tmp->strValue = "";
	myParams << tmp;
	QTableWidgetItem *item0 = new QTableWidgetItem("");
	QFont font = item0->font();
	font.setPixelSize(9);
	item0->setFont(font);
	item0->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	QTableWidgetItem *item1 = new QTableWidgetItem("");
	item1->setFont(font);
	item1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	QTableWidgetItem *item2 = new QTableWidgetItem("");
	item2->setFont(font);
	item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	QTableWidgetItem *item3 = new QTableWidgetItem("");
	item3->setFont(font);
	item3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	paramTblWdt->insertRow(row);
	paramTblWdt->setItem(row, 0, item0);
	paramTblWdt->setItem(row, 1, item1);
	paramTblWdt->setItem(row, 2, item2);
	paramTblWdt->setItem(row, 3, item3);
	paramTblWdt->item(row, 1)->setText(tmp->typeToString());
	paramTblWdt->setCurrentCell(row, 0);
	paramNameLed->setFocus();
	return;
}

void PowerDialogs::updatePos(int row, int column)
{
	switch (column) {
	case 0:
		paramNameLed->setFocus();
		break;
	case 1:
		typeCbx->setFocus();
		break;
	case 2:
		if (typeCbx->currentText() == "LIST")
			valueCbx->setFocus();
		else
			valueLed->setFocus();
		break;
	case 3:
		paramDescTed->setFocus();
		break;
	}
	return;
}

void PowerDialogs::delParam()
{
	int row = paramTblWdt->currentRow();
	myParams.removeAt(row);
	if (myParams.isEmpty()) {
		paramNameLed->clear();
		paramDescTed->clear();
		valueLed->clear();
		if (typeCbx->currentText() == "LIST") {
			valueCbx->clear();
			valueCbx->setVisible(false);
			valueLed->setVisible(true);
		}
		typeCbx->setCurrentIndex(0);
		disableEditAtomicParams();
	}
	paramTblWdt->removeRow(row);
	if (paramTblWdt->rowCount() > 0)
		paramTblWdt->setCurrentCell(paramTblWdt->currentRow(), 0);
	return;
}

void PowerDialogs::swapParams(int btn)
{
	int row = paramTblWdt->currentRow();
	int swap;
	QTableWidgetItem *item0 =
	    new QTableWidgetItem(paramTblWdt->item(row, 0)->text());
	QFont font = item0->font();
	font.setPixelSize(9);
	item0->setFont(font);
	QTableWidgetItem *item1 =
	    new QTableWidgetItem(paramTblWdt->item(row, 1)->text());
	item1->setFont(font);
	QTableWidgetItem *item2 =
	    new QTableWidgetItem(paramTblWdt->item(row, 2)->text());
	item2->setFont(font);
	QTableWidgetItem *item3 =
	    new QTableWidgetItem(paramTblWdt->item(row, 3)->text());
	item3->setFont(font);
	switch (btn) {
	case 1:
		if (row == 0)
			return;
		else
			swap = row - 1;
		break;
	case 2:
		if (row == paramTblWdt->rowCount() - 1)
			return;
		else
			swap = row + 1;
		break;
	}
	paramTblWdt->removeRow(row);
	paramTblWdt->insertRow(swap);
	paramTblWdt->setItem(swap, 0, item0);
	paramTblWdt->setItem(swap, 1, item1);
	paramTblWdt->setItem(swap, 2, item2);
	paramTblWdt->setItem(swap, 3, item3);
	myParams.swap(row, swap);
	paramTblWdt->setCurrentCell(swap, 0);
	return;
}

void PowerDialogs::newListParam()
{
	int row = listParamTblWdt->rowCount();
	QTableWidgetItem *item = new QTableWidgetItem("");
	listParamTblWdt->insertRow(row);
	listParamTblWdt->setItem(row, 0, item);
	listParamTblWdt->setCurrentCell(row, 0);
	listParamTblWdt->setFocus();
	delListParamValueBtn->setEnabled(true);
	return;
}

void PowerDialogs::delListParam()
{
	int row = listParamTblWdt->currentRow();
	listParamTblWdt->removeRow(row);
	if (listParamTblWdt->rowCount() > 0)
		listParamTblWdt->setCurrentCell(listParamTblWdt->currentRow(),
						0);
	else
		delListParamValueBtn->setEnabled(false);
	return;
}

void PowerDialogs::saveListParam()
{
	int total = listParamTblWdt->rowCount();
	int row = paramTblWdt->currentRow();
	myParams.value(row)->lsValues.clear();
	myParams.value(row)->lsValue = 0;
	for (int k = 0; k < total; k++) {
		QString param = listParamTblWdt->item(k, 0)->text();
		if (param != "")
			myParams.value(row)->lsValues << param;
	}
	typeCbx->setCurrentIndex(1);
	typeCbx->setCurrentIndex(2);
	return;
}

void PowerDialogs::editListParam()
{
	QDialog *editListParamDialog = new QDialog(this);
	editListParamDialog->setWindowModality(Qt::ApplicationModal);
	editListParamDialog->setFixedSize(DIALOGW, DIALOGH);
	editListParamDialog->setWindowTitle(tr("List parameter: "));
	QDialogButtonBox *buttonBox = new QDialogButtonBox(editListParamDialog);
	buttonBox->setEnabled(true);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->
	    setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QVBoxLayout *listParamLayout = new QVBoxLayout;
	QGroupBox *listParamGbx = new QGroupBox(editListParamDialog);
	listParamTblWdt = new QTableWidget(listParamGbx);
	listParamTblWdt->setColumnCount(1);
	QStringList labels = QStringList() << "VALUES";
	listParamTblWdt->setHorizontalHeaderLabels(labels);
	QHeaderView *hv = listParamTblWdt->horizontalHeader();
	hv->setResizeMode(QHeaderView::Fixed);
	hv->setStretchLastSection(true);
	listParamTblWdt->setSelectionBehavior(QAbstractItemView::SelectRows);
	int row = paramTblWdt->currentRow();
	QPushButton *newBtn = new QPushButton(listParamGbx);
	newBtn->setText(tr("&New"));
	delListParamValueBtn = new QPushButton(listParamGbx);
	delListParamValueBtn->setText(tr("&Delete"));
	if (!myParams.value(row)->lsValues.isEmpty()) {
		int i = 0;
		foreach(QString str, myParams.value(row)->lsValues) {
			QTableWidgetItem *item = new QTableWidgetItem(str);
			item->
			    setFlags(Qt::ItemIsSelectable | Qt::
				     ItemIsEditable | Qt::ItemIsEnabled);
			listParamTblWdt->insertRow(i);
			listParamTblWdt->setItem(i, 0, item);
			i++;
		}
	} else
		delListParamValueBtn->setEnabled(false);
	buttonBox->addButton(delListParamValueBtn,
			     QDialogButtonBox::ActionRole);
	buttonBox->addButton(newBtn, QDialogButtonBox::ActionRole);

	listParamLayout->addWidget(listParamTblWdt);
	listParamGbx->setLayout(listParamLayout);
	mainLayout->addWidget(listParamGbx);
	mainLayout->addWidget(buttonBox);
	editListParamDialog->setLayout(mainLayout);
	editListParamDialog->show();
	connect(buttonBox, SIGNAL(accepted()), editListParamDialog,
		SLOT(accept()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveListParam()));
	connect(buttonBox, SIGNAL(rejected()), editListParamDialog,
		SLOT(reject()));
	connect(newBtn, SIGNAL(clicked()), this, SLOT(newListParam()));
	connect(delListParamValueBtn, SIGNAL(clicked()), this,
		SLOT(delListParam()));
	return;
}

void PowerDialogs::selectAtomicIcon()
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString binPath =
	    settings.value("path/binPath",
			   "Value not found in file powerdevs.ini").toString();
	QString iconName =
	    QFileDialog::getOpenFileName(this, "Select Icon", binPath,
					 "Image Files (*.bmp *.jpg *.png *.svg);;All Files (*.*)");
	QDir *atomicsDir = new QDir(atomicsPath);
	if (!iconName.isEmpty()) {
		iconLed->setText(atomicsDir->relativeFilePath(iconName));
		if (block->blockType() == GpxBlock::SimpleAtomic)
			myAtomic->setIcon(atomicsDir->
					  absoluteFilePath(iconName));
		else
			myCoupled->setIcon(atomicsDir->
					   absoluteFilePath(iconName));
	}
	delete atomicsDir;
	return;
}

void PowerDialogs::updateInPorts(int n)
{
	QRectF old = scene->sceneRect();
	if (n < 0)
		return;
	int noi;
	if (block->blockType() == GpxBlock::SimpleAtomic)
		noi = myAtomic->numberOfInPorts();
	else
		noi = myCoupled->numberOfInPorts();
	if (noi < n) {
		int k = n - noi;
		while (k > 0) {
			if (block->blockType() == GpxBlock::SimpleAtomic)
				myAtomic->addPort(GpxConnectionPoint::InPort);
			else
				myCoupled->addPort(GpxConnectionPoint::InPort);
			k--;
		}
	} else {
		int k = noi - n;
		while (k > 0) {
			if (block->blockType() == GpxBlock::SimpleAtomic)
				myAtomic->removePort(myAtomic->getInportAt(n),
						     GpxConnectionPoint::
						     InPort);
			else
				myCoupled->removePort(myCoupled->getInportAt(n),
						      GpxConnectionPoint::
						      InPort);
			k--;
		}
	}
	scene->setSceneRect(old);
	return;
}

void PowerDialogs::updateOutPorts(int n)
{
	QRectF old = scene->sceneRect();
	if (n < 0)
		return;
	int noi;
	if (block->blockType() == GpxBlock::SimpleAtomic)
		noi = myAtomic->numberOfOutPorts();
	else
		noi = myCoupled->numberOfOutPorts();
	if (noi < n) {
		int k = n - noi;
		while (k > 0) {
			if (block->blockType() == GpxBlock::SimpleAtomic)
				myAtomic->addPort(GpxConnectionPoint::OutPort);
			else
				myCoupled->addPort(GpxConnectionPoint::OutPort);
			k--;
		}
	} else {
		int k = noi - n;
		while (k > 0) {
			if (block->blockType() == GpxBlock::SimpleAtomic)
				myAtomic->removePort(myAtomic->getOutportAt(n),
						     GpxConnectionPoint::
						     OutPort);
			else
				myCoupled->removePort(myCoupled->
						      getOutportAt(n),
						      GpxConnectionPoint::
						      OutPort);
			k--;
		}
	}
	scene->setSceneRect(old);
	return;
}

void PowerDialogs::updateAtomicColor(int i)
{
	if (block->blockType() == GpxBlock::SimpleAtomic)
		myAtomic->setBackgroundColor(colors.value(i));
	else
		myCoupled->setBackgroundColor(colors.value(i));
	return;
}

void PowerDialogs::applyPropertiesChanges()
{
	// Save properties tab changes. 
	// Update inports and outports, first remove all ports in the Atomic, then add 
	// myAtomic ports.
	int inPortsOrig;
	int outPortsOrig;
	int noi, noo;
	if (block->blockType() == GpxBlock::SimpleAtomic) {
		origAtomic->setName(myAtomic->getName());
		inPortsOrig = origAtomic->numberOfInPorts();
		outPortsOrig = origAtomic->numberOfOutPorts();
		noi = myAtomic->numberOfInPorts();
		noo = myAtomic->numberOfOutPorts();
		origAtomic->getData()->desc = myAtomic->getData()->desc;
		//origAtomic->setSize(myAtomic->getBlockWidth(),myAtomic->getBlockHeight());
		origAtomic->setSize(widthSbx->value(), heightSbx->value());
		origAtomic->setDirection(myAtomic->getDirection());
		origAtomic->setBackgroundColor(myAtomic->getBackgroundColor());
		origAtomic->setIcon(myAtomic->getIcon());
	} else {
		origCoupled->setName(myCoupled->getName());
		inPortsOrig = origCoupled->numberOfInPorts();
		outPortsOrig = origCoupled->numberOfOutPorts();
		noi = myCoupled->numberOfInPorts();
		noo = myCoupled->numberOfOutPorts();
		origCoupled->getData()->desc = myCoupled->getData()->desc;
		//origCoupled->setSize(myCoupled->getBlockWidth(),myCoupled->getBlockHeight());
		origCoupled->setSize(widthSbx->value(), heightSbx->value());
		origCoupled->setDirection(myCoupled->getDirection());
		origCoupled->setBackgroundColor(myCoupled->
						getBackgroundColor());
		origCoupled->setIcon(myCoupled->getIcon());
	}
	/* For the moment, only atomics ports are editable. */
	if (inPortsOrig < noi) {	// The Atomic has more ports. 
		for (int k = inPortsOrig; k < noi; k++) {
			origAtomic->addPort(GpxConnectionPoint::InPort);
		}
	} else {
		for (int k = inPortsOrig; k > noi; k--) {
			GpxConnectionPoint *p;
			if (block->blockType() == GpxBlock::SimpleAtomic)
				p = origAtomic->getInportAt(k - 1);
			else
				p = origCoupled->getInportAt(k - 1);
			origAtomic->removePort(p, GpxConnectionPoint::InPort);
		}
	}
	if (outPortsOrig < noo) {
		for (int k = outPortsOrig; k < noo; k++) {
			origAtomic->addPort(GpxConnectionPoint::OutPort);
		}
	} else {
		for (int k = outPortsOrig; k > noo; k--) {
			GpxConnectionPoint *p;
			if (block->blockType() == GpxBlock::SimpleAtomic)
				p = origAtomic->getOutportAt(k - 1);
			else
				p = origCoupled->getOutportAt(k - 1);
			origAtomic->removePort(p, GpxConnectionPoint::OutPort);
		}
	}
}

void PowerDialogs::applyParametersChanges()
{
	QList < modelParameter * >mp;
	// First, remove the original Atomic params.

	if (block->blockType() == GpxBlock::SimpleAtomic) {
		mp = origAtomic->getData()->params;
	} else
		mp = origCoupled->getData()->params;

	while (mp.count() > 0) {
		modelParameter *p = mp.takeAt(0);
		delete p;
	}

	mp.clear();
	// Replace parameters list.
	if (block->blockType() == GpxBlock::SimpleAtomic) {
		origAtomic->getData()->params.clear();
		foreach(modelParameter * p, myParams)
		    if (p->name != "")	// If there's a parameter without a name, ignore it.
			origAtomic->getData()->params << p->duplicate();
	} else {
		origCoupled->getData()->params.clear();
		foreach(modelParameter * p, myParams)
		    if (p->name != "")
			origCoupled->getData()->params << p->duplicate();
	}
}

void PowerDialogs::applyCodeChange()
{
	QString file = dirPathLbl->text();
	file.replace(QString("/"), QString("\\"));
	qDebug() << "Archivo:  " << file;
	origAtomic->getData()->path = file;
	return;
}

void PowerDialogs::applyIOChange()
{
	// The number of inports and outports have been updated before.
	origCoupled->getData()->lsInPorts.clear();
	origCoupled->getData()->lsOutPorts.clear();
	int i = 0;
	foreach(modelPort * mp, myInPorts) {
		mp->coupledPort = i;
		origCoupled->getData()->lsInPorts << mp;
		i++;
	}
	i = 0;
	foreach(modelPort * mp, myOutPorts) {
		mp->coupledPort = i;
		origCoupled->getData()->lsOutPorts << mp;
		i++;
	}

	GpxEditionScene *ges = origCoupled->getOpenEditionScene();
	if (ges != NULL) {
		foreach(QGraphicsItem * gi, ges->items()) {
			if (gi->type() == GpxBlock::Type) {
				GpxBlock *gb =
				    qgraphicsitem_cast < GpxBlock * >(gi);
				if (gb->blockType() == GpxBlock::InPort) {
					GpxInport *ip =
					    qgraphicsitem_cast <
					    GpxInport * >(gb);
					ip->setPortNumber(ip->getData()->
							  coupledPort);
				} else if (gb->blockType() == GpxBlock::OutPort) {
					GpxOutport *op =
					    qgraphicsitem_cast <
					    GpxOutport * >(gb);
					op->setPortNumber(op->getData()->
							  coupledPort);
				}
			}
		}
	}
}

void PowerDialogs::apply()
{
	switch (editTab->currentIndex()) {
	case 0:
		applyPropertiesChanges();
		break;
	case 1:
		applyParametersChanges();
		break;
	case 2:
		if (block->blockType() == GpxBlock::SimpleAtomic) {
			applyCodeChange();
		} else
			applyIOChange();
		break;
	}
	emit dirtyChanged();
}

void PowerDialogs::saveAtomic()
{
	applyPropertiesChanges();
	applyParametersChanges();
	if (block->blockType() == GpxBlock::SimpleAtomic) {
		applyCodeChange();
	} else {
		applyIOChange();
	}

	// Delete duplicates....
	if (block->blockType() == GpxBlock::SimpleAtomic) {
		delete myAtomic;
		origAtomic = NULL;
		delete origAtomic;
		delete dir;
		delete fi;
		delete dirPathLbl;
		delete atomicsLstWdt;
		delete fileDir;
		delete fileLstWdt;
		fileInfLst.clear();
		
	} else {
		origCoupled = NULL;
		delete origCoupled;
		delete myCoupled;
		delete inputLstWdt;
		delete outputLstWdt;
		while (myInPorts.count() > 0) {
			modelPort *p = myInPorts.takeAt(0);
			delete p;
		}
		while (myOutPorts.count() > 0) {
			modelPort *p = myOutPorts.takeAt(0);
			delete p;
		}
	}
	while (myParams.count() > 0) {
		modelParameter *p = myParams.takeAt(0);
		delete p;
	}
	delete nameLed;
	delete inputSbx;
	delete outputSbx;
	delete descTed;
	delete widthSbx;
	delete heightSbx;
	delete orientationCbx;
	delete colorCbx;
	delete iconLed;
	delete scene;
	delete view;
	delete paramUpBtn;
	delete paramDownBtn;
	delete paramNameLed;
	delete valueLed;
	delete valueCbx;
	delete paramDescTed;
	delete edtBtn;
	delete deleteBtn;
	delete editTab;
	delete dialog;
	emit dirtyChanged();
	return;
}

void PowerDialogs::cancelEditAtomic()
{
	if (block->blockType() == GpxBlock::SimpleAtomic) {
		delete myAtomic;
		origAtomic = NULL;
		delete origAtomic;
		delete dir;
		delete fi;
		delete dirPathLbl;
		delete atomicsLstWdt;
		delete fileDir;
		delete fileLstWdt;
		fileInfLst.clear();
		
	} else {
		origCoupled = NULL;
		delete origCoupled;
		delete myCoupled;
		delete inputLstWdt;
		delete outputLstWdt;
		while (myInPorts.count() > 0) {
			modelPort *p = myInPorts.takeAt(0);
			delete p;
		}
		while (myOutPorts.count() > 0) {
			modelPort *p = myOutPorts.takeAt(0);
			delete p;
		}
	}
	while (myParams.count() > 0) {
		modelParameter *p = myParams.takeAt(0);
		delete p;
	}
	delete nameLed;
	delete inputSbx;
	delete outputSbx;
	delete descTed;
	delete widthSbx;
	delete heightSbx;
	delete orientationCbx;
	delete colorCbx;
	delete iconLed;
	delete scene;
	delete view;
	delete paramUpBtn;
	delete paramDownBtn;
	delete paramNameLed;
	delete valueLed;
	delete valueCbx;
	delete paramDescTed;
	delete edtBtn;
	delete deleteBtn;
	delete editTab;
	delete dialog;
}

void PowerDialogs::updateAtomicDesc()
{
	if (block->blockType() == GpxBlock::SimpleAtomic)
		myAtomic->getData()->desc = descTed->toPlainText();
	else
		myCoupled->getData()->desc = descTed->toPlainText();
}

void PowerDialogs::disableEditAtomicParams()
{
	paramNameLed->setEnabled(false);
	typeCbx->setEnabled(false);
	valueLed->setEnabled(false);
	valueCbx->setEnabled(false);
	paramDescTed->setEnabled(false);
	edtBtn->setEnabled(false);
	deleteBtn->setEnabled(false);
	paramUpBtn->setEnabled(false);
	paramDownBtn->setEnabled(false);
}

void PowerDialogs::enableEditAtomicParams()
{
	paramNameLed->setEnabled(true);
	typeCbx->setEnabled(true);
	valueLed->setEnabled(true);
	valueCbx->setEnabled(true);
	paramDescTed->setEnabled(true);
	edtBtn->setEnabled(true);
	deleteBtn->setEnabled(true);
	paramUpBtn->setEnabled(true);
	paramDownBtn->setEnabled(true);
}

void PowerDialogs::makeNewFolder()
{
	folderName = QString();
	QListWidgetItem *item = new QListWidgetItem();
	QIcon folderIcon(imagesPath + "/folder.svg");
	item->
	    setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::
		     ItemIsEnabled);
	item->setIcon(folderIcon);
	atomicsLstWdt->addItem(item);
	atomicsLstWdt->setCurrentItem(item);
	atomicsLstWdt->editItem(item);
	connect(atomicsLstWdt->itemWidget(item), SIGNAL(returnPressed()), this,
		SLOT(saveNewFolder()));
	connect(atomicsLstWdt->itemWidget(item), SIGNAL(textEdited(QString)),
		this, SLOT(updateFolderName(QString)));
	return;
}

void PowerDialogs::saveNewFolder()
{
	QListWidgetItem *item = atomicsLstWdt->currentItem();
	if (!dir->mkdir(folderName)) {
		QMessageBox qMessage(QMessageBox::Critical,
				     QString("PowerDEVS"),
				     QString("Can not create directoy entry!"));
		qMessage.exec();
		atomicsLstWdt->removeItemWidget(item);
		return;
	}
	item->setText(folderName);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	return;
}

void PowerDialogs::updateFolderName(QString name)
{
	folderName = name;
	return;
}

void PowerDialogs::editAtomicDialog(GpxBlock * a)
{
	dialog = new QDialog(this);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->setFixedSize(DIALOGW, DIALOGH);
	dialog->setWindowTitle(tr("Edit: ") + a->getName());

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QGridLayout *commonPropLayout = new QGridLayout;
	QVBoxLayout *previewLayout = new QVBoxLayout;
	QGridLayout *graphicPropLayout = new QGridLayout;
	QGridLayout *propLayout = new QGridLayout;
	QVBoxLayout *paramLayout = new QVBoxLayout;
	QGridLayout *paramListLayout = new QGridLayout;
	QGridLayout *codeLayout = new QGridLayout;
	QGridLayout *locationLayout = new QGridLayout;
	QVBoxLayout *pathLayout = new QVBoxLayout;
	QGridLayout *ioLayout = new QGridLayout;
	QGridLayout *inputLayout = new QGridLayout;
	QGridLayout *outputLayout = new QGridLayout;

	// Duplicate atomic values
	block = a;
	int blockWidth = block->getBlockWidth();
	int blockHeight = block->getBlockHeight();

	if (block->blockType() == GpxBlock::SimpleAtomic) {
		origAtomic = qgraphicsitem_cast < GpxAtomic * >(a);
		myAtomic =
		    new GpxAtomic(NULL, NULL,
				  origAtomic->getData()->duplicate());
		myAtomic->setBackgroundColor(a->getBackgroundColor());
		myAtomic->setSize(50, 50);
		myParams = origAtomic->getData()->duplicate()->params;
	} else {
		origCoupled = qgraphicsitem_cast < GpxCoupled * >(a);
		myCoupled =
		    new GpxCoupled(NULL, NULL,
				   origCoupled->getData()->duplicate());
		myCoupled->setBackgroundColor(a->getBackgroundColor());
		myCoupled->setSize(50, 50);
		myParams = origCoupled->getData()->duplicate()->params;
	}

	editTab = new QTabWidget(dialog);
	QWidget *propTab = new QWidget();
	QWidget *paramTab = new QWidget();
	QWidget *codeTab = new QWidget();
	QWidget *ioTab = new QWidget();

	QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
	buttonBox->setEnabled(true);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->
	    setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::
			       Apply | QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);

	QGroupBox *commonPropGbx = new QGroupBox(propTab);
	commonPropGbx->setAutoFillBackground(true);
	commonPropGbx->setTitle(tr("Common Properties"));

	QLabel *nameLbl = new QLabel(tr("Name:"), commonPropGbx);
	QLabel *inputLbl = new QLabel(tr("Number of Inputs:"), commonPropGbx);
	QLabel *outputLbl = new QLabel(tr("Number of Outputs:"), commonPropGbx);
	QLabel *descLbl = new QLabel(tr("Description:"), commonPropGbx);

	nameLed = new QLineEdit(commonPropGbx);
	nameLed->setText(block->getName());
	inputSbx = new QSpinBox(commonPropGbx);
	inputSbx->setValue(block->numberOfInPorts());
	outputSbx = new QSpinBox(commonPropGbx);
	outputSbx->setValue(block->numberOfOutPorts());
	descTed = new QTextEdit(commonPropGbx);
	if (a->blockType() == GpxBlock::Coupled) {
		inputSbx->setEnabled(false);
		outputSbx->setEnabled(false);
		descTed->setText(myCoupled->desc());
	} else
		descTed->setText(myAtomic->desc());

	commonPropLayout->addWidget(nameLbl, 0, 0);
	commonPropLayout->addWidget(inputLbl, 0, 1);
	commonPropLayout->addWidget(outputLbl, 0, 2);
	commonPropLayout->addWidget(nameLed, 1, 0);
	commonPropLayout->addWidget(inputSbx, 1, 1);
	commonPropLayout->addWidget(outputSbx, 1, 2);
	commonPropLayout->addWidget(descLbl, 2, 0);
	commonPropLayout->addWidget(descTed, 3, 0, 1, 3);
	commonPropGbx->setLayout(commonPropLayout);

	QGroupBox *graphicPropGbx = new QGroupBox(propTab);
	graphicPropGbx->setAutoFillBackground(true);
	graphicPropGbx->setTitle(tr("Graphic Properties"));

	QLabel *widthLbl = new QLabel(tr("Width:"), graphicPropGbx);
	QLabel *heightLbl = new QLabel(tr("Height:"), graphicPropGbx);
	QLabel *orientationLbl = new QLabel(tr("Orientation:"), graphicPropGbx);
	QLabel *colorLbl = new QLabel(tr("Color:"), graphicPropGbx);
	QLabel *iconLbl = new QLabel(tr("Icon:"), graphicPropGbx);

	widthSbx = new QSpinBox(graphicPropGbx);
	widthSbx->setMinimum(20);
	widthSbx->setMaximum(500);
	widthSbx->setMinimumHeight(25);
	widthSbx->setValue(blockWidth);
	heightSbx = new QSpinBox(graphicPropGbx);
	heightSbx->setMinimum(20);
	heightSbx->setMaximum(500);
	heightSbx->setValue(blockHeight);
	heightSbx->setMinimumHeight(25);

	orientationCbx = new QComboBox(graphicPropGbx);
	orientationCbx->addItem(tr("RIGHT"));
	orientationCbx->addItem(tr("DOWN"));
	orientationCbx->addItem(tr("LEFT"));
	orientationCbx->addItem(tr("UP"));
	orientationCbx->setCurrentIndex(block->getDirection());
	orientationCbx->setMinimumWidth(110);
	orientationCbx->setMinimumHeight(25);
	colorCbx = new QComboBox(graphicPropGbx);
	colors = QList < QColor > ();
	colors << Qt::white << Qt::black << Qt::red << Qt::darkRed << Qt::
	    green << Qt::darkGreen << Qt::blue << Qt::darkBlue << Qt::
	    cyan << Qt::darkCyan << Qt::magenta << Qt::darkMagenta << Qt::
	    yellow << Qt::darkYellow << Qt::gray << Qt::darkGray << Qt::
	    lightGray;
	colorNames = QStringList();
	colorNames << tr("white") << tr("black") << tr("red") << tr("darkRed")
	    << tr("green") << tr("darkGreen")
	    << tr("blue") << tr("darkBlue") << tr("cyan") << tr("darkCyan") <<
	    tr("magenta")
	    << tr("darkMagenta") << tr("yellow") << tr("darkYellow") <<
	    tr("gray") << tr("darkGray")
	    << tr("lightGray");
	colorCbx->addItems(colorNames);
	colorCbx->setMaximumWidth(110);
	colorCbx->setMinimumHeight(25);
	colorCbx->setCurrentIndex(colors.indexOf(block->getBackgroundColor()));
	iconLed = new QLineEdit(graphicPropGbx);
	if (block->getIcon() == "")
		iconLed->setText(tr("None"));
	else
		iconLed->setText(block->getIcon());
	iconLed->setMinimumHeight(25);

	QToolButton *iconBtn = new QToolButton(graphicPropGbx);
	iconBtn->setText(tr("..."));
	iconBtn->setMinimumHeight(25);

	graphicPropLayout->addWidget(widthLbl, 0, 0);
	graphicPropLayout->addWidget(heightLbl, 0, 3);
	graphicPropLayout->addWidget(widthSbx, 1, 0, 1, 3);
	graphicPropLayout->addWidget(heightSbx, 1, 3, 1, 2);
	graphicPropLayout->addWidget(orientationLbl, 2, 0);
	graphicPropLayout->addWidget(colorLbl, 2, 3);
	graphicPropLayout->addWidget(orientationCbx, 3, 0, 1, 3);
	graphicPropLayout->addWidget(colorCbx, 3, 3, 1, 2);
	graphicPropLayout->addWidget(iconLbl, 4, 0);
	graphicPropLayout->addWidget(iconLed, 5, 0, 1, 4);
	graphicPropLayout->addWidget(iconBtn, 5, 4);

	graphicPropGbx->setLayout(graphicPropLayout);

	QGroupBox *previewGbx = new QGroupBox(propTab);
	previewGbx->setAutoFillBackground(true);
	previewGbx->setTitle(tr("Preview"));

	scene = new GpxEditionScene(previewGbx);
	scene->setSceneRect(QRectF(0, 0, 100, 100));
	if (block->blockType() == GpxBlock::SimpleAtomic) {
		scene->addItem(myAtomic);
		myAtomic->setPos(50, 50);
	} else {
		scene->addItem(myCoupled);
		myCoupled->setPos(50, 50);
	}
	view = new QGraphicsView(scene, previewGbx);
	view->setInteractive(false);
	view->setScene(scene);

	previewLayout->addWidget(view);
	previewGbx->setLayout(previewLayout);

	propLayout->addWidget(commonPropGbx, 0, 0, 1, 2);
	propLayout->addWidget(previewGbx, 1, 0);
	propLayout->addWidget(graphicPropGbx, 1, 1);
	propTab->setLayout(propLayout);

	// Properties tab connections.

	if (block->blockType() == GpxBlock::SimpleAtomic) {
		connect(nameLed, SIGNAL(textChanged(const QString)), myAtomic,
			SLOT(setName(QString)));
		connect(orientationCbx, SIGNAL(currentIndexChanged(int)),
			myAtomic, SLOT(setDirection(int)));
		connect(myAtomic, SIGNAL(heightChanged(int)), heightSbx,
			SLOT(setValue(int)));
	} else {
		connect(nameLed, SIGNAL(textChanged(const QString)), myCoupled,
			SLOT(setName(QString)));
		connect(orientationCbx, SIGNAL(currentIndexChanged(int)),
			myCoupled, SLOT(setDirection(int)));
		connect(myCoupled, SIGNAL(heightChanged(int)), heightSbx,
			SLOT(setValue(int)));
	}
	connect(inputSbx, SIGNAL(valueChanged(int)), this,
		SLOT(updateInPorts(int)));
	connect(outputSbx, SIGNAL(valueChanged(int)), this,
		SLOT(updateOutPorts(int)));
	connect(colorCbx, SIGNAL(currentIndexChanged(int)), this,
		SLOT(updateAtomicColor(int)));
	connect(descTed, SIGNAL(textChanged()), this, SLOT(updateAtomicDesc()));

	editTab->addTab(propTab, tr("Properties"));

	QGroupBox *paramGbx = new QGroupBox(paramTab);
	paramGbx->setAutoFillBackground(true);
	paramGbx->setTitle(tr("Parameters List"));

	QStringList labels =
	    QStringList() << "Name" << "Type" << "Value" << "Description";
	paramTblWdt = new QTableWidget(paramGbx);
	paramTblWdt->setColumnCount(4);
	paramTblWdt->setColumnWidth(0, 90);
	paramTblWdt->setColumnWidth(1, 60);
	paramTblWdt->setColumnWidth(2, 70);
	paramTblWdt->setHorizontalHeaderLabels(labels);
	QHeaderView *hv = paramTblWdt->horizontalHeader();
	hv->setResizeMode(QHeaderView::Fixed);
	hv->setStretchLastSection(true);
	paramTblWdt->setSelectionBehavior(QAbstractItemView::SelectRows);

	paramUpBtn = new QToolButton(paramGbx);
	paramUpBtn->setArrowType(Qt::UpArrow);
	paramDownBtn = new QToolButton(paramGbx);
	paramDownBtn->setArrowType(Qt::DownArrow);

	QSignalMapper *sgnMapper = new QSignalMapper(dialog);
	sgnMapper->setMapping(paramUpBtn, 1);
	sgnMapper->setMapping(paramDownBtn, 2);

	QLabel *paramNameLbl = new QLabel(tr("Name:"), paramGbx);
	QLabel *typeLbl = new QLabel(tr("Type:"), paramGbx);
	QLabel *valueLbl = new QLabel(tr("Value:"), paramGbx);
	QLabel *paramDescLbl = new QLabel(tr("Description:"), paramGbx);

	paramNameLed = new QLineEdit(paramGbx);
	typeCbx = new QComboBox(paramGbx);
	typeCbx->
	    addItems(QStringList() << "STRING" << "VALUE" << "LIST" <<
		     "SCILAB");
	valueLed = new QLineEdit(paramGbx);
	valueCbx = new QComboBox(paramGbx);
	paramDescTed = new QTextEdit(paramGbx);
	paramDescTed->setMaximumHeight(100);

	QSignalMapper *updateMapper = new QSignalMapper(dialog);
	updateMapper->setMapping(paramNameLed, 0);
	updateMapper->setMapping(typeCbx, 1);
	updateMapper->setMapping(valueLed, 2);
	updateMapper->setMapping(valueCbx, 3);
	updateMapper->setMapping(paramDescTed, 4);

	QDialogButtonBox *paramBbx = new QDialogButtonBox(paramGbx);
	paramBbx->setEnabled(true);
	paramBbx->setOrientation(Qt::Horizontal);
	paramBbx->setCenterButtons(true);
	edtBtn = new QPushButton(paramGbx);
	edtBtn->setText(tr("Edit"));
	QPushButton *newBtn = new QPushButton(paramGbx);
	newBtn->setText(tr("New"));
	deleteBtn = new QPushButton(paramGbx);
	deleteBtn->setText(tr("Delete"));
	paramBbx->addButton(edtBtn, QDialogButtonBox::ActionRole);
	paramBbx->addButton(newBtn, QDialogButtonBox::ActionRole);
	paramBbx->addButton(deleteBtn, QDialogButtonBox::ActionRole);

	int i = 0;
	// Disable edit params buttons, if there is no parameters in the Atomic.
	if (myParams.isEmpty()) {
		disableEditAtomicParams();
	} else {
		int numberOfInPorts, numberOfOutPorts;
		if (block->blockType() == GpxBlock::SimpleAtomic) {
			numberOfInPorts = myAtomic->numberOfInPorts();
			numberOfOutPorts = myAtomic->numberOfOutPorts();
		} else {
			numberOfInPorts = myCoupled->numberOfInPorts();
			numberOfOutPorts = myCoupled->numberOfOutPorts();
		}
		foreach(modelParameter * p, myParams) {
			QTableWidgetItem *item0 = new QTableWidgetItem(p->name);
			QFont font = item0->font();
			font.setPixelSize(9);
			item0->setFont(font);
			item0->
			    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem *item1 =
			    new QTableWidgetItem(p->typeToString());
			item1->
			    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			item1->setFont(font);
			QTableWidgetItem *item2;
			if (p->type == STR) {
				if (p->strValue == "%Inputs")
					item2 =
					    new QTableWidgetItem(QString().
								 setNum
								 (numberOfInPorts));
				else {
					if (p->strValue == "%Outputs")
						item2 =
						    new
						    QTableWidgetItem(QString().
								     setNum
								     (numberOfOutPorts));
					else
						item2 =
						    new QTableWidgetItem(p->
									 strValue);
				}
			}
			if (p->type == VAL)
				item2 =
				    new QTableWidgetItem(QString().
							 setNum(p->dValue));
			if (p->type == LST)
				item2 =
				    new QTableWidgetItem(p->lsValues.
							 value(p->lsValue-1));
			if (p->type == SCILAB)
				item2 =
				    new QTableWidgetItem(QString().
							 setNum(p->dValue));
			item2->setFont(font);
			item2->
			    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem *item3 = new QTableWidgetItem(p->desc);
			item3->
			    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			item3->setFont(font);
			paramTblWdt->insertRow(i);
			paramTblWdt->setItem(i, 0, item0);
			paramTblWdt->setItem(i, 1, item1);
			paramTblWdt->setItem(i, 2, item2);
			paramTblWdt->setItem(i, 3, item3);
			i++;
		}
	}

	if (myParams.count() > 0) {
		paramTblWdt->setCurrentCell(0, 0);
		paramNameLed->setText(paramTblWdt->item(0, 0)->text());
		typeCbx->setCurrentIndex(typeCbx->
					 findText(paramTblWdt->item(0, 1)->
						  text()));
		if (myParams.value(0)->type == LST) {
			valueCbx->addItems(myParams.value(0)->lsValues);
			valueCbx->setCurrentIndex(myParams.value(0)->lsValue-1);
			valueCbx->setVisible(true);
			valueLed->setVisible(false);
			edtBtn->setEnabled(true);
		} else {
			valueLed->setText(paramTblWdt->item(0, 2)->text());
			valueCbx->setVisible(false);
			valueLed->setVisible(true);
			edtBtn->setEnabled(false);
		}
		paramDescTed->setText(paramTblWdt->item(0, 3)->text());
	}

	paramListLayout->addWidget(paramTblWdt, 0, 0, 8, 5);
	paramListLayout->addWidget(paramUpBtn, 2, 5);
	paramListLayout->addWidget(paramDownBtn, 3, 5);

	paramListLayout->addWidget(paramNameLbl, 8, 0);
	paramListLayout->addWidget(typeLbl, 8, 1);
	paramListLayout->addWidget(valueLbl, 8, 3);
	paramListLayout->addWidget(paramNameLed, 9, 0);
	paramListLayout->addWidget(typeCbx, 9, 1, 1, 2);
	paramListLayout->addWidget(valueLed, 9, 3, 1, 3);
	paramListLayout->addWidget(valueCbx, 9, 3, 1, 3);
	paramListLayout->addWidget(paramDescLbl, 10, 0);
	paramListLayout->addWidget(paramDescTed, 11, 0, 1, 6);
	paramListLayout->addWidget(paramBbx, 12, 0);

	paramGbx->setLayout(paramListLayout);

	paramLayout->addWidget(paramGbx);

	paramTab->setLayout(paramLayout);

	editTab->addTab(paramTab, tr("Parameters"));

	if (block->blockType() == GpxBlock::SimpleAtomic) {
		QGroupBox *locGbx = new QGroupBox(codeTab);
		locGbx->setAutoFillBackground(true);
		locGbx->setTitle(tr("Location File"));

		QGroupBox *pathGbx = new QGroupBox(locGbx);
		pathGbx->setAutoFillBackground(true);
		pathGbx->setTitle(tr("Path:"));

		directoryChange = false;

		QString file = myAtomic->getData()->path.toLower();
		if (!file.isEmpty()) {
			file.replace(QString("\\"), QString("/"));
			file.prepend("/");
		}
		QSettings settings(QCoreApplication::applicationDirPath() + "/powerdevs.ini", QSettings::IniFormat);
		atomicsPath = settings.value("Path/atomicsPath", "Value not found in file powerdevs.ini").toString();
		imagesPath = settings.value("Path/imagesPath", "Value not found in file powerdevs.ini").toString();

		dir = new QDir(atomicsPath, QString(), QDir::Name | QDir::IgnoreCase, QDir::AllDirs | QDir::NoDotAndDotDot); // Atomic's folder list.
		fileInfLst = dir->entryInfoList();
		fi = new QFileInfo(dir->absolutePath() + file);
		dirPathLbl = new QLabel(pathGbx);
		if(fi->exists())
			dirPathLbl->setText(dir-> relativeFilePath(fi->absoluteFilePath()));
		pathLayout->addWidget(dirPathLbl);
		pathGbx->setLayout(pathLayout);
		QSplitter *sp = new QSplitter(locGbx);

		atomicsLstWdt = new QListWidget(sp);
		atomicsLstWdt->
		    setSelectionBehavior(QAbstractItemView::SelectItems);
		atomicsLstWdt->
		    setSelectionMode(QAbstractItemView::SingleSelection);
		foreach(QFileInfo f, fileInfLst) {
			QIcon folderIcon(imagesPath + "/folder.svg");
			QListWidgetItem *item =
			    new QListWidgetItem(folderIcon, f.baseName());
			atomicsLstWdt->addItem(item);
		}
		// Select current folder, if there is no file, selct the first one.
		if (file.isEmpty()) {
			atomicsLstWdt->setCurrentRow(0);
		} else {
			if(!fi->exists()){
				atomicsLstWdt->setCurrentRow(0);
			}else{
				bool found = false;
				for (int i = 0; i < atomicsLstWdt->count(); i++) {
					QListWidgetItem *tmp = atomicsLstWdt->item(i);
					if (tmp->text() == fi->dir().dirName()){
						atomicsLstWdt->setCurrentRow(i);
						found = true;
						break;
					}
				}
				if(!found){
					atomicsLstWdt->setCurrentRow(0);
				}
			}
		}

		fileDir =
		    new QDir(atomicsPath + "/" +
			     atomicsLstWdt->currentItem()->text(),
			     QString("*.h"), QDir::Name | QDir::IgnoreCase,
			     QDir::Files);
		fileLstWdt = new QListWidget(sp);
		// Folder file list.
		folderFileInfLst = fileDir->entryInfoList();
		foreach(QFileInfo f, folderFileInfLst) {
			fileLstWdt->addItem(f.fileName());
		}
		for (int i = 0; i < fileLstWdt->count(); i++) {
			QListWidgetItem *tmp = fileLstWdt->item(i);
			if (tmp->text() == fi->fileName())
				fileLstWdt->setCurrentItem(tmp);
		}
		fileLstWdt->
		    setSelectionBehavior(QAbstractItemView::SelectItems);
		fileLstWdt->
		    setSelectionMode(QAbstractItemView::SingleSelection);

		QPushButton *newDirBtn = new QPushButton(locGbx);
		QIcon dirIcon(imagesPath + "/newFolder.svg");
		newDirBtn->setIcon(dirIcon);
		newDirBtn->setToolTip(tr("New folder"));
		newDirBtn->setSizePolicy(QSizePolicy());
		QPushButton *newFileBtn = new QPushButton(locGbx);
		QIcon newIcon(imagesPath + "/newFile.svg");
		newFileBtn->setIcon(newIcon);
		newFileBtn->setText(tr("New"));
		newFileBtn->setSizePolicy(QSizePolicy());
		QPushButton *editFileBtn = new QPushButton(locGbx);
		QIcon editIcon(imagesPath + "/editFile.svg");
		editFileBtn->setIcon(editIcon);
		editFileBtn->setText(tr("Edit"));
		editFileBtn->setSizePolicy(QSizePolicy());

		locationLayout->addWidget(pathGbx, 0, 0, 1, 4);
		locationLayout->addWidget(sp, 1, 0, 1, 4);
		locationLayout->addWidget(newDirBtn, 4, 0);
		locationLayout->addWidget(editFileBtn, 4, 2);
		locationLayout->addWidget(newFileBtn, 4, 3);

		locGbx->setLayout(locationLayout);

		codeLayout->addWidget(locGbx);
		codeTab->setLayout(codeLayout);

		editTab->addTab(codeTab, tr("Code"));
		connect(atomicsLstWdt,
			SIGNAL(currentItemChanged
			       (QListWidgetItem *, QListWidgetItem *)), this,
			SLOT(updateDirs(QListWidgetItem *)));
		connect(fileLstWdt,
			SIGNAL(currentItemChanged
			       (QListWidgetItem *, QListWidgetItem *)), this,
			SLOT(updateFiles(QListWidgetItem *)));
		connect(newDirBtn, SIGNAL(clicked()), this,
			SLOT(makeNewFolder()));
		connect(editFileBtn, SIGNAL(clicked()), this, SLOT(editCode()));
		connect(newFileBtn, SIGNAL(clicked()), this,
			SLOT(newCodeFile()));
	} else if (a->blockType() == GpxBlock::Coupled) {
		QGroupBox *inputGbx = new QGroupBox(ioTab);
		inputGbx->setAutoFillBackground(true);
		inputGbx->setTitle(tr("Inputs:"));

		QGroupBox *outputGbx = new QGroupBox(ioTab);
		outputGbx->setAutoFillBackground(true);
		outputGbx->setTitle(tr("Outputs:"));

		inputLstWdt = new QListWidget(inputGbx);

		QToolButton *inUpBtn = new QToolButton(inputGbx);
		inUpBtn->setArrowType(Qt::UpArrow);
		QToolButton *inDownBtn = new QToolButton(inputGbx);
		inDownBtn->setArrowType(Qt::DownArrow);

		inputLayout->addWidget(inputLstWdt, 0, 0, 5, 4);
		inputLayout->addWidget(inUpBtn, 5, 2);
		inputLayout->addWidget(inDownBtn, 5, 3);
		inputGbx->setLayout(inputLayout);

		outputLstWdt = new QListWidget(outputGbx);

		QToolButton *outUpBtn = new QToolButton(outputGbx);
		outUpBtn->setArrowType(Qt::UpArrow);
		QToolButton *outDownBtn = new QToolButton(outputGbx);
		outDownBtn->setArrowType(Qt::DownArrow);

		myInPorts = QList < modelPort * >();
		myOutPorts = QList < modelPort * >();

		//myInPorts = origCoupled->getData()->lsInPorts();
		//myOutPorts = origCoupled->getData()->lsOutPorts();

		foreach(modelPort * mp, myInPorts) 
				inputLstWdt->addItem(mp->name);
		foreach(modelPort * mp, myOutPorts) 
				outputLstWdt->addItem(mp->name);
		if (inputLstWdt->count())
			inputLstWdt->setCurrentRow(0);
		if (outputLstWdt->count())
			outputLstWdt->setCurrentRow(0);

		QSignalMapper *sgnMapper = new QSignalMapper(dialog);
		sgnMapper->setMapping(inUpBtn, 1);
		sgnMapper->setMapping(inDownBtn, 2);
		sgnMapper->setMapping(outUpBtn, 3);
		sgnMapper->setMapping(outDownBtn, 4);

		outputLayout->addWidget(outputLstWdt, 0, 0, 5, 4);
		outputLayout->addWidget(outUpBtn, 5, 2);
		outputLayout->addWidget(outDownBtn, 5, 3);
		outputGbx->setLayout(outputLayout);

		ioLayout->addWidget(inputGbx, 0, 0);
		ioLayout->addWidget(outputGbx, 0, 1);
		ioTab->setLayout(ioLayout);

		connect(inUpBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
		connect(inDownBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
		connect(outUpBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
		connect(outDownBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
		connect(sgnMapper, SIGNAL(mapped(int)), this,
			SLOT(swapPorts(int)));
		editTab->addTab(ioTab, tr("Inputs/Outputs"));
	}

	mainLayout->addWidget(editTab);
	mainLayout->addWidget(buttonBox);
	dialog->setLayout(mainLayout);

	connect(buttonBox, SIGNAL(accepted()), dialog,
		SLOT(accept()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveAtomic()));
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelEditAtomic()));
	connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
		this, SLOT(apply()));
	connect(paramTblWdt, SIGNAL(currentCellChanged(int, int, int, int)),
		this, SLOT(editAtomicValues(int)));
	connect(paramTblWdt, SIGNAL(cellClicked(int, int)), this,
		SLOT(updatePos(int, int)));
	connect(updateMapper, SIGNAL(mapped(int)), this,
		SLOT(updateAtomicValues(int)));
	connect(paramNameLed, SIGNAL(textEdited(const QString)), updateMapper,
		SLOT(map()));
	connect(typeCbx, SIGNAL(currentIndexChanged(int)), updateMapper,
		SLOT(map()));
	connect(valueLed, SIGNAL(textChanged(const QString)), updateMapper,
		SLOT(map()));
	connect(valueCbx, SIGNAL(activated(int)), updateMapper, SLOT(map()));
	connect(paramDescTed, SIGNAL(textChanged()), updateMapper, SLOT(map()));
	connect(sgnMapper, SIGNAL(mapped(int)), this, SLOT(swapParams(int)));
	connect(paramUpBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
	connect(paramDownBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
	connect(newBtn, SIGNAL(clicked()), this, SLOT(newParam()));
	connect(deleteBtn, SIGNAL(clicked()), this, SLOT(delParam()));
	connect(edtBtn, SIGNAL(clicked()), this, SLOT(editListParam()));
	connect(iconBtn, SIGNAL(clicked()), this, SLOT(selectAtomicIcon()));
	dialog->show();
	return;
}

void PowerDialogs::saveModelParameters()
{
	QList < modelParameter * >mp = QList < modelParameter * >();
	// First, remove the original Atomic params.
	mp = myModelCoupled->params;

	while (mp.count() > 0) {
		modelParameter *p = mp.takeAt(0);
		delete p;
	}
	mp.clear();
	myModelCoupled->params.clear();
	// Replace parameters list.
	foreach(modelParameter * p, myParams)
	    if (p->name != "") {
		myModelCoupled->params << p;
	}
	myParams.clear();
	delete deleteBtn;
	delete edtBtn;
	delete paramDescTed;
	delete valueCbx;
	delete valueLed;
	delete typeCbx;
	delete paramNameLed;
	delete paramDownBtn;
	delete paramUpBtn;
	delete paramTblWdt;
	delete dialog;
	emit dirtyChanged();
}

void PowerDialogs::cancelModelParameters()
{
	myParams.clear();
	delete deleteBtn;
	delete edtBtn;
	delete paramDescTed;
	delete valueCbx;
	delete valueLed;
	delete typeCbx;
	delete paramNameLed;
	delete paramDownBtn;
	delete paramUpBtn;
	delete paramTblWdt;
	delete dialog;
}

void PowerDialogs::editModelParameters(modelCoupled * mc)
{
	dialog = new QDialog(this);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->setFixedSize(DIALOGW, DIALOGH);
	dialog->setWindowTitle(tr("Edit: ") + mc->name);

	myParams = QList < modelParameter * >();
	myModelCoupled = mc;
	foreach(modelParameter * p, mc->params)
	    qDebug() << p->name;
	foreach(modelParameter * p, mc->params)
	    myParams << p->duplicate();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	QVBoxLayout *paramLayout = new QVBoxLayout;
	QGridLayout *paramListLayout = new QGridLayout;

	QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
	buttonBox->setEnabled(true);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->
	    setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);

	QGroupBox *paramGbx = new QGroupBox(dialog);
	paramGbx->setAutoFillBackground(true);
	paramGbx->setTitle(tr("Parameters List"));

	QStringList labels =
	    QStringList() << "Name" << "Type" << "Value" << "Description";
	paramTblWdt = new QTableWidget(paramGbx);
	paramTblWdt->setColumnCount(4);
	paramTblWdt->setColumnWidth(0, 90);
	paramTblWdt->setColumnWidth(1, 60);
	paramTblWdt->setColumnWidth(2, 70);
	paramTblWdt->setHorizontalHeaderLabels(labels);
	QHeaderView *hv = paramTblWdt->horizontalHeader();
	hv->setResizeMode(QHeaderView::Fixed);
	hv->setStretchLastSection(true);
	paramTblWdt->setSelectionBehavior(QAbstractItemView::SelectRows);

	paramUpBtn = new QToolButton(paramGbx);
	paramUpBtn->setArrowType(Qt::UpArrow);
	paramDownBtn = new QToolButton(paramGbx);
	paramDownBtn->setArrowType(Qt::DownArrow);

	QSignalMapper *sgnMapper = new QSignalMapper(dialog);
	sgnMapper->setMapping(paramUpBtn, 1);
	sgnMapper->setMapping(paramDownBtn, 2);

	QLabel *paramNameLbl = new QLabel(tr("Name:"), paramGbx);
	QLabel *typeLbl = new QLabel(tr("Type:"), paramGbx);
	QLabel *valueLbl = new QLabel(tr("Value:"), paramGbx);
	QLabel *paramDescLbl = new QLabel(tr("Description:"), paramGbx);

	paramNameLed = new QLineEdit(paramGbx);
	typeCbx = new QComboBox(paramGbx);
	typeCbx->
	    addItems(QStringList() << "STRING" << "VALUE" << "LIST" <<
		     "SCILAB");
	valueLed = new QLineEdit(paramGbx);
	valueCbx = new QComboBox(paramGbx);
	paramDescTed = new QTextEdit(paramGbx);
	paramDescTed->setMaximumHeight(100);

	QSignalMapper *updateMapper = new QSignalMapper(dialog);
	updateMapper->setMapping(paramNameLed, 0);
	updateMapper->setMapping(typeCbx, 1);
	updateMapper->setMapping(valueLed, 2);
	updateMapper->setMapping(valueCbx, 3);
	updateMapper->setMapping(paramDescTed, 4);

	QDialogButtonBox *paramBbx = new QDialogButtonBox(paramGbx);
	paramBbx->setEnabled(true);
	paramBbx->setOrientation(Qt::Horizontal);
	paramBbx->setCenterButtons(true);
	edtBtn = new QPushButton(paramGbx);
	edtBtn->setText(tr("Edit"));
	QPushButton *newBtn = new QPushButton(paramGbx);
	newBtn->setText(tr("New"));
	deleteBtn = new QPushButton(paramGbx);
	deleteBtn->setText(tr("Delete"));
	paramBbx->addButton(edtBtn, QDialogButtonBox::ActionRole);
	paramBbx->addButton(newBtn, QDialogButtonBox::ActionRole);
	paramBbx->addButton(deleteBtn, QDialogButtonBox::ActionRole);

	int i = 0;
	// Disable edit params buttons, if there is no parameters in the Atomic.
	if (myParams.isEmpty()) {
		disableEditAtomicParams();
	} else {
		foreach(modelParameter * p, myParams) {
			QTableWidgetItem *item0 = new QTableWidgetItem(p->name);
			QFont font = item0->font();
			font.setPixelSize(9);
			item0->setFont(font);
			item0->
			    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem *item1 =
			    new QTableWidgetItem(p->typeToString());
			item1->
			    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			item1->setFont(font);
			QTableWidgetItem *item2;
			if (p->type == STR)
				item2 = new QTableWidgetItem(p->strValue);
			if (p->type == VAL)
				item2 =
				    new QTableWidgetItem(QString().
							 setNum(p->dValue));
			if (p->type == LST)
				item2 =
				    new QTableWidgetItem(p->lsValues.
							 value(p->lsValue));
			if (p->type == SCILAB)
				item2 =
				    new QTableWidgetItem(QString().
							 setNum(p->dValue));
			item2->setFont(font);
			item2->
			    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem *item3 = new QTableWidgetItem(p->desc);
			item3->
			    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			item3->setFont(font);
			paramTblWdt->insertRow(i);
			paramTblWdt->setItem(i, 0, item0);
			paramTblWdt->setItem(i, 1, item1);
			paramTblWdt->setItem(i, 2, item2);
			paramTblWdt->setItem(i, 3, item3);
			i++;
		}
	}

	if (myParams.count() > 0) {
		paramTblWdt->setCurrentCell(0, 0);
		paramNameLed->setText(paramTblWdt->item(0, 0)->text());
		typeCbx->setCurrentIndex(typeCbx->
					 findText(paramTblWdt->item(0, 1)->
						  text()));
		if (myParams.value(0)->type == LST) {
			valueCbx->addItems(myParams.value(0)->lsValues);
			valueCbx->setCurrentIndex(myParams.value(0)->lsValue);
			valueCbx->setVisible(true);
			valueLed->setVisible(false);
			edtBtn->setEnabled(true);
		} else {
			valueLed->setText(paramTblWdt->item(0, 2)->text());
			valueCbx->setVisible(false);
			valueLed->setVisible(true);
			edtBtn->setEnabled(false);
		}
		paramDescTed->setText(paramTblWdt->item(0, 3)->text());
	}

	paramListLayout->addWidget(paramTblWdt, 0, 0, 8, 5);
	paramListLayout->addWidget(paramUpBtn, 2, 5);
	paramListLayout->addWidget(paramDownBtn, 3, 5);

	paramListLayout->addWidget(paramNameLbl, 8, 0);
	paramListLayout->addWidget(typeLbl, 8, 1);
	paramListLayout->addWidget(valueLbl, 8, 3);
	paramListLayout->addWidget(paramNameLed, 9, 0);
	paramListLayout->addWidget(typeCbx, 9, 1, 1, 2);
	paramListLayout->addWidget(valueLed, 9, 3, 1, 3);
	paramListLayout->addWidget(valueCbx, 9, 3, 1, 3);
	paramListLayout->addWidget(paramDescLbl, 10, 0);
	paramListLayout->addWidget(paramDescTed, 11, 0, 1, 6);
	paramListLayout->addWidget(paramBbx, 12, 0);

	paramGbx->setLayout(paramListLayout);

	paramLayout->addWidget(paramGbx);

	mainLayout->addWidget(paramGbx);
	mainLayout->addWidget(buttonBox);

	dialog->setLayout(mainLayout);

	connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(accepted()), this,
		SLOT(saveModelParameters()));
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelModelParameters()));
	connect(paramTblWdt, SIGNAL(currentCellChanged(int, int, int, int)),
		this, SLOT(editAtomicValues(int)));
	connect(paramTblWdt, SIGNAL(cellClicked(int, int)), this,
		SLOT(updatePos(int, int)));
	connect(updateMapper, SIGNAL(mapped(int)), this,
		SLOT(updateAtomicValues(int)));
	connect(paramNameLed, SIGNAL(textEdited(const QString)), updateMapper,
		SLOT(map()));
	connect(typeCbx, SIGNAL(currentIndexChanged(int)), updateMapper,
		SLOT(map()));
	connect(valueLed, SIGNAL(textChanged(const QString)), updateMapper,
		SLOT(map()));
	connect(valueCbx, SIGNAL(activated(int)), updateMapper, SLOT(map()));
	connect(paramDescTed, SIGNAL(textChanged()), updateMapper, SLOT(map()));
	connect(sgnMapper, SIGNAL(mapped(int)), this, SLOT(swapParams(int)));
	connect(paramUpBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
	connect(paramDownBtn, SIGNAL(clicked()), sgnMapper, SLOT(map()));
	connect(newBtn, SIGNAL(clicked()), this, SLOT(newParam()));
	connect(deleteBtn, SIGNAL(clicked()), this, SLOT(delParam()));
	connect(edtBtn, SIGNAL(clicked()), this, SLOT(editListParam()));

	dialog->show();
}
