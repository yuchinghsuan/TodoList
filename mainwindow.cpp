#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QDateTime>
#include <fstream>
#include "cJSON.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->LBVersion->setText("v1.0.0");
    ui->RBHigh->setChecked(true);
    ui->PBUpdate->setEnabled(false);
    ui->TWContent->setContextMenuPolicy(Qt::CustomContextMenu);
    refresh();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refresh()
{
    if (m_vecInfo.empty()) readInfo();
    initTable(m_vecInfo);
}

void MainWindow::readInfo()
{
    m_vecInfo.clear();

    std::string strContent, strLine;
    QString qstrPath = QApplication::applicationDirPath() + "/data.json";
    std::ifstream objFile(qstrPath.toStdString());
    if (!objFile.is_open()) return;
    while (std::getline(objFile, strLine)) strContent += strLine + "\n";
    objFile.close();

    cJSON *pstJson = cJSON_Parse(strContent.c_str());
    if (NULL == pstJson) return;
    int nSumResult = cJSON_GetArraySize(pstJson);
    for (int i = 0; i < nSumResult; i++)
    {
        cJSON *pstItem = cJSON_GetArrayItem(pstJson, i);
        if (NULL == pstItem) return;
        TodoInfo stInfo;
        stInfo.nPriority = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(pstItem, "priority"));
        stInfo.strDate =  cJSON_GetStringValue(cJSON_GetObjectItem(pstItem, "date"));
        stInfo.strText =  cJSON_GetStringValue(cJSON_GetObjectItem(pstItem, "text"));
        m_vecInfo.push_back(stInfo);
    }

    cJSON_Delete(pstJson);
}

void MainWindow::writeInfo()
{
    cJSON *pstJson = cJSON_CreateArray();
    if (NULL == pstJson) return;
    for(int i = 0; i < m_vecInfo.size(); i++)
    {
        cJSON *pstItem = cJSON_CreateObject();
        if (NULL == pstItem) return;
        cJSON_AddNumberToObject(pstItem, "priority", m_vecInfo.at(i).nPriority);
        cJSON_AddStringToObject(pstItem, "date", m_vecInfo.at(i).strDate.c_str());
        cJSON_AddStringToObject(pstItem, "text", m_vecInfo.at(i).strText.c_str());
        cJSON_AddItemToArray(pstJson, pstItem);
    }

    char *pszJsonData = cJSON_Print(pstJson);
    std::string strJsonData = pszJsonData;
    cJSON_free(pszJsonData);
    cJSON_Delete(pstJson);

    QString qstrPath = QApplication::applicationDirPath() + "/data.json";
    std::ofstream objFileNew(qstrPath.toStdString(), std::ios::out | std::ios::trunc);
    if (!objFileNew.is_open()) return;
    objFileNew << strJsonData;
    objFileNew.close();
}

void MainWindow::initTable(QVector<TodoInfo> &p_vecInfo)
{
    QStringList listHeader;
    listHeader << "priority" << "text" << "date";
    ui->TWContent->clear();
    ui->TWContent->setRowCount(p_vecInfo.size());
    ui->TWContent->setColumnCount(3);
    ui->TWContent->setAlternatingRowColors(true);
    ui->TWContent->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TWContent->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->TWContent->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->TWContent->setHorizontalHeaderLabels(listHeader);
    ui->TWContent->horizontalHeader()->setSectionsClickable(true);
    ui->TWContent->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    for(int i = 0; i < p_vecInfo.size(); i++)
    {
        QTableWidgetItem *item0 = new QTableWidgetItem;
        QTableWidgetItem *item1 = new QTableWidgetItem;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        item0->setData(Qt::EditRole, p_vecInfo.at(i).nPriority);
        item1->setText(QString::fromStdString(p_vecInfo.at(i).strText));
        item2->setText(QString::fromStdString(p_vecInfo.at(i).strDate));
        ui->TWContent->setItem(i, 0, item0);
        ui->TWContent->setItem(i, 1, item1);
        ui->TWContent->setItem(i, 2, item2);
    }
}

QString MainWindow::getCurrentDateTime()
{
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString qstrDate = currentDate.toString("yyyy-MM-dd");
    QString qstrTime = currentTime.toString("hh:mm:ss");
    qint64 milliseconds = currentDateTime.toMSecsSinceEpoch() % 1000;
    return QString("%1 %2.%3").arg(qstrDate).arg(qstrTime).arg(milliseconds);
}

void MainWindow::on_RBHigh_clicked(bool checked)
{
    if (checked)
    {
        ui->RBMedium->setChecked(false);
        ui->RBLow->setChecked(false);
    }
}

void MainWindow::on_RBMedium_clicked(bool checked)
{
    if (checked)
    {
        ui->RBHigh->setChecked(false);
        ui->RBLow->setChecked(false);
    }
}

