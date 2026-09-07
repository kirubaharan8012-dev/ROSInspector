#include "Native3DGraphItem.h"
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QMatrix4x4>

class GraphFBORenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions {
public:
    GraphFBORenderer() {
        initializeOpenGLFunctions();
    }

    void render() override {
        glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (!m_item) return;

        // Step physics simulation frame
        m_item->m_physicsEngine.stepSimulation(0.016f);

        // Setup Matrix
        float w = m_item->width() > 0 ? m_item->width() : 800.0f;
        float h = m_item->height() > 0 ? m_item->height() : 600.0f;

        QMatrix4x4 projection;
        projection.perspective(45.0f, w / h, 0.1f, 1000.0f);

        QMatrix4x4 view;
        view.translate(0.0f, 0.0f, m_item->m_zoom);
        view.rotate(m_item->m_rotX, 1.0f, 0.0f, 0.0f);
        view.rotate(m_item->m_rotY, 0.0f, 1.0f, 0.0f);

        QMatrix4x4 mvp = projection * view;

        // OpenGL Pipeline Matrix setup
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(projection.constData());
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(view.constData());

        const auto &nodes = m_item->m_physicsEngine.nodes();
        const auto &edges = m_item->m_physicsEngine.edges();

        // Render Edges (Lines)
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        for (const auto &edge : edges) {
            if (edge.sourceIndex >= nodes.size() || edge.targetIndex >= nodes.size()) continue;

            const auto &nA = nodes[edge.sourceIndex];
            const auto &nB = nodes[edge.targetIndex];

            if (edge.confidence == 100) {
                glColor4f(0.2f, 0.8f, 0.4f, 0.8f);
            } else {
                glColor4f(0.9f, 0.7f, 0.2f, 0.5f);
            }

            glVertex3f(nA.position.x(), nA.position.y(), nA.position.z());
            glVertex3f(nB.position.x(), nB.position.y(), nB.position.z());
        }
        glEnd();

        // Render Nodes (Glow Points)
        glPointSize(14.0f);
        glBegin(GL_POINTS);
        for (const auto &node : nodes) {
            if (node.isForeign) {
                glColor4f(0.9f, 0.3f, 0.3f, 1.0f);
            } else if (node.score < 80) {
                glColor4f(0.9f, 0.5f, 0.2f, 1.0f);
            } else {
                glColor4f(0.3f, 0.6f, 1.0f, 1.0f);
            }
            glVertex3f(node.position.x(), node.position.y(), node.position.z());
        }
        glEnd();

        update(); // Continuous frame render for physics animation
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

    void synchronize(QQuickFramebufferObject *item) override {
        m_item = static_cast<Native3DGraphItem *>(item);
    }

private:
    Native3DGraphItem *m_item = nullptr;
};

Native3DGraphItem::Native3DGraphItem(QQuickItem *parent) : QQuickFramebufferObject(parent) {
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

QQuickFramebufferObject::Renderer *Native3DGraphItem::createRenderer() const {
    return new GraphFBORenderer();
}

void Native3DGraphItem::setGraphData(const QVariantMap &data) {
    m_graphData = data;
    m_physicsEngine.updateData(m_graphData);
    emit graphDataChanged();
    update();
}

void Native3DGraphItem::rotateCamera(float dx, float dy) {
    m_rotX += dy * 0.5f;
    m_rotY += dx * 0.5f;
    update();
}

void Native3DGraphItem::zoomCamera(float delta) {
    m_zoom += delta * 0.01f;
    if (m_zoom > -1.0f) m_zoom = -1.0f;
    if (m_zoom < -100.0f) m_zoom = -100.0f;
    update();
}