#ifndef OROGEN_ENVIRE_OROCOS_HPP
#define OROGEN_ENVIRE_OROCOS_HPP

#include <envire/Core.hpp>
#include <envire/core/Serialization.hpp>

namespace envire
{
    /**
     * Base functionality for Emitters
     */
    class OrocosEmitterBase : public SynchronizationEventHandler
    {
    public:
        typedef std::vector<BinaryEvent> BinaryEvents;
        typedef RTT::extras::ReadOnlyPointer<BinaryEvents> Ptr;
    protected:
        base::Time time;
        envire::Environment* env;


    public:
        OrocosEmitterBase(Environment* _env = 0)
            : env(0)
        {
            if(_env)
            {
                attach(_env);
            }
            useEventQueue(true);
        }

        virtual ~OrocosEmitterBase()
        {
            if (env)
                detach();
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

        virtual void handle( BinaryEvents& events ) = 0;
    };


    /**
     * Efficient OrocosEmitter using 
     * RTT::extras::ReadOnlyPointers
     */
    class OrocosEmitter : public OrocosEmitterBase
    {
    private:
        RTT::OutputPort< Ptr > &port;

    public:
        OrocosEmitter( RTT::OutputPort< Ptr > &port)
            : OrocosEmitterBase()
            , port( port )
        {
        }

        OrocosEmitter( envire::Environment* env, RTT::OutputPort< Ptr > &port)
            : OrocosEmitterBase(env)
            , port( port )
        {
        }

        void handle( std::vector<BinaryEvent>& events )
        {
            if(events.empty())
                return;

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
    };

    /**
     * OrocosPlainEmitter uses full copies of data structures
     */
    class OrocosPlainEmitter : public OrocosEmitterBase
    {
    private:
        RTT::OutputPort< BinaryEvents > &port;

    public:
        OrocosPlainEmitter( RTT::OutputPort< BinaryEvents > &port)
            : OrocosEmitterBase()
            , port( port )
        {}

        OrocosPlainEmitter( envire::Environment* env, RTT::OutputPort< BinaryEvents > &port)
            : OrocosEmitterBase(env)
            , port( port )
        {
        }

        void handle( BinaryEvents& events )
        {
	    // timestamp all the events
	    for( BinaryEvents::iterator it = events.begin();
		    it != events.end(); it++ )
	    {
		it->time = time;
	    }

	    // and write to port
	    port.write( events );
        }
    };
}
#endif

