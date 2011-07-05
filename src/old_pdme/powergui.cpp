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

#include <QtGui>
#include <Qt>

#include "powergui.h"
#include "gpxblock.h"
#include "gpxcoupled.h"
#include "gpxatomic.h"
#include "gpxinport.h"
#include "gpxoutport.h"
#include "gpxconnectionpoint.h"
#include "gpxeditionscene.h"
#include "gpxeditionwindow.h"
#include "gpxtextbox.h"
#include "libraryscene.h"
#include "librarywindow.h"
#include "parser.h"
#include "powerdialogs.h"

const int InsertTextButton = 10;
int PowerGui::modelCounter = 0;
PowerGui *mainWindow;
PowerDialogs *pDialogs;

 PowerGui::PowerGui():QMainWindow()
{
	mdiArea = new QMdiArea;
	connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)),
		this, SLOT(updateMenus()));
	windowMapper = new QSignalMapper(this);
	connect(windowMapper, SIGNAL(mapped(QWidget *)),
		this, SLOT(setActiveSubWindow(QWidget *)));

	fromLibAtomic = NULL;
	fromLibCoupled = NULL;
	fromDragCoupled = NULL;
	fromClipboardCoupled = NULL;
	createActions();
	createToolBars();
	createMenus();
	createStatusBar();
	updateMenus();
	createToolBox();

	readSettings();
	setCentralWidget(mdiArea);

	setWindowIcon(QIcon(":/images/power.png"));
	setWindowTitle(tr("PowerDEVS"));

/*    qDebug("Atomic type %d",GpxBlock::Type);
	qDebug("Port type %d",GpxConnectionPoint::Type);
	qDebug("Edge type %d",GpxEdge::Type);
	qDebug("Textbox type %d",GpxTextBox::Type);*/
	pDialogs = new PowerDialogs(this);
	connect(pDialogs, SIGNAL(dirtyChanged()), this, SLOT(dirtyChanged()));
	undoStack = new QUndoGroup(this);
}

PowerGui::~PowerGui()
{
	for (int i = 0; i < toolBox->count(); i++) {
		QGraphicsView *v = (QGraphicsView *) toolBox->widget(i);
		LibraryScene *s = (LibraryScene *) v->scene();
		delete v;
		delete s;
	}
	delete undoStack;
	delete rotateAction;
	delete deleteAction;
	delete newAction;
	delete openAction;
	delete saveAction;
	delete saveAsAction;
	delete exportAction;
	delete printAction;
	delete exitAction;
	delete cutAction;
	delete copyAction;
	delete pasteAction;
	delete editAtomicAction;
	delete selectAllAction;
	delete openEditAction;
	delete priorityAction;
	delete modelAction;
	delete closeAction;
	delete parametersAction;
	delete modelParametersAction;
	delete closeAllAction;
	delete tileAction;
	delete cascadeAction;
	delete nextAction;
	delete previousAction;
	delete separatorAction;
	delete aboutAction;
	delete aboutQtAction;
	delete librarySetupAction;
	delete menuSetupAction;
	delete pathSetupAction;
	delete locationLabel;
	delete formulaLabel ;
	delete toolBox;
}

void PowerGui::closeEvent(QCloseEvent * event)
{
	foreach(QMdiSubWindow * mdisw, mdiArea->subWindowList()) {
		mdiArea->setActiveSubWindow(mdisw);
		if (!mdisw->close()) {
			event->ignore();
			return;
		}
	}
	if (fromClipboardCoupled != NULL)
		delete fromClipboardCoupled;
	event->accept();
}

void PowerGui::newFile()
{
	modelCoupled *coupledData = new modelCoupled();
	coupledData->owner = NULL;
	coupledData->father = NULL;
	coupledData->type = TOKROOT;
	//coupledData->inPorts = 0;
	//coupledData->outPorts = 0;
	coupledData->desc = "";
	coupledData->name = QString("Model %1").arg(++modelCounter);
	modelGraphics *mg = new modelGraphics();
	coupledData->graphics = mg;
	mg->x = mg->y = 0;
	mg->width = mg->height = 500;
	mg->direction = 0;
	mg->icon = "";
	mg->color = 15;
	GpxEditionWindow *mw = new GpxEditionWindow(coupledData);
	//connect(mw,SIGNAL(saveModel(GpxEditionWindow*,modelCoupled*)),this,SLOT(saveModel(GpxEditionWindow*,modelCoupled*)));
	connect(mw, SIGNAL(currentChanged(int)), this, SLOT(updateMenus()));
	connect(mw, SIGNAL(closeEditionWindow(GpxEditionWindow *)), this,
		SLOT(closeEditionWindow(GpxEditionWindow *)));

	QMdiSubWindow *subWindow1 = new QMdiSubWindow;
    subWindow1->setWidget(mw);
	mdiArea->addSubWindow(subWindow1);
	mw->setWindowState(Qt::WindowMaximized);
	mw->dirtyChanged(true);
	mw->show();
	QUndoStack *us=mw->getUndoStack();
	qDebug("Changing to undo stack at %p",us);
	undoStack->addStack(us);
}

void PowerGui::closeEditionWindow(GpxEditionWindow * w)
{
	QMdiSubWindow *sw=qobject_cast<QMdiSubWindow*>(w->parent());
	undoStack->removeStack(w->getUndoStack());
	modelCoupled *c = w->getData();
	mdiArea->removeSubWindow(sw);
	qDebug("Releasing edition window and modelCoupled");
	
	delete w;
	/*! \todo: The QMdiSubWindow is not released! */
	//delete sw;
	delete c;
}

