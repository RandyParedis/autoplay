//
// Created by red on 14/10/18.
//

#ifndef AUTOPLAY_RANDOMIZER_H
#define AUTOPLAY_RANDOMIZER_H

#include <trng/uniform_dist.hpp>
#include <trng/uniform_int_dist.hpp>
#include <algorithm>
#include <functional>
#include <random>

#include "RNEngine.h"

namespace autoplay {
    namespace util {
        /**
         * The Randomizer struct is a container of all random/choice functions used
         */
        struct Randomizer {
            /**
             * Choose an integer element uniformly from a range
             * @param gen       A random engine generator
             * @param min       The minimum of the range
             * @param max       The maximum of the range (exclusive)
             * @return One randomly selected element in a range
             */
            static int pick_uniform(RNEngine& gen, const int& min, const int& max) {
                trng::uniform_int_dist U(min, max);
                return gen.callOnMe<int>(U);
            }

            /**
             * Choose an element uniformly from a range
             * @tparam T        element type
             * @param gen       A random engine generator
             * @param min       The minimum of the range
             * @param max       The maximum of the range
             * @return One randomly selected element in a range
             */
            template <typename T>
            static T pick_uniform(RNEngine& gen, const T& min, const T& max) {
                trng::uniform_dist<T> U(min, max);
                return gen.callOnMe<T>(U);
            }

            /**
             * Choose an element uniformly from the series of elements
             * @tparam C        container type
             * @tparam T        element type
             * @tparam Ts       other template types
             * @param gen       A random engine generator
             * @param elements  container of elements
             * @return One randomly selected element
             */
            template <template <typename...> class C, typename T, typename... Ts>
            static T pick_uniform(RNEngine& gen, const C<T, Ts...>& elements) {
                trng::uniform_int_dist U(0, elements.size());
                return elements.at(gen.callOnMe<T>(U));
            }

