import Combine
import Schwifty
import SwiftUI
import Yage

class EntityView: NSImageView {
    var zIndex: Int { entity.zIndex }
    
    private let entity: Entity
    private let assetsProvider = PetsAssetsProvider.shared
    private var imageCache: [Int: NSImage] = [:]
    private var firstMouseClick: Date?
    private var locationOnLastDrag: CGPoint = .zero
    private var locationOnMouseDown: CGPoint = .zero
    private var lastSpriteHash: Int = 0
    
    init(representing entity: Entity) {
        self.entity = entity
        super.init(frame: CGRect(size: entity.frame.size))
        translatesAutoresizingMaskIntoConstraints = false
        imageScaling = .scaleProportionallyUpOrDown
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    func update() {
        guard entity.isAlive else {
            removeFromSuperview()
            return
        }
        updateFrameIfNeeded()
        updateImageIfNeeded()
    }
    
    private func updateFrameIfNeeded() {
        guard entity.state != .drag else { return }
        guard let size = max(entity.frame.size, .oneByOne) else { return }
        frame = CGRect(
            origin: .zero
                .offset(x: entity.frame.minX)
                .offset(y: entity.worldBounds.height)
                .offset(y: -entity.frame.maxY),
            size: size
        )
    }
    
    private func updateImageIfNeeded() {
        let hash = entity.spriteHash()
        guard needsSpriteUpdate(for: hash) else { return }
        let newImage = nextImage(for: hash)
        image = newImage
    }
    
    // MARK: - Mouse Drag
    
    override func mouseDown(with event: NSEvent) {
        locationOnLastDrag = frame.origin
        locationOnMouseDown = frame.origin
    }
    
    override func mouseDragged(with event: NSEvent) {
        let newOrigin = locationOnLastDrag.offset(x: event.deltaX, y: -event.deltaY)
        frame.origin = newOrigin
        locationOnLastDrag = newOrigin
        let delta = CGSize(width: event.deltaX, height: event.deltaY)
        entity.mouseDrag?.mouseDragged(currentDelta: delta)
    }
    
    override func mouseUp(with event: NSEvent) {
        let delta = CGSize(
            width: locationOnLastDrag.x - locationOnMouseDown.x,
            height: locationOnMouseDown.y - locationOnLastDrag.y
        )
        if let finalPosition = entity.mouseDrag?.mouseUp(totalDelta: delta) {
            frame.origin = finalPosition
        }
    }

    // MARK: - Right Click

    override func rightMouseUp(with event: NSEvent) {
        entity.rightClick?.onRightClick(with: event)
    }
}

// MARK: - Hash

private extension Entity {
    func spriteHash() -> Int {
        var hasher = Hasher()
        hasher.combine(sprite)
        hasher.combine(frame.size.width)
        hasher.combine(frame.size.height)
        hasher.combine(rotation?.isFlippedHorizontally ?? false)
        hasher.combine(rotation?.isFlippedVertically ?? false)
        hasher.combine(rotation?.z ?? 0)
        return hasher.finalize()
    }
}

// MARK: - Image Loading

private extension EntityView {
    func nextImage(for hash: Int) -> NSImage? {
        if let cached = imageCache[hash] { return cached }
        guard let image = interpolatedImageForCurrentSprite() else { return nil }
        imageCache[hash] = image
        return image
    }
    
    func interpolatedImageForCurrentSprite() -> NSImage? {
        assetsProvider
            .image(sprite: entity.sprite)?
            .scaled(to: frame.size, with: interpolationMode())
            .rotated(by: entity.rotation?.z)
            .flipped(
                horizontally: entity.rotation?.isFlippedHorizontally ?? false,
                vertically: entity.rotation?.isFlippedVertically ?? false
            )
    }
    
    func needsSpriteUpdate(for newHash: Int) -> Bool {
        if newHash != lastSpriteHash {
            lastSpriteHash = newHash
            return true
        }
        return false
    }
    
    func interpolationMode() -> NSImageInterpolation {
        let scale = self.window?.screen?.backingScaleFactor ?? 1
        if scale >= 2 { return .none }
        if frame.width >= PetSize.defaultSize { return .none }
        return .default
    }
}
