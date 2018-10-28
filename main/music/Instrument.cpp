//
// Created by red on 25/10/18.
//

#include "Instrument.h"

namespace music {
    namespace instruments {
        std::shared_ptr<Instrument> findByName(const std::string& name) {
            for(auto& instrument: all) {
                if(instrument->getName() == name) {
                    return instrument;
                }
            }
            return nullptr;
        }
    }
}