void PowerGui::open()
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString examplePath =
	    settings.value("Path/examplePath", QString()).toString();
	QString fileName =
	    QFileDialog::getOpenFileName(this, "Open model", examplePath,
					 "PowerDEVS Model (*.pdm);;PowerDEVS Library (*.pdl)");

	if (!fileName.isEmpty()) {
		QMdiSubWindow *existing = findModelChild(fileName);
		if (existing) {
			mdiArea->setActiveSubWindow(existing);
			existing->widget()->setWindowState(Qt::WindowMaximized);
			return;
		}

		modelCoupled *c = parseModel(fileName);
		if (c != NULL) {	// Add to recent files
			QStringList recentFiles =
			    settings.value("recentFiles",
					   QStringList()).toStringList();
			QDir dir(QCoreApplication::applicationDirPath());
			/* Tendria que preguntar por el absoluto, si existe, cambiarlo, y guardar el relativo*/
			if (!recentFiles.
			    contains(dir.relativeFilePath(fileName))) {
				recentFiles.prepend(dir.
						    relativeFilePath(fileName));
			} else {
				recentFiles.move(recentFiles.
						 indexOf(dir.
							 relativeFilePath
							 (fileName)), 0);
			}
			settings.setValue("recentFiles", recentFiles);
			updateRecentFiles();
			GpxEditionWindow *mw = new GpxEditionWindow(c);
			connect(mw, SIGNAL(currentChanged(int)), this,
				SLOT(updateMenus()));
			connect(mw,
				SIGNAL(closeEditionWindow(GpxEditionWindow *)),
				this,
				SLOT(closeEditionWindow(GpxEditionWindow *)));
			mw->fileName = fileName;
			mw->clearDirty();
			QMdiSubWindow *subWindow1 = new QMdiSubWindow;
		    subWindow1->setWidget(mw);
			mdiArea->addSubWindow(subWindow1);
			mw->setWindowState(Qt::WindowMaximized);
			//connect(mw,SIGNAL(saveModel(GpxEditionWindow*,modelCoupled*)),this,SLOT(saveModel(GpxEditionWindow*,modelCoupled*)));


			QUndoStack *us=mw->getUndoStack();
			qDebug("Changing to undo stack at %p",us);
			undoStack->addStack(us);
		} else {
			QMessageBox qMessage(QMessageBox::Critical,
					     QString("PowerDEVS"),
					     QString
					     ("The file could not be open"));
			qMessage.exec();
		}

	}
}

void PowerGui::openRecentFile()
{
	QAction *qA = (QAction *) QObject::sender();
	QString fileName = qA->data().toString();
	if (!fileName.isEmpty()) {
		QMdiSubWindow *existing = findModelChild(fileName);
		if (existing) {
			mdiArea->setActiveSubWindow(existing);
			existing->widget()->setWindowState(Qt::WindowMaximized);
			return;
		}
		modelCoupled *c = parseModel(fileName);
		if (c == NULL) {
			QMessageBox qMessage(QMessageBox::Critical,
					     QString("PowerDEVS"),
					     QString
					     ("The file could not be open"));
			qMessage.exec();
		} else {
			GpxEditionWindow *mw = new GpxEditionWindow(c);
			//connect(mw,SIGNAL(saveModel(GpxEditionWindow*,modelCoupled*)),this,SLOT(saveModel(GpxEditionWindow*,modelCoupled*)));
			connect(mw, SIGNAL(currentChanged(int)), this,
				SLOT(updateMenus()));
			connect(mw,
				SIGNAL(closeEditionWindow(GpxEditionWindow *)),
				this,
				SLOT(closeEditionWindow(GpxEditionWindow *)));

			QMdiSubWindow *subWindow1 = new QMdiSubWindow;
		    subWindow1->setWidget(mw);
			mdiArea->addSubWindow(subWindow1);
			mw->fileName = fileName;
			mw->clearDirty();
			mw->setWindowState(Qt::WindowMaximized);
			mw->show();
			QUndoStack *us=mw->getUndoStack();
			qDebug("Changing to undo stack at %p",us);
			undoStack->addStack(us);
			QSettings settings(QCoreApplication::
					   applicationDirPath() +
					   "/powerdevs.ini",
					   QSettings::IniFormat);
			QStringList recentFiles =
			    settings.value("recentFiles",
					   QStringList()).toStringList();
			QDir dir(QCoreApplication::applicationDirPath());
			if (recentFiles.
			    contains(dir.relativeFilePath(fileName))) {
				recentFiles.move(recentFiles.
						 indexOf(dir.
							 relativeFilePath
							 (fileName)), 0);
				settings.setValue("recentFiles", recentFiles);
			}
		}
	}
}

int PowerGui::save()
{

	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	if (activeModelChild()) {
		GpxEditionWindow *mw = activeModelWindow();

		if (mw->fileName.isEmpty()) {
			return saveAs();
		} else {
			saveModelToFile(mw->getData(), mw->fileName);
			mw->dirtyChanged(false);
			QStringList recentFiles =
			    settings.value("recentFiles",
					   QStringList()).toStringList();
			QDir dir(QCoreApplication::applicationDirPath());
			if (recentFiles.
			    contains(dir.relativeFilePath(mw->fileName))) {
				recentFiles.move(recentFiles.
						 indexOf(dir.
							 relativeFilePath(mw->
									  fileName)),
						 0);
			} else {
				recentFiles.prepend(mw->fileName);
			}
			settings.setValue("recentFiles", recentFiles);
			updateRecentFiles();

			statusBar()->showMessage(tr("File saved"), 2000);
			return 0;
		}
	}
	return -1;
}

int PowerGui::saveAs()
{
	if (activeModelChild()) {
		GpxEditionWindow *mw = activeModelWindow();
		QSettings settings(QCoreApplication::applicationDirPath() +
				   "/powerdevs.ini", QSettings::IniFormat);
		QString examplePath =
		    settings.value("Path/examplePath", QString()).toString();
		QString fileName =
		    QFileDialog::getSaveFileName(this, "Save model",
						 examplePath,
						 "PowerDEVS Model (*.pdm)");
		if (!fileName.isEmpty()) {
			if (!fileName.endsWith(".pdm"))
				fileName = fileName.append(".pdm");
			QFileInfo qf(fileName);
			mw->setName(qf.baseName());
			mw->fileName = fileName;
			saveModelToFile(mw->getData(), mw->fileName);
			mw->dirtyChanged(false);
			statusBar()->showMessage(tr("File saved"), 2000);
			return 0;
		}
	}
	return -1;
}

void PowerGui::cut()
{
	if (activeModelWindow()) {
		GpxEditionWindow *gew = activeModelWindow();
		if (fromClipboardCoupled != NULL)
			delete fromClipboardCoupled;
		fromClipboardCoupled = gew->getSelectedData();
		deleteItem();
	}
	updateMenus();
}

void PowerGui::copy()
{
	if (activeModelWindow()) {
		GpxEditionWindow *gew = activeModelWindow();
		if (fromClipboardCoupled != NULL)
			delete fromClipboardCoupled;
		fromClipboardCoupled = gew->getSelectedData();
	}
	updateMenus();
}

void PowerGui::paste()
{
	if (activeModelChild()) {
		GpxEditionScene *ges = activeModelChild();
		ges->clearSelection();
		ges->paste(fromClipboardCoupled);
	}
}

