/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "SynchronizationTransmitter.hpp"

using namespace envire;

SynchronizationTransmitter::SynchronizationTransmitter(std::string const& name, TaskCore::TaskState initial_state)
    : SynchronizationTransmitterBase(name, initial_state), env(new envire::Environment()), envireEventDispatcher(NULL)
{
}

SynchronizationTransmitter::SynchronizationTransmitter(std::string const& name, RTT::ExecutionEngine* engine, TaskCore::TaskState initial_state)
    : SynchronizationTransmitterBase(name, engine, initial_state), env(new envire::Environment()), envireEventDispatcher(NULL)
{
}

SynchronizationTransmitter::~SynchronizationTransmitter()
{
}

void SynchronizationTransmitter::loadEnvironment(const std::string &path)
{
    if(envireEventDispatcher && envireEventDispatcher->isAttached())
        envireEventDispatcher->detachEventHandler(env.get());
    env.reset(envire::Environment::unserialize(path));
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
    
    delete envireEventDispatcher;
    envireEventDispatcher = new envire::BinaryEventDispatcher(_envire_event);
    envireEventDispatcher->useEventQueue(true);
    
    return true;
}

void SynchronizationTransmitter::updateHook()
{
    if(!envireEventDispatcher->isAttached() && _envire_event.connected())
        envireEventDispatcher->attachEventHandler(env.get());
    
    if(envireEventDispatcher->isAttached())
    {
        envireEventDispatcher->setTime(base::Time::now());
        envireEventDispatcher->flush();
    }
}

// void SynchronizationTransmitter::errorHook()
// {
//     SynchronizationTransmitterBase::errorHook();
// }

void SynchronizationTransmitter::stopHook()
{
    SynchronizationTransmitterBase::stopHook();
    
    delete envireEventDispatcher;
    envireEventDispatcher = NULL;
}

// void SynchronizationTransmitter::cleanupHook()
// {
//     SynchronizationTransmitterBase::cleanupHook();
// }
