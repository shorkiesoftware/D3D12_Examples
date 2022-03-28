#include <comdef.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <windows.h>
#include <xaudio2.h>
#include <xinput.h>
#include "os_interface.h"

#define WinAssert(x) \
    if (FAILED(x)) *(int*)0 = 0

u32 width = 1280;
u32 height = 720;

static void getD3D12HardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter) {
    *ppAdapter = 0;
    IDXGIAdapter1* adapter;
    IDXGIFactory6* factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
        for (UINT adapterIndex = 0;
             DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
             adapterIndex++) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), 0))) {
                break;
            }
        }
    } else {
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), 0))) {
                break;
            }
        }
    }

    *ppAdapter = adapter;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_QUIT:
        case WM_CLOSE: {
            exit(0);
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    //WINDOW SETUP /////////////////////////////////////////////////////////////////////////////////////////////////////////
    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = "COMPUTE_SHADER_TEST";
    RegisterClassEx(&windowClass);

    RECT windowRect = {0, 0, (LONG)width, (LONG)height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    u32 monitorWidth = GetSystemMetrics(SM_CXSCREEN);
    u32 monitorHeight = GetSystemMetrics(SM_CYSCREEN);
    u32 windowX = monitorWidth / 2 - width / 2;
    u32 windowY = monitorHeight / 2 - height / 2;

    HWND windowHandle = CreateWindow(windowClass.lpszClassName, "COMPUTE_SHADER_TEST", WS_OVERLAPPEDWINDOW,
                                windowX, windowY, windowRect.right - windowRect.left,
                                windowRect.bottom - windowRect.top, 0, 0, hInstance, 0);
    //WINDOW SETUP ***********************************************************************************************************
    //D3D12 PIPELINE SETUP ////////////////////////////////////////////////////////////////////////////////////////////////////
    D3D12_VIEWPORT d3d12Viewport;
    D3D12_RECT d3d12ScissorRect;
    d3d12Viewport.TopLeftX = 0;
    d3d12Viewport.TopLeftY = 0;
    d3d12Viewport.Width = width;
    d3d12Viewport.Height = height;
    d3d12Viewport.MinDepth = 0;
    d3d12Viewport.MaxDepth = 1;
    d3d12ScissorRect.left = 0;
    d3d12ScissorRect.top = 0;
    d3d12ScissorRect.right = width;
    d3d12ScissorRect.bottom = height;
    WinAssert(DXGIDeclareAdapterRemovalSupport());

    u32 dxgiFactoryFlags = 0;
    ID3D12Debug* debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }

    IDXGIFactory4* factory;
    WinAssert(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    ID3D12Device* d3d12Device;
    bool d3d12UseWarpDevice = false;
    if (d3d12UseWarpDevice) {
        IDXGIAdapter* warpAdapter;
        WinAssert(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        WinAssert(D3D12CreateDevice(warpAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device)));
    } else {
        IDXGIAdapter1* hardwareAdapter;
        getD3D12HardwareAdapter(factory, &hardwareAdapter);

        WinAssert(D3D12CreateDevice(hardwareAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device)));
    }

    ID3D12CommandQueue* d3d12CommandQueue;
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    WinAssert(d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3d12CommandQueue)));

    u32 d3d12FrameCount = 2;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = d3d12FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    IDXGISwapChain1* swapChain;
    WinAssert(factory->CreateSwapChainForHwnd(d3d12CommandQueue, windowHandle, &swapChainDesc, 0, 0, &swapChain));
    WinAssert(factory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER));
    IDXGISwapChain3* d3d12SwapChain = (IDXGISwapChain3*)swapChain;
    u32 d3d12FrameIndex = d3d12SwapChain->GetCurrentBackBufferIndex();
    
    ID3D12DescriptorHeap* d3d12RTVHeap;
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = d3d12FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    WinAssert(d3d12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&d3d12RTVHeap)));
    u32 d3d12RTVDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    ID3D12GraphicsCommandList* d3d12CommandList;
    ID3D12Resource* d3d12RenderTargets[2];
    ID3D12CommandAllocator* d3d12CommandAllocators[2];
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(d3d12RTVHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < d3d12FrameCount; n++) {
        WinAssert(d3d12SwapChain->GetBuffer(n, IID_PPV_ARGS(&d3d12RenderTargets[n])));
        d3d12Device->CreateRenderTargetView(d3d12RenderTargets[n], 0, rtvHandle);
        rtvHandle.ptr += d3d12RTVDescriptorSize;
        WinAssert(d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d12CommandAllocators[n])));
    }
    WinAssert(d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d12CommandAllocators[d3d12FrameIndex], 0, IID_PPV_ARGS(&d3d12CommandList)));

    WinAssert(d3d12CommandList->Close());

    ID3D12Fence* d3d12Fence;
    HANDLE d3d12FenceEvent;
    u64 d3d12FenceValues[2];
    WinAssert(d3d12Device->CreateFence(d3d12FenceValues[d3d12FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12Fence)));
    d3d12FenceValues[d3d12FrameIndex]++;

    d3d12FenceEvent = CreateEvent(0, false, false, 0);
    if (d3d12FenceEvent == 0) {
        WinAssert(HRESULT_FROM_WIN32(GetLastError()));
    }
    //D3D12 PIPELINE SETUP ***************************************************************************************************

    ShowWindow(windowHandle, nCmdShow);
    bool running = true;
    while(running) {
        MSG msg = {};
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }


        WinAssert(d3d12CommandAllocators[d3d12FrameIndex]->Reset());
        WinAssert(d3d12CommandList->Reset(d3d12CommandAllocators[d3d12FrameIndex], 0));

        D3D12_RESOURCE_BARRIER resBarrier = {};
        resBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        resBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        resBarrier.Transition.pResource = d3d12RenderTargets[d3d12FrameIndex];
        resBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        resBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        resBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        d3d12CommandList->ResourceBarrier(1, &resBarrier);

        rtvHandle = d3d12RTVHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += d3d12FrameIndex * d3d12RTVDescriptorSize;
        d3d12CommandList->RSSetViewports(1, &d3d12Viewport);
        d3d12CommandList->RSSetScissorRects(1, &d3d12ScissorRect);
        d3d12CommandList->OMSetRenderTargets(1, &rtvHandle, false, 0);
        Vector4 cc(0, 1, 1, 1);
        d3d12CommandList->ClearRenderTargetView(rtvHandle, cc.va, 0, 0);

        resBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        resBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        d3d12CommandList->ResourceBarrier(1, &resBarrier);

        WinAssert(d3d12CommandList->Close());

        ID3D12CommandList* ppCommandList[] = {d3d12CommandList};
        d3d12CommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

        WinAssert(d3d12SwapChain->Present(1, 0));

        const UINT64 currentFenceValue = d3d12FenceValues[d3d12FrameIndex];
        
        WinAssert(d3d12CommandQueue->Signal(d3d12Fence, currentFenceValue));
        d3d12FrameIndex = d3d12SwapChain->GetCurrentBackBufferIndex();        

        if (d3d12Fence->GetCompletedValue() < d3d12FenceValues[d3d12FrameIndex]) {
            WinAssert(d3d12Fence->SetEventOnCompletion(d3d12FenceValues[d3d12FrameIndex], d3d12FenceEvent));
            WaitForSingleObjectEx(d3d12FenceEvent, INFINITE, false);
        }
        d3d12FenceValues[d3d12FrameIndex] = currentFenceValue + 1;

        WinAssert(d3d12CommandQueue->Signal(d3d12Fence, d3d12FenceValues[d3d12FrameIndex]));
        WinAssert(d3d12Fence->SetEventOnCompletion(d3d12FenceValues[d3d12FrameIndex], d3d12FenceEvent));
        WaitForSingleObjectEx(d3d12FenceEvent, INFINITE, FALSE);
        d3d12FenceValues[d3d12FrameIndex]++;

    }
    return 0;
}