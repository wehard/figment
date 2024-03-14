#include "Camera.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"

namespace Figment
{
    std::shared_ptr<Camera> Camera::Create(CameraType cameraType)
    {
        switch (cameraType)
        {
        case CameraType::Orthographic:
            return Camera::CreateOrthographicCamera(100, 100);
        case CameraType::Perspective:
            return Camera::CreatePerspectiveCamera(1.0);
        default:
            break;
        }
        return std::shared_ptr<Camera>();
    }

    std::shared_ptr<Camera> Camera::CreateOrthographicCamera(float width, float height)
    {
        return std::make_shared<OrthographicCamera>(width, height);
    }

    std::shared_ptr<Camera> Camera::CreatePerspectiveCamera(float aspectRatio)
    {
        return std::make_shared<PerspectiveCamera>(aspectRatio);
    }
}
