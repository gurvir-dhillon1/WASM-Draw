#include "DrawingCommands.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h> 

class DrawingCommandsTests : public testing::Test {
    void SetUp() override {
        clearDrawCommandsInternal();
    }
};

TEST_F(DrawingCommandsTests, AddingDrawCommandsAddsThemToDrawCommandVectorsCorrectly) {
    int startX = 0, startY = 0, endX = 1, endY = 1, type = LINE;
    addDrawCommandInternal(startX, startY, endX, endY, type);

    ASSERT_THAT(drawStartX, testing::ElementsAre(startX));
    ASSERT_THAT(drawStartY, testing::ElementsAre(startY));
    ASSERT_THAT(drawEndX, testing::ElementsAre(endX));
    ASSERT_THAT(drawEndY, testing::ElementsAre(endY));
    ASSERT_THAT(drawTypes, testing::ElementsAre(type));
}
