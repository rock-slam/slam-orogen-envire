/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "ConfigurableSynchronizationTransmitter.hpp"
#include "Orocos.hpp"

using namespace envire;

ConfigurableSynchronizationTransmitter::ConfigurableSynchronizationTransmitter(std::string const& name)
    : ConfigurableSynchronizationTransmitterBase(name), initial_map_sent(false),env(new envire::Environment()), envireEventDispatcher(NULL)
{
}

ConfigurableSynchronizationTransmitter::ConfigurableSynchronizationTransmitter(std::string const& name, RTT::ExecutionEngine* engine)
    : ConfigurableSynchronizationTransmitterBase(name, engine), initial_map_sent(false), env(new envire::Environment()), envireEventDispatcher(NULL)
{
}

ConfigurableSynchronizationTransmitter::~ConfigurableSynchronizationTransmitter()
{
}

void ConfigurableSynchronizationTransmitter::loadEnvironment(const std::string &path)
{
    env.reset(envire::Environment::unserialize(path));
    envire::OrocosEmitter emitter(_envire_events);
    emitter.setTime(base::Time::now());
    emitter.attach(env.get());
    emitter.flush();
}


/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See ConfigurableSynchronizationTransmitter.hpp for more detailed
// documentation about them.

bool ConfigurableSynchronizationTransmitter::configureHook()
{
    if (! ConfigurableSynchronizationTransmitterBase::configureHook())
        return false;
    return true;
}
bool ConfigurableSynchronizationTransmitter::startHook()
{
    if (! ConfigurableSynchronizationTransmitterBase::startHook())
        return false;
    return true;
}
void ConfigurableSynchronizationTransmitter::updateHook()
{
    ConfigurableSynchronizationTransmitterBase::updateHook();
    if(!initial_map_sent){
        loadEnvironment(_environment_path.get());
        initial_map_sent = true;
    }    
}
void ConfigurableSynchronizationTransmitter::errorHook()
{
    ConfigurableSynchronizationTransmitterBase::errorHook();
}
void ConfigurableSynchronizationTransmitter::stopHook()
{
    ConfigurableSynchronizationTransmitterBase::stopHook();
}
void ConfigurableSynchronizationTransmitter::cleanupHook()
{
    ConfigurableSynchronizationTransmitterBase::cleanupHook();
}
