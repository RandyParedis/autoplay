//
// Created by red on 14/10/18.
//

#ifndef AUTOPLAY_RANDOMIZER_H
#define AUTOPLAY_RANDOMIZER_H

#include <trng/src/uniform_dist.hpp>
#include <trng/src/uniform_int_dist.hpp>
#include <functional>
#include <algorithm>
#include <random>

/**
 * The Randomizer struct is a container of all random/choice functions used
 */
struct Randomizer {
    /**
     * Choose an integer element uniformly from a range
     * @tparam G        generator type
     * @param gen       A random engine generator
     * @param min       The minimum of the range
     * @param max       The maximum of the range
     * @return One randomly selected element in a range
     */
    template <typename G>
    static int pick_uniform(G& gen, const int& min, const int& max) {
        trng::uniform_int_dist U(min, max);
        return U(gen);
    }

    /**
     * Choose an element uniformly from a range
     * @tparam T        element type
     * @tparam G        generator type
     * @param gen       A random engine generator
     * @param min       The minimum of the range
     * @param max       The maximum of the range
     * @return One randomly selected element in a range
     */
    template <typename T, typename G>
    static T pick_uniform(G& gen, const T& min, const T& max) {
        trng::uniform_dist<T> U(min, max);
        return U(gen);
    }

    /**
     * Choose an element uniformly from the series of elements
     * @tparam T        element type
     * @tparam C        container type
     * @tparam G        generator type
     * @param gen       A random engine generator
     * @param elements  container of elements
     * @return One randomly selected element
     */
    template <template <typename> class C, typename T, typename G>
    static T pick_uniform(const G& gen, const C<T>& elements) {
        trng::uniform_int_dist U(0, elements.size());
        return elements.at(U(gen));
    }

    /**
     * Picks a weighted element from a range.
     * @tparam T        element type
     * @tparam G        generator type
     * @param gen       A random engine generator
     * @param min       The minimum of the range
     * @param max       The maximum of the range (exclusive)
     * @param step      The size of each step in the range
     * @param weight    A weight function that can be used to determine the weight of each element in the range
     * @return One randomly selected element from a range. If none is found, returns the max.
     */
    template <typename T, typename G>
    static T pick_weighted(const G& gen, const T& min, const T& max, const T& step,
                           std::function<float (const T&)> weight) {
        float ws = 0.0f;
        for(T i = min; i < max; i += step) {
            ws += weight(i);
        }
        trng::uniform_dist<float> U(1, ws);
        float rw = U(gen);
        for(T i = min; i < max; i += step) {
            rw -= weight(i);
            if(rw <= 0) {
                return i;
            }
        }
        return max;
    }

    /**
     * Picks a weighted element from a series of elements.
     * @tparam T        element type
     * @tparam C        container type
     * @tparam G        generator type
     * @param gen       A random engine generator
     * @param elements  Container of elements
     * @param weight    A weight function that can be used to determine the weight of each element in the container
     * @return One randomly selected element
     */
    template <template <typename> class C, typename T, typename G>
    static T pick_weighted(const G& gen, const C<T>& elements, std::function<float (const T&)> weight) {
        float ws = 0.0f;
        for(const auto& w: elements) {
            ws += weight(w);
        }
        trng::uniform_dist<float> U(1, ws);
        float rw = U(gen);
        for(const auto& w: elements) {
            rw -= weight(w);
            if(rw <= 0) {
                return w;
            }
        }
        return elements.back();
    }

    /**
     * Picks a weighted element from a range, with a ranged/skewed weight/distribution function.
     * @tparam G        generator type
     * @param gen       A random engine generator
     * @param min       The minimum of the range
     * @param max       The maximum of the range (exclusive)
     * @param step      The size of each step in the range
     * @param weight    A weight function that can be used to determine the weight of each element in the range
     * @param a         The minimal value of the skewed domain
     * @param b         The maximal value of the skewed domain
     * @param fix_zero  If the zero-value should remain fixed in the computation or not.
     *                  This will determine the scale factor by the smallest value to 0,
     *                  instead of the width of the container.
     *
     * @note    This function will do a transformation on the domain of the elements container.
     *          It will basically map the indexes of each element uniformly over the domain of
     *          [a, b] before using the index as an input-value for the weight function.
     *          This makes it easier to chose a random value based upon a distribution.
     *
     * @return One randomly selected element from a range. If none is found, returns the max.
     */
    template <typename G>
    static float pick_distributed(const G& gen, const float& min, const float& max, const float& step,
                                  std::function<float (const float&)> dist, float a, float b, bool fix_zero = false) {
        float ws = 0.0f;
        float factor = (b - a)/(max - min);
        if(fix_zero) {
            factor = (b - a)/(std::min(min, max) * 2);
        }
        const auto probability = [&dist, &a, &min, &factor](const float& val) -> float {
            return dist(a + (val - min)*factor);
        };
        for(float i = min; i < max; i += step) {
            ws += probability(i);
        }
        trng::uniform_dist<float> U(1, ws);
        float rw = U(gen);
        for(float i = min; i < max; i += step) {
            rw -= probability(i);
            if(rw <= 0) {
                return i;
            }
        }
        return max;
    }

