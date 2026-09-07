# ROSInspector - C++ & Qt/QML Diagnostic & Visualization Tool

An industrial-grade static diagnostic, architecture analysis, and visualization platform designed for ROS/ROS 2 workspaces. Built with modern C++ and Qt/QML, ROSInspector parses complex launch trees, URDF models, package manifests, and configurations to detect pipeline misconfigurations and render architectural dependencies.

## Key Features
- **Modern Qt/QML UI:** Responsive interface featuring drag-and-drop workspace loading (`DropZone.qml`), interactive file inspection (`DiffViewer.qml`), and real-time issue drill-downs.
- **Architectural Analysis Engine:** Core analysis modules (`CrossFileAnalyzer`, `HealthCalculator`) that evaluate system health, package dependencies, and semantic integrity.
- **Graph Visualization:** Physics-based force-directed layout simulation (`ForceGraphPhysics`) with native rendering (`Native3DGraphItem`) to map nodes, topics, and pipeline connections.
- **Multi-Format Parsers:** Specialized parsers for CMakeLists, XML, YAML, Launch files, and URDF kinematics models.
- **Robust Facade Architecture:** Decoupled business logic using the Facade structural pattern (`InspectorFacade`) for scalable backend-to-frontend communication.

## Tech Stack
- **Languages:** Modern C++ (C++17/20), QML, JavaScript
- **Frameworks:** Qt 5 / Qt 6 (Core, GUI, Quick, QML)
- **Build System:** CMake
- **Domain:** ROS / ROS 2 Diagnostics, Software Architecture, Static Code Analysis

## Build & Run
```bash
# Clone the repository
git clone [https://github.com/kirubaharan8012-dev/ROSInspector.git](https://github.com/kirubaharan8012-dev/ROSInspector.git)
cd ROSInspector

# Configure and compile
cmake -B build -S .
cmake --build build

# Run application
./build/ROSInspector
