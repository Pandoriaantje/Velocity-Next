#include "titleidfinderdialog.h"
#include "ui_titleidfinderdialog.h"

TitleIdFinderDialog::TitleIdFinderDialog(QStatusBar *statusBar, QWidget *parent) :
    QDialog(parent), ui(new Ui::TitleIdFinderDialog), statusBar(statusBar)
{
    ui->setupUi(this);
    finder = new TitleIdFinder("", this);

    connect(finder, SIGNAL(SearchFinished(QList<TitleData>)), this,
            SLOT(onRequestFinished(QList<TitleData>)));

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeWidget->header()->resizeSection(0, 300);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showContextMenu(QPoint)));
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this,
            SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));

    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(on_pushButton_clicked()));
}

TitleIdFinderDialog::~TitleIdFinderDialog()
{
    delete ui;
}

void TitleIdFinderDialog::on_pushButton_clicked()
{
    finder->SetGameName(ui->lineEdit->text());
    finder->StartSearch();
    statusBar->showMessage("Searching...", 0x7FFFFFFF);
}

void TitleIdFinderDialog::on_clearButton_clicked()
{
    ui->lineEdit->clear();
    ui->treeWidget->clear();
    statusBar->clearMessage();
}

void TitleIdFinderDialog::onRequestFinished(QList<TitleData> matches)
{
    ui->treeWidget->clear();
    
    if (matches.isEmpty())
    {
        statusBar->showMessage("No results found or connection error. Check console for details.", 5000);
        return;
    }
    
    statusBar->showMessage("Search returned " + QString::number(matches.length()) + " result(s)", 3000);

    for (int i = 0; i < matches.length(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);

        QString newStr = ((QString*)&matches.at(i).titleName)->replace("&#174;", "®").replace("&#39;",
                "'").replace("&amp;","&").replace("&gt;",">").replace("&lt;","<").replace("â", "").replace("¢", "");
        item->setText(0, newStr);
        // Format Title ID as 8-character uppercase hex with leading zeros
        item->setText(1, QString("%1").arg(matches.at(i).titleID, 8, 16, QChar('0')).toUpper());
        // Store bing_id in UserRole for marketplace links
        item->setData(1, Qt::UserRole, matches.at(i).bingId);

        ui->treeWidget->addTopLevelItem(item);
    }
}

void TitleIdFinderDialog::showContextMenu(QPoint p)
{
    if (ui->treeWidget->selectedItems().count() == 0)
        return;

    QPoint globalPos = ui->treeWidget->mapToGlobal(p);
    QMenu contextMenu;

    contextMenu.addAction(QPixmap(":/Images/id.png"), "Copy Title ID");
    contextMenu.addAction(QPixmap(":/Images/id.png"), "Copy Game Name");
    contextMenu.addAction(QPixmap(":/Images/xbox.png"), "View on dbox.tools");
    QAction *selectedItem = contextMenu.exec(globalPos);

    if (selectedItem == nullptr)
        return;
    else if (selectedItem->text() == "Copy Title ID")
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(ui->treeWidget->selectedItems().at(0)->text(1));
        statusBar->showMessage("Title ID copied successfully", 3000);
    }
    else if (selectedItem->text() == "Copy Game Name")
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(ui->treeWidget->selectedItems().at(0)->text(0));
        statusBar->showMessage("Game name copied successfully", 3000);
    }
    else if (selectedItem->text() == "View on dbox.tools")
    {
        openDboxTools(ui->treeWidget->selectedItems().at(0));
    }
}

void TitleIdFinderDialog::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    openDboxTools(item);
}

void TitleIdFinderDialog::openDboxTools(QTreeWidgetItem *item)
{
    if (!item)
        return;
    
    // Get the bing_id from the item's UserRole data
    QString bingId = item->data(1, Qt::UserRole).toString();
    
    if (!bingId.isEmpty()) {
        // Open the marketplace page using the bing_id
        QDesktopServices::openUrl(QUrl(QString("https://dbox.tools/marketplace/products/%1/").arg(bingId)));
    } else {
        // Fallback: open title page using Title ID
        QString titleId = item->text(1);
        QDesktopServices::openUrl(QUrl(QString("https://dbox.tools/titles/xbox360/%1/").arg(titleId)));
    }
}


