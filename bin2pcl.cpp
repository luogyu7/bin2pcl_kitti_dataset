#include <fstream>
#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <boost/thread/thread.hpp>

using namespace std;

int main(int argc, char** argv)
{
	//----------------------待读取的bin文件--------------------------
	string binFile = "000002.bin";
	fstream input(binFile.c_str(), ios::in | ios::binary);
	if (!input.good()) {
		cerr << "Could not read file: " << binFile << endl;
		exit(EXIT_FAILURE);
	}
	input.seekg(0, ios::beg);
	cout << "Read KITTI point cloud with the name of " << binFile << endl;

	//----从bin文件中读取x、y、z、intensity信息并保存至PointXYZI----
	//XYZI
	pcl::PointCloud<pcl::PointXYZI>::Ptr pcd_cloud(new pcl::PointCloud<pcl::PointXYZI>);

	for (int i = 0; input.good() && !input.eof(); i++) {
		pcl::PointXYZI point;
		input.read((char*)&point.x, 3 * sizeof(float));
		input.read((char*)&point.intensity, sizeof(float));
		pcd_cloud->push_back(point);
	}
	input.close();

	//获取不带文件路径和后缀的文件名，使pcd文件的名字与bin原始名字相同
	string pcdFile = binFile.substr(0, binFile.rfind(".")) + ".pcd";
	cout << "Read KITTI point cloud with " << (*pcd_cloud).size() << " points, writing to " << pcdFile << endl;

	pcl::PCDWriter writer;
	writer.write<pcl::PointXYZI>(pcdFile, *pcd_cloud, true);

	//--------------可视化KITTI数据集的bin点云------------------------
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI>fildColor(pcd_cloud, "intensity");
	viewer->setBackgroundColor(0, 0, 0);
	viewer->setWindowName("PCL可视化KITTI点云数据集");
	viewer->addText("KITTI point clouds are shown by PCL", 50, 50, 0, 1, 0, "v1_text");
	viewer->addPointCloud<pcl::PointXYZI>(pcd_cloud, fildColor, "sample cloud");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "sample cloud");

	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}

	return 0;
}

