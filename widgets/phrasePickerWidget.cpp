/* @@@LICENSE
*
*      Copyright (c) 2012 Hewlett-Packard Development Company, L.P.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
LICENSE@@@ */

#include "phrasePickerWidget.h"

#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>


const char kLeftBGFilename[]            = "ate-left.png";
const char kRightBGFilename[]           = "ate-right.png";
const char kFillerBGFilename[]          = "ate-middle.png";
const char kUpArrowBGFilename[]         = "ate-arrow-up.png";
const char kDownArrowBGFilename[]       = "ate-arrow-down.png";
const char kDividerFilename[]           = "ate-divider.png";
const char kLeftFadeScrollFilename[]    = "ate-left-scroll-fade.png";
const char kRightFadeScrollFilename[]   = "ate-right-scroll-fade.png";

// margin/padding of text within the background
const qreal kPhraseXBorder      = 5;
const qreal kPhraseYBorder      = 10;
const qreal kPhraseXSpacing     = 2;

// border allowances for the background edges which are not rendered with the bubble edge against the image edge
const int kLeftBorderAllowance  = 6;
const int kRightBorderAllowance = 8;
const int kTopBorderAllowance   = 4;
const int kArrowDownPadding     = 0;
const int kArrowUpPadding       = 7;

// durations and framerate
const int kDragTimeout          = 250; // milliseconds without drag updates before resetting drag velocity measurements
const int kAnimFrameRate        = 30; // fps
const int kFadeDuration         = 250; // in milliseconds
const qreal kOpacityIncrement   = qreal(kAnimFrameRate) / kFadeDuration;
const qreal kDragCoefficient    = .9;

void PickerItem::mousePress(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    m_clickInProgress = true;
    emit dragStarted(); // possible drag started, used for drag velocity computations
}

void PickerItem::mouseRelease(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    if (m_clickInProgress) {
        m_clickInProgress = false;
        emit itemClicked();
    } else if (m_dragInProgress) {
        m_dragInProgress = false;
        emit dragFinished();
    }
}

void PickerItem::mouseMove(QGraphicsSceneMouseEvent *event)
{
    // check if the user has started a drag using screen coordinates
    const QPoint currPosScreen(event->screenPos());
    const QPoint clickPosScreen(event->buttonDownScreenPos(Qt::LeftButton));

    if (m_clickInProgress && QLineF(currPosScreen, clickPosScreen).length() >= QApplication::startDragDistance()) {
        m_clickInProgress = false;
        m_dragInProgress = true;
    }

    if (m_dragInProgress) {
        // check the actual drag distance using *scene* coordinates
        const QPointF currPosScene(event->scenePos());
        const QPointF clickPosScene(event->buttonDownScenePos(Qt::LeftButton));
        emit dragUpdate(currPosScene.x() - clickPosScene.x());
    }
}

// FIXME: work-around for QTBUG-15477 here: https://bugreports.qt.nokia.com/browse/QTBUG-15477
// please test mouse events without this function periodically, and remove this when possible
bool PickerItem::scene(QEvent *event)
{
    bool accepted = false;
    if (event->type() == QEvent::GraphicsSceneMousePress) {
        mousePress(static_cast<QGraphicsSceneMouseEvent *>(event));
        accepted = true;
    } else if (event->type() == QEvent::GraphicsSceneMouseRelease) {
        mouseRelease(static_cast<QGraphicsSceneMouseEvent *>(event));
        accepted = true;
    } else if (event->type() == QEvent::GraphicsSceneMouseMove) {
        mouseMove(static_cast<QGraphicsSceneMouseEvent *>(event));
        accepted = true;
    }

    event->setAccepted(accepted);
    return true;
}

