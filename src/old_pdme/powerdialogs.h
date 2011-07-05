/****************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**										 Universidad Nacional de Rosario - Argentina.
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

/*! \brief This class defines PowerDevs dialogs.
*
*		This class provides access to PowerDevs dialogs, diferent dialogs are used  
*		to customize PowerDevs gui and to edit atomics and coupled values.	
*		\todo 
*					- All Dialogs:
*							-# Change default size, its to big.
*					- Menu Dialog
*							-# Check the remove item slot when you add a new menu. 
*					- Edit Atomic Values
*							-# Update atomic values with those of myatomic. 
*							-# Update display icons. 
*							-# Define the update of the colors. 
*							-# Define the update of the ports.
*							-# Disable selection on Atomic's preview.
*							-# Atomic's preview must be fixed in the center of the scene.
*							-# Achicar las fuentes en el tab de los parametros, y en el resto de los dialogos en general.
*							-# Cambiar los PATHS en esta clase y en la clase Atomic para que usen los parametros del ini.
*							-# Hacer los distintos cherqueos correspondientes en cada Tab (Los tiene que hacer en el apply, y los guarda en el ok).
*							-# Update atomic initial position in preview.
*							-# Define how to delete the new folder if the name is empty.
*					- Params Dialog
*							-# Define how a mask on TED's, for type checking. 
*							-# Update the number of inputs and outputs in the atomic. 
*							-# The parser never returns a value of type SCILAB. * 
*							-# Update dialog minimum size.
*					- Library Setup Dialog
*							-# Change the size of the toolbox when updating libraries.
*
*/

#ifndef POWERDIALOGS_H
#define POWERDIALOGS_H

#include <QtGui>
#include "gpxblock.h"
#include "gpxeditionscene.h"
#include "gpxconnectionpoint.h"
#include "gpxatomic.h"
#include "gpxcoupled.h"

class modelCoupled;
class modelAtomic;

class PowerDialogs:public QDialog {
 Q_OBJECT public:
	/*! Constructs a dialog with the given parent, it returns an empty dialog. */
	PowerDialogs(QWidget * parent);
	/*! Show change priority dialog, given a Model, this dialog is used to change the priority order of the atomic and coupled in a model. */
	void priorityDialog(GpxEditionScene * mc);
	/*! Edit an Atomic (parameters, inports, outports, icon, horientation, etc...) */
	void editAtomicDialog(GpxBlock * a);
	void pathSetupDialog();
	void librarySetupDialog();
	void menuSetupDialog();

	//void setDialog(Dialogs d);
	 signals:
	    /*! This signal is emitted when the user finishes editing the configuration of the libraries of PowerDEVS */
	void writeToolBoxSettings();
	/*! This signal is emitted when the user finishes editing the configuration of the various menus of PowerDEVS. */
	void updateMenus();
	void editAtomicCode(QString);
	void dirtyChanged();

	public slots:
	    /*! Show edit parameters dialog, given an Atomic */
	void blockParamDialog(GpxBlock * a);
	void modelParamDialog(modelCoupled * mp);
	void editModelParameters(modelCoupled * mc);

	private slots:
	/*******************************************************
	 * These slots are used by Set Path and Tools dialog.  *
   *******************************************************/
	    /*! This slot is used to select the executable to edit atomic models. */
	void searchAtomicEditor();
	/*! This slot is used to select the path to find PowerDevs models. */
	void searchAtomicPath();
	/*! This slot is used to select the help file of PowerDevs. */
	void searchHelpFile();
	/*! This slot is used to save the new configuration of PowerDevs Path and Tools. */
	void savePathSettings();
	void cancelPathSetup();

	/******************************************************
	 * These slots are used by Library Setup dialog.      * 
	 ******************************************************/

	/*! This slot is used to select a new library. */
	void openLibrary();
	/*! This slot is used to save the new configuration of the libraries, emits a writeToolBoxSettings signal. */
	void saveToolBoxSettings();
	void cancelLibrarySetup();
	/*! This slot is used to update the library path, emits a writeToolBoxSettings signal. */
	void updatePathLbl(int row);
	/*! This slot is used to move up the library selected in the list of libraries.  */
	void upLibLst();
	/*! This slot is used to move down the library selected in the list of libraries.  */
	void downLibLst();
	/*! This slot is used to remove the library selected from the list of libraries.  */
	void removeLibrary();

	/******************************************************
	 * These slots are used by Menu Setup dialog.         * 
	 ******************************************************/

