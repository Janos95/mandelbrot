#include <iostream>


#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


cv::Mat image(500, 500, CV_8UC3);


double x_range = 2.;
double y_range = 2.;


const cv::Vec3b turquoise(208,224,64);
const cv::Vec3b purple(128,0,160);



struct ComplexNumber
{
	double real, imaginary;
};

ComplexNumber windowCenter;


std::ostream& operator <<(std::ostream& o, const ComplexNumber& z)
{
	return (o << z.real << " + i * " << z.imaginary);
}


ComplexNumber operator *(const ComplexNumber a, const ComplexNumber b)
{
	//a = a1 + i * a2
	//b = b1 + i * b2

	ComplexNumber c;
	c.real = a.real * b.real - a.imaginary * b.imaginary;
	c.imaginary = a.real*b.imaginary+a.imaginary*b.real;
	return c;
}

ComplexNumber operator +(const ComplexNumber& a, const ComplexNumber& b)
{
	//a = a1 + i * a2
	//b = b1 + i * b2
	//a+b = (a1 + b1) + i * (a2 + b2)

	ComplexNumber sum;
	sum.real = a.real + b.real;
	sum.imaginary = a.imaginary + b.imaginary;

	return sum; 

}



struct f_c
{
	ComplexNumber c;

	ComplexNumber operator()(const ComplexNumber& z) const
	{
		return z*z+c;
	}
};


struct affineTransform
{
    double width;
    double height;

    double scale_x, scale_y;
    double center_x, center_y;

    ComplexNumber operator ()(int x, int y) const
    {
        return ComplexNumber{
            x / width * scale_x + center_x - scale_x/2,
            (height - y) / height * scale_y + center_y - scale_y/2};
    }
};


affineTransform tf{500, 500, 4., 4., 0., 0.};


double absoluteValueSquared(const ComplexNumber& z)
{
    return z.real * z.real + z.imaginary * z.imaginary;
}


bool isInMandelbrotSet(const ComplexNumber& c)
{
    auto f = f_c{c};

    ComplexNumber z{0,0};

    for(int i = 0; i < 500; ++i)
    {
        z = f(z);

        if(absoluteValueSquared(z) > 4)
            return false;

    }

    return true;


}


void computeMandelbrotSet(cv::Mat& img, const affineTransform& tf)
{
    image.setTo(purple);

#pragma omp parallel for
    for(int x = 0; x < img.cols; ++x)
    {
        for(int y = 0; y < img.rows; ++y)
        {
            if(isInMandelbrotSet(tf(x,y)))
                img.at<cv::Vec3b>(y,x) = turquoise;

        }

    }
}


void drawMandelbrotSet(int event, int x, int y, int, void* param) {
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        auto z = tf(x,y);
        tf.scale_x/=10;
        tf.scale_y/=10;
        tf.center_x = z.real;
        tf.center_y = z.imaginary;
        computeMandelbrotSet(image, tf);
    }

}


int main() {
    computeMandelbrotSet(image, tf);
    cv::namedWindow("Image");
    cv::setMouseCallback("Image", drawMandelbrotSet);


    while (1)
    {

        cv::imshow("Image", image);
        if (cv::waitKey(20) == 27)  // wait until ESC is pressed
            break;

    }

}
