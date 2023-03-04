import NotAGif
import Schwifty
import SwiftUI
import Yage

struct PetDetailsView: View {
    @StateObject var viewModel: PetDetailsViewModel
    
    init(isShown: Binding<Bool>, species: Species) {
        _viewModel = StateObject(wrappedValue: PetDetailsViewModel(isShown: isShown, species: species))
    }
    
    var body: some View {
        VStack(spacing: .xl) {
            viewModel.header()
            AnimatedPreview()
            About().padding(.top, .lg)
            if DeviceRequirement.iOS.isSatisfied { Spacer() }
            Footer()
        }
        .padding(.lg)
        .frame(width: 450, when: .macOS)
        .onAppear { viewModel.didAppear() }
        .environmentObject(viewModel)
    }
}

private struct About: View {
    @EnvironmentObject var viewModel: PetDetailsViewModel
    
    var body: some View {
        Text(viewModel.speciesAbout)
            .lineLimit(10)
            .multilineTextAlignment(.center)
            .fixedSize(horizontal: false, vertical: true)
    }
}

private struct AnimatedPreview: View {
    @EnvironmentObject var appState: AppState
    @EnvironmentObject var viewModel: PetDetailsViewModel
    
    var body: some View {
        ZStack {
            AnimatedContent(frames: viewModel.animationFrames, fps: viewModel.animationFps) { frame in
                Image(frame: frame)
                    .pixelArt()
                    .frame(width: 150, height: 150)
            }
        }
        .frame(width: 150, height: 150)
    }
}

private struct Footer: View {
    @EnvironmentObject var viewModel: PetDetailsViewModel
    
    var axis: Axis.Set {
        DeviceRequirement.iOS.isSatisfied ? .vertical : .horizontal
    }
    
    var body: some View {
        VHStack(axis) {
            if viewModel.canSelect {
                Button(Lang.PetSelection.addPet, action: viewModel.selected)
                    .buttonStyle(.regular)
            }
            if viewModel.canRemove {
                Button(Lang.remove, action: viewModel.remove)
                    .buttonStyle(.regular)
            }
            
            Button(Lang.cancel, action: viewModel.close)
                .buttonStyle(.text)
        }
    }
}
