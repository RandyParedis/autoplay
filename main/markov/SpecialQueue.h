/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Randy Paredis
 *
 *  Created on 02/12/18
 */

#ifndef AUTOPLAY_SPECIALQUEUE_H
#define AUTOPLAY_SPECIALQUEUE_H

#include <queue>
#include <vector>

namespace autoplay {
    namespace markov {

        /**
         * The SpecialQueue class is a class that handles a very specific implementation of a queue,
         * where each element is also a container (= vector).
         * @tparam T The element type, that will be put inside a container.
         */
        template <typename T>
        class SpecialQueue
        {
        public:
            using queue_element_type = std::vector<T>;                 ///< The queue element type
            using queue_type         = std::queue<queue_element_type>; ///< The type of the queue (internally)

            /**
             * Enqueue an element on to the queue, or add it to the lastly added container.
             * @param element   The element to add.
             * @param to_top    When true, the element will be added to the lastly added container.
             *
             * @note If the queue is empty and to_top is true, a normal enqueue will be performed.
             */
            void enqueue(const T& element, bool to_top = false) {
                if(to_top && !m_queue.empty()) {
                    m_queue.back().emplace_back(element);
                } else {
                    m_queue.push({element});
                }
            }

            /**
             * Dequeue an element from the queue.
             */
            void dequeue() { m_queue.pop(); }

            /**
             * Get the front of the queue.
             * @return The front.
             */
            queue_element_type& front() { return m_queue.front(); }

        private:
            queue_type m_queue; ///< The queue that's used internally
        };
    }
}

#endif // AUTOPLAY_SPECIALQUEUE_H