	/*! This slot is used to edit an existing menu or a new one. */
	void editMenu(int btn);
	/*! This slot is used to update the menu buffer of the dialog. */
	void updateMenuEditBuffer(const QString & text);
	/*! This slot is used to update the menu list of the dialog. */
	void updateMenuEditLstWdt(const QString & text);
	/*! This slot is used to update the file asociated with de menu item. */
	void updateFileLed(const QString & text);
	/*! This slot is used to update the parameters asociated with the menu item. */
	void updateParamLed(const QString & text);
	/*! This slot is used to update the saveModelBefore checkBox asociated with the menu item. */
	void updateSaveModelBefore(int state);
	/*! This slot is used to update the needsOpenModel checkBox asociated with the menu item. */
	void updateNeedsOpenModel(int state);
	/*! This slot is used to update the shorcut asociated with the menu item. */
	void updateShorcutCbx(const QString & text);
	/*! This slot is used to update the actual menu. */
	void updateEditMenu(int row);
	/*! This slot is used to select the menu item icon. */
	void selectIconPxm();
	/*! This slot is used to remove the menu item icon. */
	void removeIconPxm();
	/*! This slot is used to save the menu settings. */
	void saveEditMenuSettings();
	/*! This slot is used to save the new menu settings in the menu buffer. */
	void saveNewMenu();
	/*! This slot is used to add an item to a new menu. */
	void addMenuItem();
	/*! this slot is used to save the new menu configuration. */
	void saveEditMenu();
	/*! This slot is used to remove an existing menu. */
	void removeMenu();
	/*! This slot is used to remove a menu item from a new menu. */
	void removeItem();
	/*! This slot is used to disable edit buttons, when theres no item interted in a new menu. */
	void disableNewMenuButtons();
	void cancelEditMenu();
	void cancelMenuSetup();

	/******************************************************
	 * These slots are used by Priority dialog.           * 
	 ******************************************************/

	/*! This slot is used to change an atomic or coupled priority in a model. */
	void updatePriority(int p);
	/*! This slot is used to save the new priority list of a model. */
	void savePriority();
	/*! This slot is used to release the modelchild copies. */
	//void releaseModelChilds();
	void cancelPriority();

	/******************************************************
	 * These slots are used by Params dialog.             * 
	 ******************************************************/

	/*! This slot is used to save the new params of the model. */
	void saveParams();
	void cancelParams();
	void saveModelParams();
	/*! This slot is used add or remove InPorts/OutPorts when editing an Atomic and the %Inputs or %Ouputs value are defined. */
	void addPorts(GpxConnectionPoint::ConnectionPointType p, int i);
	
	
	/********************************************************
	 * These slots are used by Edit Model Parameters dialog.* 
	 ********************************************************/
	void saveModelParameters();
	void cancelModelParameters();

	/******************************************************
	 * These slots are used by Edit Atomic dialog.        * 
	 ******************************************************/

	/*! This slot is used to update the values of the parameters when changing the values of the grid. In the parameters tab. */
	void editAtomicValues(int row);
	/*! This slot is used to update the values of the grid when changing the values of the parameters. In the parameters tab. */
	void updateAtomicValues(int column);
	/*! This slot is used to add a new parameter in the atomic. */
	void newParam();
	/*! This slot is used to remove a parameter in the atomic. */
	void delParam();
	/*! This slot is used to update the focus when changing position on the grid. */
	void updatePos(int row, int column);
	/*! This slot is used to change the order of the parameters in the atomic. */
	void swapParams(int btn);
	/*! This slot is used to edit a type list parameter of the atomic. */
	void editListParam();
	/*! This slot is used to add a type list parameter to the atomic. */
	void newListParam();
	/*! This slot is used to remove a type list parameter of the atomic. */
	void delListParam();
	/*! This slot is used to save  the new values of a type list parameter. */
	void saveListParam();
	/*! This slot is used to edit a new code file. */
	void newCodeFile();
	/*! This slot is used to edit an existing code file, just set the atomic path member and call editCode on the atomic. */
	void editCode();
	/*! This slot is used to change the order of the ports in the atomic. */
	void swapPorts(int p);
	/*! This slot is used to select the atomic icon. */
	void selectAtomicIcon();
	/*! This slot is used to update the file view in the code tab, setting the new current index of the view. */
	void updateFiles(QListWidgetItem * item);
	/*! This slot is used to update the dir view in the code tab, setting the new current index of the view. */
	void updateDirs(QListWidgetItem * item);
	/*! This slot is used to refresh de file and dir view, refreshing both views and setting the rootIndex again. */
	//void refreshFiles();
	/*! This slot is used to update the number of InPorts of the Atomic that is being edited */
	void updateInPorts(int n);
	/*! This slot is used to update the number of OutPorts of the Atomic that is being edited */
	void updateOutPorts(int n);
	/*! This slot is used to update the background color of the Atomic. */
	void updateAtomicColor(int i);
	/*! This slot is used to update the description of the Atomic. It's called by descTed textChanged signal.  */
	void updateAtomicDesc();
	/*! This slot is used to save the Atomic changes, delete the copies and update the values of the original Atomic. */
	void saveAtomic();
	/*! This slot is used to save current tab values in the original Atomic. */
	void apply();
	/*! Creates a new entry in the atomics list widget, and waits for the editor to finish. */
	void makeNewFolder();
	/*! Creates a new subdirectory in the dir estructure, with the text of folderName, if it can't be created, displays a critical message and delete
	   the item from the atomics list widget. */
	void saveNewFolder();
	/*! Updates the name of the new folder, it's called when the new item widget is edited. */
	void updateFolderName(QString name);
	void cancelEditAtomic();