PickerPhrase::PickerPhrase(const QString& text, PhrasePickerWidget* pickerParent)
    : QGraphicsTextItem(text, pickerParent), m_pickerParent(pickerParent)
{
    // delegate drag handling to m_item
    connect(&m_item, SIGNAL(dragStarted()), m_pickerParent, SLOT(onPhraseDragStarted()));
    connect(&m_item, SIGNAL(dragUpdate(qreal)), m_pickerParent, SLOT(onPhraseDragUpdate(qreal)));
    connect(&m_item, SIGNAL(dragFinished()), m_pickerParent, SLOT(onPhraseDragFinished()));

    // FIXME: Use the text color from existing code
    setDefaultTextColor(Qt::white);

    // handle phrase clicking
    connect(&m_item, SIGNAL(itemClicked()), this, SLOT(onItemClicked()));
    connect(this, SIGNAL(phraseClicked(QString)), m_pickerParent, SLOT(onPhraseClicked(QString)));
}

void PickerPhrase::onItemClicked()
{
    emit phraseClicked(toPlainText());
}

PickerGraphic::PickerGraphic(const QPixmap& image, PhrasePickerWidget* pickerParent)
    : QGraphicsPixmapItem(image, pickerParent), m_pickerParent(pickerParent)
{
    // delegate drag handling to m_item
    connect(&m_item, SIGNAL(dragStarted()), m_pickerParent, SLOT(onPhraseDragStarted()));
    connect(&m_item, SIGNAL(dragUpdate(qreal)), m_pickerParent, SLOT(onPhraseDragUpdate(qreal)));
    connect(&m_item, SIGNAL(dragFinished()), m_pickerParent, SLOT(onPhraseDragFinished()));
}

PhrasePickerWidget::PhrasePickerWidget(QObject* parent, int maxWidth, QString pathToImageDir, QGraphicsItem* graphicParent)
    : QObject(parent), QGraphicsPixmapItem(graphicParent), m_textWidth(0), m_minBGWidth(0), m_BGWidth(0), m_maxWidth(maxWidth),
      m_scrollable(false), m_scrollPosition(0), m_lastDelta(0), m_scrollTimerId(0), m_velocity(0.f), m_deltaSampleSum(0), m_totalTimeThisSample(0),
      m_hasRenderedWithCurrentPhrases(false), m_lastArrowDir(arrowDown), m_lastArrowOffset(0), m_leftScrollFadeItem(0),
      m_rightScrollFadeItem(0), m_phraseGroup(0), m_fadeInTimerId(0), m_fadeOutTimerId(0), m_opacity(0), m_fadeEffect(0)
{
    bool succeeded = m_leftBG.load(pathToImageDir + QString::fromUtf8(kLeftBGFilename));
    succeeded = succeeded && m_fillerBG.load(pathToImageDir + QString::fromUtf8(kFillerBGFilename));
    succeeded = succeeded && m_rightBG.load(pathToImageDir + QString::fromUtf8(kRightBGFilename));
    succeeded = succeeded && m_arrowUpBG.load(pathToImageDir + QString::fromUtf8(kUpArrowBGFilename));
    succeeded = succeeded && m_arrowDownBG.load(pathToImageDir + QString::fromUtf8(kDownArrowBGFilename));
    succeeded = succeeded && m_dividerImage.load(pathToImageDir + QString::fromUtf8(kDividerFilename));

    QPixmap leftScrollFade, rightScrollFade;
    succeeded = succeeded && leftScrollFade.load(pathToImageDir + QString::fromUtf8(kLeftFadeScrollFilename));
    succeeded = succeeded && rightScrollFade.load(pathToImageDir + QString::fromUtf8(kRightFadeScrollFilename));

    if (succeeded) {
        m_leftScrollFadeItem = new QGraphicsPixmapItem(leftScrollFade, this, scene());
        m_leftScrollFadeItem->setZValue(1);
        m_leftScrollFadeItem->setEnabled(false);
        m_leftScrollFadeItem->setVisible(false);
        m_rightScrollFadeItem = new QGraphicsPixmapItem(rightScrollFade, this, scene());
        m_rightScrollFadeItem->setZValue(1);
        m_rightScrollFadeItem->setEnabled(false);
        m_rightScrollFadeItem->setVisible(false);

        m_leftScrollFadeItem->setPos(0, 0);
    }

    Q_ASSERT_X(succeeded, "PhrasePickerWidet constructor", "image load failed. Verify that pathToImageDir is correct.");
    Q_ASSERT(m_arrowDownBG.width() == m_arrowUpBG.width() && m_arrowDownBG.height() == m_arrowUpBG.height());

    if (succeeded) {
        m_minBGWidth =  m_leftBG.width() + m_arrowDownBG.width() + m_rightBG.width();
        succeeded = m_maxWidth >= m_minBGWidth;
        Q_ASSERT_X(succeeded, "PhrasePickerWidet constructor", "maxWidth specified is below the minimum width indicated by the background images");
    }

    if (succeeded) {
        m_fadeEffect = new QGraphicsOpacityEffect(this);
        QGraphicsItem::setVisible(false);
        m_opacity = 0;
        m_fadeEffect->setOpacity(m_opacity);
        setGraphicsEffect(m_fadeEffect);


        // clip child text elements to background
        setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    }
}

