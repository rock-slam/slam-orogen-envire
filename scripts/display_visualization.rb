require 'orocos'
require 'vizkit'

include Orocos
Orocos::CORBA.max_message_size = 80000000
Orocos.initialize

Orocos.run 'envire_synchronisation_test' do
    
    sync_transmitter = Orocos::TaskContext.get 'sync_transmitter'

    view3d = Vizkit.vizkit3d_widget
    view3d.show
    envVis = Vizkit.default_loader.EnvireVisualization.plugins['EnvireVisualization'] 

    Vizkit::ReaderWriterProxy.default_policy[:port_proxy] = nil
    Vizkit.connect_port_to 'sync_transmitter', 'envire_event', :pull => false, :type => :buffer, :size => 100, :update_frequency => 100 do |sample,_|
        envVis.updateBinaryEvent(sample)
    end
    
    sync_transmitter.start 

    sync_transmitter.loadEnvironment(ARGV[0])

    Vizkit.exec
end