 private:
	bool checkMenuValues();
	void disableEditAtomicParams();
	void enableEditAtomicParams();
	void applyPropertiesChanges();
	void applyParametersChanges();
	void applyCodeChange();
	void applyIOChange();
	void paramDialog(QString name, QString desc, bool bt,
			 QList < modelParameter * >parameters);

	QDialog *dialog;

	// Library Setup private objects.

	QListWidget *lsLstWdt;
	QLabel *pathLbl;
	QStringList libLst;
	QStringList libEnabled;

	// Set Path anbd Tools private objects.

	QLineEdit *paramLed;
	QLineEdit *editorLed;
	QLineEdit *pathLed;
	QLineEdit *filterLed;
	QLineEdit *helpLed;

	// Menu Setup Dialog private objects.

	QLineEdit *menuLed;
	QListWidget *menuLstWdt;
	QListWidget *menuEditLstWdt;
	QLineEdit *menuEditLed;
	QLineEdit *fileLed;
	QLineEdit *paramMenuLed;
	QComboBox *shorcutCbx;
	QPixmap iconPxm;
	QStringList menuEditLst;
	QStringList menuValuesBuff;
	QList < QStringList > menuBuff;
	QStringList shorcutLst;
	QStringList shorcutDisabled;
	QLabel *toolbarLbl;
	QStringList menuLst;
	QLineEdit *newMenuLed;
	QDialog *emDialog;
	QDialogButtonBox *editMenuBbx;
	QPushButton *removeItemBtn;
	QCheckBox *saveModelBefore;
	QCheckBox *needsOpenModel;
	QGroupBox *menuGbx;
	QGroupBox *commandGbx;
	QGroupBox *toolbarGbx;

	// PriorityDialog private objects.

	QListWidget *priLstWdt;
	 QList < modelChild * >childs;
	GpxEditionScene *myEdtScene;

	// EditAtomicDialog private objects.

	QLineEdit *nameLed;
	QSpinBox *inputSbx;
	QSpinBox *outputSbx;
	QTextEdit *descTed;
	QLineEdit *iconLed;
	QSpinBox *widthSbx;
	QSpinBox *heightSbx;
	QComboBox *orientationCbx;
	QComboBox *colorCbx;
	QComboBox *valueCbx;
	QTableWidget *paramTblWdt;
	QLineEdit *paramNameLed;
	QLineEdit *valueLed;
	QTextEdit *paramDescTed;
	QComboBox *typeCbx;
	QListWidget *atomicsLstWdt;
	QFileInfoList fileInfLst;
	QFileInfoList folderFileInfLst;
	QListWidget *fileLstWdt;
	QDirModel *fileModel;
	QListWidget *inputLstWdt;
	QListWidget *outputLstWdt;
	QPixmap atomicPxm;
	// Change this in the future!!!
	GpxAtomic *myAtomic;
	GpxAtomic *origAtomic;
	GpxCoupled *myCoupled;
	GpxCoupled *origCoupled;
	GpxBlock *block;
	 QList < modelParameter * >myParams;
	QPushButton *edtBtn;
	QPushButton *deleteBtn;
	QToolButton *paramUpBtn;
	QToolButton *paramDownBtn;
	QPushButton *delListParamValueBtn;
	QTableWidget *listParamTblWdt;
	QFileInfo *fi;
	QDir *dir;
	QDir *fileDir;
	QLabel *dirPathLbl;
	 QList < modelPort * >myInPorts;
	 QList < modelPort * >myOutPorts;
	GpxEditionScene *scene;
	QGraphicsView *view;
	 QList < QColor > colors;
	QStringList colorNames;
	GpxBlock *atomicOrig;
	QTabWidget *editTab;
	QString atomicsPath;
	QString imagesPath;
	QString folderName;
	bool directoryChange;

	// ParamsDialog private objects.

	 QList < modelParameter * >params;
	 QList < QLineEdit * >edtParams;
	 QList < QComboBox * >cbxParams;
	 modelCoupled *myModelCoupled;
     QTextEdit *descLbl;

};

#endif
