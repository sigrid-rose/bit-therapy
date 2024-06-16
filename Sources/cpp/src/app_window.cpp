#include "app_window.h"
#include "geometry.h"
#include "file_utils.h"
#include "sprite_set_builder.h"
#include "sprite_set_builder_impl.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>
#include <QScrollArea>

AppWindow::AppWindow() {
    setup();
}

void AppWindow::show() {
    window.move(100, 150);
    window.show();
}

void AppWindow::setup() {
    Vector2d vector1(3.5, 2.5);
    Vector2d vector2(1.5, 4.0);
    Vector2d sum = vector1 + vector2;
    
    auto paths = listFiles("/Users/curzel/dev/bit-therapy/PetsAssets", ".png");
    
    SpriteSetBuilderImpl builderImpl = SpriteSetBuilderImpl({});
    SpriteSetBuilder& builder = builderImpl;
    auto spriteSets = builder.spriteSets(paths);

    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *label = new QLabel(QString("Sum of vectors: (%1, %2)").arg(sum.x).arg(sum.y));
    layout->addWidget(label);

    QLabel *fileCountLabel = new QLabel(QString("Found %1 PNGs").arg(paths.size()));
    layout->addWidget(fileCountLabel);

    QLabel *spriteSetCountLabel = new QLabel(QString("Found %1 sprite sets").arg(spriteSets.size()));
    layout->addWidget(spriteSetCountLabel);

    QLabel *listLabel = new QLabel("List of PNG Files:");
    layout->addWidget(listLabel);

    QString filesText;
    for (const auto &path : paths) {
        filesText += QString::fromStdString(path) + "\n";
    }

    QLabel *filesLabel = new QLabel(filesText);
    filesLabel->setWordWrap(true);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(filesLabel);

    layout->addWidget(scrollArea);

    window.setLayout(layout);
    window.setWindowFlags(Qt::WindowStaysOnTopHint);
    window.setWindowTitle("Bit Therapy");
}