void PhrasePickerWidget::addPhrase(QString phrase)
{
    if (m_phraseList.size()) { // if there are existing phrases in the list, prepend a divider element.
        PickerGraphic* dividerItem = new PickerGraphic(m_dividerImage, this);
        m_phraseList << dividerItem;
    }

    PickerPhrase* newPhrase = new PickerPhrase(phrase, this);
    m_phraseList << newPhrase;
    m_hasRenderedWithCurrentPhrases = false;
}

void PhrasePickerWidget::clearPhraseList()
{
    QGraphicsItem::setEnabled(false);
    setVisible(false);
    setPixmap(QPixmap());

    // kill all the timers
    int* timers[] = {&m_scrollTimerId, &m_fadeInTimerId, &m_fadeOutTimerId};
    for (unsigned int timerIndex = 0; timerIndex < sizeof(timers) / sizeof(int*); ++timerIndex) {
        int* currTimer = timers[timerIndex];
        if (*currTimer != (int)0) {
            killTimer(*currTimer);
            *currTimer = 0;
        }
    }

    if (m_phraseGroup) {
        m_phraseGroup->scene()->destroyItemGroup(m_phraseGroup);
        m_phraseGroup = 0;
    }

    foreach (QGraphicsItem* item, m_phraseList) {
        // remove phrase items from the scene and destroy them
        scene()->removeItem(item);

        if (item->type() == PickerItem::PickerGraphicType || item->type() == PickerItem::PickerGraphicClassType) {
            QObject* actualItem = reinterpret_cast<QObject*>(item);
            actualItem->deleteLater();
        }
    }
    m_phraseList.clear();

    m_hasRenderedWithCurrentPhrases = m_scrollable = false;
    m_opacity = m_textWidth = m_scrollPosition = m_lastDelta = m_deltaSampleSum = m_totalTimeThisSample = m_lastArrowOffset = 0;
    m_fadeEffect->setOpacity(m_opacity);
}

