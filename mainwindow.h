#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void set_chart();

private slots:
  // update curve with cosine
  void on_updateButton_clicked();

  // curve fitting test
  int on_fittingButton_clicked();

private:
  Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
