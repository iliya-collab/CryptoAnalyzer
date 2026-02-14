#include "mainwindow.hpp"
#include "CustomWindowDialogs/DebugMonitor.hpp"

#include <QImage>
#include <QRect>
#include <QPainter>
#include <QPainterPath>

/*QImage cropToCircle(const QImage& original) {
    // Создаем новое изображение с прозрачным фоном
    QImage result(original.size(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);
    
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    // Создаем круглую область
    QPainterPath path;
    path.addEllipse(QRectF(0, 0, original.width(), original.height()));
    
    // Устанавливаем обрезку по кругу
    painter.setClipPath(path);
    
    // Рисуем оригинальное изображение
    painter.drawImage(0, 0, original);
    painter.end();
    
    return result;
}


QImage removeWhiteBackground(const QImage& originalImage, int tolerance = 10) {
    int width = originalImage.width();
    int height = originalImage.height();

    int minX = width;
    int minY = height;
    int maxX = 0;
    int maxY = 0;

    // Проходим по всем пикселям, чтобы найти границы объекта
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QColor color = originalImage.pixelColor(x, y);
            // Проверяем, является ли пиксель "белым" (с учетом допуска)
            if (color.red() < 255 - tolerance || color.green() < 255 - tolerance || color.blue() < 255 - tolerance) {
                if (x < minX) minX = x;
                if (y < minY) minY = y;
                if (x > maxX) maxX = x;
                if (y > maxY) maxY = y;
            }
        }
    }

    // Если объект найден, обрезаем изображение
    if (minX <= maxX && minY <= maxY) {
        QRect boundingRect(minX, minY, maxX - minX + 1, maxY - minY + 1);
        return originalImage.copy(boundingRect);
    }

    // Возвращаем оригинальное изображение, если объект не найден
    return originalImage;
}*/

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app(argc, argv);
    
    DebugMonitor debug;
    debug.show();

    MainWindow win;
    win.show();
    
    return app.exec();
}
