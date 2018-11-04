//
// Created by red on 20/10/18.
//

#include "../../main/music/Instrument.h"
#include "../../main/music/Part.h"
#include <gtest/gtest.h>
#include <memory>

using namespace autoplay;

TEST(PartStandard, PartCreation) {
    music::Part p;

    EXPECT_EQ(p.getInstrument(), nullptr);
    EXPECT_EQ(p.getMeasures().size(), 0);

    auto i = std::make_shared<music::Instrument>("Acoustic Grand Piano", 1, 1, 0);
    p      = music::Part{i};

    EXPECT_EQ(p.getInstrument(), i);
    EXPECT_EQ(p.getMeasures().size(), 0);
}

TEST(PartStandard, PartSetters) {
    music::Part p;

    EXPECT_EQ(p.getInstrument(), nullptr);
    EXPECT_EQ(p.getMeasures().size(), 0);

    auto i = std::make_shared<music::Instrument>("Acoustic Grand Piano", 1, 1, 0);
    p.setInstrument(i);

    EXPECT_EQ(p.getInstrument(), i);
    EXPECT_EQ(p.getMeasures().size(), 0);

    music::Measure m1{music::Clef::Treble(), {3, 4}, 12};
    p.setMeasures({m1});

    EXPECT_EQ(p.getInstrument(), i);
    EXPECT_EQ(p.getMeasures().size(), 1);

    p.setMeasures(m1);

    EXPECT_EQ(p.getInstrument(), i);
    EXPECT_EQ(p.getMeasures().size(), 1);

    music::Measure m{music::Clef::Treble(), {3, 4}, 12};

    for(uint8_t k = 0; k < 3; ++k) {
        music::Note n{(uint8_t)(k + 12), 24};
        m.append(n);
    }

    p.setMeasures(m);

    EXPECT_EQ(p.getInstrument(), i);
    EXPECT_EQ(p.getMeasures().size(), 2);

    EXPECT_EQ(p.at(36)->getPitch(), 13);
}