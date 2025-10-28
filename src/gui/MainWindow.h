#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include "../core/Controller.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectFile();
    void selectDatabase();
    void togglePassword();
    void lockFile();
    void unlockFile();

private:
    void setupUI();
    void updateStatus();

    QLineEdit *filePathEdit;
    QLineEdit *dbPathEdit;
    QLineEdit *passwordEdit;
    QPushButton *fileSelectButton;
    QPushButton *dbSelectButton;
    QPushButton *showPasswordButton;
    QPushButton *lockButton;
    QPushButton *unlockButton;
    QLabel *statusLabel;

    Controller controller;
    QString lastFilePath;
    QString lastDbPath;
};