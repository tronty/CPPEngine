#if defined(WIN_PHONE_APP)
//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include <wrl.h>
#include <d3d11_2.h>
#include <agile.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
int init(unsigned int width, unsigned int height);
int render();
#ifdef __cplusplus
}
#endif

namespace DX
{
    // Convert DirectX error codes to exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DX API errors.
            throw Platform::Exception::CreateException(hr);
        }
    }

    // Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
    inline float ConvertDipsToPixels(float dips, float dpi)
    {
        static const float dipsPerInch = 96.0f;
        return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
    }

    // Converts a length in physical pixels to a length in device-independent pixels (DIPs).
    inline float ConvertPixelsToDips(float pixels)
    {
        static const float dipsPerInch = 96.0f;
        return pixels * dipsPerInch / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi; // Do not round.
    }

    // Converts a length in physical pixels to a length in device-independent pixels (DIPs) using the provided DPI. This removes the need
    // to call this function from a thread associated with a CoreWindow, which is required by DisplayInformation::GetForCurrentView().
    inline float ConvertPixelsToDips(float pixels, float dpi)
    {
        static const float dipsPerInch = 96.0f;
        return pixels * dipsPerInch / dpi; // Do not round.
    }

#if defined(_DEBUG)
    // Check for SDK Layer support.
    inline bool SdkLayersAvailable()
    {
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
            0,
            D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
            nullptr,                    // Any feature level will do.
            0,
            D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            nullptr,                    // No need to keep the D3D device reference.
            nullptr,                    // No need to know the feature level.
            nullptr                     // No need to keep the D3D device context reference.
            );

        return SUCCEEDED(hr);
    }
#endif
}
#define _USE_MATH_DEFINES
#include <math.h>

// This header defines math and matrix helper functions and structures used 
// by DirectX SDK samples.

// Common Constants

#define PI_F 3.1415927f

// Template Vector & Matrix Classes

template <class T> struct Vector2
{
    union
    {
        struct
        {
            T x;
            T y;
        };
        struct
        {
            T r;
            T g;
        };
        struct
        {
            T u;
            T v;
        };
    };

    T& operator[](unsigned int index)
    {
        return static_cast<T*>(this)[index];
    }

    Vector2(T _x = 0, T _y = 0) : x(_x), y(_y) { }
};

template <class T> struct Vector3
{
    union
    {
        struct
        {
            T x;
            T y;
            T z;
        };
        struct
        {
            T r;
            T g;
            T b;
        };
        struct
        {
            T u;
            T v;
            T w;
        };
    };

    T& operator[](unsigned int index)
    {
        return static_cast<T*>(this)[index];
    }

    Vector3(T _x = 0, T _y = 0, T _z = 0) : x(_x), y(_y), z(_z) { }
};

template <class T> struct Vector4
{
    union
    {
        struct
        {
            T x;
            T y;
            T z;
            T w;
        };
        struct
        {
            T r;
            T g;
            T b;
            T a;
        };
    };

    T& operator[](unsigned int index)
    {
        return static_cast<T*>(this)[index];
    }

    Vector4(T _x = 0, T _y = 0, T _z = 0, T _w = 0) : x(_x), y(_y), z(_z), w(_w) { }
};

template <class T> struct Matrix4x4
{
    union
    {
        struct
        {
            T _11; T _12; T _13; T _14;
            T _21; T _22; T _23; T _24;
            T _31; T _32; T _33; T _34;
            T _41; T _42; T _43; T _44;
        };
        struct
        {
            T _m00; T _m01; T _m02; T _m03;
            T _m10; T _m11; T _m12; T _m13;
            T _m20; T _m21; T _m22; T _m23;
            T _m30; T _m31; T _m32; T _m33;
        };
    };

    Matrix4x4(T value = 0)
    {
        _11 = _12 = _13 = _14 = value;
        _21 = _22 = _23 = _24 = value;
        _31 = _32 = _33 = _34 = value;
        _41 = _42 = _43 = _44 = value;
    }

    Matrix4x4(
        T i11, T i12, T i13, T i14,
        T i21, T i22, T i23, T i24,
        T i31, T i32, T i33, T i34,
        T i41, T i42, T i43, T i44
        )
    {
        _11 = i11; _12 = i12; _13 = i13; _14 = i14;
        _21 = i21; _22 = i22; _23 = i23; _24 = i24;
        _31 = i31; _32 = i32; _33 = i33; _34 = i34;
        _41 = i41; _42 = i42; _43 = i43; _44 = i44;
    }

    T* operator[](unsigned int index)
    {
        return &(reinterpret_cast<T*>(this)[index*4]);
    }
};

