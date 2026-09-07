#ifndef NATIVE3DGRAPHITEM_H
#define NATIVE3DGRAPHITEM_H

#include <QQuickFramebufferObject>
#include <QVariantMap>
#include "../engines/ForceGraphPhysics.h"

class Native3DGraphItem : public QQuickFramebufferObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap graphData READ graphData WRITE setGraphData NOTIFY graphDataChanged)

public:
    explicit Native3DGraphItem(QQuickItem *parent = nullptr);

    QVariantMap graphData() const { return m_graphData; }
    void setGraphData(const QVariantMap &data);

    Q_INVOKABLE void rotateCamera(float dx, float dy);
    Q_INVOKABLE void zoomCamera(float delta);

    Renderer *createRenderer() const override;

    // Physics Engine & Camera state (Renderer அணுகுவதற்காக)
    ForceGraphPhysics m_physicsEngine;
    float m_rotX = 20.0f;
    float m_rotY = -30.0f;
    float m_zoom = -15.0f;

signals:
    void graphDataChanged();

private:
    QVariantMap m_graphData;
};

#endif // NATIVE3DGRAPHITEM_H