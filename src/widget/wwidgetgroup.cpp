#include "widget/wwidgetgroup.h"

#include <QLayout>
#include <QPainter>

#include "widget/wwidget.h"
#include "widget/wpixmapstore.h"
#include "xmlparse.h"

WWidgetGroup::WWidgetGroup(QWidget* pParent)
        : QGroupBox(pParent),
          m_pPixmapBack(NULL) {
    setObjectName("WidgetGroup");
}

WWidgetGroup::~WWidgetGroup() {
}

void WWidgetGroup::setLayoutSpacing(int spacing) {
    //qDebug() << "WWidgetGroup::setSpacing" << spacing;
    if (spacing < 0) {
        qDebug() << "WWidgetGroup: Invalid spacing:" << spacing;
        return;
    }
    QLayout* pLayout = layout();
    if (pLayout) {
        pLayout->setSpacing(spacing);
    }
}

void WWidgetGroup::setLayoutContentsMargins(QRect rectMargins) {
    // qDebug() << "WWidgetGroup::setLayoutContentsMargins" << rectMargins.x()
    //          << rectMargins.y() << rectMargins.width() << rectMargins.height();

    if (rectMargins.x() < 0 || rectMargins.y() < 0 ||
            rectMargins.width() < 0 || rectMargins.height() < 0) {
        qDebug() << "WWidgetGroup: Invalid ContentsMargins rectangle:"
                 << rectMargins;
        return;
    }

    setContentsMargins(rectMargins.x(), rectMargins.y(),
                       rectMargins.width(), rectMargins.height());
    QLayout* pLayout = layout();
    if (pLayout) {
        pLayout->setContentsMargins(rectMargins.x(), rectMargins.y(),
                                    rectMargins.width(), rectMargins.height());
    }
}

void WWidgetGroup::setup(QDomNode node) {
    setContentsMargins(0, 0, 0, 0);

    // Set background pixmap if available
    if (!WWidget::selectNode(node, "BackPath").isNull()) {
        setPixmapBackground(WWidget::getPath(WWidget::selectNodeQString(node, "BackPath")));
    }

    QLayout* pLayout = NULL;
    if (!XmlParse::selectNode(node, "Layout").isNull()) {
        QString layout = XmlParse::selectNodeQString(node, "Layout");
        if (layout == "vertical") {
            pLayout = new QVBoxLayout();
            pLayout->setSpacing(0);
            pLayout->setContentsMargins(0, 0, 0, 0);
            pLayout->setAlignment(Qt::AlignCenter);
        } else if (layout == "horizontal") {
            pLayout = new QHBoxLayout();
            pLayout->setSpacing(0);
            pLayout->setContentsMargins(0, 0, 0, 0);
            pLayout->setAlignment(Qt::AlignCenter);
        }
    }

    if (pLayout) {
        setLayout(pLayout);
    }
}

void WWidgetGroup::setPixmapBackground(const QString &filename) {
    // Load background pixmap
    m_pPixmapBack = WPixmapStore::getPixmap(filename);
    if (!m_pPixmapBack) {
        qDebug() << "WWidgetGroup: Error loading background pixmap:" << filename;
    }
}

void WWidgetGroup::addWidget(QWidget* pChild) {
    QLayout* pLayout = layout();
    if (pLayout && pChild) {
        pLayout->addWidget(pChild);
    }
}

void WWidgetGroup::paintEvent(QPaintEvent* pe) {
    if(m_pPixmapBack) {
        if (m_pixmapBackScaled.isNull()) {
            m_pixmapBackScaled = m_pPixmapBack->scaled(size());
        }
        QPainter p(this);
        p.drawPixmap(0, 0, m_pixmapBackScaled);
    }
    // Paint things styled by style sheet
    QGroupBox::paintEvent(pe);
}

void WWidgetGroup::resizeEvent(QResizeEvent* re) {
    m_pixmapBackScaled = QPixmap();
    // Paint things styled by style sheet
    QGroupBox::resizeEvent(re);
}
