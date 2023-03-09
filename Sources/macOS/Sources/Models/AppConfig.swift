import Combine
import Schwifty
import SwiftUI

class AppConfig: ObservableObject {
    @Inject private var storage: AppConfigStorage
        
    @Published var background: String = "BackgroundMountainDynamic"
    @Published var desktopInteractions: Bool = true
    @Published var gravityEnabled: Bool = true
    @Published var names: [String: String] = [:]
    @Published var petSize: CGFloat = PetSize.defaultSize
    @Published var randomEvents: Bool = true
    @Published var speedMultiplier: CGFloat = 1
    @Published var disabledScreens: [String] = []
    @Published private(set) var selectedSpecies: [String] = []
    
    init() {
        readFromStorage()
        storage.storeValues(of: self)
    }
    
    func isEnabled(screen: Screen) -> Bool {
        !disabledScreens.contains(screen.id)
    }
    
    func set(screen: Screen, enabled: Bool) {
        if enabled {
            disabledScreens.remove(screen.id)
        } else {
            disabledScreens.append(screen.id)
        }
    }
    
    func isSelected(_ species: String) -> Bool {
        selectedSpecies.contains(species)
    }
    
    func replaceSelectedSpecies(with species: [String]) {
        selectedSpecies = species
    }
    
    func select(_ species: String) {
        deselect(species)
        selectedSpecies.append(species)
    }
    
    func deselect(_ species: String) {
        selectedSpecies.remove(species)
    }
    
    func rename(species: String, to newName: String) {
        names[species] = newName
    }

    private func readFromStorage() {
        background = storage.background
        desktopInteractions = storage.desktopInteractions
        gravityEnabled = storage.gravityEnabled
        names = storage.names
        petSize = storage.petSize
        selectedSpecies = storage.selectedSpecies
        speedMultiplier = storage.speedMultiplier
        disabledScreens = storage.disabledScreens
        randomEvents = storage.randomEvents
    }
}

extension Screen: Identifiable {
    public var id: String { localizedName.lowercased() }
}
