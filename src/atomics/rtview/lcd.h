#include "ui_lcd.h"
#include <QWidget>

class LCD: public QWidget, public Ui::lcd
{
public:
  LCD() { setupUi(this); }
  void setName(QString n) { name->setText(n); };
  void setValue(QString v) { value->display(v); };
  void setFile(QString f) { _file=f; };
private:
  QString _file;
};
