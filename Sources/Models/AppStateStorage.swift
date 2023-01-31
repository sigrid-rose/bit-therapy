import Combine
import Foundation
import SwiftUI
import Yage

protocol AppStateStorage {
    var desktopInteractions: Bool { get }
    var disabledScreens: [String] { get }
    var gravityEnabled: Bool { get }
    var petSize: Double { get }
    var selectedSpecies: [Species] { get }
    var speedMultiplier: Double { get }
    func storeValues(of appState: AppState)
}

class AppStateStorageImpl: AppStateStorage {
    @AppStorage("desktopInteractions") var desktopInteractions: Bool = true
    @AppStorage("disabledScreens") var disabledScreensValue: String = ""
    @AppStorage("gravityEnabled") var gravityEnabled = true
    @AppStorage("petSize") var petSize: Double = PetSize.defaultSize
    @AppStorage("petId") private var selectedSpeciesValue: String = kInitialPetId
    @AppStorage("speedMultiplier") var speedMultiplier: Double = 1
    
    var disabledScreens: [String] {
        get {
            disabledScreensValue
                .components(separatedBy: ",")
                .map { $0.trimmingCharacters(in: .whitespacesAndNewlines) }
                .filter { !$0.isEmpty }
        }
        set {
            disabledScreensValue = newValue.joined(separator: ",")
        }
    }
    
    var selectedSpecies: [Species] {
        get {
            let storedIds = selectedSpeciesValue
                .components(separatedBy: ",")
                .map { $0.trimmingCharacters(in: .whitespacesAndNewlines).lowercased() }
            let speciesIds = storedIds.count == 0 ? [kInitialPetId] : storedIds
            return speciesIds.compactMap { Species.by(id: $0) }
        }
        set {
            selectedSpeciesValue = newValue.map { $0.id }.joined(separator: ",")
        }
    }
    
    private var disposables = Set<AnyCancellable>()
        
    func storeValues(of appState: AppState) {
        appState.$desktopInteractions
            .sink { [weak self] in self?.desktopInteractions = $0 }
            .store(in: &disposables)
        
        appState.$gravityEnabled
            .sink { [weak self] in self?.gravityEnabled = $0 }
            .store(in: &disposables)
        
        appState.$petSize
            .sink { [weak self] in self?.petSize = $0 }
            .store(in: &disposables)
        
        appState.$selectedSpecies
            .sink { [weak self] in self?.selectedSpecies = $0 }
            .store(in: &disposables)
        
        appState.$speedMultiplier
            .sink { [weak self] in self?.speedMultiplier = $0 }
            .store(in: &disposables)
        
        appState.$disabledScreens
            .sink { [weak self] in self?.disabledScreens = $0 }
            .store(in: &disposables)
    }
}

// MARK: - Constants

private let kInitialPetId = "cat"
