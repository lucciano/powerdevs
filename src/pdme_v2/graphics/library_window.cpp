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

#include <QToolBox>
#include <QGraphicsView>

#include <library_window.h>
#include <library_scene.h>
#include <globals.h>

LibraryWindow::LibraryWindow(): QDockWidget()
{
	setFeatures(DockWidgetMovable | DockWidgetFloatable);
}
LibraryWindow::~LibraryWindow()
{
}
QSize LibraryWindow::sizeHint() const
{
  int width = getSetting("LibraryWidth").toInt();
  return QSize(width,minimumHeight());
}
void LibraryWindow::resizeEvent(QResizeEvent * e)
{
  QToolBox *tb = dynamic_cast<QToolBox*>(widget());
  for (int j=0;j<tb->count();j++)
  {
    QGraphicsView *view = (QGraphicsView*)tb->widget(j);
    LibraryScene *ls = (LibraryScene*)view->scene();
    if (!ls)
      continue;
    double max_width=1,max_height=1;
    for (int i=0;i<ls->blocks.size();i++)
    {
      if (max_width<ls->blocks.at(i)->getBoundingWidth())
        max_width=ls->blocks.at(i)->getBoundingWidth();
      if (max_height<ls->blocks.at(i)->getBoundingHeight())
        max_height=ls->blocks.at(i)->getBoundingHeight();
    } 
    max_width+=20;
    int columns = tb->width()/max_width;
    columns = (columns < 1 ? 1 : columns);
    int col=0,row=0;
    for (int i=0;i<ls->blocks.size();i++) {
      ls->blocks.at(i)->setPos(col*max_width,row*max_height);
      col++;
      if (col==columns) {
        row++;
        col=0;
      }
    }
    view->scene()->setSceneRect(-max_width/2-10,-max_height/2-10,tb->width(),tb->height());
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->resize(30,30);
  }
}
