#include "Camera.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"

namespace Figment
{
    Figment::SharedPtr<Camera> Camera::Create(CameraType cameraType)
    {
        switch (cameraType)
        {
        case CameraType::Ortho:
            return Camera::CreateOrthoCamera(100, 100);
        case CameraType::Perspective:
            return Camera::CreatePerspectiveCamera(1.0);
        default:
            break;
        }
        return Figment::SharedPtr<Camera>();
    }

    Figment::SharedPtr<Camera> Camera::CreateOrthoCamera(float width, float height)
    {
        return Figment::CreateSharedPtr<OrthographicCamera>(width, height);
    }

    Figment::SharedPtr<Camera> Camera::CreatePerspectiveCamera(float aspectRatio)
    {
        return Figment::CreateSharedPtr<PerspectiveCamera>(aspectRatio);
    }
}
