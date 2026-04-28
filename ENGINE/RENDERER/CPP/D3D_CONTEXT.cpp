#include"pch.h"

using namespace Microsoft::WRL;

bool D3D_CONTEXT::Init_Core()
{
    LOG_INFO("DX12: Starting Core Infrastructure Initialization...");

#if defined(_DEBUG)
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController))))
    {
        m_DebugController->EnableDebugLayer();
        LOG_INFO("DX12: Debug_Layer_Enabled.");
    }
    else
    {
        LOG_WARNING("DX12: Failed to enable Debug Layer (Non-critical).");
    }
#endif

    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory));
    if (FAILED(hr)) {
        LOG_ERROR("DX12: Failed to create DXGI Factory. HRESULT: " + std::to_string(hr));
        return false;
    }
  


    ComPtr<IDXGIAdapter1> adapter;
    bool adapterFound = false;

    for (uint32_t i = 0; m_Factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);


        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;


        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            m_Adapter = adapter;
            adapterFound = true;


            std::wstring ws(desc.Description);
            std::string gpuName(ws.begin(), ws.end());
            LOG_INFO("DX12: Hardware_Adapter_Found: " + gpuName);
            break;
        }
    }

    if (!adapterFound) {
        LOG_ERROR("DX12: No hardware GPU found that supports Feature Level 11.0!");
        return false;
    }

    hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
    if (FAILED(hr)) {
        LOG_ERROR("DX12: Failed to create Device. HRESULT: " + std::to_string(hr));
        return false;
    }
   

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_Command_Queue));
    if (FAILED(hr)) {
        LOG_ERROR("DX12: Failed to create Command Queue.");
        return false;
    }
   

    hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_Command_Allocator));
    if (FAILED(hr)) {
        LOG_ERROR("DX12: Failed to create Command Allocator.");
        return false;
    }


    hr = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_Command_Allocator.Get(), nullptr, IID_PPV_ARGS(&m_Command_List));
    if (FAILED(hr)) {
        LOG_ERROR("DX12: Failed to create Command List.");
        return false;
    }
    m_Command_List->Close();
   

    hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
    if (FAILED(hr)) {
        LOG_ERROR("DX12: Failed to create Fence.");
        return false;
    }
    m_CurrentFenceValue = 1;
    m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_FenceEvent) {
        LOG_ERROR("DX12: Failed to create Fence Event.");
        return false;
    }

    LOG_INFO("DX12: Core infrastructure is ready.");
    return true;

 }
bool D3D_CONTEXT::Build_Swap_Chain(Init_Config config)
 {
    HWND hwnd = static_cast<HWND>(config.hwnd);
      DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
      swapChainDesc.BufferCount      = 2;
      swapChainDesc.Width            = config.Window_width;
      swapChainDesc.Height           = config.Window_height;
      swapChainDesc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
      swapChainDesc.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swapChainDesc.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;
      swapChainDesc.SampleDesc.Count = 1;
      ComPtr<IDXGISwapChain1> swapChain1;
      HRESULT hr = m_Factory->CreateSwapChainForHwnd(m_Command_Queue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
      if (FAILED(hr))
      {
          LOG_ERROR("DX12: Failed to create Swap Chain1");
         return false;
      }
      hr = swapChain1.As(&m_Swap_Chain);
      m_Frame_Index = m_Swap_Chain->GetCurrentBackBufferIndex();
      if (FAILED(hr))
      {
          LOG_ERROR("DX12: Failed to query IDXGISwapChain from IDXGISwapChain1");
         return false;
      }
      LOG_INFO("DX12: Swap Chain initialized.");
      return true;

 }
bool D3D_CONTEXT::Build_Desc_Heap()
 {
     
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTV_Heap));
    m_RTV_Descriptor_Size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    if (FAILED(hr))
    {
        LOG_ERROR("DX12: Failed to Create the DESC_HEAP");
    }
    // Get the start of the rack
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTV_Heap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < 2; i++)
    {
        m_Swap_Chain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
        m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);

        // Move to the next slot in the rack
        rtvHandle.ptr += m_RTV_Descriptor_Size;
    }
    LOG_INFO("DX12: Desc_heap Created.");
    return true;

 }
