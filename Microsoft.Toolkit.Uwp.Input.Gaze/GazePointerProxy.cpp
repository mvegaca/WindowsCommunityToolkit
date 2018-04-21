#include "pch.h"
#include "GazePointerProxy.h"
#include "GazePointer.h"

BEGIN_NAMESPACE_GAZE_INPUT

DependencyProperty^ GazePointerProxy::GazePointerProxyProperty::get()
{
    static auto value = DependencyProperty::RegisterAttached("_GazePointerProxy", GazePointerProxy::typeid, GazePointerProxy::typeid,
        ref new PropertyMetadata(nullptr));
    return value;
}

GazePointerProxy::GazePointerProxy(FrameworkElement^ element)
    : _element(element)
{
    _loadedToken = element->Loaded += ref new RoutedEventHandler(this, &GazePointerProxy::OnPageLoaded);
    _unloadedToken = element->Unloaded += ref new RoutedEventHandler(this, &GazePointerProxy::OnPageUnloaded);
}

void GazePointerProxy::SetGazeEnabled(FrameworkElement^ element, bool value)
{
    auto proxy = safe_cast<GazePointerProxy^>(element->GetValue(GazePointerProxyProperty));
    if (proxy == nullptr)
    {
        proxy = ref new GazePointerProxy(element);
        element->SetValue(GazePointerProxyProperty, proxy);
    }

    proxy->IsEnabled = value;
}

void GazePointerProxy::IsEnabled::set(bool value)
{
    if (_isEnabled != value)
    {
        _isEnabled = value;

        if (_isLoaded)
        {
            if (value)
            {
                GazePointer::Instance->AddRoot(_element);
            }
            else
            {
                GazePointer::Instance->RemoveRoot(_element);
            }
        }
    }
}

void GazePointerProxy::OnPageLoaded(Object^ sender, RoutedEventArgs^ args)
{
    _element->Loaded -= _loadedToken;

    _isLoaded = true;

    if (_isEnabled)
    {
        GazePointer::Instance->AddRoot(_element);
    }
}

void GazePointerProxy::OnPageUnloaded(Object^ sender, RoutedEventArgs^ args)
{
    if (!_isLoaded)
    {
        _element->Loaded -= _loadedToken;
    }
    _element->Unloaded -= _loadedToken;

    _isLoaded = false;

    if (_isEnabled)
    {
        GazePointer::Instance->RemoveRoot(_element);
    }
}


END_NAMESPACE_GAZE_INPUT