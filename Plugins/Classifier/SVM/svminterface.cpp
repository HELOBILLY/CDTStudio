#include "svminterface.h"
#include <limits>

#ifdef Q_OS_WIN
#include <opencv2/core/version.hpp>
#define CV_VERSION_ID   \
    CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
#ifdef QT_DEBUG
#define cvLIB(name) \
    "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) \
    "opencv_" name CV_VERSION_ID
#endif
#pragma comment(lib,cvLIB("core"))
#pragma comment(lib,cvLIB("imgproc"))
#pragma comment(lib,cvLIB("highgui"))
#pragma comment(lib,cvLIB("ml"))
#endif




SVMInterface::SVMInterface(QObject *parent)
    :data(new cv::SVMParams)
{
}

SVMInterface::~SVMInterface()
{
    delete data;
}

QString SVMInterface::classifierName() const
{
    return tr("Support Vector Machines");
}

cv::Mat SVMInterface::startClassification(const cv::Mat &data, const cv::Mat &train_data, const cv::Mat &responses)
{
    cv::Mat result(data.rows,1,CV_32FC1);
    cv::SVM classifier;

    classifier.train(train_data,responses,cv::Mat(),cv::Mat(),*(this->data));
    classifier.predict(data,result);

    return result;
}

SVMInterface::Type SVMInterface::type() const
{
    return static_cast<Type>(data->svm_type);
}

SVMInterface::Kernel SVMInterface::kernel() const
{
    return static_cast<Kernel>(data->kernel_type);
}

double SVMInterface::degree() const
{
    return data->degree;
}

double SVMInterface::gamma() const
{
    return data->gamma;
}

double SVMInterface::coef0() const
{
    return data->coef0;
}

double SVMInterface::c() const
{
    return data->C;
}

double SVMInterface::nu() const
{
    return data->nu;
}

void SVMInterface::setType(SVMInterface::Type val)
{
    data->svm_type = val;
}

void SVMInterface::setKernel(SVMInterface::Kernel val)
{
    data->kernel_type = val;
}

void SVMInterface::setDegree(double val)
{
    data->degree = val;
}

void SVMInterface::setGamma(double val)
{
    data->gamma = val;
}

void SVMInterface::setCoef0(double val)
{
    data->coef0 = val;
}

void SVMInterface::setC(double val)
{
    data->C = val;
}

void SVMInterface::setNu(double val)
{
    data->nu = val;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(SVM, SVMInterface)
#endif // QT_VERSION < 0x050000