void PowerGui::about()
{
	QMessageBox::about(this, tr("About PowerDEVS"),
			   tr("<h2>PowerDEVS 2.0</h2>"
			      "<p>Integrated Tool for Edition and Simulation "
			      "of Discrete Event Systems.</p>"
			      "<p>Developed by:</p> "
			      "<p> Federico Bergero (bergero@cifasis-conicet.gov.ar) </p>"
			      "<p> Enrique Hansen  (enrique.hansen@gmail.com) </p>"
			      "<p> Joaquin Fernandez (joaquin.f.fernandez@gmail.com) </p>"
			      "<p>Directed by:</p> "
			      "<p> Ernesto Kofman (kofman@fceia.unr.edu.ar) </p>"));
}

void PowerGui::updateStatusBar()
{
}

void PowerGui::deleteItem()
{
	if (activeModelChild() != 0) {
		QGraphicsItem *itemEdge;
		GpxEditionScene *scene = activeModelChild();
		GpxEditionWindow *editWindow = activeModelWindow();
		if (scene->selectedItems().size() == 1 &&
		    (itemEdge =
		     scene->selectedItems().at(0))->type() == GpxEdge::Type) {

			editWindow->setDirty();
			GpxEdge *e = qgraphicsitem_cast < GpxEdge * >(itemEdge);
			e->removeConnections();
			scene->removeItem(itemEdge);
			delete itemEdge;
			return;
		}
		foreach(QGraphicsItem * item, scene->selectedItems()) {
			if (item->type() == GpxEdge::Type
			    && item->scene() == scene) {
				editWindow->setDirty();
				GpxEdge *e =
				    qgraphicsitem_cast < GpxEdge * >(item);
				e->removeConnections();
				scene->removeItem(item);
				delete item;
			}
		}
		foreach(QGraphicsItem * item, scene->selectedItems()) {
			editWindow->setDirty();
			switch (item->type()) {
			case GpxBlock::Type:{
					qgraphicsitem_cast < GpxBlock * >(item)->removePorts();
					editWindow-> removeBlock((GpxBlock *) item);
					delete item;
				}
				break;
			case GpxEdge::Type:
				break;
			case GpxConnectionPoint::Type:
			case GpxTextBox::Type:
				if (item->parentItem() == NULL) {
					if (item->scene() == scene)
						scene->removeItem(item);
					delete item;
				}
			}
		}
		scene->setMode(GpxEditionScene::MoveItem);
	}
}

void PowerGui::rotateItem()
{
	if (activeModelChild() != 0) {
		GpxEditionScene *scene = activeModelChild();

		if (scene->selectedItems().isEmpty())
			return;

		QGraphicsItem *selectedItem = scene->selectedItems().first();
		if (selectedItem->type() == GpxBlock::Type) {
			GpxBlock *b =
			    qgraphicsitem_cast < GpxBlock * >(selectedItem);
			if (b->blockType() == GpxBlock::SimpleAtomic)
				((GpxAtomic *) b)->rotate();
			if (b->blockType() == GpxBlock::Coupled)
				((GpxCoupled *) b)->rotate();
			if (b->blockType() == GpxBlock::InPort)
				((GpxInport *) b)->rotate();
			if (b->blockType() == GpxBlock::OutPort)
				((GpxOutport *) b)->rotate();
		}
	}
}

void PowerGui::itemInserted(GpxBlock * item)
{
	if (activeModelChild() != 0) {
		GpxEditionScene *scene = activeModelChild();
		scene->setMode(GpxEditionScene::MoveItem);
		if (item->blockType() == GpxBlock::InPort) {
			if (scene->getOwner() != NULL) {
				scene->getOwner()->
				    addPort(GpxConnectionPoint::InPort);
			}
		}
		if (item->blockType() == GpxBlock::OutPort) {
			if (scene->getOwner() != NULL) {
				scene->getOwner()->
				    addPort(GpxConnectionPoint::OutPort);
			}
		}
		if (item->blockType() == GpxBlock::SimpleAtomic) {
			connect(item, SIGNAL(editParameters(GpxBlock *)),
				pDialogs, SLOT(blockParamDialog(GpxBlock *)));
			connect(item, SIGNAL(contextMenu(GpxBlock *, QPoint)),
				this, SLOT(contextMenu(GpxBlock *, QPoint)));
			connect(item, SIGNAL(dragCopy(QPointF)), this,
				SLOT(dragCopy(QPointF)));
		}
		if (item->blockType() == GpxBlock::Coupled) {
			connect(item, SIGNAL(editParameters(GpxBlock *)),
				pDialogs, SLOT(blockParamDialog(GpxBlock *)));
			connect(item, SIGNAL(contextMenu(GpxBlock *, QPoint)),
				this, SLOT(contextMenu(GpxBlock *, QPoint)));
			connect(item, SIGNAL(dragCopy(QPointF)), this,
				SLOT(dragCopy(QPointF)));
		}
	}
}

void PowerGui::edgeInserted(GpxEdge * edge)
{
	if (activeModelChild() != 0 && edge) {
		GpxEditionScene *scene = activeModelChild();
		scene->setMode(GpxEditionScene::MoveItem);
	}
}

void PowerGui::updateMenus()
{
	bool hasModelChild = (activeModelChild() != 0);

	saveAction->setEnabled(hasModelChild);
	saveAsAction->setEnabled(hasModelChild);
	pasteAction->setEnabled(fromClipboardCoupled != NULL);
	closeAction->setEnabled(hasModelChild);
	closeAllAction->setEnabled(hasModelChild);
	tileAction->setEnabled(hasModelChild);
	cascadeAction->setEnabled(hasModelChild);
	nextAction->setEnabled(hasModelChild);
	previousAction->setEnabled(hasModelChild);
	separatorAction->setEnabled(hasModelChild);
	printAction->setEnabled(hasModelChild);
	exportAction->setEnabled(hasModelChild);
	selectAllAction->setEnabled(hasModelChild);
	priorityAction->setEnabled(hasModelChild);
	modelAction->setEnabled(hasModelChild);

	bool hasSelection = false;
	if (activeModelChild() != 0
	    && activeModelChild()->selectedItems().size())
		hasSelection = true;
	cutAction->setEnabled(hasSelection);
	copyAction->setEnabled(hasSelection);
	bool hasAtomicSelection = false, hasChildSelection =
	    false, hasCoupledSelection = false;
	bool hasSelectedItems = false;
	if (activeModelChild()
	    && activeModelChild()->selectedItems().size() == 1) {
		GpxEditionScene *scene = activeModelChild();
		QGraphicsItem *item = scene->selectedItems().first();
		if (item->type() == GpxBlock::Type) {
			GpxBlock *a = qgraphicsitem_cast < GpxBlock * >(item);
			hasAtomicSelection = true;
			if (a->blockType() == GpxBlock::SimpleAtomic
			    || a->blockType() == GpxBlock::Coupled)
				hasChildSelection = true;
			if (a->blockType() == GpxBlock::Coupled)
				hasCoupledSelection = true;
		}
	}
	if (activeModelChild() && activeModelChild()->selectedItems().size()) {
		GpxEditionScene *scene = activeModelChild();
		foreach(QGraphicsItem * item, scene->selectedItems()) {
			if (item->type() == GpxBlock::Type || item->type() == GpxEdge::Type || 
				item->type() == GpxConnection::Type || item->type() == GpxConnectionPoint::Type) {
				hasSelectedItems = true;
				break;
			}
		}
	}
	deleteAction->setEnabled(hasSelectedItems);
	rotateAction->setEnabled(hasAtomicSelection);

	parametersAction->setEnabled(hasAtomicSelection);
	modelParametersAction->setEnabled(hasModelChild);
	editAtomicAction->setEnabled(hasChildSelection);
	openEditAction->setEnabled(hasCoupledSelection);

	foreach(QAction * qAct, userActions) {
		QStringList itemData = qAct->data().toStringList();
		if (itemData.at(5).toInt()) {
			qAct->setEnabled(hasModelChild);
		}
	}
	if (activeModelWindow()!=NULL) {
		GpxEditionWindow *ew= activeModelWindow();
		qDebug("Changing undo stack at %p",ew->getUndoStack());
		undoStack->setActiveStack(ew->getUndoStack());
	}
}