// Template Vector Operations

template <class T>
T dot(Vector2<T> a, Vector2<T> b)
{
    return a.x * b.x + a.y * b.y;
}

template <class T>
T dot(Vector3<T> a, Vector3<T> b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <class T>
T dot(Vector4<T> a, Vector4<T> b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w + b.w;
}

template <class T>
T length(Vector2<T> a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

template <class T>
T length(Vector3<T> a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

template <class T>
T length(Vector4<T> a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

template <class T>
Vector3<T> cross(Vector3<T> a, Vector3<T> b)
{
    return Vector3<T>((a.y*b.z)-(a.z*b.y), (a.z*b.x)-(a.x*b.z), (a.x*b.y)-(a.y*b.x));
}

template <class T>
Vector2<T> normalize(Vector2<T> a)
{
    T len = length(a);
    return Vector2<T>(a.x / len, a.y / len);
}

template <class T>
Vector3<T> normalize(Vector3<T> a)
{
    T len = length(a);
    return Vector3<T>(a.x / len, a.y / len, a.z / len);
}

template <class T>
Vector4<T> normalize(Vector4<T> a)
{
    T len = length(a);
    return Vector4<T>(a.x / len, a.y / len, a.z / len, a.w / len);
}

// Template Vector Operators

template <class T>
Vector2<T> operator-(Vector2<T> a, Vector2<T> b)
{
    return Vector2<T>(a.x - b.x, a.y - b.y);
}

template <class T>
Vector2<T> operator-(Vector2<T> a)
{
    return Vector2<T>(-a.x, -a.y);
}

template <class T>
Vector3<T> operator-(Vector3<T> a, Vector3<T> b)
{
    return Vector3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
}

template <class T>
Vector3<T> operator-(Vector3<T> a)
{
    return Vector3<T>(-a.x, -a.y, -a.z);
}

template <class T>
Vector4<T> operator-(Vector4<T> a, Vector4<T> b)
{
    return Vector4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

template <class T>
Vector4<T> operator-(Vector4<T> a)
{
    return Vector4<T>(-a.x, -a.y, -a.z, -a.w);
}

template <class T>
Vector2<T> operator+(Vector2<T> a, Vector2<T> b)
{
    return Vector2<T>(a.x + b.x, a.y + b.y);
}

template <class T>
Vector3<T> operator+(Vector3<T> a, Vector3<T> b)
{
    return Vector3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
}

template <class T>
Vector4<T> operator+(Vector4<T> a, Vector4<T> b)
{
    return Vector4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

template <class T>
Vector2<T> operator*(Vector2<T> a, T s)
{
    return Vector2<T>(a.x * s, a.y * s);
}

template <class T>
Vector2<T> operator*(T s, Vector2<T> a)
{
    return a * s;
}

template <class T>
Vector2<T> operator*(Vector2<T> a, Vector2<T> b)
{
    return Vector2<T>(a.x * b.x, a.y * b.y);
}

template <class T>
Vector2<T> operator/(Vector2<T> a, T s)
{
    return Vector2<T>(a.x / s, a.y / s);
}

template <class T>
Vector3<T> operator*(Vector3<T> a, T s)
{
    return Vector3<T>(a.x * s, a.y * s, a.z * s);
}

template <class T>
Vector3<T> operator*(T s, Vector3<T> a)
{
    return a * s;
}

template <class T>
Vector3<T> operator*(Vector3<T> a, Vector3<T> b)
{
    return Vector3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
}

template <class T>
Vector3<T> operator/(Vector3<T> a, T s)
{
    return Vector3<T>(a.x / s, a.y / s, a.z / s);
}

template <class T>
Vector4<T> operator*(Vector4<T> a, T s)
{
    return Vector4<T>(a.x * s, a.y * s, a.z * s, a.w * s);
}

template <class T>
Vector4<T> operator*(T s, Vector4<T> a)
{
    return a * s;
}

template <class T>
Vector4<T> operator*(Vector4<T> a, Vector4<T> b)
{
    return Vector4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

template <class T>
Vector4<T> operator/(Vector4<T> a, T s)
{
    return Vector4<T>(a.x / s, a.y / s, a.z / s, a.w / s);
}



// Template Matrix Operations

template <class T>
Matrix4x4<T> transpose(Matrix4x4<T> m)
{
    return Matrix4x4<T>(
        m._11, m._21, m._31, m._41,
        m_.12, m._22, m._32, m._42,
        m._13, m._23, m._33, m._43,
        m._14, m._24, m._34, m._44
        );
}

template <class T>
Matrix4x4<T> mul(Matrix4x4<T> m1, Matrix4x4<T> m2)
{
    Matrix4x4<T> mOut;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                mOut[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }

    return mOut;
}

// Common HLSL-compatible vector typedefs

typedef unsigned int uint;

typedef Vector2<float> float2;
typedef Vector3<float> float3;
typedef Vector4<float> float4;

typedef Matrix4x4<float> float4x4;

// Standard Matrix Intializers

inline float4x4 identity()
{
    float4x4 mOut;

    mOut._11 = 1.0f; mOut._12 = 0.0f; mOut._13 = 0.0f; mOut._14 = 0.0f;
    mOut._21 = 0.0f; mOut._22 = 1.0f; mOut._23 = 0.0f; mOut._24 = 0.0f;
    mOut._31 = 0.0f; mOut._32 = 0.0f; mOut._33 = 1.0f; mOut._34 = 0.0f;
    mOut._41 = 0.0f; mOut._42 = 0.0f; mOut._43 = 0.0f; mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 translation(float x, float y, float z)
{
    float4x4 mOut;

    mOut._11 = 1.0f; mOut._12 = 0.0f; mOut._13 = 0.0f; mOut._14 = x;
    mOut._21 = 0.0f; mOut._22 = 1.0f; mOut._23 = 0.0f; mOut._24 = y;
    mOut._31 = 0.0f; mOut._32 = 0.0f; mOut._33 = 1.0f; mOut._34 = z;
    mOut._41 = 0.0f; mOut._42 = 0.0f; mOut._43 = 0.0f; mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 scale(float x, float y, float z)
{
    float4x4 mOut;

    mOut._11 = x;    mOut._12 = 0.0f; mOut._13 = 0.0f; mOut._14 = 0.0f;
    mOut._21 = 0.0f; mOut._22 = y;    mOut._23 = 0.0f; mOut._24 = 0.0f;
    mOut._31 = 0.0f; mOut._32 = 0.0f; mOut._33 = z;    mOut._34 = 0.0f;
    mOut._41 = 0.0f; mOut._42 = 0.0f; mOut._43 = 0.0f; mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 rotationX(float degreeX)
{
    float angleInRadians = degreeX * (PI_F / 180.0f);

    float sinAngle = sinf(angleInRadians);
    float cosAngle = cosf(angleInRadians);

    float4x4 mOut;

    mOut._11 = 1.0f; mOut._12 = 0.0f;     mOut._13 = 0.0f;      mOut._14 = 0.0f;
    mOut._21 = 0.0f; mOut._22 = cosAngle; mOut._23 = -sinAngle; mOut._24 = 0.0f;
    mOut._31 = 0.0f; mOut._32 = sinAngle; mOut._33 = cosAngle;  mOut._34 = 0.0f;
    mOut._41 = 0.0f; mOut._42 = 0.0f;     mOut._43 = 0.0f;      mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 rotationY(float degreeY)
{
    float angleInRadians = degreeY * (PI_F / 180.0f);

    float sinAngle = sinf(angleInRadians);
    float cosAngle = cosf(angleInRadians);

    float4x4 mOut;

    mOut._11 = cosAngle;  mOut._12 = 0.0f; mOut._13 = sinAngle; mOut._14 = 0.0f;
    mOut._21 = 0.0f;      mOut._22 = 1.0f; mOut._23 = 0.0f;     mOut._24 = 0.0f;
    mOut._31 = -sinAngle; mOut._32 = 0.0f; mOut._33 = cosAngle; mOut._34 = 0.0f;
    mOut._41 = 0.0f;      mOut._42 = 0.0f; mOut._43 = 0.0f;     mOut._44 = 1.0f;

    return mOut;
}

inline float4x4 rotationZ(float degreeZ)
{
    float angleInRadians = degreeZ * (PI_F / 180.0f);

    float sinAngle = sinf(angleInRadians);
    float cosAngle = cosf(angleInRadians);

    float4x4 mOut;

    mOut._11 = cosAngle; mOut._12 = -sinAngle; mOut._13 = 0.0f; mOut._14 = 0.0f;
    mOut._21 = sinAngle; mOut._22 = cosAngle;  mOut._23 = 0.0f; mOut._24 = 0.0f;
    mOut._31 = 0.0f;     mOut._32 = 0.0f;      mOut._33 = 1.0f; mOut._34 = 0.0f;
    mOut._41 = 0.0f;     mOut._42 = 0.0f;      mOut._43 = 0.0f; mOut._44 = 1.0f;

    return mOut;
}

// 3D Rotation matrix for an arbitrary axis specified by x, y and z
inline float4x4 rotationArbitrary(float3 axis, float degree)
{
    axis = normalize(axis);

    float angleInRadians = degree * (PI_F / 180.0f);

    float sinAngle = sinf(angleInRadians);
    float cosAngle = cosf(angleInRadians);
    float oneMinusCosAngle = 1 - cosAngle;

    float4x4 mOut;

    mOut._11 = 1.0f + oneMinusCosAngle * (axis.x * axis.x - 1.0f);
    mOut._12 = axis.z * sinAngle + oneMinusCosAngle * axis.x * axis.y;
    mOut._13 = -axis.y * sinAngle + oneMinusCosAngle * axis.x * axis.z;
    mOut._41 = 0.0f;

    mOut._21 = -axis.z * sinAngle + oneMinusCosAngle * axis.y * axis.x;
    mOut._22 = 1.0f + oneMinusCosAngle * (axis.y * axis.y - 1.0f);
    mOut._23 = axis.x * sinAngle + oneMinusCosAngle * axis.y * axis.z;
    mOut._24 = 0.0f;

    mOut._31 = axis.y * sinAngle + oneMinusCosAngle * axis.z * axis.x;
    mOut._32 = -axis.x * sinAngle + oneMinusCosAngle * axis.z * axis.y;
    mOut._33 = 1.0f + oneMinusCosAngle * (axis.z * axis.z - 1.0f);
    mOut._34 = 0.0f;

    mOut._41 = 0.0f;
    mOut._42 = 0.0f;
    mOut._43 = 0.0f;
    mOut._44 = 1.0f;

    return mOut;
}
#include <ppltasks.h>

using namespace Microsoft::WRL;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace concurrency;

// A simple reader/writer class that provides support for reading and writing
// files on disk. Provides synchronous and asynchronous methods.
ref class BasicReaderWriter
{
private:
    Windows::Storage::StorageFolder^ m_location;

internal:
    BasicReaderWriter()
{
    m_location = Package::Current->InstalledLocation;
}
BasicReaderWriter(
    _In_ Windows::Storage::StorageFolder^ folder
    )
{
    m_location = folder;
    Platform::String^ path = m_location->Path;
    if (path->Length() == 0)
    {
        // Applications are not permitted to access certain
        // folders, such as the Documents folder, using this
        // code path.  In such cases, the Path property for
        // the folder will be an empty string.
        throw ref new Platform::FailureException();
    }
}


Platform::Array<byte>^ ReadData(
    _In_ Platform::String^ filename
    )
{
    CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {0};
    extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
    extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
    extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
    extendedParams.lpSecurityAttributes = nullptr;
    extendedParams.hTemplateFile = nullptr;

    Wrappers::FileHandle file(
        CreateFile2(
            filename->Data(),
            GENERIC_READ,
            FILE_SHARE_READ,
            OPEN_EXISTING,
            &extendedParams
            )
        );
    if (file.Get() == INVALID_HANDLE_VALUE)
    {
        throw ref new Platform::FailureException();
    }

    FILE_STANDARD_INFO fileInfo = {0};
    if (!GetFileInformationByHandleEx(
        file.Get(),
        FileStandardInfo,
        &fileInfo,
        sizeof(fileInfo)
        ))
    {
        throw ref new Platform::FailureException();
    }

    if (fileInfo.EndOfFile.HighPart != 0)
    {
        throw ref new Platform::OutOfMemoryException();
    }

    Platform::Array<byte>^ fileData = ref new Platform::Array<byte>(fileInfo.EndOfFile.LowPart);

    if (!ReadFile(
        file.Get(),
        fileData->Data,
        fileData->Length,
        nullptr,
        nullptr
        ))
    {
        throw ref new Platform::FailureException();
    }

    return fileData;
}

task<Platform::Array<byte>^> ReadDataAsync(
    _In_ Platform::String^ filename
    )
{
    return task<StorageFile^>(m_location->GetFileAsync(filename)).then([=](StorageFile^ file)
    {
        return FileIO::ReadBufferAsync(file);
    }).then([=](IBuffer^ buffer)
    {
        auto fileData = ref new Platform::Array<byte>(buffer->Length);
        DataReader::FromBuffer(buffer)->ReadBytes(fileData);
        return fileData;
    });
}


uint32 WriteData(
    _In_ Platform::String^ filename,
    _In_ const Platform::Array<byte>^ fileData
    )
{
    CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {0};
    extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
    extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
    extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
    extendedParams.lpSecurityAttributes = nullptr;
    extendedParams.hTemplateFile = nullptr;

    Wrappers::FileHandle file(
        CreateFile2(
            filename->Data(),
            GENERIC_WRITE,
            0,
            CREATE_ALWAYS,
            &extendedParams
            )
        );
    if (file.Get() == INVALID_HANDLE_VALUE)
    {
        throw ref new Platform::FailureException();
    }

    __DWORD__ numBytesWritten;
    if (
        !WriteFile(
            file.Get(),
            fileData->Data,
            fileData->Length,
            &numBytesWritten,
            nullptr
            ) ||
        numBytesWritten != fileData->Length
        )
    {
        throw ref new Platform::FailureException();
    }

    return numBytesWritten;
}

task<void> WriteDataAsync(
    _In_ Platform::String^ filename,
    _In_ const Platform::Array<byte>^ fileData
    )
{
    return task<StorageFile^>(m_location->CreateFileAsync(filename, CreationCollisionOption::ReplaceExisting)).then([=](StorageFile^ file)
    {
        FileIO::WriteBytesAsync(file, fileData);
    });
}
};

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;

struct BasicVertex
{
    float3 pos;  // position
    float3 norm; // surface normal vector
    float2 tex;  // texture coordinate
};

struct ConstantBuffer
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

// This class defines the application as a whole.
ref class Direct3DTutorialFrameworkView : public IFrameworkView
{
private:
    Platform::Agile<CoreWindow> m_window;
    ComPtr<IDXGISwapChain1> m_swapChain;
    ComPtr<ID3D11Device1> m_d3dDevice;
    ComPtr<ID3D11DeviceContext1> m_d3dDeviceContext;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11Buffer> m_constantBuffer;
    ConstantBuffer m_constantBufferData;

public:
    // This method is called on application launch.
    virtual void Initialize(
        _In_ CoreApplicationView^ applicationView
        )
    {
        applicationView->Activated +=
            ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &Direct3DTutorialFrameworkView::OnActivated);

        CoreApplication::Suspending +=
            ref new EventHandler<SuspendingEventArgs^>(this, &Direct3DTutorialFrameworkView::OnSuspending);
    }

    // This method is called after Initialize.
    virtual void SetWindow(
        _In_ CoreWindow^ window
        )
    {
        m_window = window;

#if WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP
        // Specify the cursor type as the standard arrow if not Windows Phone.
        // Windows Phone doesn't have a mouse and these APIs are not supported.
        m_window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
#endif
        // Allow the application to respond when the window size changes.
        m_window->SizeChanged +=
            ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(
                this,
                &Direct3DTutorialFrameworkView::OnWindowSizeChanged
                );
    }

    void OnActivated(
        _In_ CoreApplicationView^ applicationView,
        _In_ IActivatedEventArgs^ args
        )
    {
        // Activate the application window, making it visible and enabling it to receive events.
        CoreWindow::GetForCurrentThread()->Activate();
    }

    virtual void Load(_In_ Platform::String^ entryPoint)
    {
    }

    // This method is called after Load.
    virtual void Run()
    {
        // First, create the Direct3D device.

        // This flag is required in order to enable compatibility with Direct2D.
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
        // If the project is in a debug build, enable debugging via SDK Layers with this flag.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        // This array defines the ordering of feature levels that D3D should attempt to create.
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_1
        };

        ComPtr<ID3D11Device> d3dDevice;
        ComPtr<ID3D11DeviceContext> d3dDeviceContext;
        DX::ThrowIfFailed(
            D3D11CreateDevice(
                nullptr,                    // specify nullptr to use the default adapter
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,                    // leave as nullptr if hardware is used
                creationFlags,              // optionally set debug and Direct2D compatibility flags
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,          // always set this to D3D11_SDK_VERSION
                &d3dDevice,
                nullptr,
                &d3dDeviceContext
                )
            );

        // Retrieve the Direct3D 11.1 interfaces.
        DX::ThrowIfFailed(
            d3dDevice.As(&m_d3dDevice)
            );

        DX::ThrowIfFailed(
            d3dDeviceContext.As(&m_d3dDeviceContext)
            );


        // After the D3D device is created, create additional application resources.
        CreateWindowSizeDependentResources();

        // Create a Basic Reader-Writer class to load data from disk.  This class is examined
        // in the Resource Loading sample.
        BasicReaderWriter^ reader = ref new BasicReaderWriter();

	D3D11_TEXTURE2D_DESC backBufferDesc = {0};
        backBuffer->GetDesc(&backBufferDesc);

        init(static_cast<unsigned int>(backBufferDesc.Width), static_cast<unsigned int>(backBufferDesc.Height));
        // Enter the render loop. Note that Windows Store apps should never exit.
        while (true)
        {
            // Process events incoming to the window.
            m_window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

            render();

            // Present the rendered image to the window.  Because the maximum frame latency is set to 1,
            // the render loop will generally be throttled to the screen refresh rate, typically around
            // 60Hz, by sleeping the application on Present until the screen is refreshed.
            DX::ThrowIfFailed(
                m_swapChain->Present(1, 0)
                );
        }
    }

    // This method is called before the application exits.
    virtual void Uninitialize()
    {
    }

private:

    // This method is called whenever the application window size changes.
    void OnWindowSizeChanged(
        _In_ CoreWindow^ sender,
        _In_ WindowSizeChangedEventArgs^ args
        )
    {
        m_renderTargetView = nullptr;
        m_depthStencilView = nullptr;
        CreateWindowSizeDependentResources();
    }

    // This method is called when the app is being suspended.
    void OnSuspending(
        _In_ Platform::Object^ sender,
        _In_ SuspendingEventArgs^ args
        )
    {
        // Call this method when the app suspends to hint to the driver that the app is entering an idle state
        // and that its memory can be used temporarily for other apps.
        ComPtr<IDXGIDevice3> dxgiDevice;
        m_d3dDevice.As(&dxgiDevice);
        dxgiDevice->Trim();
    }

    // This method creates all application resources that depend on
    // the application window size.  It is called at app initialization,
    // and whenever the application window size changes.
    void CreateWindowSizeDependentResources()
    {
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
		// ResizeBuffers is not available on the Phone so the swapChain needs to be recreated.
		m_swapChain = nullptr;
#endif
        if (m_swapChain != nullptr)
        {
            // If the swap chain already exists, resize it.
            DX::ThrowIfFailed(
                m_swapChain->ResizeBuffers(
                    2,
                    0,
                    0,
                    DXGI_FORMAT_B8G8R8A8_UNORM,
                    0
                    )
                );
        }
        else
        {
            // If the swap chain does not exist, create it.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};

            swapChainDesc.Stereo = false;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.Scaling = DXGI_SCALING_NONE;
            swapChainDesc.Flags = 0;

            // Use automatic sizing.
            swapChainDesc.Width = 0;
            swapChainDesc.Height = 0;

            // This is the most common swap chain format.
            swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

            // Don't use multi-sampling.
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;

            // Use two buffers to enable flip effect.
            swapChainDesc.BufferCount = 2;

            // We recommend using this swap effect for all applications.
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;


            // Once the swap chain description is configured, it must be
            // created on the same adapter as the existing D3D Device.

            // First, retrieve the underlying DXGI Device from the D3D Device.
            ComPtr<IDXGIDevice2> dxgiDevice;
            DX::ThrowIfFailed(
                m_d3dDevice.As(&dxgiDevice)
                );

            // Ensure that DXGI does not queue more than one frame at a time. This both reduces
            // latency and ensures that the application will only render after each VSync, minimizing
            // power consumption.
            DX::ThrowIfFailed(
                dxgiDevice->SetMaximumFrameLatency(1)
                );

            // Next, get the parent factory from the DXGI Device.
            ComPtr<IDXGIAdapter> dxgiAdapter;
            DX::ThrowIfFailed(
                dxgiDevice->GetAdapter(&dxgiAdapter)
                );

            ComPtr<IDXGIFactory2> dxgiFactory;
            DX::ThrowIfFailed(
                dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
                );

            // Finally, create the swap chain.
            CoreWindow^ window = m_window.Get();
            DX::ThrowIfFailed(
                dxgiFactory->CreateSwapChainForCoreWindow(
                    m_d3dDevice.Get(),
                    reinterpret_cast<IUnknown*>(window),
                    &swapChainDesc,
                    nullptr, // allow on all displays
                    &m_swapChain
                    )
                );
        }

        // Once the swap chain is created, create a render target view.  This will
        // allow Direct3D to render graphics to the window.

        ComPtr<ID3D11Texture2D> backBuffer;
        DX::ThrowIfFailed(
            m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))
            );

        DX::ThrowIfFailed(
            m_d3dDevice->CreateRenderTargetView(
                backBuffer.Get(),
                nullptr,
                &m_renderTargetView
                )
            );

        // Once the render target view is created, create a depth stencil view.  This
        // allows Direct3D to efficiently render objects closer to the camera in front
        // of objects further from the camera.

        D3D11_TEXTURE2D_DESC backBufferDesc = {0};
        backBuffer->GetDesc(&backBufferDesc);

        D3D11_TEXTURE2D_DESC depthStencilDesc;
        depthStencilDesc.Width = backBufferDesc.Width;
        depthStencilDesc.Height = backBufferDesc.Height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;
        ComPtr<ID3D11Texture2D> depthStencil;
        DX::ThrowIfFailed(
            m_d3dDevice->CreateTexture2D(
                &depthStencilDesc,
                nullptr,
                &depthStencil
                )
            );

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        depthStencilViewDesc.Format = depthStencilDesc.Format;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Flags = 0;
        depthStencilViewDesc.Texture2D.MipSlice = 0;
        DX::ThrowIfFailed(
            m_d3dDevice->CreateDepthStencilView(
                depthStencil.Get(),
                &depthStencilViewDesc,
                &m_depthStencilView
                )
            );

        // After the render target and depth stencil views are created, specify that
        // the viewport, which describes what portion of the window to draw to, should
        // cover the entire window.

        D3D11_VIEWPORT viewport;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(backBufferDesc.Width);
        viewport.Height = static_cast<float>(backBufferDesc.Height);
        viewport.MinDepth = D3D11_MIN_DEPTH;
        viewport.MaxDepth = D3D11_MAX_DEPTH;

        m_d3dDeviceContext->RSSetViewports(1, &viewport);

        // Finally, update the constant buffer perspective projection parameters
        // to account for the size of the application window.  In this sample,
        // the parameters are fixed to a 70-degree field of view, with a depth
        // range of 0.01 to 100.  See Lesson 5 for a generalized camera class.

        float xScale = 1.42814801f;
        float yScale = 1.42814801f;
        if (backBufferDesc.Width > backBufferDesc.Height)
        {
            xScale = yScale *
                static_cast<float>(backBufferDesc.Height) /
                static_cast<float>(backBufferDesc.Width);
        }
        else
        {
            yScale = xScale *
                static_cast<float>(backBufferDesc.Width) /
                static_cast<float>(backBufferDesc.Height);
        }

        m_constantBufferData.projection = float4x4(
            xScale, 0.0f,    0.0f,  0.0f,
            0.0f,   yScale,  0.0f,  0.0f,
            0.0f,   0.0f,   -1.0f, -0.01f,
            0.0f,   0.0f,   -1.0f,  0.0f
            );
    }
};

// This class defines how to create the custom View Provider defined above.
ref class Direct3DTutorialFrameworkViewSource : IFrameworkViewSource
{
public:
    virtual IFrameworkView^ CreateView()
    {
        return ref new Direct3DTutorialFrameworkView();
    }
};

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto frameworkViewSource = ref new Direct3DTutorialFrameworkViewSource();
    Windows::ApplicationModel::Core::CoreApplication::Run(frameworkViewSource);
    return 0;
}
#endif

