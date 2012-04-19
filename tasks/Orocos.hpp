#ifndef OROGEN_ENVIRE_OROCOS_HPP
#define OROGEN_ENVIRE_OROCOS_HPP

#include <envire/Core.hpp>
#include <envire/core/Serialization.hpp>

namespace envire
{
    class OrocosEmitter : public SynchronizationEventHandler
    {
        typedef std::vector<BinaryEvent> BinaryEvents;
        RTT::OutputPort< BinaryEvents > &port;
        base::Time time;

        envire::Environment* env;

    public:
        OrocosEmitter( RTT::OutputPort< BinaryEvents > &port)
            : port( port ), env(0)
        {
        }

        OrocosEmitter( envire::Environment* env, RTT::OutputPort< BinaryEvents > &port)
            : port( port ), env(0)
        {
            attach(env);
        }

        ~OrocosEmitter()
        {
            if (env)
                detach();
        }

        void handle( std::vector<BinaryEvent>& events )
        {
	    // timestamp all the events
	    for( std::vector<BinaryEvent>::iterator it = events.begin();
		    it != events.end(); it++ )
	    {
		it->time = time;
	    }

	    // and write to port
	    port.write( events );
        }

        void setTime( base::Time time )
        {
            this->time = time;
        }
        
        void attach( Environment* env )
        {
            // register this class as event handler for environment
            env->addEventHandler( this );
            this->env = env;
        }
        
        void detach()
        {
            env->removeEventHandler( this );
            this->env = NULL;
        }
        
        bool isAttached()
        {
            return env;
        }
    };
}

#endif

