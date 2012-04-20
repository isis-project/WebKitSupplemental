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

#ifndef phrasePickerWidget_h
#define phrasePickerWidget_h

#include <QGraphicsOpacityEffect>
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTime>

const int kPickerMaxWidth = 200; // FIXME: replace this constant with one from legacy code

class PhrasePickerWidget;

// class designed to be encapsulated by phrase picker items in order to handle common drag functionality
class PickerItem : public QObject {
    Q_OBJECT;

public:

    enum { PickerPhraseType = QGraphicsItem::UserType + 1, PickerGraphicType, PickerGraphicClassType};

    PickerItem() : m_clickInProgress(false), m_dragInProgress(false) { }

    void mousePress(QGraphicsSceneMouseEvent*);
    void mouseMove(QGraphicsSceneMouseEvent*);
    void mouseRelease(QGraphicsSceneMouseEvent*);

    // work-around for QTBUG-15477 here: https://bugreports.qt.nokia.com/browse/QTBUG-15477
    // please test mouse clicks without this function periodically, and remove this when possible
    bool scene(QEvent*);

signals:
    void dragStarted();
    void dragUpdate(qreal xDelta);
    void dragFinished();
    void itemClicked();

protected:
    bool m_clickInProgress;
    bool m_dragInProgress;
};

class PickerPhrase : public QGraphicsTextItem {
    Q_OBJECT;

public:
    PickerPhrase(const QString& text, PhrasePickerWidget* pickerParent = 0);

signals:
    void phraseClicked(QString phrase);

protected slots:
    void onItemClicked();

protected:

    // Qt overrides for mouse events - delegate to m_item
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) { m_item.mousePress(event); }
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) { m_item.mouseMove(event); }
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) { m_item.mouseRelease(event); }
    virtual bool sceneEvent(QEvent *event) { return m_item.scene(event); }

    // this is needed for qgraphicsitem_cast to work
    virtual int type() const { return PickerItem::PickerPhraseType; }


    PhrasePickerWidget* m_pickerParent;

    // encapsulated object to handle common drag functionality
    PickerItem m_item;
};

class PickerGraphic : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT;

public:
    PickerGraphic(const QPixmap& image, PhrasePickerWidget* pickerParent = 0);

protected:

    // Qt overrides for mouse events - delegate to m_item
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) { m_item.mousePress(event); }
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) { m_item.mouseMove(event); }
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) { m_item.mouseRelease(event); }
    virtual bool sceneEvent(QEvent *event) { return m_item.scene(event); }

    // this is needed for qgraphicsitem_cast to work
    virtual int type() const { return PickerItem::PickerGraphicClassType; }

    // NB: note this class currently ignores the itemClicked event from m_item because it's not necessary. This could easily be changed.

    PhrasePickerWidget* m_pickerParent;

    // encapsulated object to handle common drag functionality
    PickerItem m_item;
};

class PhrasePickerWidget : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT;

public:
    PhrasePickerWidget(QObject* parent = 0, int maxWidth = kPickerMaxWidth, QString pathToImageDir = QString(), QGraphicsItem* graphicParent = 0);

    // phrase list management
    void addPhrase(QString phrase);
    inline void addPhraseList(QStringList phraseList)  { foreach (QString newPhrase, phraseList) addPhrase(newPhrase); }
    void clearPhraseList();

    /*! show the phrase picker with its arrow pointed down to "pointAt" if possible, otherwise with its arrow pointed up.
      @param pointAt The coordinate in the graphics area where the arrow will point to
      @param drawableArea The rectangular area used to determine if the arrow needs to be drawn upwards or off-center
      @param areaHeight The vertical offset the phraseWidget needs to be moved to point upwards to the same area. Typically the text height of the word being pointed to.
    */
    void showAt(QPointF pointAt, const QRectF& drawableArea, int areaHeight = 0);

    inline bool isScrollable() const { return m_scrollable; }

signals:
    void phraseClicked(QString phrase); // emited when a particular phrase is picked

protected slots:
    void onPhraseClicked(QString phrase);
    void onPhraseDragStarted();
    void onPhraseDragUpdate(qreal xDelta);
    void onPhraseDragFinished();

protected:

    // this is needed for qgraphicsitem_cast to work
    virtual int type() const { return PickerItem::PickerGraphicType; }

    void updateDragVelocity(int xDelta);

    // handles fading in and out
    void timerEvent(QTimerEvent*);

    // describes the background of the widget, whether it has an arrow and which direction it points
    enum ArrowDirection { arrowDown, arrowUp };

    // re-render full phrase background to own pixmap, including properly sized background
    void renderPhrasesToImage(ArrowDirection, int pointerOffsetFromCenter, int maxImageWidth);

    int m_textWidth;
    int m_minBGWidth;
    int m_BGWidth;
    int m_maxWidth;
    bool m_scrollable;

    // mouse-drag related
    int m_scrollPosition;
    int m_lastDelta;
    int m_scrollTimerId;
    qreal m_velocity;

    // drag acceleration related
    QTime m_dragTimer;
    int m_deltaSampleSum;
    int m_totalTimeThisSample;

    // text render caching related
    bool m_hasRenderedWithCurrentPhrases;
    ArrowDirection m_lastArrowDir;
    int m_lastArrowOffset; // offset of last arrow image render


    // pixmaps for drawing phrase backgrounds
    QPixmap m_leftBG;
    QPixmap m_arrowUpBG;
    QPixmap m_arrowDownBG;
    QPixmap m_fillerBG;
    QPixmap m_rightBG;

    // foreground images dealing with text
    QPixmap m_dividerImage;
    QPixmap m_leftScrollFade;
    QPixmap m_rightScrollFade;
    QGraphicsPixmapItem* m_leftScrollFadeItem;
    QGraphicsPixmapItem* m_rightScrollFadeItem;

    // phrase strings that can be picked
    QList<QGraphicsItem*> m_phraseList;
    QGraphicsItemGroup* m_phraseGroup;

    // fade-in / out related variables
    int m_fadeInTimerId;
    int m_fadeOutTimerId;
    qreal m_opacity;
    QGraphicsOpacityEffect* m_fadeEffect;
};

#endif // phrasePickerWidget_h
