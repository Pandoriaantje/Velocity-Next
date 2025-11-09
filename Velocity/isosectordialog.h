#ifndef ISOSECTORDIALOG_H
#define ISOSECTORDIALOG_H

#include <QDialog>
#include "XboxInternals/Iso/IsoImage.h"

namespace Ui {
class IsoSectorDialog;
}

class IsoSectorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IsoSectorDialog(XboxInternals::Iso::IsoImage *iso, QWidget *parent = nullptr);
    ~IsoSectorDialog();

private slots:
    void onSectorValueChanged(int value);

private:
    Ui::IsoSectorDialog *ui;
    XboxInternals::Iso::IsoImage *iso_;
};

#endif // ISOSECTORDIALOG_H
