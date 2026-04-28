#pragma once

#include"pch.h"

using namespace Microsoft::WRL;


enum class API { D3D12 , VULKAN , OPENGL};

class render : public IEngine
{

	
public:
    virtual       ~render() = default; 

	virtual  bool Init(Init_Config config) override  = 0;
	virtual  void Update()                 override    = 0;
	virtual  void End()                    override  = 0;
	
	
};

struct Vertex
{
};



class D3D_CONTEXT final : public render 
{

 public :
	virtual ~D3D_CONTEXT()          override = default;
	virtual  bool Init(Init_Config config)   override ;
	virtual  void Update()                   override;
	virtual  void End ()                     override;
 private :
	  bool Build_Swap_Chain(Init_Config config);
	  bool Build_Desc_Heap();
	  bool Init_Core();

	  void Build_Shaders();
	  void Draw();
      void Exec();	                         
      void SignalAndWait() ;	            
      void UpdatePipeline();	            	
	  void Create_RootSignature();
      void Create_PSO();
	  struct FLAME_CB_RESOURCE {
		  ID3D12Resource* pResource;
		  void* pMappedData;
	  };

	
	  inline UINT Align256(UINT size) {
		  return (size + 255) & ~255;
	  }
	  struct alignas(16) SceneBuffer
      {
	      flame::Mat4 viewProjection; 
	      flame::Mat4 modelMatrix;    
          flame::Vec4 cameraPos;     
      }; 
	  void UpdateShaderMathD3D12(const FLAME_CB_RESOURCE& gpuBuffer, const SceneBuffer& cpuData)
	  {
		  memcpy(gpuBuffer.pMappedData, &cpuData, sizeof(SceneBuffer));
	  }	
	 ComPtr<ID3D12Debug>                m_DebugController;
	 ComPtr<ID3D12Device>               m_Device;
	 ComPtr<IDXGIFactory4>              m_Factory;
	 ComPtr<IDXGIAdapter1>              m_Adapter;
	 ComPtr<ID3D12CommandQueue>         m_Command_Queue;
	 ComPtr<ID3D12Fence>                m_Fence;
	 ComPtr<ID3D12CommandAllocator>     m_Command_Allocator;
	 ComPtr<ID3D12GraphicsCommandList>  m_Command_List;
	 ComPtr<IDXGISwapChain3>            m_Swap_Chain;
	 ComPtr<ID3D12Resource>             m_RenderTargets[2];
	 ComPtr<ID3D12DescriptorHeap>       m_RTV_Heap;
	 ComPtr<ID3D12RootSignature>		m_Root_Signature;
	 ComPtr<ID3DBlob>					m_ps_BC;
	 ComPtr<ID3DBlob>					m_vs_BC;
	 UINT                               m_RTV_Descriptor_Size = 0;
	 UINT                               m_Frame_Index = 0;
	 uint64_t                           m_CurrentFenceValue = 0;
	 HANDLE                             m_FenceEvent = nullptr;
};


class GL_CONTEXT final : public render
{
 public:
	 virtual ~GL_CONTEXT() override = default;
	 virtual  bool Init(Init_Config config) override;
	 virtual  void Update()                 override;
	 virtual  void End()                    override;
private:

};
 inline	render* CreateDevice(API api) 
	{
		switch (api) 
		{
	      using enum API;
		  case D3D12  :	 return MemoryManager::NewPermanent<D3D_CONTEXT>();
		  case VULKAN :   
		  case OPENGL :   
		  default     :   return nullptr;
	  }
	}