void D3D_CONTEXT::SignalAndWait()
{
    const uint64_t fence = m_CurrentFenceValue;
    m_Command_Queue->Signal(m_Fence.Get(), fence);
    m_CurrentFenceValue++;

    if (m_Fence->GetCompletedValue() < fence)
    {
        m_Fence->SetEventOnCompletion(fence, m_FenceEvent);
        WaitForSingleObject(m_FenceEvent, INFINITE);
    }
}
void D3D_CONTEXT::Create_RootSignature()
{
    D3D12_ROOT_SIGNATURE_DESC RSDesc = {};
   RSDesc.NumParameters     = 0;
   RSDesc.pParameters       = nullptr;
   RSDesc.pStaticSamplers   = 0;
   RSDesc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
   RSDesc.NumStaticSamplers = 0;

       ComPtr<ID3DBlob> serRS = nullptr;
       ComPtr<ID3DBlob> errRS = nullptr;
       
       D3D12SerializeRootSignature(&RSDesc , D3D_ROOT_SIGNATURE_VERSION_1 , &serRS , &errRS);

   HRESULT hr =    m_Device->CreateRootSignature(0,serRS->GetBufferPointer(),serRS->GetBufferSize(),IID_PPV_ARGS(&m_Root_Signature));
   if (FAILED(hr))
   {
       LOG_ERROR("DX12: Failed to create Root_Signature");
   }
}
void D3D_CONTEXT::Create_PSO()
{
    D3D12_INPUT_ELEMENT_DESC inpDesc[] = 
    {
        { "POSITION" , 0 , DXGI_FORMAT_R32G32B32_FLOAT , 0 ,0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0},
        { "COLOR"    , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT , 0 ,12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 }
    };


}
void D3D_CONTEXT::Exec()
{

	ID3D12CommandList* ppCommandLists[] = { m_Command_List.Get() };
	m_Command_Queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

}
void D3D_CONTEXT::UpdatePipeline()
{
	m_Command_Allocator->Reset();
	m_Command_List     ->Reset(m_Command_Allocator.Get(), nullptr);


	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   =  m_RenderTargets[m_Frame_Index].Get();
	barrier.Transition.StateBefore =  D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter  =  D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource =  D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_Command_List->ResourceBarrier(1, &barrier);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTV_Heap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_Frame_Index * m_RTV_Descriptor_Size;

	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	m_Command_List->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
    m_Command_List->ResourceBarrier(1, &barrier);

    m_Command_List->Close();

}  
void D3D_CONTEXT::Build_Shaders()
{
    UINT compileFlags = 0;

 #if defined _DEBUG
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ComPtr<ID3DBlob> errblob;
   // D3DCompileFromFile(L"" , nullptr , nullptr , "VS" , "vs_5_0", D3DCOMPILE_DEBUG,0 , &m_vs_BC , errblob);
   // D3DCompileFromFile(L"",  nullptr , nullptr , "PS", "ps_5_0" , D3DCOMPILE_DEBUG, 0, &m_ps_BC , errblob);
}  
void D3D_CONTEXT::Draw()
{
    UpdatePipeline(); 
    Exec();          


    m_Swap_Chain->Present(1, 0);

    SignalAndWait();

    m_Frame_Index = m_Swap_Chain->GetCurrentBackBufferIndex();
}


 //=========================================================================
 //                   MAIN_FUNCTIONS
 //=========================================================================

bool D3D_CONTEXT::Init(Init_Config config)
{
    Init_Core();
    Build_Swap_Chain(config);
    Build_Desc_Heap();
    return true;
}

void D3D_CONTEXT::Update()
{
    Draw();
}
void D3D_CONTEXT::End()
{

	if (m_FenceEvent)
	{
		CloseHandle(m_FenceEvent);
		LOG_INFO("Event_Closed");
	}
	else
	{
		LOG_ERROR("Event_not_found");
	}

	m_DebugController.Reset();
	m_Adapter.Reset();
	m_Command_Queue.Reset();
	m_Device.Reset();
	m_Factory.Reset();
	m_Fence.Reset(); 	LOG_INFO("RENDER_Ended");
}