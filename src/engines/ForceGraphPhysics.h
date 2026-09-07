#ifndef FORCEGRAPHPHYSICS_H
#define FORCEGRAPHPHYSICS_H

#include <QVector3D>
#include <QString>
#include <QVector>
#include <QVariantMap>

struct Node3D {
    QString id;
    QString label;
    QString type;
    int score = 100;
    bool isForeign = false;
    
    QVector3D position;
    QVector3D velocity;
    QVector3D force;
};

struct Edge3D {
    int sourceIndex;
    int targetIndex;
    int confidence; // 100 = Direct, 60 = Package Sync
};

class ForceGraphPhysics {
public:
    ForceGraphPhysics();

    void updateData(const QVariantMap &graphData);
    void stepSimulation(float deltaTime = 0.016f);

    const QVector<Node3D>& nodes() const { return m_nodes; }
    const QVector<Edge3D>& edges() const { return m_edges; }

private:
    QVector<Node3D> m_nodes;
    QVector<Edge3D> m_edges;

    float m_repulsionStrength = 120.0f;
    float m_springStrength = 0.05f;
    float m_restingDistance = 4.0f;
    float m_damping = 0.85f;
};

#endif // FORCEGRAPHPHYSICS_H