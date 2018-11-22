//
// Created by red on 20/10/18.
//

#include "../../main/music/Note.h"
#include <gtest/gtest.h>

using namespace autoplay;

TEST(NoteStandard, PauseCreation) {
    unsigned int dur = 12;

    music::Note n{dur};
    EXPECT_EQ(n.getDuration(), dur);
    EXPECT_EQ(n.getPitch(), 0);
    EXPECT_EQ(n.getVelocityOn(), 0);
    EXPECT_EQ(n.getVelocityOff(), 0);
    EXPECT_TRUE(n.isPause());
    EXPECT_FALSE(n.isNote());
}

TEST(NoteStandard, NoteDefaultVelocityCreation) {
    unsigned int dur   = 12;
    uint8_t      pitch = 69; // A4

    music::Note n{pitch, dur};
    EXPECT_EQ(n.getDuration(), dur);
    EXPECT_EQ(n.getPitch(), pitch);
    EXPECT_EQ(n.getVelocityOn(), music::Note::DEFAULT_ON);
    EXPECT_EQ(n.getVelocityOff(), music::Note::DEFAULT_OFF);
    EXPECT_FALSE(n.isPause());
    EXPECT_TRUE(n.isNote());
}

TEST(NoteStandard, NoteConstantVelocityCreation) {
    unsigned int dur   = 12;
    uint8_t      pitch = 69; // A4
    uint8_t      vel   = 50;

    music::Note n{pitch, vel, dur};
    EXPECT_EQ(n.getDuration(), dur);
    EXPECT_EQ(n.getPitch(), pitch);
    EXPECT_EQ(n.getVelocityOn(), vel);
    EXPECT_EQ(n.getVelocityOff(), vel);
    EXPECT_FALSE(n.isPause());
    EXPECT_TRUE(n.isNote());
}

TEST(NoteStandard, NoteNormalCreation) {
    unsigned int dur     = 12;
    uint8_t      pitch   = 69; // A4
    uint8_t      vel_on  = 63;
    uint8_t      vel_off = 75;

    music::Note n{pitch, vel_on, vel_off, dur};
    EXPECT_EQ(n.getDuration(), dur);
    EXPECT_EQ(n.getPitch(), pitch);
    EXPECT_EQ(n.getVelocityOn(), vel_on);
    EXPECT_EQ(n.getVelocityOff(), vel_off);
    EXPECT_FALSE(n.isPause());
    EXPECT_TRUE(n.isNote());

    pitch = 128;
    EXPECT_DEATH(music::Note m(pitch, vel_on, vel_off, dur), "");

    pitch  = 69;
    vel_on = 128;
    EXPECT_DEATH(music::Note m(pitch, vel_on, vel_off, dur), "");

    vel_on  = music::Note::DEFAULT_ON;
    vel_off = 128;
    EXPECT_DEATH(music::Note m(pitch, vel_on, vel_off, dur), "");
}

TEST(NoteStandard, NoteSetters) {
    unsigned int dur     = 12;
    uint8_t      pitch   = 69; // A4
    uint8_t      vel_on  = 63;
    uint8_t      vel_off = 75;

    music::Note n{pitch, vel_on, vel_off, dur};
    n.setDuration(8);
    EXPECT_EQ(n.getDuration(), 8);
    n.setPitch(60); // C4
    EXPECT_EQ(n.getPitch(), 60);
    n.setVelocityOn(music::Note::DEFAULT_ON);
    EXPECT_EQ(n.getVelocityOn(), music::Note::DEFAULT_ON);
    n.setVelocityOff(music::Note::DEFAULT_OFF);
    EXPECT_EQ(n.getVelocityOff(), music::Note::DEFAULT_OFF);

    EXPECT_DEATH(n.setPitch(128), "");
    EXPECT_DEATH(n.setVelocityOn(128), "");
    EXPECT_DEATH(n.setVelocityOff(128), "");

    n.setTieStart();

    EXPECT_TRUE(n.getTieStart());

    n.setTieEnd();

    EXPECT_TRUE(n.getTieEnd());
}

TEST(NoteMessages, OnMessage) {
    unsigned int dur     = 12;
    uint8_t      pitch   = 69; // A4
    uint8_t      vel_on  = music::Note::DEFAULT_ON;
    uint8_t      vel_off = music::Note::DEFAULT_OFF;

    music::Note                n1{pitch, vel_on, vel_off, dur};
    std::vector<unsigned char> res = {0x90, pitch, vel_on};
    EXPECT_EQ(n1.getOnMessage(), n1.getMessage());
    EXPECT_EQ(n1.getOnMessage(), res);

    music::Note n2{pitch, dur};
    EXPECT_EQ(n2.getOnMessage(), n2.getMessage());
    EXPECT_EQ(n2.getOnMessage(), res);
}

