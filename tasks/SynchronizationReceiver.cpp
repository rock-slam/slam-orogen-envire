/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "SynchronizationReceiver.hpp"
#include <envire/core/Event.hpp>

using namespace envire;

SynchronizationReceiver::SynchronizationReceiver(std::string const& name, TaskCore::TaskState initial_state)
    : SynchronizationReceiverBase(name, initial_state), env(new Environment())
{
}

SynchronizationReceiver::SynchronizationReceiver(std::string const& name, RTT::ExecutionEngine* engine, TaskCore::TaskState initial_state)
    : SynchronizationReceiverBase(name, engine, initial_state), env(new Environment())
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
// bool SynchronizationReceiver::startHook()
// {
//     if (! SynchronizationReceiverBase::startHook())
//         return false;
//     return true;
// }

void SynchronizationReceiver::updateHook()
{
    envire::EnvireBinaryEvent binary_event;
    while (_evire_event.read(binary_event) == RTT::NewData) 
    {
        EnvironmentItem* item = 0;
        if(binary_event.type == event::ITEM && (binary_event.operation == event::ADD || binary_event.operation == event::UPDATE ))
        {
            // unserialize item
            item = serialization.unserializeBinaryEvent(binary_event);
        }
        
        // set up event
        EnvironmentItem::Ptr item_ptr(item);
        envire::Event event(binary_event.type, binary_event.operation, item_ptr);
        event.id_a = binary_event.id_a;
        event.id_b = binary_event.id_b;
        
        // apply event
        event.apply(env.get());
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

