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
        bool attached;
        long event_counter;

        std::vector<EnvireBinaryEvent> msg_buffer;

    public:
        OrocosEmitter( RTT::OutputPort< BinaryEvents > &port)
            : port( port ), attached(false), event_counter(0)
        {
        }

        void handle( EnvireBinaryEvent* binary_event )
        {
            // set the current timestamp
            binary_event->time = time;
            msg_buffer.push_back( EnvireBinaryEvent() );
            msg_buffer.back().move(*binary_event);
            delete binary_event;
        }

        void flush()
        {
            msg_buffer.reserve(msgQueue.size());
            msg_buffer.clear();
            SynchronizationEventHandler::flush();
            port.write(msg_buffer);
        }

        void setTime( base::Time time )
        {
            this->time = time;
        }
        
        void attachEventHandler( Environment* env )
        {
            // register this class as event handler for environment
            env->addEventHandler( this );
            attached = true;
        }
        
        void detachEventHandler( Environment* env )
        {
            env->removeEventHandler( this );
            attached = false;
        }
        
        bool isAttached()
        {
            return attached;
        }
    };
}

#endif