    /**
     * Picks a weighted element from a series of elements, with a ranged/skewed weight/distribution function.
     * @tparam C        container type
     * @tparam G        generator type
     * @param gen       A random engine generator
     * @param elements  Container of elements
     * @param weight    A weight function that can be used to determine the weight of each element in the container
     * @param a         The minimal value of the skewed domain
     * @param b         The maximal value of the skewed domain
     * @param fix_zero  If the zero-value should remain fixed in the computation or not.
     *                  This will determine the scale factor by the smallest value to 0,
     *                  instead of the width of the container.
     *
     * @note    This function will do a transformation on the domain of the elements container.
     *          It will basically map the indexes of each element uniformly over the domain of
     *          [a, b] before using the index as an input-value for the weight function.
     *          This makes it easier to chose a random value based upon a distribution.
     *
     * @return One randomly selected element
     */
    template <template <typename> class C, typename G>
    static float pick_distributed(const G& gen, const C<float>& elements, std::function<float (const float&)> dist,
                                  float a, float b, bool fix_zero = false) {
        float ws = 0.0f;
        float min = std::min_element(std::begin(elements), std::end(elements));
        float max = std::max_element(std::begin(elements), std::end(elements));
        float factor = (b - a)/(max - min);
        if(fix_zero) {
            factor = (b - a)/(std::min(min, max) * 2);
        }
        const auto probability = [&dist, &a, &min, &factor](const long idx) -> float {
            return dist(a + (idx - min)*factor);
        };
        for(long i = 0; i < elements.size(); ++i) {
            ws += probability(i);
        }
        trng::uniform_dist<float> U(1, ws);
        float rw = U(gen);
        for(long i = 0; i < elements.size(); ++i) {
            rw -= probability(i);
            if(rw <= 0) {
                return elements.at(i);
            }
        }
        return elements.back();
    }

    /**
     * Picks a weighted element from a range, with respect to a standard normal (Gaussian) distribution.
     * Each element has a bigger chance of being chosen if it's in the middle of the range.
     * @tparam G        generator type
     * @param gen       A random engine generator
     * @param min       The minimum of the range
     * @param max       The maximum of the range (exclusive)
     * @param step      The size of each step in the range
     * @param a         The minimal value of the skewed domain
     * @param b         The maximal value of the skewed domain
     * @param fix_zero  If the zero-value should remain fixed in the computation or not.
     *                  This will determine the scale factor by the smallest value to 0,
     *                  instead of the width of the container.
     *
     * @return One randomly selected element from range. If none is found, it returns max.
     */
    template <typename G>
    static float gaussian(const G& gen, const float& min, const float& max, const float& step,
                          float a=-3.0f, float b=3.0f, bool fix_zero=false) {
        std::normal_distribution<float> distribution;
        return pick_distributed(gen, min, max, step, distribution, a, b, fix_zero);
    };

    /**
     * Picks a weighted element from a container, with respect to a standard normal (Gaussian) distribution.
     * Each element has a bigger chance of being chosen if it's in the middle of the container.
     * @tparam C        container type
     * @tparam G        generator type
     * @param gen       A random engine generator
     * @param elements  Container of elements
     * @param a         The minimal value of the skewed domain
     * @param b         The maximal value of the skewed domain
     * @param fix_zero  If the zero-value should remain fixed in the computation or not.
     *                  This will determine the scale factor by the smallest value to 0,
     *                  instead of the width of the container.
     *
     * @return One randomly selected element.
     */
    template <template <typename> class C, typename G>
    static float gaussian(const G& gen, const C<float>& elements, float a=-3.0f, float b=3.0f, bool fix_zero=false) {
        std::normal_distribution<float> distribution;
        return pick_distributed(gen, elements, distribution, a, b, fix_zero);
    };
};


#endif //AUTOPLAY_RANDOMIZER_H
