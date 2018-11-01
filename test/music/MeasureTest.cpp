//
// Created by red on 21/10/18.
//

#include "../../main/music/Measure.h"
#include <gtest/gtest.h>

TEST(MeasureStandard, MeasureCreation) {
    music::Measure m{};
    EXPECT_FALSE(m.hasAttributes());
    EXPECT_EQ(m.max_length(), 0);
    EXPECT_TRUE(m.isOverflowing());

    music::Clef c = music::Clef::Treble();

    EXPECT_DEATH(music::Measure n(c, {0, 0}, 0), "");
    EXPECT_DEATH(music::Measure n(c, {0, 0}, 1), "");
    EXPECT_DEATH(music::Measure n(c, {0, 1}, 0), "");
    EXPECT_DEATH(music::Measure n(c, {0, 1}, 1), "");
    EXPECT_DEATH(music::Measure n(c, {1, 0}, 0), "");
    EXPECT_DEATH(music::Measure n(c, {1, 0}, 1), "");
    EXPECT_DEATH(music::Measure n(c, {1, 1}, 0), "");
}

TEST(MeasureStandard, MeasureGettersSetters) {
    music::Clef    f = music::Clef::Bass();
    music::Measure m{};

    m.setClef(f);
    EXPECT_EQ(f, m.getClef());

    m.setTime(2, 4);
    std::pair<uint8_t, uint8_t> p{2, 4};
    EXPECT_EQ(m.getTime(), p);
    m.setTime({3, 4});
    p.first  = 3;
    p.second = 4;
    EXPECT_EQ(m.getTime(), p);
    m.setCommonTime();
    p.first = 4;
    EXPECT_EQ(m.getTime(), p);
    p.first  = 2;
    p.second = 2;
    m.setCutTime();
    EXPECT_EQ(m.getTime(), p);

    EXPECT_DEATH(m.setTime(2, 3), "");

    m.setStaves(5);
    EXPECT_EQ(m.getStaves(), 5);
    m.setStaves();
    EXPECT_EQ(m.getStaves(), 1);

    m.setDivisions(4);
    EXPECT_EQ(m.getDivisions(), 4);
}

TEST(MeasureStandard, MeasureLength) {
    music::Clef    c = music::Clef::Alto();
    music::Measure m{c, {3, 4}, 24};

    EXPECT_EQ(m.max_length(), 72);

    music::Note n1{16};
    m += n1;
    EXPECT_EQ(m.length(), 16);
    music::Note n2{16};
    m.append(n2);
    EXPECT_EQ(m.length(), 32);
    EXPECT_FALSE(m.isOverflowing());

    music::Note n3{41};
    m += n3;
    EXPECT_TRUE(m.isOverflowing());
}

TEST(MeasureStandard, Measurize) {
    music::Clef    c = music::Clef::Alto();
    music::Measure m{c, {3, 4}, 12};

    for(uint8_t i = 0; i < 3; ++i) {
        music::Note n{24};
        m.append(n);
    }

    EXPECT_TRUE(m.isOverflowing());
    EXPECT_EQ(m.length(), 72);

    auto ms = m.measurize();
    EXPECT_EQ(ms.size(), 2);
    EXPECT_FALSE(ms[0]->isOverflowing());
    EXPECT_EQ(ms[0]->length(), m.max_length());
    EXPECT_FALSE(ms[1]->isOverflowing());
    EXPECT_EQ(ms[1]->length(), m.max_length());
}