void MainWindow::on_RBLow_clicked(bool checked)
{
    if (checked)
    {
        ui->RBHigh->setChecked(false);
        ui->RBMedium->setChecked(false);
    }
}

void MainWindow::on_PBAdd_released()
{
    QString qstrText = ui->LEInput->text();
    if (qstrText.isEmpty()) return;
    TodoInfo stInfo;
    if (ui->RBHigh->isChecked()) stInfo.nPriority = TODO_HIGH;
    else if (ui->RBMedium->isChecked()) stInfo.nPriority = TODO_MEDIUM;
    else if (ui->RBLow->isChecked()) stInfo.nPriority = TODO_LOW;
    else stInfo.nPriority = TODO_NONE;
    stInfo.strDate = getCurrentDateTime().toStdString();
    stInfo.strText = qstrText.toStdString();
    m_vecInfo.push_back(stInfo);
    writeInfo();
    initTable(m_vecInfo);
    ui->LEInput->setText("");
}

void MainWindow::on_PBUpdate_released()
{
    TodoInfo stInfo;
    if (ui->RBHigh->isChecked()) stInfo.nPriority = TODO_HIGH;
    else if (ui->RBMedium->isChecked()) stInfo.nPriority = TODO_MEDIUM;
    else if (ui->RBLow->isChecked()) stInfo.nPriority = TODO_LOW;
    else stInfo.nPriority = TODO_NONE;
    stInfo.strDate = m_strDateTime;
    stInfo.strText = ui->LEInput->text().toStdString();

    QVector<TodoInfo>::iterator iter = m_vecInfo.begin();
    while(iter != m_vecInfo.end())
    {
        if (0 == iter->strDate.compare(m_strDateTime))
        {
            iter = m_vecInfo.erase(iter);
            continue;
        }
        iter++;
    }
    m_vecInfo.push_back(stInfo);
    writeInfo();
    initTable(m_vecInfo);
    ui->LEInput->setText("");
}

void MainWindow::on_PBAll_released()
{
    initTable(m_vecInfo);
}

void MainWindow::on_PBHigh_released()
{
    QVector<TodoInfo> vecInfo;
    for (int i = 0; i < m_vecInfo.size(); i++)
    {
        if (TODO_HIGH == m_vecInfo.at(i).nPriority)
        {
            vecInfo.push_back(m_vecInfo.at(i));
        }
    }
    initTable(vecInfo);
}

void MainWindow::on_PBMedium_released()
{
    QVector<TodoInfo> vecInfo;
    for (int i = 0; i < m_vecInfo.size(); i++)
    {
        if (TODO_MEDIUM == m_vecInfo.at(i).nPriority)
        {
            vecInfo.push_back(m_vecInfo.at(i));
        }
    }
    initTable(vecInfo);
}

void MainWindow::on_PBLow_released()
{
    QVector<TodoInfo> vecInfo;
    for (int i = 0; i < m_vecInfo.size(); i++)
    {
        if (TODO_LOW == m_vecInfo.at(i).nPriority)
        {
            vecInfo.push_back(m_vecInfo.at(i));
        }
    }
    initTable(vecInfo);
}

void MainWindow::on_Action_triggered()
{
    int nCurrentRow = ui->TWContent->currentRow();
    QString qstrDateTime = ui->TWContent->model()->index(nCurrentRow, 2).data().toString();
    std::string strDateTime = qstrDateTime.toStdString();
    QVector<TodoInfo>::iterator iter = m_vecInfo.begin();
    while(iter != m_vecInfo.end())
    {
        if (0 == iter->strDate.compare(strDateTime))
        {
            iter = m_vecInfo.erase(iter);
            continue;
        }
        iter++;
    }
    writeInfo();
    initTable(m_vecInfo);
}

void MainWindow::on_TWContent_cellDoubleClicked(int row, int column)
{
    QTableWidgetItem *item0 = ui->TWContent->item(row, 0);
    if(TODO_HIGH == item0->text().toInt()) ui->RBHigh->setChecked(true);
    else if(TODO_MEDIUM == item0->text().toInt()) ui->RBMedium->setChecked(true);
    else if(TODO_LOW == item0->text().toInt()) ui->RBLow->setChecked(true);
    QTableWidgetItem *item1 = ui->TWContent->item(row, 1);
    ui->LEInput->setText(item1->text());
    QTableWidgetItem *item2 = ui->TWContent->item(row, 2);
    m_strDateTime = item2->text().toStdString();
    if (!m_strDateTime.empty()) ui->PBUpdate->setEnabled(true);
}

void MainWindow::on_TWContent_customContextMenuRequested(const QPoint &pos)
{
    QMenu *pobjMenu = new QMenu(this);
    QAction *pobjAction = new QAction("delete", this);
    connect(pobjAction, &QAction::triggered, this, &MainWindow::on_Action_triggered);
    pobjMenu->addAction(pobjAction);
    pobjMenu->popup(ui->TWContent->viewport()->mapToGlobal(pos));
}