void PhrasePickerWidget::showAt(QPointF pointAt, const QRectF& drawableArea, int areaHeight)
{
    if (!m_phraseList.isEmpty()) {
        QGraphicsItem::setVisible(true);
        // if the phrase picker is still fading out somewhere else, kill that animation and start from 0
        if (m_fadeOutTimerId)
            killTimer(m_fadeOutTimerId);
        else if (m_fadeInTimerId)
            killTimer(m_fadeInTimerId);

        if (m_opacity) { // always start over at opacity 0
            m_opacity = 0;
            m_fadeEffect->setOpacity(m_opacity);
        }

        // only draw the picker if the point is within the drawable area
        if (drawableArea.contains(pointAt)) {
            m_scrollPosition = 0;
            m_lastDelta = 0;

            // determine whether there's enough room to render the phrase picker above pointAt
            ArrowDirection pointDirection = arrowDown;
            if (pointAt.y() - drawableArea.top() < m_arrowDownBG.height() - kTopBorderAllowance) {
                pointDirection = arrowUp;
                pointAt.ry() += areaHeight +  m_arrowDownBG.height();
            }

            // determine the widest allowable render space
            int maxRenderWidth = drawableArea.width();
            if (maxRenderWidth > m_maxWidth)
                maxRenderWidth = m_maxWidth;

            QGraphicsScene* currScene = scene();
            Q_ASSERT_X(currScene, "PhrasePickerWidget::renderPhrasesToImage", "PhrasePickerWidget must be added to the scene before phrases are added to it.");

            // if the text hasn't been shown before, place it properly record its size
            if (currScene && !m_hasRenderedWithCurrentPhrases) {
                // remove any existing group from prior render
                if (m_phraseGroup) {
                    currScene->destroyItemGroup(m_phraseGroup);
                    m_phraseGroup = 0;
                }

                // place the text items correctly
                if (m_phraseList.size()) {
                    // create the group first and then position children within the group
                    m_phraseGroup = currScene->createItemGroup(m_phraseList);
                    m_phraseGroup->setParentItem(this);

                    m_textWidth = kPhraseXBorder;
                    foreach (QGraphicsItem* currPhrase, m_phraseList) {
                        currPhrase->setX(m_textWidth);
                        currPhrase->setY(currPhrase->type() == PickerItem::PickerPhraseType ? kPhraseYBorder : kTopBorderAllowance);
                        m_textWidth += currPhrase->boundingRect().width() + kPhraseXSpacing;
                    }
                    m_textWidth += (kPhraseXBorder - kPhraseXSpacing); // replace the final spacing width with the proper border width
                } else {
                    m_textWidth = 0;
                    m_scrollable = false;
                }
                m_scrollable = (m_textWidth > maxRenderWidth); // check if the picker needs to be scrollable
            }
            m_phraseGroup->setX(0);
            m_phraseGroup->setY(pointDirection == arrowDown ? kArrowDownPadding : kArrowUpPadding);

            int arrowOffset = 0;
            const int halfRenderWidth = (m_textWidth < maxRenderWidth ? m_textWidth : maxRenderWidth) / 2;
            const int leftEdge = pointAt.x() - halfRenderWidth;
            const int rightEdge = pointAt.x() + halfRenderWidth;

            // determine if the arrow has to be drawn off-center due to the constraints of the drawableArea
            if (leftEdge < drawableArea.left())
                arrowOffset = leftEdge - drawableArea.left() + kLeftBorderAllowance;
            else if (rightEdge > drawableArea.right())
                arrowOffset = rightEdge - drawableArea.right() - kRightBorderAllowance;

            Q_ASSERT(!(leftEdge < drawableArea.left() && rightEdge > drawableArea.right())); // this shouldn't happen, the width should've been constrained above.

            // re-render background if necessary
            if (!m_hasRenderedWithCurrentPhrases || !(m_lastArrowDir == pointDirection && m_lastArrowOffset == arrowOffset))
                renderPhrasesToImage(pointDirection, arrowOffset, maxRenderWidth);

            // disable left-scroll area, as we are rendering from the left origin
            m_leftScrollFadeItem->setVisible(false);
            m_leftScrollFadeItem->setEnabled(false);

            // draw right-scroll fade if there are phrases past the boundary
            if (m_scrollable) {
                m_rightScrollFadeItem->setVisible(true);
                m_rightScrollFadeItem->setEnabled(true);
            }

            // set the picker's position relative to what it's pointing to
            const int topEdge = pointAt.y() - m_arrowDownBG.height();
            setPos(leftEdge - arrowOffset, (pointDirection == arrowUp ? topEdge + areaHeight : topEdge));

            // start the fade-in
            QGraphicsItem::setEnabled(true);
            m_fadeInTimerId = startTimer(1000 / kAnimFrameRate);
        }
    }
}

