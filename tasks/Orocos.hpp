#ifndef OROGEN_ENVIRE_OROCOS_HPP
#define OROGEN_ENVIRE_OROCOS_HPP

#include <envire/Core.hpp>

namespace envire
{
    /** Class that allows to bind an envire environment to an orocos port. It
     * dumps the data from the environment to the port
     */
    class OrocosEmitter : public envire::SynchronizationEventHandler
    {
        envire::Environment* env;
        RTT::OutputPort<envire::EnvireBinaryEvent>& port;

        std::vector<envire::EnvireBinaryEvent*> buffer;

    public:
        OrocosEmitter(envire::Environment* env, RTT::OutputPort<envire::EnvireBinaryEvent>& port)
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
            if (binary_event)
                port.write(*binary_event);
            delete binary_event;
        }
    };
}

#endif

