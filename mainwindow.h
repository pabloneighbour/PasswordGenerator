#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void generatePassword();
    void copyToClipboard();

private:
    QLineEdit *inputPassword;
    QSpinBox *passesInput;
    QSpinBox *lengthInput;
    QLabel *resultLabel;
    QPushButton *copyButton;
};

#endif // MAINWINDOW_H