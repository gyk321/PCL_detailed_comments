#include <iostream>

#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_cloud.h>
#include <pcl/console/parse.h>
#include <pcl/common/transforms.h> // 点云坐标变换
#include <pcl/visualization/pcl_visualizer.h>

// This function displays the help
void showHelp(char *program_name)
{
    std::cout << std::endl;
    std::cout << "Usage: " << program_name << " cloud_filename.[pcd|ply]" << std::endl;
    std::cout << "-h:  Show this help." << std::endl;
}

// This is the main function
int main(int argc, char **argv)
{

    // Show help 展示帮助信息 
    // pcl::console::find_switch 它可以在命令行中搜索是否存在特定的参数，如果存在则返回特定的参数值，如果不存在则返回默认值。
    if (pcl::console::find_switch(argc, argv, "-h") || pcl::console::find_switch(argc, argv, "--help"))
    {
        showHelp(argv[0]);
        return 0;
    }

    // Fetch point cloud filename in arguments | Works with PCD and PLY files
    std::vector<int> filenames;
    bool file_is_pcd = false;

    // 用于解析可能输入的文件扩展名参数
    filenames = pcl::console::parse_file_extension_argument(argc, argv, ".ply");
    // 输入文件扩展名不是.ply
    if (filenames.size() != 1)
    {
        // 继续读取文件扩展名
        filenames = pcl::console::parse_file_extension_argument(argc, argv, ".pcd");
        // 输入文件扩展名不是.pcd
        if (filenames.size() != 1)
        {
            // 显示帮助信息
            showHelp(argv[0]);
            return -1;
        }
        else
        {
            // 输入文件扩展名是.pcd
            file_is_pcd = true;
        }
    }

    // Load file | Works with PCD and PLY files 在参数中寻找pcd或者ply文件
    pcl::PointCloud<pcl::PointXYZ>::Ptr source_cloud(new pcl::PointCloud<pcl::PointXYZ>());
    // 不能加载PCD文件，它会打印一条错误消息，然后显示帮助信息
    if (file_is_pcd)
    {
        if (pcl::io::loadPCDFile(argv[filenames[0]], *source_cloud) < 0)
        {
            std::cout << "Error loading point cloud " << argv[filenames[0]] << std::endl
                      << std::endl;
            showHelp(argv[0]);
            return -1;
        }
    }
    else
    {
        // 不能加载PLY文件，它会打印一条错误消息，然后显示帮助信息
        if (pcl::io::loadPLYFile(argv[filenames[0]], *source_cloud) < 0)
        {
            std::cout << "Error loading point cloud " << argv[filenames[0]] << std::endl
                      << std::endl;
            showHelp(argv[0]);
            return -1;
        }
    }

    /* Reminder: how transformation matrices work :

           |-------> This column is the translation
    | 1 0 0 x |  \
    | 0 1 0 y |   }-> The identity 3x3 matrix (no rotation) on the left
    | 0 0 1 z |  /
    | 0 0 0 1 |    -> We do not use this line (and it has to stay 0,0,0,1)

    METHOD #1: Using a Matrix4f ====================================第二种方法
    This is the "manual" method, perfect to understand but error prone !
  */
    // 创建4x4单位阵
    Eigen::Matrix4f transform_1 = Eigen::Matrix4f::Identity();

    // Define a rotation matrix (see https://en.wikipedia.org/wiki/Rotation_matrix)
    float theta = M_PI / 4; // The angle of rotation in radians
    transform_1(0, 0) = std::cos(theta);
    transform_1(0, 1) = -sin(theta);
    transform_1(1, 0) = sin(theta);
    transform_1(1, 1) = std::cos(theta);
    //    (row, column)

    // Define a translation of 2.5 meters on the x axis.
    transform_1(0, 3) = 2.5;

    // Print the transformation
    printf("Method #1: using a Matrix4f\n");
    std::cout << transform_1 << std::endl;

    /*  METHOD #2: Using a Affine3f ====================================第二种方法
    This method is easier and less error prone
  */
    // 一个单位矩阵 主对角线上的元素值都是1
    Eigen::Affine3f transform_2 = Eigen::Affine3f::Identity();

    // Define a translation of 2.5 meters on the x axis.
    transform_2.translation() << 2.5, 0.0, 0.0;

    // The same rotation matrix as before; theta radians around Z axis
    // 以Vector3f的z轴(UnitZ)为中心，将角度设置为theta弧度，进行旋转
    transform_2.rotate(Eigen::AngleAxisf(theta, Eigen::Vector3f::UnitZ()));

    // Print the transformation
    printf("\nMethod #2: using an Affine3f\n");
    std::cout << transform_2.matrix() << std::endl;

    // Executing the transformation
    pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>());
    // You can either apply transform_1 or transform_2; they are the same
    // 将 source_cloud 点云应用 transform_2 的变换，得到一个新的 transformed_cloud 点云
    pcl::transformPointCloud(*source_cloud, *transformed_cloud, transform_2);

    // Visualization  可视化
    printf("\nPoint cloud colors :  white  = original point cloud\n"
           "                        red  = transformed point cloud\n");
    pcl::visualization::PCLVisualizer viewer("Matrix transformation example");

    // Define R,G,B colors for the point cloud
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> source_cloud_color_handler(source_cloud, 255, 255, 255);
    // We add the point cloud to the viewer and pass the color handler
    // 使用viewer对象来添加一个名为原始云的点云，该点云的颜色由source_cloud_color_handler定义。
    viewer.addPointCloud(source_cloud, source_cloud_color_handler, "original_cloud");

    // 创建一个用自定义颜色着色器（红色）设置的颜色处理器（类似于上色一个点云）
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> transformed_cloud_color_handler(transformed_cloud, 230, 20, 20); // Red
    // 将一个点云（transformed_cloud）添加到viewer对象中，并指定它的颜色处理（transformed_cloud_color_handler）以及它的名称（"transformed_cloud"）
    viewer.addPointCloud(transformed_cloud, transformed_cloud_color_handler, "transformed_cloud");
    // 添加一个坐标系（1.0）到查看器中以帮助视觉化点云（使用带有 “cloud” 标签的点云）
    viewer.addCoordinateSystem(1.0, "cloud", 0);
    // 设置可视化查看器的背景颜色为深灰色（0.05, 0.05, 0.05, 0）。
    viewer.setBackgroundColor(0.05, 0.05, 0.05, 0); // Setting background to a dark grey
    // 两个点云（ Original_cloud 和 transformed_cloud） 设置大小为 2
    viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2, "original_cloud");
    viewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2, "transformed_cloud");
    //viewer.setPosition(800, 400); // Setting visualiser window position

    while (!viewer.wasStopped())
    { 
        // Display the visualiser until 'q' key is pressed
        viewer.spinOnce();
    }

    return 0;
}