void PhrasePickerWidget::onPhraseClicked(QString phrase)
{
    QGraphicsItem::setEnabled(false);

    // if we're still fading in, stop that animation
    if (m_fadeInTimerId) {
        killTimer(m_fadeInTimerId);
        m_fadeInTimerId = 0;
    }

    Q_ASSERT(!m_fadeOutTimerId);
    m_fadeOutTimerId = startTimer(1000 / kAnimFrameRate);
    emit phraseClicked(phrase);
}

void PhrasePickerWidget::onPhraseDragStarted()
{
    if (m_scrollTimerId) { // kill any current velocity when a new drag is started
        killTimer(m_scrollTimerId);
        m_scrollTimerId = 0;
    }
    m_dragTimer.start();
}

void PhrasePickerWidget::onPhraseDragUpdate(qreal xDelta)
{
    // constrain the scrolling to the text edges
    const int calculatedX = xDelta + m_scrollPosition;
    const int rightEdge = m_BGWidth - m_textWidth;

    if (m_scrollable) {
        const bool scrolledFullRight = calculatedX >= 0;
        const bool scrolledFullLeft = calculatedX <= rightEdge;

        if (scrolledFullRight)
            xDelta = -m_scrollPosition;
        else if (scrolledFullLeft)
            xDelta = rightEdge - m_scrollPosition;

        if (m_scrollTimerId && (scrolledFullLeft || scrolledFullRight)) { // kill any current velocity when an edge is hit
            killTimer(m_scrollTimerId);
            m_scrollTimerId = 0;
        }

        // enable / disable scroll fades
        m_leftScrollFadeItem->setVisible(!scrolledFullRight);
        m_leftScrollFadeItem->setEnabled(!scrolledFullRight);
        m_rightScrollFadeItem->setVisible(!scrolledFullLeft);
        m_rightScrollFadeItem->setEnabled(!scrolledFullLeft);

        // set the current position
        m_phraseGroup->setX(m_scrollPosition + xDelta);

        // track the velocity of the drag in progress
        updateDragVelocity(xDelta);

        m_lastDelta = xDelta;
    }
}

void PhrasePickerWidget::onPhraseDragFinished()
{
    m_scrollPosition += m_lastDelta;

    // do one last update to make sure the user hasn't waited ages before releasing
    updateDragVelocity(m_lastDelta);

    if (m_totalTimeThisSample) {
        if (m_scrollTimerId) {
            killTimer(m_scrollTimerId);
            m_scrollTimerId = 0;
        }

        m_velocity = m_deltaSampleSum / (qreal(m_totalTimeThisSample) / 1000);
        m_scrollTimerId = startTimer(1000 / kAnimFrameRate);
    }

    m_lastDelta = m_deltaSampleSum = m_totalTimeThisSample = 0;
}

void PhrasePickerWidget::updateDragVelocity(int xDelta)
{
    int msElapsed = m_dragTimer.restart();
    if (msElapsed > kDragTimeout) { // reset the sampling if the user has paused too long between drag events
        m_deltaSampleSum = 0;
        m_totalTimeThisSample = 0;
    } else if ((xDelta > m_lastDelta && m_deltaSampleSum < 0) || (xDelta < m_lastDelta && m_deltaSampleSum > 0)) {
        // reset the sampling if the user has changed direction from the current motion
        m_deltaSampleSum = 0;
        m_totalTimeThisSample = 0;
    } else {
        m_deltaSampleSum += xDelta - m_lastDelta;
        m_totalTimeThisSample += msElapsed;
    }
}

