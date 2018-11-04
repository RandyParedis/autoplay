//
// Created by red on 20/10/18.
//

#include "../../main/music/Instrument.h"
#include <gtest/gtest.h>

using namespace autoplay;

TEST(InstrumentStandard, InstrumentCreation) {
    music::Instrument i("Example", 10, 5, 0);

    EXPECT_STREQ(i.getName().c_str(), "Example");
    EXPECT_EQ(i.getChannel(), 10);
    EXPECT_EQ(i.getProgram(), 5);
    EXPECT_EQ(i.getUnpitched(), 0);
}

TEST(InstrumentStandard, InstrumentSetters) {
    music::Instrument i("Example", 10, 5, 0);

    EXPECT_STREQ(i.getName().c_str(), "Example");
    EXPECT_EQ(i.getChannel(), 10);
    EXPECT_EQ(i.getProgram(), 5);
    EXPECT_EQ(i.getUnpitched(), 0);

    i.setName("Acoustic Grand Piano");

    EXPECT_STREQ(i.getName().c_str(), "Acoustic Grand Piano");
    EXPECT_EQ(i.getChannel(), 10);
    EXPECT_EQ(i.getProgram(), 5);
    EXPECT_EQ(i.getUnpitched(), 0);

    i.setChannel(1);

    EXPECT_STREQ(i.getName().c_str(), "Acoustic Grand Piano");
    EXPECT_EQ(i.getChannel(), 1);
    EXPECT_EQ(i.getProgram(), 5);
    EXPECT_EQ(i.getUnpitched(), 0);

    i.setChannel(0);

    EXPECT_STREQ(i.getName().c_str(), "Acoustic Grand Piano");
    EXPECT_EQ(i.getChannel(), 0);
    EXPECT_EQ(i.getProgram(), 5);
    EXPECT_EQ(i.getUnpitched(), 0);

    EXPECT_DEATH(i.setChannel(20), "");

    i.setProgram(17);

    EXPECT_STREQ(i.getName().c_str(), "Acoustic Grand Piano");
    EXPECT_EQ(i.getChannel(), 0);
    EXPECT_EQ(i.getProgram(), 17);
    EXPECT_EQ(i.getUnpitched(), 0);

    i.setUnpitched(8);

    EXPECT_STREQ(i.getName().c_str(), "Acoustic Grand Piano");
    EXPECT_EQ(i.getChannel(), 0);
    EXPECT_EQ(i.getProgram(), 17);
    EXPECT_EQ(i.getUnpitched(), 8);
}