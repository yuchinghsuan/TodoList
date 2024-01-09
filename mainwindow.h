#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

enum TODO_PRIORITY_E
{
    TODO_NONE, TODO_HIGH, TODO_MEDIUM, TODO_LOW
};

typedef struct TODO_INFO_S
{
    int nPriority;  // TODO_PRIORITY_E
    std::string strDate;
    std::string strText;
} TodoInfo, *IPTodoInfo;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void refresh();
    void readInfo();
    void writeInfo();
    void initTable(QVector<TodoInfo> &p_vecInfo);
    QString getCurrentDateTime();

private slots:
    void on_RBHigh_clicked(bool checked);
    void on_RBMedium_clicked(bool checked);
    void on_RBLow_clicked(bool checked);
    void on_PBAdd_released();
    void on_PBUpdate_released();
    void on_PBAll_released();
    void on_PBHigh_released();
    void on_PBMedium_released();
    void on_PBLow_released();
    void on_Action_triggered();
    void on_TWContent_cellDoubleClicked(int row, int column);
    void on_TWContent_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
    std::string m_strDateTime;
    QVector<TodoInfo> m_vecInfo;
};
#endif // MAINWINDOW_H
