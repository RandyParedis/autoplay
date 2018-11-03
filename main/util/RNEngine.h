//
// Created by red on 28/10/18.
//

#ifndef AUTOPLAY_RNENGINE_H
#define AUTOPLAY_RNENGINE_H

#include <boost/variant.hpp>
#include <trng/lcg64.hpp>
#include <trng/lcg64_shift.hpp>
#include <trng/mrg2.hpp>
#include <trng/mrg3.hpp>
#include <trng/mrg4.hpp>
#include <trng/mrg5.hpp>
#include <trng/mrg5s.hpp>
#include <trng/mt19937.hpp>
#include <trng/mt19937_64.hpp>
#include <trng/yarn2.hpp>
#include <trng/yarn3.hpp>
#include <trng/yarn4.hpp>
#include <trng/yarn5.hpp>
#include <trng/yarn5s.hpp>

namespace autoplay {
    namespace util {
        class RNEngine
        {
        public:
            /**
             * Set the type of the engine by string
             * @param type The type to set.
             */
            void operator()(const std::string& type);

            /**
             * Set the type of the engine by string
             * @param type  The type to set.
             * @param seed  The seed of the engine.
             */
            void operator()(const std::string& type, unsigned long seed);

            /**
             * Seed the engine
             * @param seed The seed of the engine
             */
            void seed(unsigned long seed);

            /**
             * Call something that takes the engine as argument.
             * @tparam R    The return value type
             * @tparam T    The caller type
             * @param t     The caller
             * @return The result of t(rng)
             */
            template <typename R, typename T>
            R callOnMe(T& t) {
                switch(m_engine.which()) {
                case 0: return t(boost::get<trng::lcg64>(m_engine));
                case 1: return t(boost::get<trng::mrg5>(m_engine));
                default: return R();
                }
            }

        private:
            typedef boost::variant<trng::lcg64, trng::lcg64_shift, trng::mrg2, trng::mrg3, trng::mrg4, trng::mrg5,
                                   trng::mrg5s, trng::mt19937, trng::mt19937_64, trng::yarn2, trng::yarn3, trng::yarn4,
                                   trng::yarn5, trng::yarn5s>
                        engine_type; ///< The engine set/union
            engine_type m_engine;    ///< The engine
        };
    }
}

#endif // AUTOPLAY_RNENGINE_H