void PhrasePickerWidget::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == m_fadeInTimerId) {
        m_opacity += kOpacityIncrement;
        if (m_opacity > 1) {
            m_opacity = 1;
            killTimer(m_fadeInTimerId);
            m_fadeInTimerId = 0;
        }
        m_fadeEffect->setOpacity(m_opacity);

    } else if (event->timerId() == m_fadeOutTimerId) {
        m_opacity -= kOpacityIncrement;
        if (m_opacity < 0) {
            m_opacity = 0;
            killTimer(m_fadeOutTimerId);
            m_fadeOutTimerId = 0;
            QGraphicsItem::setVisible(false);
        }

        m_fadeEffect->setOpacity(m_opacity);
    } else if (event->timerId() == m_scrollTimerId) {
        qreal deltaX = m_velocity / kAnimFrameRate;
        onPhraseDragUpdate(deltaX);
        m_scrollPosition += m_lastDelta;
        m_velocity *= kDragCoefficient;

        if (deltaX > -1 && deltaX < 1) {
            m_velocity = 0;
            killTimer(m_scrollTimerId);
            m_scrollTimerId = 0;
        }
    } else
        Q_ASSERT(0 && "Timer lost in PhrasePickerWidget::timerEvent");
}

void PhrasePickerWidget::renderPhrasesToImage(ArrowDirection direction, int arrowOffsetFromCenter, int maxImageWidth)
{
    // determine the amount of filler (if any) needs to be rendered into the background
    QPixmap& middleBG = (direction == arrowDown ? m_arrowDownBG : m_arrowUpBG);
    const int kImageHeight =  middleBG.height();
    int halfFillerWidth = 0;

    if (m_textWidth > m_minBGWidth)
        halfFillerWidth = ((maxImageWidth <  m_textWidth ? maxImageWidth : m_textWidth) - m_minBGWidth) / 2;
    else if (m_textWidth < m_minBGWidth) {
        // text width is less than the minimum background width -> center text to background
        m_phraseGroup->setX((m_minBGWidth - m_textWidth) / 2);
    }

    m_BGWidth = m_minBGWidth + 2 * halfFillerWidth;
    QPixmap drawnImage(m_BGWidth, kImageHeight);
    drawnImage.fill(QColor(0, 0, 0, 0));

    const int yPos = (direction ==  arrowDown ? kArrowDownPadding : kArrowUpPadding);

    QPainter painter(&drawnImage);
    painter.drawPixmap(0, yPos, m_leftBG);

    int xPos = m_leftBG.width();

    // make sure the arrow offset does not exceed the edges of the bubble outline
    if (arrowOffsetFromCenter + halfFillerWidth < 0)
        arrowOffsetFromCenter = -halfFillerWidth;
    else if (halfFillerWidth - arrowOffsetFromCenter < 0)
        arrowOffsetFromCenter = halfFillerWidth;

    if (halfFillerWidth) {
        const int leftFillerWidth = halfFillerWidth + arrowOffsetFromCenter;
        if (leftFillerWidth) {
            QRect targetArea(xPos, yPos, leftFillerWidth, kImageHeight);
            painter.drawPixmap(targetArea, m_fillerBG, m_fillerBG.rect());
            xPos += leftFillerWidth;
        }
    }

    // draw the arrow
    painter.drawPixmap(xPos, 0, middleBG);
    xPos += middleBG.width();

    if (halfFillerWidth) {
        const int rightFillerWidth = halfFillerWidth - arrowOffsetFromCenter;
        QRect targetArea(xPos, yPos, rightFillerWidth, kImageHeight);
        painter.drawPixmap(targetArea, m_fillerBG, m_fillerBG.rect());
        xPos += rightFillerWidth;
    }

    // draw and set the background pixmap
    painter.drawPixmap(xPos, yPos, m_rightBG);
    setPixmap(drawnImage);

    m_hasRenderedWithCurrentPhrases = true;
    m_lastArrowDir = direction;
    m_lastArrowOffset = arrowOffsetFromCenter;

    // place fade images if we're scrollable
    if (m_scrollable) {
        m_rightScrollFadeItem->setPos(boundingRect().width() - m_rightScrollFadeItem->boundingRect().width(), yPos);
        m_leftScrollFadeItem->setPos(0, yPos);
    }
}
