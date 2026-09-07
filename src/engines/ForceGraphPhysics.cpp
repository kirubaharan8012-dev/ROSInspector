#include "ForceGraphPhysics.h"
#include <QVariantList>
#include <QRandomGenerator>
#include <cmath>

ForceGraphPhysics::ForceGraphPhysics() {}

void ForceGraphPhysics::updateData(const QVariantMap &graphData) {
    m_nodes.clear();
    m_edges.clear();

    QVariantList nodesList = graphData.value("nodes").toList();
    QVariantList edgesList = graphData.value("edges").toList();

    QMap<QString, int> idToIndexMap;

    // 1. Populate Nodes with random initial 3D space positions
    for (int i = 0; i < nodesList.size(); ++i) {
        QVariantMap nm = nodesList[i].toMap();
        Node3D node;
        node.id = nm.value("id").toString();
        node.label = nm.value("label").toString();
        node.type = nm.value("type").toString();
        node.score = nm.value("score", 100).toInt();
        node.isForeign = nm.value("isForeign", false).toBool();

        float rx = (QRandomGenerator::global()->generateDouble() - 0.5f) * 10.0f;
        float ry = (QRandomGenerator::global()->generateDouble() - 0.5f) * 10.0f;
        float rz = (QRandomGenerator::global()->generateDouble() - 0.5f) * 10.0f;
        node.position = QVector3D(rx, ry, rz);
        node.velocity = QVector3D(0, 0, 0);

        m_nodes.append(node);
        idToIndexMap[node.id] = i;
    }

    // 2. Populate Edges
    for (const QVariant &eVar : edgesList) {
        QVariantMap em = eVar.toMap();
        QString fromId = em.value("from").toString();
        QString toId = em.value("to").toString();

        if (idToIndexMap.contains(fromId) && idToIndexMap.contains(toId)) {
            Edge3D edge;
            edge.sourceIndex = idToIndexMap[fromId];
            edge.targetIndex = idToIndexMap[toId];
            edge.confidence = em.value("confidence", 100).toInt();
            m_edges.append(edge);
        }
    }
}

void ForceGraphPhysics::stepSimulation(float deltaTime) {
    if (m_nodes.isEmpty()) return;

    // Reset forces
    for (auto &node : m_nodes) {
        node.force = QVector3D(0, 0, 0);
    }

    // 1. Coulomb Repulsion between all nodes
    for (int i = 0; i < m_nodes.size(); ++i) {
        for (int j = i + 1; j < m_nodes.size(); ++j) {
            QVector3D dir = m_nodes[i].position - m_nodes[j].position;
            float dist = dir.length();
            if (dist < 0.1f) dist = 0.1f; // Avoid division by zero

            dir.normalize();
            float forceMagnitude = m_repulsionStrength / (dist * dist);
            QVector3D f = dir * forceMagnitude;

            m_nodes[i].force += f;
            m_nodes[j].force -= f;
        }
    }

    // 2. Hooke's Law Attraction along Edges
    for (const auto &edge : m_edges) {
        Node3D &nA = m_nodes[edge.sourceIndex];
        Node3D &nB = m_nodes[edge.targetIndex];

        QVector3D dir = nB.position - nA.position;
        float dist = dir.length();
        dir.normalize();

        float displacement = dist - m_restingDistance;
        QVector3D springForce = dir * (m_springStrength * displacement);

        nA.force += springForce;
        nB.force -= springForce;
    }

    // 3. Update Positions & Velocities
    for (auto &node : m_nodes) {
        node.velocity = (node.velocity + node.force * deltaTime) * m_damping;
        node.position += node.velocity * deltaTime;
    }
}