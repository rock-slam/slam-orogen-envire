/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "SynchronizationReceiver.hpp"
#include <envire/core/Event.hpp>

using namespace envire;

SynchronizationReceiver::SynchronizationReceiver(std::string const& name, TaskCore::TaskState initial_state)
    : SynchronizationReceiverBase(name, initial_state), env(new Environment())
    , mCount(0)
{
}

SynchronizationReceiver::SynchronizationReceiver(std::string const& name, RTT::ExecutionEngine* engine, TaskCore::TaskState initial_state)
    : SynchronizationReceiverBase(name, engine, initial_state), env(new Environment())
    , mCount(0)
{
}

SynchronizationReceiver::~SynchronizationReceiver()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See SynchronizationReceiver.hpp for more detailed
// documentation about them.

// bool SynchronizationReceiver::configureHook()
// {
//     if (! SynchronizationReceiverBase::configureHook())
//         return false;
//     return true;
// }

bool SynchronizationReceiver::startHook()
{
    if (! SynchronizationReceiverBase::startHook())
        return false;

    return true;
}

void SynchronizationReceiver::updateHook()
{
    std::vector<envire::EnvireBinaryEvent> binary_events;
    while (_envire_events.read(binary_events) == RTT::NewData) 
    {
        env->applyEvents(binary_events);
        std::cout << "triggered" << std::endl;

        std::string export_dir = _export_directory.get();
        if (!export_dir.empty())
        {
            std::cout << "saving" << std::endl;
            export_dir = export_dir + "/" + boost::lexical_cast<std::string>(++mCount);
            env->serialize(export_dir);
        }
    }
}

// void SynchronizationReceiver::errorHook()
// {
//     SynchronizationReceiverBase::errorHook();
// }
// void SynchronizationReceiver::stopHook()
// {
//     SynchronizationReceiverBase::stopHook();
// }
// void SynchronizationReceiver::cleanupHook()
// {
//     SynchronizationReceiverBase::cleanupHook();
// }

