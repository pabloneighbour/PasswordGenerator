#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QClipboard>
#include <QApplication>
#include <openssl/evp.h>
#include <QMessageBox>

const std::string ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int ALPHABET_SIZE = ALPHABET.length();

std::string sha256(const std::string &input) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("No se pudo crear el contexto EVP");
    }

    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, input.c_str(), input.length());
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    // Devolver el hash como una cadena de bytes crudos
    return std::string(reinterpret_cast<char*>(hash), hash_len);
}

std::string generatePassword(const std::string &input, int passes, int length) {
    std::string current = input;
    for (int i = 0; i < passes; i++) {
        current = sha256(current);
    }

    std::string password;
    size_t pos = 0;
    while (password.length() < static_cast<size_t>(length)) {
        if (pos >= current.length()) {
            current = sha256(current); // Generar más bytes si es necesario
            pos = 0;
        }
        unsigned char c = current[pos++]; // Usar bytes crudos
        int index = (c % ALPHABET_SIZE + ALPHABET_SIZE) % ALPHABET_SIZE;
        password += ALPHABET[index];
    }

    return password.substr(0, length);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Generador de Contraseñas");
    resize(450, 350);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QString styleSheet = R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QLabel {
            font-size: 14px;
            color: #333;
        }
        QLineEdit, QSpinBox {
            padding: 8px;
            font-size: 14px;
            border: 1px solid #ccc;
            border-radius: 5px;
            background-color: #fff;
            color: #000;
        }
        QLineEdit:focus, QSpinBox:focus {
            border-color: #007bff;
            box-shadow: 0 0 5px rgba(0,123,255,0.5);
        }
        QPushButton {
            padding: 10px;
            font-size: 14px;
            border-radius: 5px;
            background-color: #007bff;
            color: white;
            border: none;
        }
        QPushButton:hover {
            background-color: #0056b3;
        }
        QPushButton#copyButton {
            background-color: #28a745;
        }
        QPushButton#copyButton:hover {
            background-color: #218838;
        }
        QLabel#resultLabel {
            background-color: #e9ecef;
            padding: 10px;
            border-radius: 5px;
            font-family: monospace;
            font-size: 16px;
            color: #212529;
        }
    )";
    setStyleSheet(styleSheet);

    QLabel *inputLabel = new QLabel("Contraseña base:", this);
    inputPassword = new QLineEdit(this);
    layout->addWidget(inputLabel);
    layout->addWidget(inputPassword);

    QLabel *passesLabel = new QLabel("Número de pasadas (1-100):", this);
    passesInput = new QSpinBox(this);
    passesInput->setRange(1, 100);
    layout->addWidget(passesLabel);
    layout->addWidget(passesInput);

    QLabel *lengthLabel = new QLabel("Longitud de la contraseña (1-100):", this);
    lengthInput = new QSpinBox(this);
    lengthInput->setRange(1, 100);
    lengthInput->setValue(20);
    layout->addWidget(lengthLabel);
    layout->addWidget(lengthInput);

    QPushButton *generateButton = new QPushButton("Generar Contraseña", this);
    layout->addWidget(generateButton);

    QHBoxLayout *resultLayout = new QHBoxLayout();
    QLabel *resultTitle = new QLabel("Contraseña generada:", this);
    resultLabel = new QLabel("", this);
    resultLabel->setObjectName("resultLabel");
    resultLabel->setWordWrap(true);
    copyButton = new QPushButton("Copiar", this);
    copyButton->setObjectName("copyButton");
    copyButton->setFixedWidth(100);
    resultLayout->addWidget(resultLabel);
    resultLayout->addWidget(copyButton);
    layout->addWidget(resultTitle);
    layout->addLayout(resultLayout);

    layout->addStretch();

    connect(generateButton, &QPushButton::clicked, this, &MainWindow::generatePassword);
    connect(copyButton, &QPushButton::clicked, this, &MainWindow::copyToClipboard);
}

void MainWindow::generatePassword() {
    std::string input = inputPassword->text().toStdString();
    int passes = passesInput->value();
    int length = lengthInput->value();

    if (input.empty()) {
        QMessageBox::warning(this, "Error", "Por favor, ingrese una contraseña base.");
        resultLabel->setText("");
        return;
    }

    try {
        std::string password = ::generatePassword(input, passes, length);
        resultLabel->setText(QString::fromStdString(password));
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error", "No se pudo generar la contraseña: " + QString(e.what()));
    }
}

void MainWindow::copyToClipboard() {
    QString password = resultLabel->text();
    if (!password.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(password);
        QMessageBox::information(this, "Éxito", "Contraseña copiada al portapapeles.");
    } else {
        QMessageBox::warning(this, "Error", "No hay contraseña para copiar.");
    }
}