            /**
             * Picks a weighted element from a range.
             * @tparam T        element type
             * @param gen       A random engine generator
             * @param min       The minimum of the range
             * @param max       The maximum of the range (exclusive)
             * @param step      The size of each step in the range
             * @param weight    A weight function that can be used to determine the weight
             *                  of each element in the range
             * @return One randomly selected element from a range. If none is found,
             * returns the max.
             */
            template <typename T>
            static T pick_weighted(RNEngine& gen, const T& min, const T& max, const T& step,
                                   std::function<float(const T&)> weight) {
                float ws = 0.0f;
                for(T i = min; i < max; i += step) {
                    ws += weight(i);
                }
                trng::uniform_dist<float> U(0.0f, ws);

                auto rw = gen.callOnMe<float>(U);
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
             * @tparam Ts       Other types
             * @param gen       A random engine generator
             * @param elements  Container of elements
             * @param weight    A weight function that can be used to determine the weight
             *                  of each element in the container
             * @return One randomly selected element
             */
            template <template <typename...> class C, typename T, typename... Ts>
            static T pick_weighted(RNEngine& gen, const C<T, Ts...>& elements, std::function<float(const T&)> weight) {
                float ws = 0.0f;
                for(const auto& w : elements) {
                    ws += weight(w);
                }
                trng::uniform_dist<float> U(0, ws);

                auto rw = gen.callOnMe<float>(U);
                for(const auto& w : elements) {
                    rw -= weight(w);
                    if(rw <= 0) {
                        return w;
                    }
                }
                return elements.back();
            }

            /**
             * Picks a weighted element from a range, with a ranged/skewed
             * weight/distribution function.
             * @param gen       A random engine generator
             * @param min       The minimum of the range
             * @param max       The maximum of the range (exclusive)
             * @param step      The size of each step in the range
             * @param weight    A weight function that can be used to determine the weight
             *                  of each element in the range
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
            static float pick_distributed(RNEngine& gen, const float& min, const float& max, const float& step,
                                          std::function<float(const float&)> dist, float a, float b,
                                          bool fix_zero = false) {
                float ws     = 0.0f;
                float factor = (b - a) / (max - min);
                if(fix_zero) {
                    factor = (b - a) / (std::min(min, max) * 2);
                }
                const auto probability = [&dist, &a, &min, &factor](const float& val) -> float {
                    return dist(a + (val - min) * factor);
                };
                for(float i = min; i < max; i += step) {
                    ws += probability(i);
                }
                trng::uniform_dist<float> U(0, ws);

                auto rw = gen.callOnMe<float>(U);
                for(float i = min; i < max; i += step) {
                    rw -= probability(i);
                    if(rw <= 0) {
                        return i;
                    }
                }
                return max;
            }

            /**
             * Picks a weighted element from a series of elements, with a ranged/skewed
             * weight/distribution function.
             * @tparam C        container type
             * @tparam Ts       other template values
             * @param gen       A random engine generator
             * @param elements  Container of elements
             * @param weight    A weight function that can be used to determine the weight
             *                  of each element in the container
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
            template <template <typename...> class C, typename... Ts>
            static float pick_distributed(RNEngine& gen, const C<float, Ts...>& elements,
                                          std::function<float(const float&)> dist, float a, float b,
                                          bool fix_zero = false) {
                float ws     = 0.0f;
                auto  _min   = std::min_element(std::begin(elements), std::end(elements));
                auto  _max   = std::max_element(std::begin(elements), std::end(elements));
                float min    = std::distance(std::begin(elements), _min);
                float max    = std::distance(std::begin(elements), _max);
                float factor = (b - a) / (max - min);
                if(fix_zero) {
                    factor = (b - a) / (std::min(min, max) * 2);
                }
                const auto probability = [&dist, &a, &min, &factor](const long idx) -> float {
                    return dist(a + (idx - min) * factor);
                };
                for(long i = 0; i < (signed)elements.size(); ++i) {
                    ws += probability(i);
                }
                trng::uniform_dist<float> U(0, ws);

                auto rw = gen.callOnMe<float>(U);
                for(long i = 0; i < (signed)elements.size(); ++i) {
                    rw -= probability(i);
                    if(rw <= 0) {
                        return elements.at(i);
                    }
                }
                return elements.back();
            }

            /**
             * Easy-to-use function that implements the gauss curve we all know and love.
             * @param x     X-value
             * @param mu    Mean
             * @param sigma Standard Derivation
             * @return The y-value of the normal distribution (distributed with mu and sigma) for a given x.
             */
            static float gauss_curve(const float& x, const float& mu = 0.0f, const float& sigma = 1.0f) {
                float res = 1.0f / (sigma * std::sqrt(2.0f * (float)M_PI));
                res *= std::exp(-std::pow(x - mu, 2.0f) / (2.0f * std::pow(sigma, 2.0f)));
                return res;
            }

            /**
             * Picks a weighted element from a range, with respect to a standard normal
             * (Gaussian) distribution.
             * Each element has a bigger chance of being chosen if it's in the middle of
             * the range.
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
            static float gaussian(RNEngine& gen, const float& min, const float& max, const float& step, float a = -3.0f,
                                  float b = 3.0f, bool fix_zero = false) {
                auto dst = [&](const float& f) -> float { return gauss_curve(f); };
                return pick_distributed(gen, min, max, step, dst, a, b, fix_zero);
            };

            /**
             * Picks a weighted element from a container, with respect to a standard
             * normal (Gaussian) distribution.
             * Each element has a bigger chance of being chosen if it's in the middle of
             * the container.
             * @tparam C        container type
             * @tparam Ts       other template values
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
            template <template <typename...> class C, typename... Ts>
            static float gaussian(RNEngine& gen, const C<float, Ts...>& elements, float a = -3.0f, float b = 3.0f,
                                  bool fix_zero = false) {
                auto dst = [&](const float& f) -> float { return gauss_curve(f); };
                return pick_distributed(gen, elements, dst, a, b, fix_zero);
            };
        };
    }
}

#endif // AUTOPLAY_RANDOMIZER_H
