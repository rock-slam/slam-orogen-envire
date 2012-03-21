/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "SynchronizationTransmitter.hpp"

using namespace envire;

SynchronizationTransmitter::SynchronizationTransmitter(std::string const& name, TaskCore::TaskState initial_state)
    : SynchronizationTransmitterBase(name, initial_state), env(new Environment())
{
}

SynchronizationTransmitter::SynchronizationTransmitter(std::string const& name, RTT::ExecutionEngine* engine, TaskCore::TaskState initial_state)
    : SynchronizationTransmitterBase(name, engine, initial_state), env(new Environment())
{
}

SynchronizationTransmitter::~SynchronizationTransmitter()
{
}

void SynchronizationTransmitter::loadEnvironment(const std::string &path)
{
    env.reset(envire::Environment::unserialize(path));
    env->addEventHandler(this);
}

void SynchronizationTransmitter::handle(envire::EnvireBinaryEvent* binary_event)
{
    // save binary event and trigger update hook
    mutex.lock();
        buffer.push_back(binary_event);
    mutex.unlock();
    
    this->getActivity()->trigger();
}


/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See SynchronizationTransmitter.hpp for more detailed
// documentation about them.

// bool SynchronizationTransmitter::configureHook()
// {
//     if (! SynchronizationTransmitterBase::configureHook())
//         return false;
//     return true;
// }

bool SynchronizationTransmitter::startHook()
{
    if (! SynchronizationTransmitterBase::startHook())
        return false;
    
    env->addEventHandler(this);
    
    return true;
}

void SynchronizationTransmitter::updateHook()
{
    while(buffer.size() > 0)
    {
        // write binary events to port
        mutex.lock();
            envire::EnvireBinaryEvent* binary_event = buffer.front();
            buffer.pop_front();
        mutex.unlock();
        
        if(binary_event)
        {
            _envire_event.write(*binary_event);
        }
        
        delete binary_event;
    }
}

// void SynchronizationTransmitter::errorHook()
// {
//     SynchronizationTransmitterBase::errorHook();
// }

void SynchronizationTransmitter::stopHook()
{
    SynchronizationTransmitterBase::stopHook();
    
    env->removeEventHandler(this);
}

// void SynchronizationTransmitter::cleanupHook()
// {
//     SynchronizationTransmitterBase::cleanupHook();
// }
