import Combine
import NotAGif
import Swinject
import Schwifty
import SwiftUI

class EntityViewModel: ObservableObject {
    @Inject var assetsProvider: PetsAssetsProvider
    
    @Published var frame: CGRect = .init(square: 1)
    @Published var isAlive: Bool = true
    @Published var image: ImageFrame?
    
    var entityId: String { entity.id }
    var isInteractable: Bool { entity.isInteractable }
    var screenScaleFactor: () -> CGFloat = { 1 }
    var zIndex: Int { entity.zIndex }
    
    private let entity: RenderableEntity
    private var firstMouseClick: Date?
    private var imageCache: [Int: ImageFrame] = [:]
    private let imageInterpolation = ImageInterpolationUseCase()
    private var locationOnLastDrag: CGPoint = .zero
    private var locationOnMouseDown: CGPoint = .zero
    private var lastSpriteHash: Int = 0
    
    init(representing entity: RenderableEntity) {
        self.entity = entity
    }
}

// MARK: - Updates

extension EntityViewModel {
    func update() {
        if isAlive != entity.isAlive {
            isAlive = entity.isAlive
        }
        guard entity.isAlive else { return }
        updateFrameIfNeeded()
        updateImageIfNeeded()
    }
    
    private func updateFrameIfNeeded() {
        guard !entity.isBeingDragged() else { return }
        guard let size = max(entity.frame.size, .oneByOne) else { return }
        frame = CGRect(
            origin: .zero
                .offset(x: entity.frame.minX)
                .offset(y: entity.windowSize.height)
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
}

// MARK: - Mouse Events

extension EntityViewModel {
    func mouseDown() {
        locationOnLastDrag = frame.origin
        locationOnMouseDown = frame.origin
    }
    
    func mouseDragged(eventDelta: CGSize, viewDelta: CGSize) {
        let newOrigin = locationOnLastDrag.offset(by: viewDelta)
        frame.origin = newOrigin
        locationOnLastDrag = newOrigin
        entity.mouseDragged(currentDelta: eventDelta)
    }
    
    func mouseUp() {
        let delta = CGSize(
            width: locationOnLastDrag.x - locationOnMouseDown.x,
            height: locationOnMouseDown.y - locationOnLastDrag.y
        )
        entity.mouseUp(totalDelta: delta)
    }
    
    func rightMouseUp(from window: SomeWindow?, at point: CGPoint) {
        entity.rightClicked(from: window, at: point)
    }
}

// MARK: - Image Loading

private extension EntityViewModel {
    func nextImage(for hash: Int) -> ImageFrame? {
        if let cached = imageCache[hash] { return cached }
        guard let image = interpolatedImageForCurrentSprite() else { return nil }
        imageCache[hash] = image
        return image
    }
    
    func interpolatedImageForCurrentSprite() -> ImageFrame? {
        guard let asset = assetsProvider.image(sprite: entity.sprite) else { return nil }
        let interpolationMode = imageInterpolation.interpolationMode(
            for: asset,
            renderingSize: frame.size,
            screenScale: screenScaleFactor()
        )
        
        return asset
            .scaled(to: frame.size, with: interpolationMode)
            .rotated(by: entity.spriteRotation?.zAngle)
            .flipped(
                horizontally: entity.spriteRotation?.isFlippedHorizontally ?? false,
                vertically: entity.spriteRotation?.isFlippedVertically ?? false
            )
    }
    
    func needsSpriteUpdate(for newHash: Int) -> Bool {
        if newHash != lastSpriteHash {
            lastSpriteHash = newHash
            return true
        }
        return false
    }
}
