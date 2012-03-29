/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "SynchronizationTransmitter.hpp"
#include "Orocos.hpp"

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
    env.reset(envire::Environment::unserialize(path));
    envire::OrocosEmitter emitter(_envire_events);
    emitter.setTime(base::Time::now());
    emitter.attach(env.get());
    emitter.flush();
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
    
    return true;
}

void SynchronizationTransmitter::updateHook()
{
}

// void SynchronizationTransmitter::errorHook()
// {
//     SynchronizationTransmitterBase::errorHook();
// }

void SynchronizationTransmitter::stopHook()
{
    SynchronizationTransmitterBase::stopHook();
}

// void SynchronizationTransmitter::cleanupHook()
// {
//     SynchronizationTransmitterBase::cleanupHook();
// }