TEST(NoteMessages, OffMessage) {
    unsigned int dur     = 12;
    uint8_t      pitch   = 69; // A4
    uint8_t      vel_on  = music::Note::DEFAULT_ON;
    uint8_t      vel_off = music::Note::DEFAULT_OFF;

    music::Note                n1{pitch, vel_on, vel_off, dur};
    std::vector<unsigned char> res = {0x80, pitch, vel_off};
    EXPECT_EQ(n1.getOffMessage(), n1.getMessage(0, false));
    EXPECT_EQ(n1.getOffMessage(), res);

    music::Note n2{pitch, dur};
    EXPECT_EQ(n2.getOffMessage(), n2.getMessage(0, false));
    EXPECT_EQ(n2.getOffMessage(), res);
}

TEST(NoteMessages, ChannelMessage) {
    unsigned int dur     = 12;
    uint8_t      pitch   = 69; // A4
    uint8_t      vel     = music::Note::DEFAULT_ON;
    uint8_t      channel = 5;

    music::Note                n{pitch, vel, dur};
    std::vector<unsigned char> res = {0x95, pitch, vel};
    EXPECT_EQ(n.getOnMessage(channel), res);

    res[0] = 0x85;
    EXPECT_EQ(n.getMessage(channel, false), res);
    EXPECT_EQ(n.getOffMessage(channel), res);
}

TEST(NoteConversions, PitchStringConversion) {
    EXPECT_EQ(music::Note::pitch("A4"), 69);
    EXPECT_EQ(music::Note::pitch("Bb4"), 70);
    EXPECT_EQ(music::Note::pitch("C-1"), 0);
    EXPECT_EQ(music::Note::pitch("D4", music::Note::Semitone::SHARP), 63);
}

TEST(NoteConversions, PitchHertzConversion) {
    EXPECT_EQ(music::Note::pitch(440), 69);
    EXPECT_EQ(music::Note::pitch(27.5), 21);
    EXPECT_EQ(music::Note::pitch(261.63), 60);
    EXPECT_EQ(music::Note::pitch(138.59), 49);
}

TEST(NoteConversions, PitchReprConversion) {
    music::Note::Semitone s;
    std::string           res = music::Note::pitchRepr(69, s);
    EXPECT_STREQ(res.c_str(), "A4");
    EXPECT_EQ(s, music::Note::Semitone::NONE);

    res = music::Note::pitchRepr(70, s);
    EXPECT_STREQ(res.c_str(), "B4");
    EXPECT_EQ(s, music::Note::Semitone::FLAT);

    res = music::Note::pitchRepr(music::Note::pitch(440), s);
    EXPECT_STREQ(res.c_str(), "A4");

    EXPECT_STREQ(music::Note::pitchRepr(70).c_str(), "Bb4");

    auto p = music::Note::splitPitch("Bb4", s);
    EXPECT_EQ(p.first, 'B');
    EXPECT_EQ(p.second, 4);
    EXPECT_EQ(s, music::Note::Semitone::FLAT);
}

TEST(NoteComparisons, NoteComparisons) {
    music::Note p{12};
    music::Note n1{60, 12};
    music::Note n2{60, 12};
    music::Note n3{0, 12};
    music::Note n4{60, 8};
    music::Note n5{60, 50, 12};
    music::Note n6{60, 50, 40, 12};

    EXPECT_EQ(n1, n2);
    EXPECT_EQ(n2, n1);

    EXPECT_NE(p, n1);
    EXPECT_NE(p, n2);
    EXPECT_NE(p, n3);

    EXPECT_NE(n1, n5);
    EXPECT_NE(n4, n5);
    EXPECT_NE(p, n5);

    EXPECT_NE(n1, n6);
    EXPECT_NE(n5, n6);
    EXPECT_NE(p, n6);

    EXPECT_LT(n6, n5);
    EXPECT_LE(n6, n5);
    EXPECT_LE(n1, n2);
    EXPECT_GE(n1, n2);
    EXPECT_GE(n5, n6);
    EXPECT_GT(n5, n6);
    EXPECT_GT(n1, n3);
    EXPECT_GE(n1, n3);
    EXPECT_GT(n1, n4);
    EXPECT_GE(n1, n4);
}