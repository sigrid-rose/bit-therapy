import Combine
import Schwifty
import SwiftUI
import WindowsDetector
import Yage

class DesktopObstaclesServiceImpl: DesktopObstaclesService {
    private static let windowsDetector = WindowsDetector().started(pollInterval: 1)
    private weak var world: World?
    private var disposables = Set<AnyCancellable>()

    func start(in world: World) {
        self.world = world
        DesktopObstaclesServiceImpl.windowsDetector.$userWindows
            .sink { [weak self] in self?.onWindows($0) }
            .store(in: &disposables)
    }

    func stop() {
        disposables.removeAll()
    }
    
    func obstacles(from frame: CGRect, borderThickness: CGFloat) -> [CGRect] {
        [CGRect(x: frame.minX, y: frame.minY, width: frame.width, height: borderThickness)]
    }

    private func onWindows(_ windows: [WindowInfo]) {
        let obstacles = obstacles(from: windows)
        world?.update(withObstacles: obstacles)
    }

    private func obstacles(from windows: [WindowInfo]) -> [Entity] {
        guard let world else { return [] }
        return windows.reversed()
            .filter { $0.isValidObstacle(within: world.bounds) }
            .map { $0.frame }
            .reduce([]) { obstacles, rect in
                let visibleObstacles = obstacles.flatMap { $0.parts(bySubtracting: rect) }
                let newObstacles = self.obstacles(fromWindowFrame: rect)
                return visibleObstacles + newObstacles
            }
            .filter { $0.minY > AppState.global.petSize }
            .compactMap { WindowObstacle(of: $0, in: world) }
    }

    private func obstacles(fromWindowFrame frame: CGRect) -> [CGRect] {
        obstacles(from: frame, borderThickness: 10)
    }
}

private class WindowObstacle: Entity {
    init(of frame: CGRect, in world: World) {
        let id = WindowObstacle.nextId()
        super.init(species: .desktopObstacle, id: id, frame: frame, in: world)
        isStatic = true
    }

    static func nextId() -> String {
        id += 1
        return "window-\(id)"
    }

    static var id: Int = 0
}

private extension WindowInfo {
    var owner: String { processName?.lowercased() ?? "" }

    func isValidObstacle(within worldBounds: CGRect) -> Bool {
        if frame.isNull || frame.isEmpty || frame.isInfinite { return false }
        if frame.size == worldBounds.size { return false }
        if ignoreList.contains(owner) { return false }
        return true
    }
}

private let ignoreList: [String] = [
    "parallels desktop",
    "shades",
    "tiles"
]
