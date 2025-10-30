#ifndef TITLEIDFINDERDIALOG_H
#define TITLEIDFINDERDIALOG_H

// qt
#include <QDialog>
#include <QMenu>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include <QStatusBar>
#include <QTreeWidgetItem>

// other
#include "titleidfinder.h"

namespace Ui
{
class TitleIdFinderDialog;
}

class TitleIdFinderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TitleIdFinderDialog(QStatusBar *statusBar, QWidget *parent = 0);
    ~TitleIdFinderDialog();

private slots:
    void on_pushButton_clicked();
    void on_clearButton_clicked();
    void onRequestFinished(QList<TitleData>);
    void showContextMenu(QPoint p);
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    void openDboxTools(QTreeWidgetItem *item);
    Ui::TitleIdFinderDialog *ui;

    TitleIdFinder *finder;
    QStatusBar *statusBar;
};

#endif // TITLEIDFINDERDIALOG_H


