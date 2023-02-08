/*
 * @Description: 
 * @Author: HCQ
 * @Company(School): UCAS
 * @Date: 2020-10-04 18:33:16
 * @LastEditors: GuoYinkai
 * @LastEditTime: 2023-02-09 00:07:15
 */
#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

int main(int argc, char **argv)
{
    // 声明一个pcl::PointCloud类型的变量 cloud
    pcl::PointCloud<pcl::PointXYZ> cloud;

    // Fill in the cloud data
    cloud.width = 5;            // 宽度
    cloud.height = 1;           // 高度
    cloud.is_dense = false;     // 点云中可能包含缺失点或无效点
    // 将名为 cloud 的点云容器（cloud）的长度调整为cloud.width和cloud.height乘积的大小
    cloud.points.resize(cloud.width * cloud.height);
    // TODO   auto
    for (auto &point : cloud)
    {
        // 返回的随机数被限定在[0,1024]范围内
        point.x = 1024 * rand() / (RAND_MAX + 1.0f);
        point.y = 1024 * rand() / (RAND_MAX + 1.0f);
        point.z = 1024 * rand() / (RAND_MAX + 1.0f);
    }
    // 码将被给定的点云（cloud）存储为一个PCD文件 该文件的存储格式为ASCII
    pcl::io::savePCDFileASCII("test_pcd.pcd", cloud);
    std::cerr << "Saved " << cloud.size() << " data points to test_pcd.pcd." << std::endl;

    for (const auto &point : cloud)
        // std::cerr 标准库声明的流,它是一个错误流;
        std::cerr << "    " << point.x << " " << point.y << " " << point.z << std::endl;

    return (0);
}
