#! /usr/bin/env ruby

require 'vizkit'
require 'asguard'
require 'optparse'
include Orocos

def usage
    STDERR.puts 
    exit 1
end

out_file = "/tmp/env"

opt_parse = OptionParser.new do |opt|
    opt.banner = "process_logs.rb <log_file_dir>"
    opt.on("-o output_environment", String, "path to output environment") do |name|
	out_file = name
    end
end

args = opt_parse.parse( ARGV )
if args.size < 1
    puts opt_parse
    exit 1
end

log_file = args[0]
# filter out properties.log files
replay = Asguard::Replay.new( log_file )

Orocos.initialize
Orocos::Process.run 'envire_test', :valgrind=>false, :wait => 1 do |p|
    envire = p.task('envire')

    envire.environment_debug_path = out_file

    replay.log.hokuyo.scans.connect_to( envire.scan_samples, :type => :buffer, :size => 1000 ) 
    replay.log.odometry.bodystate_samples.connect_to( envire.bodystate_samples, :type => :buffer, :size => 1000 )
    replay.log.odometry.odometry_samples.connect_to( envire.dynamic_transformations, :type => :buffer, :size => 1000 ) do |data|
	yaw = data.orientation.to_euler(2,1,0)[0]
	data.orientation = Eigen::Quaternion.from_angle_axis( -yaw, Eigen::Vector3.UnitZ ) *
	    data.orientation
	data.position[0] = 0
	data.position[1] = 0
	data
    end
    replay.log.dynamixel.lowerDynamixel2UpperDynamixel.connect_to( envire.dynamic_transformations, :type => :buffer, :size => 1000 )
    if  replay.log.has_task? :stereo and replay.log.stereo.has_port? :distance_frame
        replay.log.stereo.distance_frame.connect_to( envire.distance_frames, :type => :buffer, :size => 2 )
        @has_distance_images = true
        puts "INFO: Using distance images."
    end

    tf = Asguard::Transform.new [:dynamixel]
    tf.setup_filters replay

    envire.configure
    envire.start
    tf.configure_task envire

    replay.log.run
end
