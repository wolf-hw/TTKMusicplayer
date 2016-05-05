#include "musiclrclocallinkwidget.h"
#include "ui_musiclrclocallinkwidget.h"
#include "musicbackgroundmanager.h"
#include "musicconnectionpool.h"
#include "musicmessagebox.h"
#include "musicuiobject.h"

#include <QDir>
#include <QFileDialog>

MusicLrcLocalLinkTableWidget::MusicLrcLocalLinkTableWidget(QWidget *parent)
    : MusicAbstractTableWidget(parent)
{
    setColumnCount(2);
    QHeaderView *headerview = horizontalHeader();
    headerview->resizeSection(0, 132);
    headerview->resizeSection(1, 200);
}


MusicLrcLocalLinkWidget::MusicLrcLocalLinkWidget(QWidget *parent)
    : MusicAbstractMoveDialog(parent),
      ui(new Ui::MusicLrcLocalLinkWidget)
{
    ui->setupUi(this);

    ui->topTitleCloseButton->setIcon(QIcon(":/share/searchclosed"));
    ui->topTitleCloseButton->setStyleSheet(MusicUIObject::MToolButtonStyle03);
    ui->topTitleCloseButton->setCursor(QCursor(Qt::PointingHandCursor));
    ui->topTitleCloseButton->setToolTip(tr("Close"));
    connect(ui->topTitleCloseButton, SIGNAL(clicked()), SLOT(close()));

    ui->fuzzyButton->setStyleSheet(MusicUIObject::MCheckBoxStyle01);
    ui->localSearchButton->setStyleSheet(MusicUIObject::MPushButtonStyle08);
    ui->commitButton->setStyleSheet(MusicUIObject::MPushButtonStyle08);
    ui->previewButton->setStyleSheet(MusicUIObject::MPushButtonStyle08);
    ui->titleEdit->setStyleSheet(MusicUIObject::MLineEditStyle01);

    ui->fuzzyButton->setChecked(true);
    ui->titleEdit->setEnabled(false);

    connect(ui->fuzzyButton, SIGNAL(clicked(bool)), SLOT(fuzzyStateChanged()));
    connect(ui->previewButton, SIGNAL(clicked()), SLOT(findInLocalFile()));
    connect(ui->localSearchButton, SIGNAL(clicked()), SLOT(searchInLocalDir()));
    connect(ui->commitButton, SIGNAL(clicked()), SLOT(confirmButtonClicked()));

    M_CONNECTION->setValue("MusicLrcLocalLinkWidget", this);
    M_CONNECTION->poolConnect("MusicLrcLocalLinkWidget", "MusicDownloadStatusLabel");
}

MusicLrcLocalLinkWidget::~MusicLrcLocalLinkWidget()
{
    M_CONNECTION->poolDisConnect("MusicLrcLocalLinkWidget");
    delete ui;
}

void MusicLrcLocalLinkWidget::setCurrentSongName(const QString &name)
{
    ui->titleEdit->setText(name);
    searchInLocalMLrc();
}

void MusicLrcLocalLinkWidget::createAllItems(const LocalDataItems &items)
{
    int count = ui->searchedTable->rowCount();
    ui->searchedTable->setRowCount(ui->searchedTable->rowCount() + items.count());
    for(int i=0; i<items.count(); ++i)
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText( QFontMetrics(font()).elidedText(items[i].m_name, Qt::ElideRight, 128));
        item->setToolTip( items[i].m_name );
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->searchedTable->setItem(count + i, 0, item);

                          item = new QTableWidgetItem;
        item->setText( QFontMetrics(font()).elidedText(items[i].m_path, Qt::ElideRight, 195));
        item->setToolTip( items[i].m_path );
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->searchedTable->setItem(count + i, 1, item);
    }
}

void MusicLrcLocalLinkWidget::searchInLocalMLrc()
{
    QString title = ui->titleEdit->text().trimmed();
    if(title.isEmpty())
    {
        return;
    }

    ui->fuzzyButton->isChecked();
    QStringList list = QDir(LRC_DOWNLOAD_AL).entryList(QDir::Files |  QDir::Hidden |
                                                       QDir::NoSymLinks | QDir::NoDotAndDotDot);
    LocalDataItems items;
    foreach(QString var, list)
    {
        if(var.contains(title, ui->fuzzyButton->isChecked() ? Qt::CaseInsensitive : Qt::CaseSensitive))
        {
            LocalDataItem item;
            item.m_name = var;
            item.m_path = LRC_DOWNLOAD_AL + var;
            items << item;
        }
    }
    createAllItems(items);
}

void MusicLrcLocalLinkWidget::fuzzyStateChanged()
{
    ui->searchedTable->clear();
    searchInLocalMLrc();
}

void MusicLrcLocalLinkWidget::searchInLocalDir()
{

}

void MusicLrcLocalLinkWidget::findInLocalFile()
{
    QString picPath = QFileDialog::getOpenFileName(
                      this, QString(), "./", "LRC (*.lrc)");
    if(picPath.isEmpty())
    {
        return;
    }

    LocalDataItems items;
    LocalDataItem item;
    item.m_name = picPath.split("/").last();
    item.m_path = picPath;
    items << item;
    createAllItems(items);
}

void MusicLrcLocalLinkWidget::confirmButtonClicked()
{
    int row = ui->searchedTable->currentRow();
    if(row < 0)
    {
        MusicMessageBox message;
        message.setText(tr("please select one item"));
        message.exec();
        return;
    }

    QString path = ui->searchedTable->item(row, 1)->toolTip();
    QFile fileIn(path);
    if(!fileIn.open(QIODevice::ReadOnly))
    {
        M_LOGGER_ERROR("Lrc Input File Error!");
        fileIn.close();
        close();
        return;
    }
    QFile fileOut(path.left(path.lastIndexOf("/") + 1) + ui->titleEdit->text() + LRC_FILE);
    if(!fileOut.open(QIODevice::WriteOnly))
    {
        M_LOGGER_ERROR("Lrc Output File Error!");
        fileOut.close();
        close();
        return;
    }
    fileOut.write(fileIn.readAll());
    fileOut.flush();
    fileOut.close();
    fileIn.close();

    emit currentLrcChanged("Lrc");
    close();
}

int MusicLrcLocalLinkWidget::exec()
{
    QPixmap pix(M_BG_MANAGER->getMBackground());
    ui->background->setPixmap(pix.scaled( size() ));
    return MusicAbstractMoveDialog::exec();
}