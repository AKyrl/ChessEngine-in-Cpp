#include "EngineFactory.hpp"

std::unique_ptr<Engine> EngineFactory::createEngine() {
    
    std::unique_ptr<Engine> myEngine (new ChessEngine);

    return myEngine;
    
}
