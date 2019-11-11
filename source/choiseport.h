#ifndef CHOISEPORT_H
#define CHOISEPORT_H

#include <QDialog>

namespace Ui {
class choisePort;
}

class choisePort : public QDialog
{
    Q_OBJECT

public:
    explicit choisePort(QWidget *parent = nullptr);
    ~choisePort();

private:
    Ui::choisePort *ui;
};

#endif // CHOISEPORT_H
