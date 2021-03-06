/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "SynchronizationReceiver.hpp"
#include <envire/core/Event.hpp>
#include "Orocos.hpp"
#include <boost/format.hpp>

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
    envire::OrocosEmitter::Ptr binary_events;
    while (_envire_events.read(binary_events, false) == RTT::NewData) 
    {
        env->applyEvents(*binary_events);

        std::string export_dir = _export_directory.get();
        if (!export_dir.empty())
        {
            using namespace boost::io;
            boost::format formatter(export_dir);
            formatter.exceptions(all_error_bits ^ ( too_many_args_bit | too_few_args_bit ));
            export_dir = (formatter % (++mCount)).str();
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

