/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <envire/Core.hpp>
#include <envire/maps/Pointcloud.hpp>
#include <envire/maps/LaserScan.hpp>
#include <envire/maps/Grids.hpp>
#include <envire/operators/ScanMeshing.hpp>
#include <envire/operators/DistanceGridToPointcloud.hpp>

#include <base/angle.h>
#include <algorithm>

using namespace envire;

Task::Task(std::string const& name)
    : TaskBase(name), env( new Environment() )
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine), env( new Environment() )
{
}

Task::~Task()
{
}

void Task::bodystate_samplesTransformerCallback(const base::Time &ts, const ::asguard::BodyState &bodystate_samples_sample)
{
    bodyState = bodystate_samples_sample;
}

void Task::distance_framesTransformerCallback(const base::Time &ts, const ::base::samples::DistanceImage &distance_frames_sample)
{
    Eigen::Affine3d body2odometry, lcamera2body;
    if( !_body2odometry.get( ts, body2odometry ) || !_lcamera2body.get( ts, lcamera2body ) )
        return;

    FrameNode* bodyFrame = new FrameNode( body2odometry );
    env->addChild( env->getRootNode(), bodyFrame );
    
    // get the distance image and set up the operator chain
    FrameNode* lcameraFrame = new FrameNode( lcamera2body );
    env->addChild( bodyFrame, lcameraFrame );

    DistanceGrid *distanceGrid = new DistanceGrid(distance_frames_sample);
    env->setFrameNode( distanceGrid, lcameraFrame );

    Pointcloud *distancePc = new Pointcloud();
    env->setFrameNode( distancePc, lcameraFrame );

    DistanceGridToPointcloud *dgOp = new DistanceGridToPointcloud();
    env->attachItem( dgOp );
    dgOp->addInput( distanceGrid );
    dgOp->addOutput( distancePc );

    dgOp->updateAll();

    std::cout << "distance_frame converted to pointcloud" << std::endl;
}

struct Statistics
{
    double S_x;
    double S_xx;
    size_t n;
    mutable std::vector<double> values;

    Statistics() : S_x(0), S_xx(0), n(0) {};
    void addValue( double v ) { S_x += v; S_xx += pow(v,2); n++; values.push_back( v ); }
    double getMean() const { return S_x / n; }
    double getStdev() const { return sqrt( S_xx / n - pow( S_x / n, 2 ) ); }
    double getMedian() const 
    { 
	std::sort( values.begin(), values.end() ); 
	return values[values.size()/2]; 
    }
};

inline std::ostream& operator << (std::ostream& io, Statistics const& stat)
{
    io << " samples: " << stat.n
	<< " mean: " << stat.getMean() 
	<< " stdev: " << stat.getStdev() 
	<< " median: " << (stat.n > 0 ? stat.getMedian() : 0);
    return io;
}

struct LaserBeam
{
    Eigen::Affine3d laser2odometry;
    Eigen::Vector3d point;
};

std::vector<LaserBeam> beams;
Statistics bodyStat;
Statistics marker1, marker2;

void Task::scan_samplesTransformerCallback(const base::Time &ts, const ::base::samples::LaserScan &scan_samples_sample)
{
    Eigen::Affine3d body2odometry, laser2body;
    if( !_body2odometry.get( ts, body2odometry ) || !_laser2body.get( ts, laser2body ) )
	return;

    // get all the contact points for the body and add them to pointcloud
    FrameNode* bodyFrame = new FrameNode( body2odometry );
    env->addChild( env->getRootNode(), bodyFrame );

    Pointcloud *contactPc = new Pointcloud();
    env->setFrameNode( contactPc, bodyFrame );

    asguard::Configuration asguardConfig( _asguard_config.get() );

    for(int i=0;i<4;i++)
    {
	for(int j=0;j<5;j++) 
	{
	    base::Vector3d f = asguardConfig.getFootPosition( bodyState, static_cast<asguard::wheelIdx>(i), j );
	    contactPc->vertices.push_back( f );

	    if( f.z() < 0 )
	    {
		bodyStat.addValue( (body2odometry * f).z() );
	    }
	}
    }

    // get the laserscan and set up the operator chain
    FrameNode* laserFrame = new FrameNode( laser2body );
    env->addChild( bodyFrame, laserFrame );

    LaserScan *scanNode = new LaserScan();
    scanNode->addScanLine( 0, scan_samples_sample );
    env->setFrameNode( scanNode, laserFrame );

    TriMesh *laserPc = new TriMesh();
    env->setFrameNode( laserPc, laserFrame );

    ScanMeshing *smOp = new ScanMeshing();
    env->attachItem( smOp );
    smOp->addInput( scanNode );
    smOp->addOutput( laserPc );

    smOp->updateAll();

    if( laserPc->vertices.size() > 500 )
    {
	LaserBeam beam;
	beam.laser2odometry = body2odometry * laser2body;
	beam.point = laserPc->vertices[460];
	beam.point = laserPc->vertices[560];

	beams.push_back( beam );

	for( int i=0; i<100 ; i++ )
	{
	    Eigen::Vector3d p = beam.laser2odometry * laserPc->vertices[1080/2-50+i];
	    if( p.z() > 0.1 )
	    {
		if( p.y() > 0.7 )
		    marker1.addValue( p.y() );
		else
		    marker2.addValue( p.y() );
	    }
	}
    }

}

void Task::updateHook()
{
    TaskBase::updateHook();

    // only write output if the aggregator actually had some data
    // this is slightly implicit, and could be made more explicit in
    // the aggregator
//    if( transformer.getStatus().latest_time > base::Time() )
//	_streamaligner_status.write( transformer.getStatus() );
}

void Task::stopHook()
{
    std::cout << "bodyStat: " << bodyStat << std::endl;
    std::cout << "marker1: " << marker1 << std::endl;
    std::cout << "marker2: " << marker2 << std::endl;

    // write environment, if path is given
    if( !_environment_debug_path.value().empty() )
    {
	envire::Serialization so;
	so.serialize(env.get(), _environment_debug_path.value() );
    }

    double min_stdev = 100;
    double min_mean = 0;
    double min_y = 0;
    double min_z = 0;
    double min_alpha = 0;
    double min_median = 0;

    for( float alpha = base::Angle::deg2Rad(-5); alpha < base::Angle::deg2Rad(5); alpha += base::Angle::deg2Rad(.1) )
    {
	std::cerr << ".";
	for( float y = -0.0; y <= 0.0; y += 0.005 )
	{
	    for( float z = -0.0; z <= 0.0; z += 0.005 )
	    {
		Statistics s;
		for( size_t i=0; i<beams.size(); i++ )
		{
		    s.addValue( (beams[i].laser2odometry * Eigen::AngleAxisd( alpha, Eigen::Vector3d::UnitX() ) * Eigen::Translation3d( Eigen::Vector3d( 0, y, z ) ) * beams[i].point ).z() );
		}

		if( s.getStdev() < min_stdev )
		{
		    min_alpha = alpha;
		    min_y = y;
		    min_z = z;
		    min_stdev = s.getStdev();
		    min_mean = s.getMean();
		    min_median = s.getMedian();
		}
	    }
	}
    }

    std::cout 
	<< " min_mean: " << min_mean
	<< " min_stdev: " << min_stdev 
	<< " min_median: " << min_median 
	<< " min_alpha: " << min_alpha
	<< " min_y: " << min_y
	<< " min_z: " << min_z
	<< std::endl;
}

