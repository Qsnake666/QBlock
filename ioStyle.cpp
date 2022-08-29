#include "ioStyle.h"

ioStyle::ioStyle()
{

}

/*
void MainWindow::readBlkFile(std::vector<PointCloud::Point>& pointCloud, QString filepath, QString mode, int code, Para_Struct parameters, QString output){
    output= QString("Console: Open Block file");
    ui->codeEdit->append(output);
    //PointCloudLibrary
#ifdef PCLINCLUDE
    if (code == 1){
        if (mode == "XYZ"){
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
            if (pcl::io::loadPLYFile<pcl::PointXYZ>(filepath.toStdString(), *cloud) == -1){
                output= QString("Error: can not open such file!");
                ui->codeEdit->append(output);
                return ;
            }
            //写入pointCloud
            for (int i = 0; i < cloud->points.size(); i++){
                pointCloud.push_back(PointCloud::Point(cloud->points[i].x, cloud->points[i].y, cloud->points[i].z));
            }
        }
        else if(mode == "XYZI"){
            pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
            if (pcl::io::loadPLYFile<pcl::PointXYZI>(filepath.toStdString(), *cloud) == -1){
                output= QString("Error: can not open such file!");
                ui->codeEdit->append(output);
                return ;
            }
            //写入pointCloud
            for (int i = 0; i < cloud->points.size(); i++){
                pointCloud.push_back(PointCloud::Point(cloud->points[i].x, cloud->points[i].y, cloud->points[i].z, cloud->points[i].intensity));
            }
        }

        else if(mode == "XYZL"){
            pcl::PointCloud<pcl::PointXYZL>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
            if (pcl::io::loadPLYFile<pcl::PointXYZL>(filepath.toStdString(), *cloud) == -1){
                output= QString("Error: can not open such file!");
                ui->codeEdit->append(output);
                return ;
            }
            //写入pointCloud
            for (int i = 0; i < cloud->points.size(); i++){
                pointCloud.push_back(PointCloud::Point(cloud->points[i].x, cloud->points[i].y, cloud->points[i].z, cloud->points[i].label));
            }
        }
    }
#else
    //iostream
    if (code == 2){
        QByteArray pathTmp = filepath.toLatin1();
        const char *fpath = pathTmp.data();
        FILE* f = fopen(fpath, "rb");
        if (f == NULL){
            output= QString("Error: can not open such file!");
            ui->codeEdit->append(output);
            return;
        }
        if (mode == "XYZRGB"){
            //https://www.cnblogs.com/justsong/p/12219716.html
        }
        else if (mode == "XYZA"){

        }
        else if (mode == "XYZ"){

        }
        else {
            output= QString("Error: wrong file type!");
            ui->codeEdit->append(output);
            return;
        }
    }
    //ifstream  qblk单独成函数
    if (code == 3){
        if (mode == "txt" || mode == "blk" || mode == "qblk"){
            std::ifstream inFile;
            inFile.open(filepath.toStdString());
            if (!inFile) {
                output= QString("Error: can not open such file or you need use unicode path!");
                ui->codeEdit->append(output);
                return;
            }
            char tmp[50];
            double res = 0;
            double ox = 0;
            double oy = 0;
            double oz = 0;
            for (int i = 0; i < 100; i++) {
                inFile.getline(tmp, 50);
                int idx = 0;
                //getResolution
                if (i == 2) {
                    while (idx < 50 && tmp[idx] != '=')
                    {
                        idx++;
                    }
                    int j = 50 - idx;
                    int k = 0;
                    while (k < j && tmp[idx + k + 1] != NULL) {
                        k++;
                    }
                    if (k < 1) {
                        res = 10;
                    }
                    else if (k >= 1) {
                        int a;
                        for (int m = 0; m < k; m++) {
                            a = tmp[idx + m + 1] - 48; //记住转码
                            res += a * pow(10, k - m - 1);
                        }
                    }
                    parameters->resolution = res;
                }
                //Orign x
                if (i == 4) {
                    while (idx < 50 && tmp[idx] != '=')
                    {
                        idx++;
                    }
                    int j = 50 - idx;
                    int k = 0;
                    while (k < j && tmp[idx + k + 1] != '.') {
                        k++;
                    }
                    if (k < 1) {
                        ox = 0;
                    }
                    else if (k >= 1) {
                        int a;
                        for (int m = 0; m < k; m++) {
                            a = tmp[idx + m + 1] - 48; //记住转码
                            ox += a * pow(10, k - m - 1);
                        }
                    }
                }
                //Orign y
                if (i == 5) {
                    while (idx < 50 && tmp[idx] != '=')
                    {
                        idx++;
                    }
                    int j = 50 - idx;
                    int k = 0;
                    while (k < j && tmp[idx + k + 1] != '.') {
                        k++;
                    }
                    if (k < 1) {
                        oy = 0;
                    }
                    else if (k >= 1) {
                        int a;
                        for (int m = 0; m < k; m++) {
                            a = tmp[idx + m + 1] - 48; //记住转码
                            oy += a * pow(10, k - m - 1);
                        }
                    }
                }
                //Orign z
                if (i == 6) {
                    while (idx < 50 && tmp[idx] != '=')
                    {
                        idx++;
                    }
                    int j = 50 - idx;
                    int k = 0;
                    while (k < j && tmp[idx + k + 1] != '.') {
                        k++;
                    }
                    if (k < 1) {
                        oz = 0;
                    }
                    else if (k >= 1) {
                        int a;
                        for (int m = 0; m < k; m++) {
                            a = tmp[idx + m + 1] - 48; //记住转码
                            oz += a * pow(10, k - m - 1);
                        }
                    }
                }
                //goEnd
                if (tmp[0] == 'b') {
                    break;
                }
            }
            while (inFile.getline(tmp, 50)) {
                double x, y, z, l;
                x = y = z = 0;
                l = NO_ATTRIBUTE;
                //x
                int idx = 0;
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                double a; //. ascii = 46
                int b = 0;
                for (int m = 0; m < idx && tmp[m] != '.' ; m++) {
                    a = tmp[m] - 48;
                    x += a * pow(10, idx - m - 1);
                    b = m;
                }
                b += 1;
                if (tmp[b] == '.'){
                    b += 1;
                    x = 0; //重构x值
                    for (int m = 0; m < idx && tmp[m] != '.' ; m++) {
                        a = tmp[m] - 48;
                        x += a * pow(10, idx - m - 3);
                    }
                    for (int m = b; m < idx; m++) {
                        a = tmp[m] - 48;
                        x += a * pow(10, -idx + m);
                    }
                }
                idx += 1;
                int preIdx = idx;
                //y
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                for (int m = preIdx; m < idx && tmp[m] != '.' ; m++) {
                    a = tmp[m] - 48;
                    y += a * pow(10, idx - m - 1);
                    b = m;
                }
                b += 1;
                if (tmp[b] == '.'){
                    b += 1;
                    y = 0;
                    for (int m = preIdx; m < idx && tmp[m] != '.' ; m++) {
                        a = tmp[m] - 48;
                        y += a * pow(10, idx - m - 3);
                    }
                    for (int m = b; m < idx; m++) {
                        a = tmp[m] - 48;
                        y += a * pow(10, -idx + m);
                    }
                }
                idx += 1;
                preIdx = idx;
                //z
                while (idx < strlen(tmp) && tmp[idx] != ',' ) {
                    idx += 1;
                }
                for (int m = preIdx; m < idx && tmp[m] != '.' ; m++) {
                    a = tmp[m] - 48;
                    z += a * pow(10, idx - m - 1);
                    b = m;
                }
                b += 1;
                if (tmp[b] == '.'){
                    b += 1;
                    z = 0;
                    for (int m = preIdx; m < idx && tmp[m] != '.' ; m++) {
                        a = tmp[m] - 48;
                        z += a * pow(10, idx - m - 3);
                    }
                    for (int m = b; m < idx; m++) {
                        a = tmp[m] - 48;
                        z += a * pow(10, -idx + m);
                    }
                }
                idx += 1;
                //a
                //RGBA
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                idx += 1; //R
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                idx += 1; //G
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                idx += 1; //B
                preIdx = idx;
                while (idx < strlen(tmp) && tmp[idx] != ',') {
                    idx += 1;
                }
                //string
                std::string s;
                char tmp2[5];
                int count = 0;
                for (int m = preIdx; m < idx; m++) {
                    s = s + tmp[m];
                    tmp2[count]=tmp[m];
                    count++;
                }
                if (s=="岩石")
                    l = 35;
                else if (s=="夹石")
                    l = 45;
                else if (s=="矿体")
                    l = 55;
                if (strlen(tmp2) >= 4){
                    if(tmp2[0] == -47 && tmp2[1] == -46 && tmp2[2] == -54 && tmp2[3] == -81)
                        l = 35;
                    else if(tmp2[0] == -68 && tmp2[1] == -48 && tmp2[2] == -54 && tmp2[3] == -81)
                        l = 45;
                    if(tmp2[0] == -65 && tmp2[1] == -13 && tmp2[2] == -52 && tmp2[3] == -27)
                        l = 55;
                }
                pointCloud.push_back(PointCloud::Point(x - ox, y - oy, z - oz, 0));
                //std::cout<<x-ox<<" "<<y-oy<<" "<<z-oz<<std::endl;
            }
        }
    }
    return;
#endif
}
*/
