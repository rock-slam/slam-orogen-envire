#ifndef OROGEN_ENVIRE_OROCOS_HPP
#define OROGEN_ENVIRE_OROCOS_HPP

#include <envire/Core.hpp>
#include <envire/core/Serialization.hpp>

namespace envire
{
    /** Class that allows to bind an envire environment to an orocos port. It
     * dumps the data from the environment to the port
     */
    class OrocosEmitter : public envire::SynchronizationEventHandler
    {
        envire::Environment* env;

        typedef std::vector<envire::BinaryEvent> BinaryEvents;
        RTT::OutputPort<BinaryEvents>& port;

        std::vector<envire::EnvireBinaryEvent*> buffer;
        BinaryEvents events;

    public:
        OrocosEmitter(envire::Environment* env, RTT::OutputPort<BinaryEvents>& port)
            : env(env), port(port)
        {
            env->addEventHandler(this);
        }

        ~OrocosEmitter()
        {
            env->removeEventHandler(this);
        }

        void handle(envire::EnvireBinaryEvent* binary_event)
        {
            buffer.push_back(binary_event);
        }

        void flush()
        {
            if (buffer.empty())
                return;

            events.resize(buffer.size());
            for (unsigned int i = 0; i < buffer.size(); ++i)
            {
                events[i].move(*buffer[i]);
                delete buffer[i];
            }
            buffer.clear();
            port.write(events);
            events.clear();
        }
    };
}

#endif

