#ifndef OROGEN_ENVIRE_OROCOS_HPP
#define OROGEN_ENVIRE_OROCOS_HPP

#include <envire/Core.hpp>
#include <envire/core/Serialization.hpp>

namespace envire
{
    class OrocosEmitter : public SynchronizationEventHandler
    {
    public:
        typedef std::vector<BinaryEvent> BinaryEvents;
        typedef RTT::extras::ReadOnlyPointer<BinaryEvents> Ptr;
    private:
        RTT::OutputPort< Ptr > &port;
        base::Time time;

        envire::Environment* env;


    public:
        OrocosEmitter( RTT::OutputPort< Ptr > &port)
            : port( port ), env(0)
        {
            useEventQueue(true);
        }

        OrocosEmitter( envire::Environment* env, RTT::OutputPort< Ptr > &port)
            : port( port ), env(0)
        {
            useEventQueue(true);
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

            BinaryEvents* binary_events = new BinaryEvents;
            binary_events->swap(events);

	    // and write to port
	    port.write( Ptr(binary_events) );
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

