#include "ui_knob.h"
#include <QWidget>

class Knob: public QWidget, public Ui::knob
{
public:
  Knob() { setupUi(this); }
};
