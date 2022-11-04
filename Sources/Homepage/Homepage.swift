import DesignSystem
import InAppPurchases
import PetDetails
import Schwifty
import SwiftUI

struct Homepage: View {
    @EnvironmentObject var appState: AppState
    @StateObject var viewModel = HomepageViewModel()
    
    var body: some View {
        ScrollView {
            VStack(spacing: .xxl) {
                if DeviceRequirement.iOS.isSatisfied {
                    Text(Lang.Page.home).title()
                }
                PetsGrid(
                    title: Lang.PetSelection.yourPets,
                    columns: viewModel.gridColums,
                    pets: viewModel.selectedPets
                )
                PetsGrid(
                    title: Lang.PetSelection.morePets,
                    columns: viewModel.gridColums,
                    pets: viewModel.unselectedPets
                )
                RequestPetsViaSurvey()
            }
            .padding(.md)
        }
        .sheet(isPresented: viewModel.showingDetails) {
            if let pet = viewModel.selectedPet {
                PetDetailsCoordinator.view(isShown: viewModel.showingDetails, pet: pet)
            }
        }
        .environmentObject(viewModel)
        .environmentObject(PricingService.global)
    }
}
