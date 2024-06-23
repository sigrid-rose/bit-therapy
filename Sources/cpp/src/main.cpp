#include <QApplication>

#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "app_window.h"

#include "game/game.h"
#include "rendering/rendering.h"
#include "species/species.h"
#include "sprites/sprites.h"
#include "utils/utils.h"

namespace po = boost::program_options;

static const double GAME_FPS = 30.0;
static const double ANIMATIONS_FPS = 10.0;
static const double BASE_ENTITY_SIZE = 50.0;
static const std::string SPECIES_PATH = "/Users/curzel/dev/bit-therapy/Species";
static const std::string ASSETS_PATH = "/Users/curzel/dev/bit-therapy/PetsAssets";

static SpriteSetBuilder builder;
static SpritesRepository spritesRepo(builder);
static SpeciesParser parser;
static SpeciesRepository speciesRepo(parser);

std::vector<std::thread> gameThreads;

po::variables_map parseArgs(int argc, char *argv[]);
std::vector<Screen> screensMatching(po::variables_map args);
std::vector<std::string> selectedSpecies(po::variables_map args);
void startGame(Screen screen, std::vector<std::string> species);
void startGames(std::vector<Screen> screens, std::vector<std::string> species);
void joinGameThreads();
std::thread startGameLoop(Game* game);

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    spritesRepo.setup(ASSETS_PATH);
    speciesRepo.setup(SPECIES_PATH);

    auto args = parseArgs(argc, argv);
    if (args.count("help")) {
        return EXIT_SUCCESS;
    }
    auto screens = screensMatching(args);
    auto species = selectedSpecies(args);

    startGames(screens, species);

    // AppWindow appWindow;
    // appWindow.show();

    auto result = app.exec();

    joinGameThreads();
    return result;
}

po::variables_map parseArgs(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Shows this help message")
        ("species", po::value<std::vector<std::string>>()->multitoken(), "Species of pets to spawn (exact id)")
        ("screen", po::value<std::vector<std::string>>()->multitoken(), "Monitors the app will display on (part of the name)");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
    }
    return vm;
}

void joinGameThreads() {
    for (auto& t : gameThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void startGames(std::vector<Screen> screens, std::vector<std::string> species) {
    for (const Screen& screen : screens) {
        startGame(screen, species);
    }
}

void startGame(Screen screen, std::vector<std::string> species) {
    Game* game = new Game(&spritesRepo, &speciesRepo, screen.name, GAME_FPS, ANIMATIONS_FPS, BASE_ENTITY_SIZE);
    game->addEntities(species);

    GameWindow* window = new GameWindow();
    window->setup(game, screen.name, screen.frame);
    window->show();

    auto newThread = startGameLoop(game);
    gameThreads.push_back(std::move(newThread));
}

std::vector<Screen> screensMatching(po::variables_map args) {
    if (args.count("screen")) {
        auto names = args["screen"].as<std::vector<std::string>>();
        auto screens = screensMatching(names);

        if (screens.size() == 0) {
            std::cerr << "No matching screens found! The following one(s) should be available:" << std::endl;

            for (const Screen& screen : listAvailableScreens()) {
                std::cerr << "  - " << screen.description() << std::endl;
            }
            std::exit(EXIT_FAILURE);
        }
        return screens;
    } else {
        auto screens = listAvailableScreens();
        if (screens.size() == 0) {
            std::cerr << "No screen found!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        return screens;
    }
}

std::vector<std::string> selectedSpecies(po::variables_map args) {
    if (args.count("species")) {
        return args["species"].as<std::vector<std::string>>();
    } else {
        std::cerr << "No species selected, plase run with `--species ape` or something" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

std::thread startGameLoop(Game* game) {
    return std::thread([game]() {
        bool gameIsRunning = true;
        auto frameDuration = std::chrono::milliseconds(uint32_t(1000 / game->gameFps));
        
        while (gameIsRunning) { 
            auto frameStart = std::chrono::steady_clock::now();
            game->update(frameDuration);
            auto frameEnd = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
            
            auto sleepDuration = frameDuration - elapsedTime;
            if (sleepDuration > std::chrono::milliseconds(0)) {
                std::this_thread::sleep_for(sleepDuration);
            }
        }
    });
}
