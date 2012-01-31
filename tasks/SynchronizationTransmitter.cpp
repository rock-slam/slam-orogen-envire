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


void SynchronizationTransmitter::handle(envire::EnvireBinaryEvent* binary_event)
{
    // save binary event and trigger update hook
    buffer.push_back(binary_event);
    
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
    
    buffer.set_capacity(50);
    env->addEventHandler(this);
    
    return true;
}

void SynchronizationTransmitter::updateHook()
{
    while(buffer.size() > 0)
    {
        // write binary events to port
        envire::EnvireBinaryEvent* binary_event = buffer.front();
        buffer.pop_front();
        
        if(binary_event)
        {
            _evire_event.write(*binary_event);
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