void PowerGui::updateWindowMenu()
{
	windowMenu->clear();
	windowMenu->addAction(closeAction);
	windowMenu->addAction(closeAllAction);
	windowMenu->addSeparator();
	windowMenu->addAction(tileAction);
	windowMenu->addAction(cascadeAction);
	windowMenu->addSeparator();
	windowMenu->addAction(previousAction);
	windowMenu->addAction(separatorAction);

	QList < QMdiSubWindow * >windows = mdiArea->subWindowList();
	separatorAction->setVisible(!windows.isEmpty());

	for (int i = 0; i < windows.size(); i++) {
		GpxEditionWindow *mw =
		    qobject_cast <
		    GpxEditionWindow * >(windows.at(i)->widget());
		QAction *action = windowMenu->addAction(mw->windowTitle());
		action->setCheckable(true);
		action->setChecked(mw == activeModelWindow());
		connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
		windowMapper->setMapping(action, windows.at(i));
	}
}

void PowerGui::updateUserMenus()
{
	menuBar()->clear();
	userTool->clear();
	defineMenus();
	updateMenus();
	return;
}

void PowerGui::defineMenus()
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);

	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAction);
	fileMenu->addAction(openAction);
	fileMenu->addAction(exportAction);
	fileMenu->addAction(saveAction);
	fileMenu->addAction(saveAsAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exportAction);
	fileMenu->addSeparator();
	fileMenu->addAction(printAction);
	fileMenu->addSeparator();

	fileMenu->addAction(exitAction);

	updateRecentFiles();

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(cutAction);
	editMenu->addAction(copyAction);
	editMenu->addAction(pasteAction);
	editMenu->addAction(selectAllAction);
	editMenu->addSeparator();
	editMenu->addAction(editAtomicAction);
	editMenu->addAction(openEditAction);
	editMenu->addAction(priorityAction);
	editMenu->addSeparator();
	editMenu->addAction(modelAction);

	itemMenu = menuBar()->addMenu(tr("&Item"));
	itemMenu->addAction(deleteAction);
	itemMenu->addSeparator();
	itemMenu->addAction(rotateAction);

	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addSeparator();
	viewMenu->addAction(parametersAction);
	viewMenu->addAction(modelParametersAction);

	optionMenu = menuBar()->addMenu(tr("&Options"));
	optionMenu->addAction(librarySetupAction);
	optionMenu->addSeparator();
	optionMenu->addAction(menuSetupAction);
	optionMenu->addAction(pathSetupAction);

	QStringList userMenus =
	    settings.value("userMenus", QStringList()).toStringList();
	foreach(QString m, userMenus) {
		QMenu *userMenu = menuBar()->addMenu(m);
		QStringList menu =
		    settings.value("menu/" + m, QStringList()).toStringList();
		foreach(QString item, menu) {
			QAction *qAct = new QAction(item, this);
			userActions.append(qAct);
			userMenu->addAction(qAct);
			QStringList itemData =
			    settings.value("menu/" + m + "/" + item,
					   QStringList()).toStringList();
			qAct->setData(itemData);
			qAct->setText(item);
			qAct->setShortcut(itemData.at(2));
			if (itemData.at(3) != ""
			    && QFile::exists(itemData.at(3))) {
				qAct->setIcon(QIcon(itemData.at(3)));
				userTool->addAction(qAct);
			}
			connect(qAct, SIGNAL(triggered()), this,
				SLOT(runAction()));

		}
	}

	windowMenu = menuBar()->addMenu(tr("&Window"));
	updateWindowMenu();
	connect(windowMenu, SIGNAL(aboutToShow()), this,
		SLOT(updateWindowMenu()));

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAction);
	helpMenu->addAction(aboutQtAction);
}

void PowerGui::createMenus()
{
	userTool = addToolBar("UserToolBar");
	defineMenus();
}

void PowerGui::runAction()
{
	GpxEditionScene *activeModel = activeModelChild();
	GpxEditionWindow *activeWindow = activeModelWindow();
	QAction *qAct = (QAction *) QObject::sender();
	QStringList l = qAct->data().toStringList();
	if (l.at(4) == QString("1") && activeModel != NULL) {
		save();
		QString arg = l.at(1);
		arg.replace(QString("%F"), activeWindow->fileName);
		qDebug("Runing action %s %s %s", QSTR(l.first()), QSTR(arg),
		       QSTR(activeWindow->fileName));
		QProcess q;
		q.startDetached(l.first(), arg.split(" "));
	} else {
		QProcess q;
		QString arg = l.at(1);
		q.startDetached(l.first(), arg.split(" "));
	}
}

