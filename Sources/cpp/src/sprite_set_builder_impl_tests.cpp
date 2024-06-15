#include "sprite_set.h"
#include "sprite_set_builder_impl.h"

#include <gtest/gtest.h>
#include <map>
#include <optional>
#include <string>
#include <vector>

TEST(SpriteSetBuilderTests, SpriteFrameFromPath) {
    SpriteSetBuilderImpl builder({});

    EXPECT_EQ(
        builder.spriteFrameFromPath("/ape_eat-0.png").value(), 
        SpriteFrame({"/ape_eat-0.png", "ape", "eat", 0})
    );
    EXPECT_EQ(
        builder.spriteFrameFromPath("/ape_chef_eat-0.png").value(), 
        SpriteFrame({"/ape_chef_eat-0.png", "ape_chef", "eat", 0})
    );
    EXPECT_EQ(
        builder.spriteFrameFromPath("/ape_eat-123.png").value(), 
        SpriteFrame({"/ape_eat-123.png", "ape", "eat", 123})
    );
    EXPECT_EQ(
        builder.spriteFrameFromPath("/ape_chef_eat-123.png").value(), 
        SpriteFrame({"/ape_chef_eat-123.png", "ape_chef", "eat", 123})
    );
};

TEST(SpriteSetBuilderTests, SpriteFramesFromPaths) {
    SpriteSetBuilderImpl builder({});

    std::vector<std::string> paths({
        "/ape_chef_eat-1.png",
        "/ape_chef_eat-2.png",
        "/ape_chef_eat-3.png",
        "/ape_chef_eat-4.png",
        "/ape_chef_eat-invalid.png",
        "/invalid.png"
    });
    EXPECT_EQ(builder.spriteFramesFromPaths(paths).size(), 4);
};

TEST(SpriteSetBuilderTests, CanAggregateFramesBySpecies) {
    SpriteSetBuilderImpl builder({});

    const std::vector<SpriteFrame> frames({
        SpriteFrame({"a_b-0", "a", "b", 0}),
        SpriteFrame({"a_b-1", "a", "b", 1}),
        SpriteFrame({"a_b-2", "a", "b", 2}),
        SpriteFrame({"c_d-0", "c", "d", 0}),
        SpriteFrame({"c_d-1", "c", "d", 1}),
        SpriteFrame({"e_f-0", "e", "f", 0}),
        SpriteFrame({"e_f-1", "e", "f", 1})
    });

    std::map<std::string, std::vector<SpriteFrame>> result = builder.aggregateFramesBySpecies(frames);

    const std::vector<SpriteFrame> framesA({
        SpriteFrame({"a_b-0", "a", "b", 0}),
        SpriteFrame({"a_b-1", "a", "b", 1}),
        SpriteFrame({"a_b-2", "a", "b", 2})
    });
    EXPECT_EQ(result["a"], framesA);

    const std::vector<SpriteFrame> framesC({
        SpriteFrame({"c_d-0", "c", "d", 0}),
        SpriteFrame({"c_d-1", "c", "d", 1})
    });
    EXPECT_EQ(result["c"], framesC);

    const std::vector<SpriteFrame> framesE({
        SpriteFrame({"e_f-0", "e", "f", 0}),
        SpriteFrame({"e_f-1", "e", "f", 1})
    });
    EXPECT_EQ(result["e"], framesE);
};

TEST(SpriteSetBuilderTests, CanGenerateSpriteSetFromFrames) {
    SpriteSetBuilderImpl builder({});

    const std::vector<SpriteFrame> frames({
        SpriteFrame({"a_movement-0", "a", "movement", 0}),
        SpriteFrame({"a_movement-1", "a", "movement", 1}),
        SpriteFrame({"a_movement-2", "a", "movement", 2}),
        SpriteFrame({"a_fall-0", "a", "fall", 0}),
        SpriteFrame({"a_fall-1", "a", "fall", 1}),
        SpriteFrame({"a_front-0", "a", "front", 0}),
        SpriteFrame({"a_front-1", "a", "front", 1}),
        SpriteFrame({"a_eat-0", "a", "eat", 0}),
        SpriteFrame({"a_eat-1", "a", "eat", 1})
    });

    auto someResult = builder.spriteSet(frames);
    EXPECT_NE(someResult, std::nullopt);

    auto result = someResult.value();

    std::vector<std::string> expectedEat({"a_eat-0", "a_eat-1"});
    std::map<std::string, std::vector<std::string>> expectedOtherAnimations({{"eat", expectedEat}});

    SpriteSet expected({
        std::vector<std::string>({"a_movement-0", "a_movement-1", "a_movement-2"}),
        std::vector<std::string>({"a_fall-0",  "a_fall-1"}), 
        std::vector<std::string>({"a_front-0", "a_front-1"}),
        expectedOtherAnimations
    });

    EXPECT_EQ(result.movementSprite(1.0).currentFrame(), "a_movement-0");
    EXPECT_EQ(result.fallSprite(1.0).currentFrame(), "a_fall-0");
    EXPECT_EQ(result.frontSprite(1.0).currentFrame(), "a_front-0");
    EXPECT_EQ(result.sprite("eat", 1.0).currentFrame(), "a_eat-0");
};