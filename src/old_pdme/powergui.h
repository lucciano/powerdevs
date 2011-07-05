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

#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QAction>
#include <QUndoGroup>

#include "gpxblock.h"
#include "gpxcoupled.h"
#include "gpxedge.h"
#define  TWIPSTOPIXEL (1/15.0)

class QAction;
class QLabel;
class GpxEditionScene;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QGraphicsView;
class QToolBox;
class QToolButton;
class QButtonGroup;
class QComboBox;
class PowerDialogs;
class GpxEditionWindow;
class LibraryWindow;

extern PowerDialogs *pDialogs;
class PowerGui:public QMainWindow {
 Q_OBJECT public:
	PowerGui();
	~PowerGui();
	/* Hack to use the delete editing name */
	void editingName() {	/* deleteAction->setEnabled(false); */
	};
	void doneEditingName() {	/* deleteAction->setEnabled(true); */
	};
	modelAtomic *fromLibAtomic;
	modelCoupled *fromLibCoupled;
	modelPort *fromLibPort;
	modelCoupled *fromDragCoupled;
	modelCoupled *fromClipboardCoupled;
	//static int colorToInt(QColor);
	//static QColor intToColor(int);
	int saveModel(GpxEditionWindow *, modelCoupled *);

	public slots: int save();
	int saveAs();
	void cut();
	void copy();
	void updateToolBox();
	void setStatusBar(QString *);
	void contextMenu(GpxBlock *, QPoint);
	void blankContextMenu(QPoint);
	void dirtyChanged();
	void dragCopy(QPointF);
	void closeEditionWindow(GpxEditionWindow * w);

 protected:
	void closeEvent(QCloseEvent * event);

	private slots: void updateMenus();
	void newFile();
	void open();
	void openRecentFile();
	void deleteItem();
	void rotateItem();
	void paste();
	void about();
	void itemColorChanged();
	void fillButtonTriggered();
	void updateWindowMenu();
	//GpxEditionScene *createModelChild(QString,modelCoupled*);
	void setActiveSubWindow(QWidget * window);
	void updateStatusBar();
	void itemInserted(GpxBlock * item);
	void itemSelected(QGraphicsItem * item);
	void edgeInserted(GpxEdge * edge);
	void runAction();
	void librarySetup();
	void menuSetup();
	void pathSetup();
	void editAtomic();
	void editParams();
	void editModelParams();
	void editModelParameters();
	void updateUserMenus();
	void prioritySetup();
	void selectAll();
	void editCoupled();
	void editAtomicCode(QString path);

 private:
	void createActions();
	void createContextMenu();
	void createToolBars();
	void createStatusBar();
	void createMenus();
	void defineMenus();
	void readSettings();
	void writeSettings();
	void createToolBox();
	void updateRecentFiles();
	static int modelCounter;

	QMenu *createColorMenu(const char *slot, QColor defaultColor);
	QIcon createColorToolButtonIcon(const QString & image, QColor color);
	QIcon createColorIcon(QColor color);

	GpxEditionWindow *activeModelWindow();
	GpxEditionScene *activeModelChild();
	QMdiSubWindow *findModelChild(const QString & fileName);

	QMdiArea *mdiArea;

	QSignalMapper *windowMapper;

	QLabel *locationLabel;
	QLabel *formulaLabel;

	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *windowMenu;
	QMenu *helpMenu;
	QMenu *itemMenu;
	QMenu *viewMenu;
	QMenu *optionMenu;

	QToolBar *fileToolBar;
	QToolBar *colorToolBar;
	QToolBar *editToolBar;
	QToolBar *userTool;

	QToolBox *toolBox;

	QToolButton *fillColorToolButton;

	QAction *newAction;
	QAction *openAction;
	QAction *saveAction;
	QAction *saveAsAction;
	QAction *exportAction;
	QAction *printAction;
	QAction *exitAction;
	QAction *cutAction;
	QAction *copyAction;
	QAction *pasteAction;
	QAction *editAtomicAction;
	QAction *openEditAction;
	QAction *selectAllAction;
	QAction *priorityAction;
	QAction *modelAction;
	QAction *closeAction;
	QAction *closeAllAction;
	QAction *toolbarAction;
	QAction *parametersAction;
	QAction *modelParametersAction;
	QAction *tileAction;
	QAction *cascadeAction;
	QAction *nextAction;
	QAction *previousAction;
	QAction *separatorAction;
	QAction *aboutAction;
	QAction *aboutQtAction;
	//QAction *toFrontAction;
	//QAction *sendBackAction;
	QAction *deleteAction;
	QAction *rotateAction;
	QAction *fillAction;
	//QAction *libraryAction;
	QAction *librarySetupAction;
	QAction *menuSetupAction;
	QAction *pathSetupAction;
	QList < QAction * >userActions;
	QList < QAction * >recentFilesActions;
	LibraryWindow *dock;
 	QUndoGroup *undoStack;
};

extern PowerGui *mainWindow;

#endif
