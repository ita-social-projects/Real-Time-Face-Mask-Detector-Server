#pragma once
#include "FaceRecognitionUI.h"

FaceRecognitionUI::FaceRecognitionUI(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.exit_button, &QPushButton::clicked, this, &FaceRecognitionUI::onExitButtonClicked);
}

void FaceRecognitionUI::onExitButtonClicked()
{
    m_exit_button_clicked = true;
    close();
};

void FaceRecognitionUI::on_return_button_clicked()
{
    m_return_button_clicked = true;
}

int FaceRecognitionUI::updateWindow(TCPClient& client)
{
    int number_of_faces;

    cv::VideoCapture camera;
    camera.open(IDCAM);

    auto shared_face_recognizer = std::make_shared<FaceRecognizer>();

    std::future<faceInfo> future_faces;
    faceInfo faces;
    
    while (!m_exit_button_clicked)
    {

        if (m_return_button_clicked)
        {
            hide();
            return RETURN_BUTTON_CLICKED;  
        }

        camera >> m_image;

        if (m_async_is_permitted)
        {
            future_faces = std::async(std::launch::async, &FaceRecognizer::runAnalysis,
                shared_face_recognizer, m_image);
            m_async_is_permitted = false;
        }

        if (is_ready(future_faces))
        {
            m_async_is_permitted = true;
            faces = future_faces.get();

            m_in_mask = std::count_if(faces.begin(), faces.end(), 
                [](std::pair<cv::Rect, bool> i) {return !i.second; }
            );

            for (auto& face : faces)
            {
                //if current face without mask - trying to send on server
                if (!face.second)
                {
                    cv::UMat face_img(m_image, face.first);

                    if (m_in_mask != number_of_faces)
                    {
                        std::cout << "send" << std::endl;

                        std::thread t(&FaceRecognitionUI::sendImage, this, std::ref(client), face_img.clone());
                        t.detach();
                    }
                }
                auto rect_color = face.second == true ? GREEN : RED;
                cv::rectangle(m_image, face.first, rect_color, 3, 8, 0);
            }  
            number_of_faces = m_in_mask;
        }
        //if faces were found, then set info into frame
        if (!faces.empty())
        {
            SetPanelText();
        }
        displayFrame();
    }
    return 0;
}


void FaceRecognitionUI::SetPanelText()
{
    cv::Scalar color;
    std::string message;
    if (m_in_mask == 0)
    {
        color = GREEN;
        message = "Thanks for wearing mask :)";
    }
    else
    {
        color = RED;
        message = "Put on a mask please!";
    }
    rectangle(m_image, cv::Point(70, 0), cv::Point(600, 50), color, cv::FILLED, 8, 0);
    putText(m_image, message, cv::Point(170, 30),
        cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0, 0, 0), 1, cv::LINE_AA);
}

bool FaceRecognitionUI::is_ready(std::future<faceInfo> const& f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

void FaceRecognitionUI::displayFrame()
{
    QImage frame = mat2QImage();
    QPixmap map = QPixmap::fromImage(frame.scaled(ui.frame->width(), ui.frame->height(), 
                                     Qt::IgnoreAspectRatio, g_video_quality));
    ui.frame->setPixmap(map);
    ui.frame->show();

    cv::waitKey(30);
}

QImage FaceRecognitionUI::mat2QImage()
{
    // make the same cv::Mat
    cv::UMat temp;

    // cvtColor Makes a copt, that what i need
    cvtColor(m_image, temp, cv::COLOR_BGR2RGB);

    QImage dest((const uchar*)temp.getMat(cv::ACCESS_READ).data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    
    // enforce deep copy, see documentation 
    dest.bits(); 

    // of QImage::QImage ( const uchar * data, int width, int height, Format format )
    return dest;
}

void FaceRecognitionUI::sendImage(TCPClient& client, cv::UMat face_img)
{
    std::vector<uchar> ubuffer;
    cv::imencode(".png", face_img.clone(), ubuffer);
    
    std::vector<char> buffer(ubuffer.begin(), ubuffer.end());

    client.SendBinaryMessage(buffer);
}

FaceRecognitionUI::~FaceRecognitionUI()
{

}