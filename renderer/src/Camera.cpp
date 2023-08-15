#include "Camera.h"
#include "OrthoCamera.h"
#include "PerspectiveCamera.h"

std::shared_ptr<Camera> Camera::Create(CameraType cameraType)
{
    switch (cameraType)
    {
    case CameraType::Ortho:
        return Camera::CreateOrthoCamera(100, 100);
        break;

    case CameraType::Perspective:
        return Camera::CreatePerspectiveCamera(1.0);
        break;
    default:
        break;
    }
    return std::shared_ptr<Camera>();
}

std::shared_ptr<Camera> Camera::CreateOrthoCamera(float width, float height)
{
    return std::make_shared<OrthoCamera>(width, height);
}

std::shared_ptr<Camera> Camera::CreatePerspectiveCamera(float aspectRatio)
{
    return std::make_shared<PerspectiveCamera>(aspectRatio);
}