void PowerGui::createToolBars()
{
	fileToolBar = addToolBar(tr("&File"));
	fileToolBar->addAction(newAction);
	fileToolBar->addAction(openAction);
	fileToolBar->addAction(saveAction);

	editToolBar = addToolBar(tr("&Edit"));
	editToolBar->addAction(cutAction);
	editToolBar->addAction(copyAction);
	editToolBar->addAction(pasteAction);
	editToolBar->addAction(selectAllAction);
	editToolBar->addSeparator();
	editToolBar->addAction(openEditAction);
	editToolBar->addAction(priorityAction);

	fillColorToolButton = new QToolButton;
	fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	fillColorToolButton->
	    setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
	fillAction = fillColorToolButton->menu()->defaultAction();
	fillColorToolButton->
	    setIcon(createColorToolButtonIcon
		    (":/images/floodfill.png", Qt::white));
	connect(fillColorToolButton, SIGNAL(clicked()), this,
		SLOT(fillButtonTriggered()));

	/*! TODO: Change icon 
	   colorToolBar = addToolBar(tr("Color"));
	   colorToolBar->addWidget(fillColorToolButton);
	 */

}

void PowerGui::readSettings()
{
	/* El problema es con el path, en el caso de Ernesto no lo esta encontrando */
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(400, 400)).toSize();
	QString scilabPath = settings.value("scilabPath", QString()).toString();
	bool startScilab = settings.value("startScilab", false).toBool();
	if (startScilab) {
		QProcess q;
		QString outputPath =
		    settings.value("Path/outputPath", QString()).toString();
		q.startDetached(scilabPath, QStringList(), outputPath);
	}
	move(pos);
	resize(size);
}

void PowerGui::writeSettings()
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

void PowerGui::createStatusBar()
{
	locationLabel = new QLabel("");
	locationLabel->setAlignment(Qt::AlignHCenter);
	locationLabel->setMinimumSize(locationLabel->sizeHint());

	formulaLabel = new QLabel;
	formulaLabel->setIndent(3);

	statusBar()->addWidget(locationLabel);
	statusBar()->addWidget(formulaLabel, 1);

}

GpxEditionScene *PowerGui::activeModelChild()
{
	GpxEditionWindow *w;
	if ((w = activeModelWindow())) {
		return ((GpxEditionScene *) ((QGraphicsView *) w->
					     widget(w->currentIndex()))->
			scene());
	}
	return NULL;
}

GpxEditionWindow *PowerGui::activeModelWindow()
{
	if (QMdiSubWindow * activeSubWindow = mdiArea->currentSubWindow()) {
		return qobject_cast <
		    GpxEditionWindow * >(activeSubWindow->widget());
	}
	return NULL;
}

QMdiSubWindow *PowerGui::findModelChild(const QString & fileName)
{
	foreach(QMdiSubWindow * window, mdiArea->subWindowList()) {
		GpxEditionWindow *ew = qobject_cast < GpxEditionWindow * >(window->widget());
		if (ew!=NULL && ew->fileName == fileName)
			return window;
	}
	return NULL;
}

void PowerGui::setActiveSubWindow(QWidget * window)
{
	if (!window)
		return;
	mdiArea->setActiveSubWindow(qobject_cast < QMdiSubWindow * >(window));
}

void PowerGui::updateToolBox()
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString pathLib = settings.value("Path/libPath").toString();

	QStringList libs =
	    settings.value("Libraries", QStringList()).toStringList();
	QStringList libsEnabled =
	    settings.value("libraryEnabled", QStringList()).toStringList();
	int items = toolBox->count();
	for (int k = 0; k < items; k++) {
		QWidget *cwidget = toolBox->currentWidget();
		toolBox->removeItem(k);
		delete cwidget;
	}
	foreach(QString fileLib, libs) {
		modelCoupled *c = parseModel(fileLib);
		if (c == NULL)
			continue;
		LibraryScene *scene = new LibraryScene(this, c);
		QGraphicsView *view = new QGraphicsView(scene);
		view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
		if (libsEnabled.value(libs.indexOf(fileLib)) == "false")
			view->setEnabled(false);
		toolBox->addItem(view, QIcon(c->graphics->icon), c->name);
	}
	QGraphicsScene *s;
	QGraphicsView *v;
	qreal libWidth = dock->width() - 20;
	for (int i = 0; i < toolBox->count(); i++) {
		v = (QGraphicsView *) toolBox->widget(i);
		s = v->scene(); 
		qreal x = 0, y = 0, maxHeight = 0;
		foreach(QGraphicsItem * a, s->items()) {
			GpxBlock *atomic = qgraphicsitem_cast < GpxBlock * >(a);
			if (atomic == NULL)
				continue;
			qreal w = atomic->getWidth();
			if (x + w + 10 > libWidth) {
				x = 0;
				y += maxHeight;
				maxHeight = 0;
			}
			atomic->setPos(x + w / 2, y + 20);
			x += w;
			maxHeight =
			    (atomic->getHeight() >
			     maxHeight ? atomic->getHeight() : maxHeight);
		}
		s->setSceneRect(-10, -10, x + 10, y + 10);
	}
}

