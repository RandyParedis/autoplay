//
// Created by red on 20/10/18.
//

#include "../../main/music/Clef.h"
#include <gtest/gtest.h>

using namespace autoplay;

TEST(ClefStandard, ClefCreation) {
    EXPECT_DEATH(music::Clef c('D', 1, 4), "");

    music::Clef c('G', 1, 4);

    EXPECT_EQ(c.getSign(), 'G');
    EXPECT_EQ(c.getLine(), 1);
    EXPECT_EQ(c.getOctaveChange(), 4);
}

TEST(ClefStandard, DefaultClefs) {
    music::Clef c1 = music::Clef::Treble();
    music::Clef c2 = music::Clef::Bass();
    music::Clef c3 = music::Clef::Alto();

    EXPECT_EQ(c1.getSign(), 'G');
    EXPECT_EQ(c1.getLine(), 2);
    EXPECT_EQ(c1.getOctaveChange(), 0);

    EXPECT_EQ(c2.getSign(), 'F');
    EXPECT_EQ(c2.getLine(), 4);
    EXPECT_EQ(c2.getOctaveChange(), 0);

    EXPECT_EQ(c3.getSign(), 'C');
    EXPECT_EQ(c3.getLine(), 3);
    EXPECT_EQ(c3.getOctaveChange(), 0);
}

TEST(ClefStandard, ClefSetters) {
    music::Clef c('G', 1, 4);

    EXPECT_EQ(c.getSign(), 'G');
    EXPECT_EQ(c.getLine(), 1);
    EXPECT_EQ(c.getOctaveChange(), 4);

    c.setLine(2);

    EXPECT_EQ(c.getSign(), 'G');
    EXPECT_EQ(c.getLine(), 2);
    EXPECT_EQ(c.getOctaveChange(), 4);

    c.setOctaveChange(0);

    EXPECT_EQ(c.getSign(), 'G');
    EXPECT_EQ(c.getLine(), 2);
    EXPECT_EQ(c.getOctaveChange(), 0);

    EXPECT_DEATH(c.setSign('D'), "");

    c.setSign('F');

    EXPECT_EQ(c.getSign(), 'F');
    EXPECT_EQ(c.getLine(), 2);
    EXPECT_EQ(c.getOctaveChange(), 0);
}

TEST(ClefStandard, EqualityCheck) {
    music::Clef c1('G', 1, 4);
    music::Clef c2 = music::Clef::Treble();
    music::Clef c3 = music::Clef::Treble();

    EXPECT_NE(c1, c2);
    EXPECT_NE(c1, c3);
    EXPECT_EQ(c2, c3);

    c1.setLine(2);

    EXPECT_NE(c1, c2);
    EXPECT_NE(c1, c3);
    EXPECT_EQ(c2, c3);

    c1.setOctaveChange(0);

    EXPECT_EQ(c1, c2);
    EXPECT_EQ(c1, c3);
    EXPECT_EQ(c2, c3);
}