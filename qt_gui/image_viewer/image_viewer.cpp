#include <QtWidgets>

#include "image_viewer.hpp"

ImageViewer::ImageViewer()
   : imageLabel(new QLabel)
   , scrollArea(new QScrollArea)
   , scaleFactor(1)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
   // setCentralWidget(scrollArea);

    createActions();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}


bool ImageViewer::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);

    setWindowFilePath(fileName);

    return true;
}

void ImageViewer::setImage(const QImage &newImage)
{
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;

    scrollArea->setVisible(true);
    printAct->setEnabled(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

void ImageViewer::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void ImageViewer::createActions()
{
    //QMenu *fileMenu;// = menuBar()->addMenu(tr("&File"));

    //QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &ImageViewer::open);
    //openAct->setShortcut(QKeySequence::Open);

    //saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &ImageViewer::saveAs);
    //saveAsAct->setEnabled(false);

    //printAct = fileMenu->addAction(tr("&Print..."), this, &ImageViewer::print);
    //printAct->setShortcut(QKeySequence::Print);
    //printAct->setEnabled(false);

    //fileMenu->addSeparator();

    //QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    //exitAct->setShortcut(tr("Ctrl+Q"));

    //QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    //copyAct = editMenu->addAction(tr("&Copy"), this, &ImageViewer::copy);
    //copyAct->setShortcut(QKeySequence::Copy);
    //copyAct->setEnabled(false);

    //QAction *pasteAct = editMenu->addAction(tr("&Paste"), this, &ImageViewer::paste);
    //pasteAct->setShortcut(QKeySequence::Paste);

    //QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    //zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
    //zoomInAct->setShortcut(QKeySequence::ZoomIn);
    //zoomInAct->setEnabled(false);

    //zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
    //zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    //zoomOutAct->setEnabled(false);

    //normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
    //normalSizeAct->setShortcut(tr("Ctrl+S"));
    //normalSizeAct->setEnabled(false);

    //viewMenu->addSeparator();

    //fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &ImageViewer::fitToWindow);
    //fitToWindowAct->setEnabled(false);
    //fitToWindowAct->setCheckable(true);
    //fitToWindowAct->setShortcut(tr("Ctrl+F"));

    //QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    //helpMenu->addAction(tr("&About"), this, &ImageViewer::about);
    //helpMenu->addAction(tr("About &Qt"), &QApplication::aboutQt);
}

void ImageViewer::updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}