void PowerGui::createToolBox()
{
	toolBox = new QToolBox;
	dock = new LibraryWindow(tr("Library"), this);
	toolBox->setMinimumWidth(250);
	updateToolBox();
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setWidget(toolBox);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void PowerGui::itemSelected(QGraphicsItem * item)
{
	GpxTextBox *textItem = qgraphicsitem_cast < GpxTextBox * >(item);
	QGraphicsView *view = (QGraphicsView *) activeModelWindow()->widget(0);

	foreach(QGraphicsItem * item, view->scene()->items()) {
		if (item->type() == GpxTextBox::Type) {
			GpxTextBox *text =
			    qgraphicsitem_cast < GpxTextBox * >(item);
			if (textItem->toPlainText() == text->toPlainText()) {
				//              number= atomic->myNumber() + 1;
				//              qDebug("2. The number is: %d and the atomic number: %d.\n", number, atomic->myNumber());
			}
		}
	}

}

void PowerGui::itemColorChanged()
{
	fillAction = qobject_cast < QAction * >(sender());
	fillColorToolButton->
	    setIcon(createColorToolButtonIcon
		    (":/images/floodfill.png",
		     qVariantValue < QColor > (fillAction->data())));
	fillButtonTriggered();
}

void PowerGui::fillButtonTriggered()
{
	GpxEditionScene *model = activeModelChild();
	model->setItemColor(qVariantValue < QColor > (fillAction->data()));
}

/*QColor PowerGui::intToColor(int c)
{
	QList < QColor > colors;
	colors << Qt::black << Qt::darkBlue << Qt::darkGreen << Qt::
	    darkCyan << Qt::darkRed << Qt::darkMagenta << Qt::darkYellow << Qt::
	    gray << Qt::darkGray << Qt::blue << Qt::green << Qt::cyan << Qt::
	    red << Qt::lightGray << Qt::white;
	return colors.at(c);
}

int PowerGui::colorToInt(QColor c)
{
	QList < QColor > colors;
	colors << Qt::black << Qt::darkBlue << Qt::darkGreen << Qt::
	    darkCyan << Qt::darkRed << Qt::darkMagenta << Qt::darkYellow << Qt::
	    gray << Qt::darkGray << Qt::blue << Qt::green << Qt::cyan << Qt::
	    red << Qt::lightGray << Qt::white;
	return colors.indexOf(c);
}*/

QMenu *PowerGui::createColorMenu(const char *slot, QColor defaultColor)
{
	QList < QColor > colors;
	colors << Qt::white << Qt::black << Qt::red << Qt::darkRed << Qt::
	    green << Qt::darkGreen << Qt::blue << Qt::darkBlue << Qt::
	    cyan << Qt::darkCyan << Qt::magenta << Qt::darkMagenta << Qt::
	    yellow << Qt::darkYellow << Qt::gray << Qt::darkGray << Qt::
	    lightGray;
	QStringList names;
	names << tr("white") << tr("black") << tr("red") << tr("darkRed") <<
	    tr("green") << tr("darkGreen")
	    << tr("blue") << tr("darkBlue") << tr("cyan") << tr("darkCyan") <<
	    tr("magenta")
	    << tr("darkMagenta") << tr("yellow") << tr("darkYellow") <<
	    tr("gray") << tr("darkGray")
	    << tr("lightGray");

	QMenu *colorMenu = new QMenu;
	for (int i = 0; i < colors.count(); ++i) {
		QAction *action = new QAction(names.at(i), this);
		action->setData(colors.at(i));
		action->setIcon(createColorIcon(colors.at(i)));
		connect(action, SIGNAL(triggered()), this, slot);
		colorMenu->addAction(action);
		if (colors.at(i) == defaultColor) {
			colorMenu->setDefaultAction(action);
		}
	}
	return colorMenu;
}

QIcon PowerGui::createColorToolButtonIcon(const QString & imageFile,
					  QColor color)
{
	QPixmap pixmap(50, 80);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	QPixmap image(imageFile);
	QRect target(0, 0, 50, 60);
	QRect source(0, 0, 42, 42);
	painter.fillRect(QRect(0, 60, 50, 80), color);
	painter.drawPixmap(target, image, source);

	return QIcon(pixmap);
}

QIcon PowerGui::createColorIcon(QColor color)
{
	QPixmap pixmap(20, 20);
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	painter.fillRect(QRect(0, 0, 20, 20), color);

	return QIcon(pixmap);
}

void PowerGui::librarySetup()
{
	pDialogs->librarySetupDialog();
	connect(pDialogs, SIGNAL(writeToolBoxSettings()),
		SLOT(updateToolBox()));
	return;
}

void PowerGui::menuSetup()
{
	pDialogs->menuSetupDialog();
	connect(pDialogs, SIGNAL(updateMenus()), this, SLOT(updateUserMenus()));
	return;
}

void PowerGui::pathSetup()
{
	pDialogs->pathSetupDialog();
	return;
}

void PowerGui::prioritySetup()
{
	if (!activeModelChild())
		return;
	GpxEditionScene *scene = activeModelChild();
	pDialogs->priorityDialog(scene);
	return;
}

void PowerGui::editCoupled()
{
	if (!activeModelChild())
		return;
	GpxEditionScene *scene = activeModelChild();
	if (scene->selectedItems().size() != 1)
		return;
	QGraphicsItem *item = scene->selectedItems().first();
	if (item->type() == GpxBlock::Type) {
		GpxBlock *a = qgraphicsitem_cast < GpxBlock * >(item);
		if (a->blockType() == GpxBlock::Coupled) {
			//doEditCoupled(qgraphicsitem_cast<GpxCoupled*>(a));
			GpxCoupled *c = qgraphicsitem_cast < GpxCoupled * >(a);
			GpxEditionWindow *mw = activeModelWindow();
			for (int i = 0; i < mw->count(); i++) {
				QGraphicsView *gv =
				    (QGraphicsView *) mw->widget(i);
				GpxEditionScene *ges =
				    (GpxEditionScene *) gv->scene();
				if (c->getData() == ges->coupledData) {
					mw->setCurrentIndex(i);
					return;
				}
			}
			if (c->getOpenEditionScene() == NULL) {
				(mw->addTab(c->getData(), c));
			}
			mw->setCurrentIndex(c->openedIn());
		QUndoStack *us=mw->getUndoStack();
		qDebug("Changing to undo stack at %p",us);
		undoStack->addStack(us);
			
		}

	}
}

void PowerGui::editAtomic()
{
	if (!activeModelChild())
		return;
	GpxEditionScene *scene = activeModelChild();
	if (scene->selectedItems().size() != 1)
		return;
	QGraphicsItem *item = scene->selectedItems().first();
	if (item->type() == GpxBlock::Type) {
		GpxBlock *a = qgraphicsitem_cast < GpxBlock * >(item);
		if (a->blockType() == GpxBlock::SimpleAtomic
		    || a->blockType() == GpxBlock::Coupled) {
			pDialogs->editAtomicDialog(a);
			connect(pDialogs, SIGNAL(editAtomicCode(QString)), this,
				SLOT(editAtomicCode(QString)));
		}
	}
}

void PowerGui::editParams()
{
	if (!activeModelChild())
		return;
	GpxEditionScene *scene = activeModelChild();
	if (scene->selectedItems().size() != 1)
		return;
	QGraphicsItem *item = scene->selectedItems().first();
	if (item->type() == GpxBlock::Type) {
		GpxBlock *a = qgraphicsitem_cast < GpxBlock * >(item);
		if (a->blockType() == GpxBlock::SimpleAtomic
		    || a->blockType() == GpxBlock::Coupled)
			pDialogs->blockParamDialog(a);
	}
}

void PowerGui::editModelParams()
{
	if (!activeModelChild())
		return;
	GpxEditionScene *scene = activeModelChild();
	pDialogs->modelParamDialog(scene->getData());
}

void PowerGui::editModelParameters()
{
	if (!activeModelChild())
		return;
	GpxEditionScene *scene = activeModelChild();
	pDialogs->editModelParameters(scene->getData());
}

/*
void PowerGui::doEditCoupled(GpxCoupled *c)
{
	qDebug("Open coupled");
	GpxEditionWindow *mw=activeModelWindow();
	for (int i=0; i<mw->count(); i++) {
		QGraphicsView *gv=(QGraphicsView*)mw->widget(i);
		GpxEditionScene *ges=(GpxEditionScene*)gv->scene();
		if (c->getData()==ges->coupledData) {
			mw->setCurrentIndex(i);
			return;
		}
	}
	if (c->openedIn()<0) {
		c->setOpen(mw->addTab(c->getData()));
		//mc->setOwner(c);
	}
	mw->setCurrentIndex(c->openedIn());
}	
*/

void PowerGui::selectAll()
{
	if (!activeModelChild())
		return;
	GpxEditionScene *scene = activeModelChild();
	foreach(QGraphicsItem * i, scene->items())
	    i->setSelected(i->type() != GpxTextBox::Type);
}

void PowerGui::setStatusBar(QString * s)
{
	locationLabel->setText(*s);
}

void PowerGui::contextMenu(GpxBlock * b, QPoint pos)
{
	QMenu *qMenu = new QMenu(NULL);
	qMenu->addAction(cutAction);
	qMenu->addAction(copyAction);
	qMenu->addAction(pasteAction);
	qMenu->addAction(deleteAction);
	qMenu->addSeparator();

	qMenu->addAction(parametersAction);
	qMenu->addAction(editAtomicAction);
		
	editAtomicAction->setEnabled(true);
	parametersAction->setEnabled(true);
	QAction *qActEditCode = NULL, *qActOpen = NULL;
	// TODO: Clear selection
	b->setSelected(true);
	GpxAtomic *a;
	if (b->blockType() == GpxBlock::SimpleAtomic) {
		qActEditCode = qMenu->addAction("Edit Code");
		a = qgraphicsitem_cast < GpxAtomic * >(b);
		if (a->getPath().isEmpty())
			qActEditCode->setEnabled(false);
	}
	if (b->blockType() == GpxBlock::Coupled) {
		qMenu->addAction(openEditAction);
	}
	QAction *selected = qMenu->exec(pos);
	if (selected == NULL)
		return;
	if (selected == qActEditCode)
		editAtomicCode(a->getPath());
	if (selected == qActOpen)
		//editCoupled((GpxCoupled*)b);
		editCoupled();
	delete qMenu;
}

void PowerGui::createActions()
{

	rotateAction = new QAction(QIcon(":/images/rotate.png"),
				   tr("Rotate"), this);
	rotateAction->setShortcut(tr("Ctrl+R"));
	rotateAction->setStatusTip(tr("Rotate item"));
	connect(rotateAction, SIGNAL(triggered()), this, SLOT(rotateItem()));

	deleteAction =
	    new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
	//deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
	//deleteAction->setShortcut(tr("Ctrl+Del"));
	deleteAction->setShortcut(tr("Del"));
	deleteAction->setStatusTip(tr("Delete item from diagram"));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

	newAction = new QAction(tr("&New"), this);
	newAction->setIcon(QIcon(":/images/new.png"));
	newAction->setShortcut(QKeySequence::New);
	newAction->setStatusTip(tr("Create a new Power DEVS model"));
	connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

	openAction = new QAction(tr("&Open"), this);
	openAction->setIcon(QIcon(":/images/open.png"));
	openAction->setShortcut(QKeySequence::Open);
	openAction->setStatusTip(tr("Open a existing Power DEVS model"));
	connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

	saveAction = new QAction(tr("&Save"), this);
	saveAction->setIcon(QIcon(":/images/save.png"));
	saveAction->setStatusTip(tr("Save the Power DEVS model to disk"));
	saveAction->setShortcut(QKeySequence::Save);
	connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAction = new QAction(tr("Save &As ..."), this);
	saveAsAction->setStatusTip(tr("Save the Power DEVS model under a new "
				      "name"));
	saveAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

	exportAction = new QAction(tr("&Export as library"), this);
	exportAction->setStatusTip(tr("..."));
	//connect(exportAction, SIGNAL(triggered()), this, SLOT(newFile()));

	printAction = new QAction(tr("&Print"), this);
	printAction->setShortcut(tr("Ctrl+P"));
	printAction->setStatusTip(tr("Print a Power DEVS model?"));
	//connect(printAction, SIGNAL(triggered()), this, SLOT(newFile()));

	exitAction = new QAction(tr("E&xit"), this);
	exitAction->setShortcut(tr("Ctrl+Q"));
	exitAction->setStatusTip(tr("Exit Power DEVS"));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	cutAction = new QAction(tr("Cu&t"), this);
	cutAction->setShortcut(QKeySequence::Cut);
	cutAction->setIcon(QIcon(":/images/cut.png"));
	cutAction->setStatusTip(tr("Copy the current selection's contents"
				   "to the clipboard"));
	connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

	copyAction = new QAction(tr("&Copy"), this);
	copyAction->setIcon(QIcon(":/images/copy.png"));
	copyAction->setShortcut(QKeySequence::Copy);
	copyAction->setStatusTip(tr("Copy the current selection's contents "
				    "to the clipboard"));
	connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

	pasteAction = new QAction(tr("&Paste"), this);
	pasteAction->setIcon(QIcon(":/images/paste.png"));
	pasteAction->setShortcut(QKeySequence::Paste);
	pasteAction->setStatusTip(tr("Paste the clipboard's contents "
				     "into the current selection"));
	connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

	editAtomicAction = new QAction(tr("&Edit..."), this);
	editAtomicAction->setShortcut(tr("Ctrl+E"));
	editAtomicAction->setStatusTip(tr("Edit an atomic"));
	connect(editAtomicAction, SIGNAL(triggered()), this,
		SLOT(editAtomic()));

	selectAllAction = new QAction(tr("&Select All"), this);
	selectAllAction->setShortcut(tr("Ctrl+A"));
	selectAllAction->setStatusTip(tr("Select all items"));
	selectAllAction->setIcon(QIcon(":/images/select.png"));
	connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

	openEditAction = new QAction(tr("&Open"), this);
	openEditAction->setShortcut(tr("Ctrl+U"));
	openEditAction->setStatusTip(tr("Open a coupled"));
	openEditAction->setIcon(QIcon(":/images/coupled.png"));
	connect(openEditAction, SIGNAL(triggered()), this, SLOT(editCoupled()));

	priorityAction = new QAction(tr("Priorit&y"), this);
	priorityAction->setStatusTip(tr("Set item's priority"));
	priorityAction->setIcon(QIcon(":/images/priority.png"));
	connect(priorityAction, SIGNAL(triggered()), this,
		SLOT(prioritySetup()));

	modelAction = new QAction(tr("&Model"), this);
	modelAction->setStatusTip(tr("Set model's properties"));
	connect(modelAction, SIGNAL(triggered()), this,
		SLOT(editModelParameters()));

	closeAction = new QAction(tr("Cl&ose"), this);
	closeAction->setShortcut(tr("Ctrl+F4"));
	closeAction->setStatusTip(tr("Close the active window"));
	connect(closeAction, SIGNAL(triggered()), mdiArea,
		SLOT(closeActiveSubWindow()));

	parametersAction = new QAction(tr("&Parameters"), this);
	parametersAction->setShortcut(tr("Ctrl+T"));
	connect(parametersAction, SIGNAL(triggered()), this,
		SLOT(editParams()));

	modelParametersAction = new QAction(tr("&Model Parameters"), this);
	modelParametersAction->setShortcut(tr("Ctrl+M"));
	connect(modelParametersAction, SIGNAL(triggered()), this,
		SLOT(editModelParams()));

	closeAllAction = new QAction(tr("Close &All"), this);
	closeAllAction->setStatusTip(tr("Close all the  windows"));
	connect(closeAllAction, SIGNAL(triggered()), mdiArea,
		SLOT(closeAllSubWindows()));

	tileAction = new QAction(tr("&Tile"), this);
	tileAction->setStatusTip(tr("Tile the windows"));
	connect(tileAction, SIGNAL(triggered()), mdiArea,
		SLOT(tileSubWindows()));

	cascadeAction = new QAction(tr("&Cascade"), this);
	cascadeAction->setStatusTip(tr("Cascade the windows"));
	connect(cascadeAction, SIGNAL(triggered()),
		mdiArea, SLOT(cascadeSubWindows()));

	nextAction = new QAction(tr("Ne&xt"), this);
	nextAction->setStatusTip(tr("Move the focus to the next window"));
	connect(nextAction, SIGNAL(triggered()),
		mdiArea, SLOT(activateNextSubWindow()));

	previousAction = new QAction(tr("Pre&vious"), this);
	previousAction->
	    setStatusTip(tr("Move the focus to the previous window"));
	connect(previousAction, SIGNAL(triggered()), mdiArea,
		SLOT(activatePreviousSubWindow()));

	separatorAction = new QAction(this);
	separatorAction->setSeparator(true);

	aboutAction = new QAction(tr("&About"), this);
	aboutAction->setStatusTip(tr("Show the application's About box"));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

	aboutQtAction = new QAction(tr("About &Qt"), this);
	aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	librarySetupAction = new QAction(tr("Add/Remove Libraries..."), this);
	connect(librarySetupAction, SIGNAL(triggered()), this,
		SLOT(librarySetup()));

	menuSetupAction = new QAction(tr("Menu setup"), this);
	connect(menuSetupAction, SIGNAL(triggered()), this, SLOT(menuSetup()));

	pathSetupAction = new QAction(tr("Set path and tools"), this);
	pathSetupAction->
	    setStatusTip(tr("Set configuration files for PowerDEvs"));
	connect(pathSetupAction, SIGNAL(triggered()), this, SLOT(pathSetup()));

}

void PowerGui::editAtomicCode(QString path)
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	if (path.isEmpty())
		QProcess::startDetached(settings.value("Path/atomicsEditor").
					toString(), QStringList());
	else {
		QString file = path;
		file.replace(QString("\\"), QString("/"));
		file.prepend("/");
		QDir *dir = new QDir(settings.value("Path/atomicsPath").toString());
		QFileInfo *fi = new QFileInfo(dir->absolutePath() + file);
		path.prepend(settings.value("Path/atomicsPath").toString()).
		    replace("\\", "/");
		QProcess::startDetached(settings.value("Path/atomicsEditor").
					toString(),
					QStringList() << fi->
					absoluteFilePath());
		delete fi;
		delete dir;
	}
}

