//
// Created by red on 27/10/18.
//

#ifndef AUTOPLAY_SCORE_H
#define AUTOPLAY_SCORE_H

#include "Part.h"

namespace music {
/**
 * The Score struct contains all detailed data about a certain Score.
 *
 * @note Currently, this is just a container that holds a series of Parts.
 *
 * TODO: Add (possibly randomized) data for the 'optional header data' of a
 * Score
 */
struct Score {
    std::vector<std::shared_ptr<Part>> parts;
};
}
#endif // AUTOPLAY_SCORE_H
