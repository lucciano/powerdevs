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

#ifndef GPXEDITIONWINDOW_H
#define GPXEDITIONWINDOW_H
#include "powergui.h"
#include <QGraphicsView>

class GpxEditionWindow:public QTabWidget {
 Q_OBJECT public:
	GpxEditionWindow(modelCoupled * c);
	~GpxEditionWindow();
	int addTab(modelCoupled * d, GpxCoupled * gc = 0);
	bool isDirty() {
		return dirty;
	};
	void clearDirty() {
	};
	void setDirty() {
	};
	QString fileName;
	modelCoupled *getData();
	//GpxEditionScene *getSceneAt(int i) { return sceneAt(i); };
	void setName(QString);
	modelCoupled *getSelectedData();
	void removeBlock(GpxBlock * b);
	QUndoStack *getUndoStack();
 protected:
	void closeEvent(QCloseEvent * event);
public slots: void dirtyChanged(bool d);
	void closeTab(int);
 signals:
	void saveModel(GpxEditionWindow *, modelCoupled *);
	void closeEditionWindow(GpxEditionWindow *);
 private:
	bool dirty;
	GpxEditionScene *sceneAt(int i) {
		return (GpxEditionScene *) ((QGraphicsView *) widget(i))->
		    scene();
	};
	void deleteScenes(modelCoupled *mc);
	void updateTabs(modelCoupled *mc);
	GpxEditionScene *first;
};
#endif