int PowerGui::saveModel(GpxEditionWindow * w, modelCoupled * coupledData)
{
	QMessageBox msgBox;
	msgBox.setWindowTitle("Save file");
	msgBox.setText(QString("Save changes to %1").arg(windowTitle()));
	msgBox.addButton(QMessageBox::Yes);
	msgBox.addButton(QMessageBox::No);
	msgBox.addButton(QMessageBox::Cancel);
	int result = msgBox.exec();
	if (result == QMessageBox::Cancel) {
		return -1;
	};
	if (result == QMessageBox::Yes) {
		if (w->fileName.isEmpty()) {
			return saveAs();
		} else {
			saveModelToFile(coupledData, w->fileName);
		}
	}
	return 0;
}

void PowerGui::dirtyChanged()
{
	GpxEditionWindow *mw = activeModelWindow();
	if (mw != NULL) {
		mw->dirtyChanged(true);
	}

}

void PowerGui::blankContextMenu(QPoint pos)
{
	QMenu *qMenu = new QMenu(NULL);
	qMenu->addAction(cutAction);
	qMenu->addAction(copyAction);
	qMenu->addAction(pasteAction);
	qMenu->addSeparator();
	qMenu->addAction(deleteAction);
	qMenu->exec(pos);
	delete qMenu;
}

