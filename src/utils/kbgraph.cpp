/*
 * Copyright (c) 2021-... Thomas Berthelot -- <thomas.berthelot@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "kbgraph.h"
#include <QDir>
#include <QTextStream>
#include <QPainter>
#include <QRegularExpression>
#include "math.h"

/*
 * kbGraph : main class to manage the graph of notes
*/
kbGraph::kbGraph() {
    QGraphicsScene();
}

void kbGraph::GenerateKBGraph(const QString noteFolder) {
    QDir dir = QDir(noteFolder);

    QStringList noteFiles = dir.entryList(QStringList() << "*.md" << "*.Md" << "*.MD", QDir::Files);
    foreach(QString filename, noteFiles) {
        QString fullFileName = noteFolder + "/" + filename;
        QFile noteFile(fullFileName);
        if (!noteFile.open(QIODevice::ReadOnly)) // | QIODevice::Text))
            return;

        kbGraphNode* node = new kbGraphNode(filename.left(filename.length() - 3));
        _noteNodes << node;
        addItem(node);
        noteFile.close();
    }

    foreach(kbGraphNode* node, _noteNodes) {
        QString fullFileName = noteFolder + "/" + node->name() + ".md";
        QFile noteFile(fullFileName);
        if (!noteFile.open(QIODevice::ReadOnly)) // | QIODevice::Text))
            return;

        QTextStream flux(&noteFile);
        QString fluxText = flux.readAll();

        QRegularExpression re = QRegularExpression(R"(\[([A-Za-zÀ-ÖØ-öø-ÿ_\s]*)\]\([AA-Za-zÀ-ÖØ-öø-ÿ_\s\d?%]*\.md\))");
        QRegularExpressionMatchIterator reIterator = re.globalMatch(fluxText);
        while (reIterator.hasNext()) {
            QRegularExpressionMatch reMatch = reIterator.next();
            QString targetNoteName = reMatch.captured(1);

            for (int i = 0; i < _noteNodes.size(); i++) {
                if (_noteNodes.at(i)->name() == targetNoteName) {
                    kbGraphLink* link = new kbGraphLink(node, _noteNodes.at(i));
                    link->adjust();
                    node->addLink(link);
                    addItem(link);

                    break;
                }
            }
        }

        if (node->getNumberOfLinks() > _maxLinkNumber)
            _maxLinkNumber = node->getNumberOfLinks();

        addItem(node);
        noteFile.close();
    }
}

/*
 * kbGraphNode : class representing each note in the graph
*/
kbGraphNode::kbGraphNode(QString note) : _noteName(note) {
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(10);

    _position = QPointF(rand() % 1000, rand() % 1000);
    setPos(_position);

    _noteLinkCount = 0;
}

void kbGraphNode::addLink(kbGraphLink* link) {
    _noteLinks << link;
    _noteLinkCount++;
    link->adjust();
}

QString kbGraphNode::name() {
    return _noteName;
}

int kbGraphNode::getNumberOfLinks() {
    return _noteLinkCount;
}

QRectF kbGraphNode::boundingRect() const
{
    return QRectF(_rectText);
}

void kbGraphNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    QFont font = painter->font();
    font.setPointSize(font.pointSize() * (1 + getNumberOfLinks() / 20));
    painter->setFont(font);
    QFontMetrics fontMetrics (font);
    _rectText = fontMetrics.boundingRect(_noteName);
    _rectText.setWidth(fontMetrics.horizontalAdvance(_noteName));
    _rectText.setHeight(fontMetrics.height());

    painter->setPen(QPen(Qt::lightGray, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setBrush(Qt::white);
    painter->drawRect(_rectText);

    if (_noteLinks.size() == 0)
        painter->setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    else {

        painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }

    painter->drawText(0, 0, _noteName);
}

QVariant kbGraphNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        for (kbGraphLink *link : qAsConst(_noteLinks))
            link->adjust();
        update();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

/*
 * kbGraphLink : class representing each relation between notes in the graph
*/
kbGraphLink::kbGraphLink(kbGraphNode* source, kbGraphNode* dest) : _source(source), _dest(dest) {
    setAcceptedMouseButtons(Qt::NoButton);
    _source->addLink(this);
    _dest->addLink(this);
    adjust();
}

void kbGraphLink::adjust()
{
    if (!_source || !_dest)
        return;

    QRectF rectSource = _source->boundingRect();
    QRectF rectDest = _dest->boundingRect();
    QLineF line(mapFromItem(_source, (rectSource.left() + rectSource.width()) / 2, (rectSource.top() + rectSource.height()) / 2), mapFromItem(_dest, (rectDest.left() + rectDest.width()) / 2, (rectDest.top() + rectDest.height()) / 2));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        _sourcePoint = line.p1() + edgeOffset;
        _destPoint = line.p2() - edgeOffset;
    } else {
        _sourcePoint = _destPoint = line.p1();
    }
}

QRectF kbGraphLink::boundingRect() const
{
    if (!_source || !_dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + _arrowSize) / 2.0;

    return QRectF(_sourcePoint, QSizeF(_destPoint.x() - _sourcePoint.x(),
                                      _destPoint.y() - _sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void kbGraphLink::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!_source || !_dest)
        return;

    QLineF line(_sourcePoint, _destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    // Draw the line itself
    painter->setPen(QPen(Qt::lightGray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
}