#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QIcon>
#include <QStyle>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setWindowTitle("Password File Manager");
    setMinimumSize(600, 200);

    // Load last used paths
    QSettings settings("PasswordFileManager", "GUI");
    lastFilePath = settings.value("lastFilePath").toString();
    lastDbPath = settings.value("lastDbPath").toString();
    if (!lastFilePath.isEmpty()) {
        filePathEdit->setText(lastFilePath);
    }
    if (!lastDbPath.isEmpty()) {
        dbPathEdit->setText(lastDbPath);
    }
}

MainWindow::~MainWindow()
{
    // Save last used paths
    QSettings settings("PasswordFileManager", "GUI");
    settings.setValue("lastFilePath", lastFilePath);
    settings.setValue("lastDbPath", lastDbPath);
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QGridLayout *layout = new QGridLayout(centralWidget);

    // File selection
    QLabel *fileLabel = new QLabel("File:", this);
    filePathEdit = new QLineEdit(this);
    filePathEdit->setReadOnly(true);
    fileSelectButton = new QPushButton("Browse...", this);

    // Database selection
    QLabel *dbLabel = new QLabel("Database:", this);
    dbPathEdit = new QLineEdit(this);
    dbPathEdit->setReadOnly(true);
    dbSelectButton = new QPushButton("Browse...", this);

    // Password
    QLabel *passwordLabel = new QLabel("Password:", this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    showPasswordButton = new QPushButton("Show", this);

    // Action buttons
    lockButton = new QPushButton("Lock", this);
    unlockButton = new QPushButton("Unlock", this);

    // Status
    statusLabel = new QLabel(this);
    statusLabel->setStyleSheet("QLabel { color: gray; }");

    // Layout
    layout->addWidget(fileLabel, 0, 0);
    layout->addWidget(filePathEdit, 0, 1);
    layout->addWidget(fileSelectButton, 0, 2);

    layout->addWidget(dbLabel, 1, 0);
    layout->addWidget(dbPathEdit, 1, 1);
    layout->addWidget(dbSelectButton, 1, 2);

    layout->addWidget(passwordLabel, 2, 0);
    layout->addWidget(passwordEdit, 2, 1);
    layout->addWidget(showPasswordButton, 2, 2);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(lockButton);
    buttonLayout->addWidget(unlockButton);
    layout->addLayout(buttonLayout, 3, 1);

    layout->addWidget(statusLabel, 4, 0, 1, 3);

    // Connect signals
    connect(fileSelectButton, &QPushButton::clicked, this, &MainWindow::selectFile);
    connect(dbSelectButton, &QPushButton::clicked, this, &MainWindow::selectDatabase);
    connect(showPasswordButton, &QPushButton::clicked, this, &MainWindow::togglePassword);
    connect(lockButton, &QPushButton::clicked, this, &MainWindow::lockFile);
    connect(unlockButton, &QPushButton::clicked, this, &MainWindow::unlockFile);

    // Initial status
    updateStatus();
}

void MainWindow::selectFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select File");
    if (!filePath.isEmpty()) {
        filePathEdit->setText(filePath);
        lastFilePath = filePath;
        updateStatus();
    }
}

void MainWindow::selectDatabase()
{
    QString dbPath = QFileDialog::getSaveFileName(this, "Select Database File",
                                                QString(), "SQLite Database (*.db)");
    if (!dbPath.isEmpty()) {
        dbPathEdit->setText(dbPath);
        lastDbPath = dbPath;
    }
}

void MainWindow::togglePassword()
{
    if (passwordEdit->echoMode() == QLineEdit::Password) {
        passwordEdit->setEchoMode(QLineEdit::Normal);
        showPasswordButton->setText("Hide");
    } else {
        passwordEdit->setEchoMode(QLineEdit::Password);
        showPasswordButton->setText("Show");
    }
}

void MainWindow::lockFile()
{
    QString filePath = filePathEdit->text();
    QString dbPath = dbPathEdit->text();
    QString password = passwordEdit->text();

    if (filePath.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file and enter a password");
        return;
    }

    if (dbPath.isEmpty()) {
        dbPath = ":memory:";
    }

    try {
        int result = controller.run("lock", filePath.toStdString(), 
                                  password.toStdString(), dbPath.toStdString());
        if (result == 0) {
            QMessageBox::information(this, "Success", "File locked successfully");
            updateStatus();
        } else {
            QMessageBox::critical(this, "Error", "Failed to lock file");
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Error: %1").arg(e.what()));
    }
}

void MainWindow::unlockFile()
{
    QString filePath = filePathEdit->text();
    QString dbPath = dbPathEdit->text();
    QString password = passwordEdit->text();

    if (filePath.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file and enter a password");
        return;
    }

    if (dbPath.isEmpty()) {
        dbPath = ":memory:";
    }

    try {
        int result = controller.run("unlock", filePath.toStdString(), 
                                  password.toStdString(), dbPath.toStdString());
        if (result == 0) {
            QMessageBox::information(this, "Success", "File unlocked successfully");
            updateStatus();
        } else {
            QMessageBox::critical(this, "Error", "Failed to unlock file");
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Error: %1").arg(e.what()));
    }
}

void MainWindow::updateStatus()
{
    QString filePath = filePathEdit->text();
    if (filePath.isEmpty()) {
        statusLabel->setText("Status: No file selected");
        return;
    }

    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        statusLabel->setText("Status: File is unlocked");
        lockButton->setEnabled(true);
        unlockButton->setEnabled(false);
    } else {
        statusLabel->setText("Status: File is locked");
        lockButton->setEnabled(false);
        unlockButton->setEnabled(true);
    }
}