void PowerGui::dragCopy(QPointF p)
{
	if (activeModelWindow()) {
		GpxEditionWindow *gew = activeModelWindow();
		if (fromDragCoupled != NULL)
			delete fromDragCoupled;

		fromDragCoupled = gew->getSelectedData();
		QDrag *drag = new QDrag(this);
		QMimeData *mimeData = new QMimeData;
		mimeData->setText(QString("Drag %1 %2").arg(p.x()).arg(p.y()));
		drag->setMimeData(mimeData);
		drag->setPixmap(QString(":/images/copy.png"));
		drag->setHotSpot(QPoint(15, 30));
		drag->start();
	}
}

void PowerGui::updateRecentFiles()
{
	QStringList::iterator rf;
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QStringList recentFiles =
	    settings.value("recentFiles", QStringList()).toStringList();
	QString examplePath =
	    settings.value("Path/examplePath", QString()).toString();
	int i = 0;
	foreach(QAction * a, recentFilesActions) {
		fileMenu->removeAction(a);
		delete a;
	}
	recentFilesActions.clear();
	for (rf = recentFiles.begin(); rf != recentFiles.end(); rf++) {
		QAction *rfAction;
		if ((*rf).startsWith(examplePath)) {
			rfAction =
			    new QAction((*rf).mid(examplePath.length() + 1),
					this);
		} else {
			rfAction = new QAction(*rf, this);
		}
		recentFilesActions << rfAction;
		rfAction->setData(*rf);
		connect(rfAction, SIGNAL(triggered()), this,
			SLOT(openRecentFile()));
		fileMenu->insertAction(exitAction, rfAction);
		if (i++ > 3)
			break;
	}
	QAction *s = new QAction(this);
	s->setSeparator(true);
	recentFilesActions << s;
	fileMenu->insertAction(exitAction, s);
}
